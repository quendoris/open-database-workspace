#include "MainWindow.hpp"

#include "views/AdaptiveInspector.hpp"

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/LayoutSaver.h>
#include <kddockwidgets/core/MainWindow.h>

#include <QAbstractItemView>
#include <QAction>
#include <QCloseEvent>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace odw::ui {
namespace {

constexpr int kObjectTypeRole = Qt::UserRole;
constexpr int kObjectNameRole = Qt::UserRole + 1;

QTableWidget* createDataGrid(QWidget* parent,
                             const QStringList& headers,
                             const QList<QStringList>& rows) {
    auto* table = new QTableWidget(rows.size(), headers.size(), parent);
    table->setHorizontalHeaderLabels(headers);
    table->setAlternatingRowColors(true);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(30);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    for (qsizetype row = 0; row < rows.size(); ++row) {
        const QStringList& values = rows.at(row);
        for (qsizetype column = 0; column < values.size(); ++column) {
            auto* item = new QTableWidgetItem(values.at(column));
            if (values.at(column) == QStringLiteral("NULL")) {
                QFont font = item->font();
                font.setItalic(true);
                item->setFont(font);
                item->setToolTip(QStringLiteral("SQL NULL"));
            }
            table->setItem(static_cast<int>(row), static_cast<int>(column), item);
        }
    }

    return table;
}

QList<QStringList> usersRows() {
    QList<QStringList> rows;
    for (int row = 0; row < 12; ++row) {
        const int id = 101 + row;
        rows.append({
            QString::number(id),
            QStringLiteral("user_%1").arg(id),
            row == 4 ? QStringLiteral("NULL")
                     : QStringLiteral("user%1@example.dev").arg(id),
            row % 3 == 0 ? QStringLiteral("pending") : QStringLiteral("active"),
            QStringLiteral("2026-09-%1  1%2:%3")
                .arg(6 - (row % 6), 2, 10, QLatin1Char('0'))
                .arg(row % 10)
                .arg((row * 7) % 60, 2, 10, QLatin1Char('0')),
            row == 8 ? QStringLiteral("NULL")
                     : QStringLiteral("%1.%2")
                           .arg(120 + row * 17)
                           .arg((row * 13) % 100, 2, 10, QLatin1Char('0')),
        });
    }
    return rows;
}

QList<QStringList> ordersRows() {
    QList<QStringList> rows;
    for (int row = 0; row < 10; ++row) {
        rows.append({
            QString::number(5001 + row),
            QString::number(101 + (row % 7)),
            QStringLiteral("%1.%2")
                .arg(48 + row * 23)
                .arg((row * 19) % 100, 2, 10, QLatin1Char('0')),
            row % 4 == 0 ? QStringLiteral("processing") : QStringLiteral("paid"),
            QStringLiteral("2026-09-%1  %2:%3")
                .arg(6 - (row % 5), 2, 10, QLatin1Char('0'))
                .arg(9 + row, 2, 10, QLatin1Char('0'))
                .arg((row * 11) % 60, 2, 10, QLatin1Char('0')),
        });
    }
    return rows;
}

QList<QStringList> recentOrdersRows() {
    QList<QStringList> rows;
    const auto source = ordersRows();
    for (int row = 0; row < 6; ++row) {
        rows.append(source.at(row));
    }
    return rows;
}

QWidget* wrapGrid(const QString& countText, QTableWidget* grid, QWidget* parent) {
    auto* root = new QWidget(parent);
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    auto* filterRow = new QWidget(root);
    auto* filterLayout = new QHBoxLayout(filterRow);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(8);

    auto* filter = new QLineEdit(filterRow);
    filter->setPlaceholderText(QStringLiteral("Filter visible rows…"));
    auto* count = new QLabel(countText, filterRow);
    count->setObjectName(QStringLiteral("odwSecondaryText"));

    filterLayout->addWidget(filter, 1);
    filterLayout->addWidget(count);
    layout->addWidget(filterRow);
    layout->addWidget(grid, 1);

    QObject::connect(filter, &QLineEdit::textChanged, grid, [grid](const QString& text) {
        for (int row = 0; row < grid->rowCount(); ++row) {
            bool match = text.trimmed().isEmpty();
            for (int column = 0; !match && column < grid->columnCount(); ++column) {
                const auto* item = grid->item(row, column);
                match = item != nullptr && item->text().contains(text, Qt::CaseInsensitive);
            }
            grid->setRowHidden(row, !match);
        }
    });

    return root;
}

QWidget* createStructureView(const QString& objectName,
                             const QString& objectType,
                             QWidget* parent) {
    auto* root = new QWidget(parent);
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    auto* summary = new QLabel(
        QStringLiteral("public.%1   ·   %2   ·   PostgreSQL · local")
            .arg(objectName, objectType),
        root);
    summary->setObjectName(QStringLiteral("odwSecondaryText"));
    layout->addWidget(summary);

    QStringList headers{
        QStringLiteral("column"),
        QStringLiteral("type"),
        QStringLiteral("nullable"),
        QStringLiteral("key"),
    };
    QList<QStringList> rows;

    if (objectName == QStringLiteral("users")) {
        rows = {
            {QStringLiteral("id"), QStringLiteral("bigint"), QStringLiteral("no"), QStringLiteral("PK")},
            {QStringLiteral("name"), QStringLiteral("text"), QStringLiteral("no"), QString()},
            {QStringLiteral("email"), QStringLiteral("text"), QStringLiteral("yes"), QStringLiteral("unique")},
            {QStringLiteral("status"), QStringLiteral("user_status"), QStringLiteral("no"), QString()},
            {QStringLiteral("created_at"), QStringLiteral("timestamptz"), QStringLiteral("no"), QString()},
            {QStringLiteral("balance"), QStringLiteral("numeric(12,2)"), QStringLiteral("yes"), QString()},
        };
    } else {
        rows = {
            {QStringLiteral("id"), QStringLiteral("bigint"), QStringLiteral("no"), QStringLiteral("PK")},
            {QStringLiteral("user_id"), QStringLiteral("bigint"), QStringLiteral("no"), QStringLiteral("FK")},
            {QStringLiteral("total"), QStringLiteral("numeric(12,2)"), QStringLiteral("no"), QString()},
            {QStringLiteral("state"), QStringLiteral("text"), QStringLiteral("no"), QString()},
            {QStringLiteral("created_at"), QStringLiteral("timestamptz"), QStringLiteral("no"), QString()},
        };
    }

    auto* structure = createDataGrid(root, headers, rows);
    structure->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    structure->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(structure, 1);
    return root;
}

} // namespace

MainWindow::MainWindow(bool restorePersistentState, QWidget* parent)
    : KDDockWidgets::QtWidgets::MainWindow(
          QStringLiteral("odw.main-window"),
          KDDockWidgets::MainWindowOption_HasCentralGroup,
          parent),
      persistTimer_(new QTimer(this)),
      persistenceEnabled_(restorePersistentState) {
    setWindowTitle(QStringLiteral("ODW — Open Database Workspace"));
    resize(1440, 900);

    setAffinities({QStringLiteral("odw.tools"), QStringLiteral("odw.documents")});
    mainWindow()->setDocumentAffinity(QStringLiteral("odw.documents"));

    buildWorkspace();
    wireDurableState();

    if (persistenceEnabled_) {
        restoreWorkspace();
    }
}

MainWindow::DockWidget* MainWindow::createToolDock(const QString& title,
                                                   const QString& uniqueName,
                                                   QWidget* content) {
    auto* dock = new DockWidget(uniqueName);
    dock->setTitle(title);
    dock->setWidget(content);
    dock->setAffinities({QStringLiteral("odw.tools")});
    wireDockPersistence(dock);
    return dock;
}

MainWindow::DockWidget* MainWindow::createDocumentDock(const QString& title,
                                                       const QString& uniqueName,
                                                       QWidget* content) {
    auto* dock = new DockWidget(uniqueName);
    dock->setTitle(title);
    dock->setWidget(content);
    dock->setAffinities({QStringLiteral("odw.documents")});
    wireDockPersistence(dock);
    return dock;
}

void MainWindow::buildWorkspace() {
    auto* toolbar = addToolBar(QStringLiteral("Workspace"));
    toolbar->setObjectName(QStringLiteral("odwMainToolbar"));
    toolbar->setMovable(false);
    toolbar->setFloatable(false);

    auto* connectionAction = toolbar->addAction(QStringLiteral("＋ Connection"));
    auto* queryAction = toolbar->addAction(QStringLiteral("＋ Query"));
    auto* automationAction = toolbar->addAction(QStringLiteral("Automation"));
    toolbar->addSeparator();
    runAction_ = toolbar->addAction(QStringLiteral("▶ Run"));
    stopAction_ = toolbar->addAction(QStringLiteral("■ Stop"));
    toolbar->addSeparator();
    auto* appearanceAction = toolbar->addAction(QStringLiteral("Appearance"));

    runAction_->setEnabled(false);
    stopAction_->setEnabled(false);

    buildConnections();

    inspector_ = new views::AdaptiveInspector();
    inspectorDock_ = createToolDock(
        QStringLiteral("Inspector"),
        QStringLiteral("odw.tool.inspector"),
        inspector_);

    mainWindow()->addDockWidgetToSide(
        connectionsDock_->asDockWidgetController(),
        KDDockWidgets::Location_OnLeft);
    mainWindow()->addDockWidgetToSide(
        inspectorDock_->asDockWidgetController(),
        KDDockWidgets::Location_OnRight);

    registerDocuments();

    connect(connectionAction, &QAction::triggered, this, [this] {
        statusBar()->showMessage(
            QStringLiteral("Connection editor comes next; Git is not a permanent workspace mode."),
            3500);
    });

    connect(queryAction, &QAction::triggered, this, [this] {
        openDocument(QStringLiteral("query"));
    });

    connect(automationAction, &QAction::triggered, this, [this] {
        openDocument(QStringLiteral("automation"));
    });

    connect(runAction_, &QAction::triggered, this, [this] {
        if (queryDock_ == nullptr || !queryDock_->isOpen()) {
            return;
        }
        queryDock_->raise();
        statusBar()->showMessage(QStringLiteral("Mock query completed · 12 rows · 18 ms"), 3500);
    });

    connect(appearanceAction, &QAction::triggered, this, [this] {
        statusBar()->showMessage(
            QStringLiteral("Appearance remains a dedicated tool, not a workspace document."),
            3000);
    });

    statusBar()->showMessage(
        QStringLiteral("Double-click a table or view. Every document tab can be moved or detached."));
}

void MainWindow::buildConnections() {
    connectionsTree_ = new QTreeWidget();
    connectionsTree_->setHeaderHidden(true);
    connectionsTree_->setIndentation(14);
    connectionsTree_->setUniformRowHeights(true);
    connectionsTree_->setMinimumWidth(150);

    auto* connection = new QTreeWidgetItem(
        connectionsTree_, {QStringLiteral("PostgreSQL · local")});
    auto* schemas = new QTreeWidgetItem(connection, {QStringLiteral("Schemas")});
    auto* publicSchema = new QTreeWidgetItem(schemas, {QStringLiteral("public")});
    auto* tables = new QTreeWidgetItem(publicSchema, {QStringLiteral("Tables")});

    auto* users = new QTreeWidgetItem(tables, {QStringLiteral("users")});
    users->setData(0, kObjectTypeRole, QStringLiteral("table"));
    users->setData(0, kObjectNameRole, QStringLiteral("users"));

    auto* orders = new QTreeWidgetItem(tables, {QStringLiteral("orders")});
    orders->setData(0, kObjectTypeRole, QStringLiteral("table"));
    orders->setData(0, kObjectNameRole, QStringLiteral("orders"));

    auto* views = new QTreeWidgetItem(publicSchema, {QStringLiteral("Views")});
    auto* recentOrders = new QTreeWidgetItem(views, {QStringLiteral("recent_orders")});
    recentOrders->setData(0, kObjectTypeRole, QStringLiteral("view"));
    recentOrders->setData(0, kObjectNameRole, QStringLiteral("recent_orders"));

    new QTreeWidgetItem(publicSchema, {QStringLiteral("Functions")});

    connection->setExpanded(true);
    schemas->setExpanded(true);
    publicSchema->setExpanded(true);
    tables->setExpanded(true);
    views->setExpanded(true);

    connect(connectionsTree_, &QTreeWidget::itemDoubleClicked, this,
            [this](QTreeWidgetItem* item, int) {
                if (item == nullptr) {
                    return;
                }
                const QString objectName = item->data(0, kObjectNameRole).toString();
                if (!objectName.isEmpty()) {
                    openDocument(QStringLiteral("object:%1").arg(objectName));
                }
            });

    connect(connectionsTree_, &QTreeWidget::itemSelectionChanged, this, [this] {
        const auto selected = connectionsTree_->selectedItems();
        if (selected.isEmpty()) {
            return;
        }
        const auto* item = selected.constFirst();
        const QString type = item->data(0, kObjectTypeRole).toString();
        const QString name = item->data(0, kObjectNameRole).toString();
        if (name.isEmpty()) {
            return;
        }

        const QString rows = name == QStringLiteral("users")
                                 ? QStringLiteral("12 mock rows")
                                 : name == QStringLiteral("orders")
                                       ? QStringLiteral("10 mock rows")
                                       : QStringLiteral("6 mock rows");
        const QString primaryKey = type == QStringLiteral("view")
                                       ? QStringLiteral("—")
                                       : QStringLiteral("id");
        inspectObject(type, rows, primaryKey, QStringLiteral("PostgreSQL · local"));
    });

    connectionsDock_ = createToolDock(
        QStringLiteral("Connections"),
        QStringLiteral("odw.tool.connections"),
        connectionsTree_);
}

void MainWindow::registerDocuments() {
    auto* usersDock = createDocumentDock(
        QStringLiteral("users"),
        QStringLiteral("odw.document.table.users"),
        makeTableDocument(QStringLiteral("users"), QStringLiteral("table")));
    documents_.insert(QStringLiteral("object:users"), usersDock);

    auto* ordersDock = createDocumentDock(
        QStringLiteral("orders"),
        QStringLiteral("odw.document.table.orders"),
        makeTableDocument(QStringLiteral("orders"), QStringLiteral("table")));
    documents_.insert(QStringLiteral("object:orders"), ordersDock);

    auto* recentOrdersDock = createDocumentDock(
        QStringLiteral("recent_orders"),
        QStringLiteral("odw.document.view.recent-orders"),
        makeTableDocument(QStringLiteral("recent_orders"), QStringLiteral("view")));
    documents_.insert(QStringLiteral("object:recent_orders"), recentOrdersDock);

    queryDock_ = createDocumentDock(
        QStringLiteral("Query"),
        QStringLiteral("odw.document.query.1"),
        makeQueryDocument());
    documents_.insert(QStringLiteral("query"), queryDock_);

    automationDock_ = createDocumentDock(
        QStringLiteral("Automation"),
        QStringLiteral("odw.document.automation"),
        makeAutomationDocument());
    documents_.insert(QStringLiteral("automation"), automationDock_);

    addDockWidgetAsTab(usersDock);
    usersDock->addDockWidgetAsTab(ordersDock);
    usersDock->addDockWidgetAsTab(recentOrdersDock);
    usersDock->addDockWidgetAsTab(queryDock_);
    usersDock->addDockWidgetAsTab(automationDock_);

    ordersDock->forceClose();
    recentOrdersDock->forceClose();
    queryDock_->forceClose();
    automationDock_->forceClose();
    usersDock->raise();

    connect(queryDock_, &DockWidget::isCurrentTabChanged, this, [this](bool current) {
        runAction_->setEnabled(current && queryDock_->isOpen());
    });
    connect(queryDock_, &DockWidget::isOpenChanged, this, [this](bool open) {
        if (!open) {
            runAction_->setEnabled(false);
        }
    });

    inspectObject(
        QStringLiteral("table"),
        QStringLiteral("12 mock rows"),
        QStringLiteral("id"),
        QStringLiteral("PostgreSQL · local"));
}

void MainWindow::openDocument(const QString& id) {
    DockWidget* dock = documents_.value(id, nullptr);
    if (dock == nullptr) {
        return;
    }

    if (!dock->isOpen()) {
        dock->open();
    }
    dock->raise();

    if (id.startsWith(QStringLiteral("object:"))) {
        const QString name = id.mid(QStringLiteral("object:").size());
        const QString type = name == QStringLiteral("recent_orders")
                                 ? QStringLiteral("view")
                                 : QStringLiteral("table");
        const QString rows = name == QStringLiteral("users")
                                 ? QStringLiteral("12 mock rows")
                                 : name == QStringLiteral("orders")
                                       ? QStringLiteral("10 mock rows")
                                       : QStringLiteral("6 mock rows");
        inspectObject(
            type,
            rows,
            type == QStringLiteral("view") ? QStringLiteral("—") : QStringLiteral("id"),
            QStringLiteral("PostgreSQL · local"));
    }

    schedulePersist();
}

void MainWindow::inspectObject(const QString& type,
                               const QString& rows,
                               const QString& primaryKey,
                               const QString& source) {
    if (inspector_ != nullptr) {
        inspector_->setObjectDetails(type, rows, primaryKey, source);
    }
}

QWidget* MainWindow::makeTableDocument(const QString& objectName,
                                       const QString& objectType) {
    auto* tabs = new QTabWidget();
    tabs->setDocumentMode(true);

    QStringList headers;
    QList<QStringList> rows;
    if (objectName == QStringLiteral("users")) {
        headers = {
            QStringLiteral("id"),
            QStringLiteral("name"),
            QStringLiteral("email"),
            QStringLiteral("status"),
            QStringLiteral("created_at"),
            QStringLiteral("balance"),
        };
        rows = usersRows();
    } else {
        headers = {
            QStringLiteral("id"),
            QStringLiteral("user_id"),
            QStringLiteral("total"),
            QStringLiteral("state"),
            QStringLiteral("created_at"),
        };
        rows = objectName == QStringLiteral("orders") ? ordersRows() : recentOrdersRows();
    }

    auto* grid = createDataGrid(tabs, headers, rows);
    if (headers.size() >= 2) {
        grid->setColumnWidth(0, 90);
        grid->setColumnWidth(1, 140);
    }
    if (objectName == QStringLiteral("users")) {
        grid->setColumnWidth(2, 220);
        grid->setColumnWidth(4, 180);
    }

    tabs->addTab(
        wrapGrid(QStringLiteral("%1 rows · mock data").arg(rows.size()), grid, tabs),
        QStringLiteral("Data"));
    tabs->addTab(
        createStructureView(objectName, objectType, tabs),
        QStringLiteral("Structure"));
    return tabs;
}

QWidget* MainWindow::makeQueryDocument() {
    auto* root = new QWidget();
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    auto* context = new QLabel(QStringLiteral("PostgreSQL · local   /   public"), root);
    context->setObjectName(QStringLiteral("odwSecondaryText"));
    layout->addWidget(context);

    auto* splitter = new QSplitter(Qt::Vertical, root);
    queryEditor_ = new QTextEdit(splitter);
    queryEditor_->setAcceptRichText(false);
    queryEditor_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    queryEditor_->setPlaceholderText(QStringLiteral("Write SQL…"));
    queryEditor_->setPlainText(
        QStringLiteral("SELECT id, name, email, status, created_at, balance\n"
                       "FROM users\n"
                       "ORDER BY created_at DESC;"));

    auto* resultGrid = createDataGrid(
        splitter,
        {
            QStringLiteral("id"),
            QStringLiteral("name"),
            QStringLiteral("email"),
            QStringLiteral("status"),
            QStringLiteral("created_at"),
            QStringLiteral("balance"),
        },
        usersRows());
    resultGrid->setColumnWidth(0, 80);
    resultGrid->setColumnWidth(1, 130);
    resultGrid->setColumnWidth(2, 220);
    resultGrid->setColumnWidth(4, 180);

    splitter->addWidget(queryEditor_);
    splitter->addWidget(resultGrid);
    splitter->setSizes({330, 420});
    layout->addWidget(splitter, 1);
    return root;
}

QWidget* MainWindow::makeAutomationDocument() {
    auto* root = new QWidget();
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    auto* summary = new QLabel(
        QStringLiteral("customers  →  Python · normalize.py  →  Validate  →  result.csv"),
        root);
    summary->setWordWrap(true);
    summary->setObjectName(QStringLiteral("odwPipelineSummary"));
    layout->addWidget(summary);

    auto* steps = new QTreeWidget(root);
    steps->setColumnCount(2);
    steps->setHeaderLabels({QStringLiteral("Step"), QStringLiteral("State")});
    steps->setRootIsDecorated(false);
    steps->setUniformRowHeights(true);
    steps->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    steps->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    const QList<QPair<QString, QString>> rows{
        {QStringLiteral("Input · customers"), QStringLiteral("ready")},
        {QStringLiteral("Python · normalize.py"), QStringLiteral("completed")},
        {QStringLiteral("Validation"), QStringLiteral("completed")},
        {QStringLiteral("Export · result.csv"), QStringLiteral("completed")},
    };
    for (const auto& [name, state] : rows) {
        new QTreeWidgetItem(steps, {name, state});
    }

    layout->addWidget(steps, 1);
    return root;
}

void MainWindow::applyPresentationScenario(const QString& scenario) {
    if (scenario == QStringLiteral("narrow-inspector")) {
        resize(1100, 720);
        if (inspector_ != nullptr) {
            inspector_->setMaximumWidth(135);
        }
        if (inspectorDock_ != nullptr) {
            inspectorDock_->raise();
        }
    } else if (scenario == QStringLiteral("automation")) {
        openDocument(QStringLiteral("automation"));
    } else if (scenario == QStringLiteral("query")) {
        openDocument(QStringLiteral("query"));
    } else {
        openDocument(QStringLiteral("object:users"));
    }
}

void MainWindow::wireDockPersistence(DockWidget* dock) {
    connect(dock, &DockWidget::isFloatingChanged, this, [this](bool) { schedulePersist(); });
    connect(dock, &DockWidget::isOpenChanged, this, [this](bool) { schedulePersist(); });
    connect(dock, &DockWidget::isCurrentTabChanged, this, [this](bool) { schedulePersist(); });
}

void MainWindow::wireDurableState() {
    persistTimer_->setSingleShot(true);
    persistTimer_->setInterval(100);
    connect(persistTimer_, &QTimer::timeout, this, &MainWindow::persistWorkspace);

    if (queryEditor_ != nullptr) {
        connect(queryEditor_, &QTextEdit::textChanged, this, [this] { schedulePersist(); });
    }
}

bool MainWindow::event(QEvent* event) {
    const bool handled = KDDockWidgets::QtWidgets::MainWindow::event(event);

    if (persistenceEnabled_ && persistTimer_ != nullptr &&
        (event->type() == QEvent::Move ||
         event->type() == QEvent::Resize ||
         event->type() == QEvent::WindowStateChange)) {
        schedulePersist();
    }

    return handled;
}

void MainWindow::schedulePersist() {
    if (persistenceEnabled_ && persistTimer_ != nullptr) {
        persistTimer_->start();
    }
}

QString MainWindow::workspaceStatePath() const {
    const QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir directory(root);
    if (!directory.exists()) {
        directory.mkpath(QStringLiteral("."));
    }
    return directory.filePath(QStringLiteral("workspace-v0.json"));
}

void MainWindow::persistWorkspace() {
    if (!persistenceEnabled_) {
        return;
    }

    KDDockWidgets::LayoutSaver saver;
    const QByteArray layoutState = saver.serializeLayout();

    QJsonObject root;
    root.insert(QStringLiteral("schemaVersion"), 2);
    root.insert(QStringLiteral("dockLayout"),
                QString::fromLatin1(layoutState.toBase64()));
    root.insert(QStringLiteral("queryDraft"),
                queryEditor_ != nullptr ? queryEditor_->toPlainText() : QString());

    QSaveFile file(workspaceStatePath());
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.commit();
}

void MainWindow::restoreWorkspace() {
    QFile file(workspaceStatePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return;
    }

    const QJsonObject root = document.object();
    if (root.value(QStringLiteral("schemaVersion")).toInt() != 2) {
        return;
    }

    if (queryEditor_ != nullptr) {
        const QSignalBlocker blocker(queryEditor_);
        queryEditor_->setPlainText(root.value(QStringLiteral("queryDraft")).toString());
    }

    const QByteArray layoutState = QByteArray::fromBase64(
        root.value(QStringLiteral("dockLayout")).toString().toLatin1());
    if (!layoutState.isEmpty()) {
        KDDockWidgets::LayoutSaver saver;
        saver.restoreLayout(layoutState);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (persistenceEnabled_) {
        persistWorkspace();
    }
    KDDockWidgets::QtWidgets::MainWindow::closeEvent(event);
}

} // namespace odw::ui
