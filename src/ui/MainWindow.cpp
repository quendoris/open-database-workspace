#include "MainWindow.hpp"

#include "views/AdaptiveInspector.hpp"

#include <QAbstractItemView>
#include <QAction>
#include <QCloseEvent>
#include <QDir>
#include <QDockWidget>
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
#include <QPushButton>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace odw::ui {
namespace {

QWidget* makeAutomationView(QWidget* parent) {
    auto* root = new QWidget(parent);
    root->setObjectName(QStringLiteral("odwAutomationView"));
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
    steps->header()->setStretchLastSection(false);
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

    auto* footer = new QLabel(QStringLiteral("Last run  2.4 s   ·   isolated runner   ·   mock pipeline"), root);
    footer->setObjectName(QStringLiteral("odwSecondaryText"));
    layout->addWidget(footer);
    return root;
}

QWidget* makeVcsView(QWidget* parent) {
    auto* root = new QWidget(parent);
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    auto* tree = new QTreeWidget(root);
    tree->setColumnCount(2);
    tree->setHeaderLabels({QStringLiteral("Git"), QStringLiteral("Value")});
    tree->setRootIsDecorated(false);
    tree->setUniformRowHeights(true);
    tree->header()->setStretchLastSection(true);

    const QList<QPair<QString, QString>> rows{
        {QStringLiteral("Repository"), QStringLiteral("open-database-workspace")},
        {QStringLiteral("Branch"), QStringLiteral("design/playground-v0.1")},
        {QStringLiteral("Working tree"), QStringLiteral("clean after publish")},
        {QStringLiteral("Upstream"), QStringLiteral("origin")},
    };
    for (const auto& [name, value] : rows) {
        new QTreeWidgetItem(tree, {name, value});
    }

    layout->addWidget(tree, 1);

    auto* hint = new QLabel(
        QStringLiteral("ODW exposes useful repository context; deeper Git work can hand off to SmartGit."),
        root);
    hint->setWordWrap(true);
    hint->setObjectName(QStringLiteral("odwSecondaryText"));
    layout->addWidget(hint);
    return root;
}

QTextEdit* makeLogView(QWidget* parent) {
    auto* logs = new QTextEdit(parent);
    logs->setReadOnly(true);
    logs->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    logs->setPlainText(
        QStringLiteral("12:00:00  workspace  restored playground layout\n"
                       "12:00:01  connector  PostgreSQL mock capability manifest ready\n"
                       "12:00:02  query      mock execution completed · 12 rows\n"
                       "12:00:03  automation normalize.py completed · 2.4 s"));
    return logs;
}

} // namespace

MainWindow::MainWindow(bool restorePersistentState, QWidget* parent)
    : QMainWindow(parent),
      persistTimer_(new QTimer(this)),
      persistenceEnabled_(restorePersistentState) {
    setWindowTitle(QStringLiteral("ODW — Open Database Workspace"));
    resize(1440, 900);

    setDockNestingEnabled(true);
    setDockOptions(QMainWindow::AnimatedDocks |
                   QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks |
                   QMainWindow::GroupedDragging);

    buildPlaygroundWorkspace();
    wireDurableState();
    if (persistenceEnabled_) {
        restoreWorkspace();
    }
}

QDockWidget* MainWindow::createDock(const QString& title,
                                    const QString& objectName,
                                    QWidget* content) {
    auto* dock = new QDockWidget(title, this);
    dock->setObjectName(objectName);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFeatures(QDockWidget::DockWidgetClosable |
                      QDockWidget::DockWidgetMovable |
                      QDockWidget::DockWidgetFloatable);
    dock->setWidget(content);
    return dock;
}

void MainWindow::buildPlaygroundWorkspace() {
    auto* toolbar = addToolBar(QStringLiteral("Workspace"));
    toolbar->setObjectName(QStringLiteral("odwMainToolbar"));
    toolbar->setMovable(false);
    toolbar->setFloatable(false);

    auto* connectionAction = toolbar->addAction(QStringLiteral("＋ Connection"));
    auto* runAction = toolbar->addAction(QStringLiteral("▶ Run"));
    auto* stopAction = toolbar->addAction(QStringLiteral("■ Stop"));
    toolbar->addSeparator();
    auto* appearanceAction = toolbar->addAction(QStringLiteral("Appearance"));

    stopAction->setEnabled(false);
    connect(connectionAction, &QAction::triggered, this, [this] {
        statusBar()->showMessage(QStringLiteral("Connection editor is the next connector-facing slice."), 3000);
    });
    connect(appearanceAction, &QAction::triggered, this, [this] {
        statusBar()->showMessage(QStringLiteral("Appearance tool uses live semantic theme tokens."), 3000);
    });

    auto* canvas = new QWidget(this);
    canvas->setObjectName(QStringLiteral("odwCanvas"));
    auto* canvasLayout = new QVBoxLayout(canvas);
    canvasLayout->setContentsMargins(48, 48, 48, 48);
    canvasLayout->setSpacing(8);
    canvasLayout->addStretch(1);

    auto* title = new QLabel(QStringLiteral("ODW Playground"), canvas);
    title->setObjectName(QStringLiteral("odwEmptyTitle"));
    title->setAlignment(Qt::AlignCenter);
    auto* subtitle = new QLabel(
        QStringLiteral("A user-first workspace for data.\n"
                       "Dock, tab, detach and resize every surrounding tool."),
        canvas);
    subtitle->setObjectName(QStringLiteral("odwEmptySubtitle"));
    subtitle->setAlignment(Qt::AlignCenter);
    canvasLayout->addWidget(title);
    canvasLayout->addWidget(subtitle);
    canvasLayout->addStretch(1);
    setCentralWidget(canvas);

    auto* databases = new QTreeWidget(this);
    databases->setHeaderHidden(true);
    databases->setIndentation(14);
    databases->setUniformRowHeights(true);
    databases->setMinimumWidth(120);

    auto* connection = new QTreeWidgetItem(databases, {QStringLiteral("PostgreSQL · local")});
    auto* schemas = new QTreeWidgetItem(connection, {QStringLiteral("Schemas")});
    auto* publicSchema = new QTreeWidgetItem(schemas, {QStringLiteral("public")});
    auto* tables = new QTreeWidgetItem(publicSchema, {QStringLiteral("Tables")});
    new QTreeWidgetItem(tables, {QStringLiteral("users")});
    new QTreeWidgetItem(tables, {QStringLiteral("orders")});
    auto* views = new QTreeWidgetItem(publicSchema, {QStringLiteral("Views")});
    new QTreeWidgetItem(views, {QStringLiteral("recent_orders")});
    new QTreeWidgetItem(publicSchema, {QStringLiteral("Functions")});
    connection->setExpanded(true);
    schemas->setExpanded(true);
    publicSchema->setExpanded(true);
    tables->setExpanded(true);
    views->setExpanded(true);

    auto* queryRoot = new QWidget(this);
    auto* queryLayout = new QVBoxLayout(queryRoot);
    queryLayout->setContentsMargins(8, 8, 8, 8);
    queryLayout->setSpacing(6);
    auto* queryContext = new QLabel(QStringLiteral("PostgreSQL · local   /   public"), queryRoot);
    queryContext->setObjectName(QStringLiteral("odwSecondaryText"));
    queryLayout->addWidget(queryContext);

    queryEditor_ = new QTextEdit(queryRoot);
    queryEditor_->setAcceptRichText(false);
    queryEditor_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    queryEditor_->setPlaceholderText(QStringLiteral("Write SQL…"));
    queryEditor_->setPlainText(
        QStringLiteral("SELECT id, name, email, status, created_at, balance\n"
                       "FROM users\n"
                       "ORDER BY created_at DESC;"));
    queryLayout->addWidget(queryEditor_, 1);

    auto* resultsRoot = new QWidget(this);
    auto* resultsLayout = new QVBoxLayout(resultsRoot);
    resultsLayout->setContentsMargins(8, 8, 8, 8);
    resultsLayout->setSpacing(6);

    auto* filterRow = new QWidget(resultsRoot);
    auto* filterLayout = new QHBoxLayout(filterRow);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(8);
    auto* filter = new QLineEdit(filterRow);
    filter->setObjectName(QStringLiteral("odwGridFilter"));
    filter->setPlaceholderText(QStringLiteral("Filter visible rows…"));
    auto* rowCount = new QLabel(QStringLiteral("12 rows · mock result"), filterRow);
    rowCount->setObjectName(QStringLiteral("odwSecondaryText"));
    filterLayout->addWidget(filter, 1);
    filterLayout->addWidget(rowCount);
    resultsLayout->addWidget(filterRow);

    auto* results = new QTableWidget(12, 6, resultsRoot);
    results->setAlternatingRowColors(true);
    results->setSelectionMode(QAbstractItemView::ExtendedSelection);
    results->setSelectionBehavior(QAbstractItemView::SelectItems);
    results->verticalHeader()->setVisible(false);
    results->verticalHeader()->setDefaultSectionSize(30);
    results->horizontalHeader()->setStretchLastSection(true);
    results->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    results->setHorizontalHeaderLabels({
        QStringLiteral("id"),
        QStringLiteral("name"),
        QStringLiteral("email"),
        QStringLiteral("status"),
        QStringLiteral("created_at"),
        QStringLiteral("balance"),
    });

    for (int row = 0; row < results->rowCount(); ++row) {
        const int id = 101 + row;
        const QString name = QStringLiteral("user_%1").arg(id);
        const QString email = row == 4 ? QStringLiteral("NULL")
                                       : QStringLiteral("user%1@example.dev").arg(id);
        const QString status = row % 3 == 0 ? QStringLiteral("pending")
                                            : QStringLiteral("active");
        const QString date = QStringLiteral("2026-09-%1  1%2:%3")
                                 .arg(6 - (row % 6), 2, 10, QLatin1Char('0'))
                                 .arg(row % 10)
                                 .arg((row * 7) % 60, 2, 10, QLatin1Char('0'));
        const QString balance = row == 8 ? QStringLiteral("NULL")
                                         : QStringLiteral("%1.%2")
                                               .arg(120 + row * 17)
                                               .arg((row * 13) % 100, 2, 10, QLatin1Char('0'));

        const QStringList values{
            QString::number(id), name, email, status, date, balance,
        };
        for (int column = 0; column < values.size(); ++column) {
            auto* item = new QTableWidgetItem(values.at(column));
            if (values.at(column) == QStringLiteral("NULL")) {
                QFont italic = item->font();
                italic.setItalic(true);
                item->setFont(italic);
                item->setToolTip(QStringLiteral("SQL NULL"));
            }
            results->setItem(row, column, item);
        }
    }
    results->setColumnWidth(0, 72);
    results->setColumnWidth(1, 130);
    results->setColumnWidth(2, 220);
    results->setColumnWidth(3, 100);
    results->setColumnWidth(4, 180);
    resultsLayout->addWidget(results, 1);

    connect(filter, &QLineEdit::textChanged, results, [results](const QString& text) {
        for (int row = 0; row < results->rowCount(); ++row) {
            bool match = text.trimmed().isEmpty();
            for (int column = 0; !match && column < results->columnCount(); ++column) {
                const auto* item = results->item(row, column);
                match = item != nullptr && item->text().contains(text, Qt::CaseInsensitive);
            }
            results->setRowHidden(row, !match);
        }
    });

    auto* inspector = new views::AdaptiveInspector(this);

    auto* databaseDock = createDock(QStringLiteral("Connections"),
                                    QStringLiteral("odw.view.connections"),
                                    databases);
    auto* queryDock = createDock(QStringLiteral("Query"),
                                 QStringLiteral("odw.view.query"),
                                 queryRoot);
    resultsDock_ = createDock(QStringLiteral("Results"),
                              QStringLiteral("odw.view.results"),
                              resultsRoot);
    inspectorDock_ = createDock(QStringLiteral("Inspector"),
                                QStringLiteral("odw.view.inspector"),
                                inspector);
    automationDock_ = createDock(QStringLiteral("Automation"),
                                 QStringLiteral("odw.view.automation"),
                                 makeAutomationView(this));
    vcsDock_ = createDock(QStringLiteral("Version control"),
                          QStringLiteral("odw.view.vcs"),
                          makeVcsView(this));
    logsDock_ = createDock(QStringLiteral("Tasks / Logs"),
                           QStringLiteral("odw.view.logs"),
                           makeLogView(this));

    addDockWidget(Qt::LeftDockWidgetArea, databaseDock);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock_);
    addDockWidget(Qt::BottomDockWidgetArea, resultsDock_);
    addDockWidget(Qt::TopDockWidgetArea, queryDock);

    splitDockWidget(databaseDock, queryDock, Qt::Horizontal);
    splitDockWidget(queryDock, inspectorDock_, Qt::Horizontal);
    splitDockWidget(queryDock, resultsDock_, Qt::Vertical);

    addDockWidget(Qt::BottomDockWidgetArea, automationDock_);
    tabifyDockWidget(resultsDock_, automationDock_);
    addDockWidget(Qt::BottomDockWidgetArea, logsDock_);
    tabifyDockWidget(resultsDock_, logsDock_);

    addDockWidget(Qt::RightDockWidgetArea, vcsDock_);
    tabifyDockWidget(inspectorDock_, vcsDock_);

    resizeDocks({databaseDock, queryDock, inspectorDock_}, {250, 820, 280}, Qt::Horizontal);
    resizeDocks({queryDock, resultsDock_}, {480, 310}, Qt::Vertical);
    resultsDock_->raise();
    inspectorDock_->raise();

    connect(runAction, &QAction::triggered, this, [this] {
        resultsDock_->show();
        resultsDock_->raise();
        statusBar()->showMessage(QStringLiteral("Mock query completed · 12 rows · 18 ms"), 3000);
    });

    statusBar()->showMessage(QStringLiteral("Playground · mock data · no live database connection"));
}

void MainWindow::applyPresentationScenario(const QString& scenario) {
    if (scenario == QStringLiteral("narrow-inspector") && inspectorDock_ != nullptr) {
        inspectorDock_->show();
        inspectorDock_->raise();
        resizeDocks({inspectorDock_}, {125}, Qt::Horizontal);
    } else if (scenario == QStringLiteral("automation") && automationDock_ != nullptr) {
        automationDock_->show();
        automationDock_->raise();
    } else if (scenario == QStringLiteral("vcs") && vcsDock_ != nullptr) {
        vcsDock_->show();
        vcsDock_->raise();
    }
}

void MainWindow::wireDurableState() {
    persistTimer_->setSingleShot(true);
    persistTimer_->setInterval(75);
    connect(persistTimer_, &QTimer::timeout, this, &MainWindow::persistWorkspace);

    for (auto* dock : findChildren<QDockWidget*>()) {
        connect(dock, &QDockWidget::dockLocationChanged, this, [this] { schedulePersist(); });
        connect(dock, &QDockWidget::topLevelChanged, this, [this] { schedulePersist(); });
        connect(dock, &QDockWidget::visibilityChanged, this, [this] { schedulePersist(); });
    }

    connect(queryEditor_, &QTextEdit::textChanged, this, [this] { schedulePersist(); });
}

bool MainWindow::event(QEvent* event) {
    const bool handled = QMainWindow::event(event);

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

    QJsonObject root;
    root.insert(QStringLiteral("schemaVersion"), 1);
    root.insert(QStringLiteral("geometry"),
                QString::fromLatin1(saveGeometry().toBase64()));
    root.insert(QStringLiteral("qtMainWindowState"),
                QString::fromLatin1(saveState(kLayoutStateVersion).toBase64()));
    root.insert(QStringLiteral("queryDraft"), queryEditor_->toPlainText());

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
    if (root.value(QStringLiteral("schemaVersion")).toInt() != 1) {
        return;
    }

    const QByteArray geometry = QByteArray::fromBase64(
        root.value(QStringLiteral("geometry")).toString().toLatin1());
    const QByteArray windowState = QByteArray::fromBase64(
        root.value(QStringLiteral("qtMainWindowState")).toString().toLatin1());

    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    if (!windowState.isEmpty()) {
        restoreState(windowState, kLayoutStateVersion);
    }

    const QSignalBlocker blocker(queryEditor_);
    queryEditor_->setPlainText(root.value(QStringLiteral("queryDraft")).toString());
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (persistenceEnabled_) {
        persistWorkspace();
    }
    QMainWindow::closeEvent(event);
}

} // namespace odw::ui
