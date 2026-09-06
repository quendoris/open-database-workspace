#include "MainWindow.hpp"

#include <QAbstractItemView>
#include <QCloseEvent>
#include <QDir>
#include <QDockWidget>
#include <QEvent>
#include <QFile>
#include <QFontDatabase>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace odw::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      persistTimer_(new QTimer(this)) {
    setWindowTitle(QStringLiteral("ODW — Open Database Workspace"));
    resize(1440, 900);

    setDockNestingEnabled(true);
    setDockOptions(QMainWindow::AnimatedDocks |
                   QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks |
                   QMainWindow::GroupedDragging);

    buildDemoWorkspace();
    wireDurableState();
    restoreWorkspace();
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

void MainWindow::buildDemoWorkspace() {
    auto* canvas = new QWidget(this);
    canvas->setObjectName(QStringLiteral("odwCanvas"));

    auto* canvasLayout = new QVBoxLayout(canvas);
    canvasLayout->setContentsMargins(48, 48, 48, 48);
    canvasLayout->setSpacing(8);
    canvasLayout->addStretch(1);

    auto* title = new QLabel(QStringLiteral("Open Database Workspace"), canvas);
    title->setObjectName(QStringLiteral("odwEmptyTitle"));
    title->setAlignment(Qt::AlignCenter);

    auto* subtitle = new QLabel(
        QStringLiteral("A durable, composable workspace for your data.\n"
                       "Move, tab, split or detach the surrounding views."),
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
    auto* connection = new QTreeWidgetItem(databases, {QStringLiteral("Local PostgreSQL")});
    auto* schemas = new QTreeWidgetItem(connection, {QStringLiteral("Schemas")});
    new QTreeWidgetItem(schemas, {QStringLiteral("public")});
    new QTreeWidgetItem(connection, {QStringLiteral("Tables")});
    connection->setExpanded(true);
    schemas->setExpanded(true);

    queryEditor_ = new QTextEdit(this);
    queryEditor_->setAcceptRichText(false);
    queryEditor_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    queryEditor_->setPlaceholderText(QStringLiteral("Write SQL…"));
    queryEditor_->setPlainText(QStringLiteral("SELECT *\nFROM example;"));

    auto* results = new QTableWidget(8, 4, this);
    results->setAlternatingRowColors(true);
    results->setSelectionMode(QAbstractItemView::ExtendedSelection);
    results->setSelectionBehavior(QAbstractItemView::SelectItems);
    results->verticalHeader()->setVisible(false);
    results->verticalHeader()->setDefaultSectionSize(30);
    results->horizontalHeader()->setStretchLastSection(true);
    results->setHorizontalHeaderLabels({
        QStringLiteral("id"),
        QStringLiteral("name"),
        QStringLiteral("status"),
        QStringLiteral("updated_at")
    });
    for (int row = 0; row < results->rowCount(); ++row) {
        results->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        results->setItem(row, 1, new QTableWidgetItem(QStringLiteral("sample_%1").arg(row + 1)));
        results->setItem(row, 2, new QTableWidgetItem(row % 2 == 0 ? QStringLiteral("ready")
                                                                    : QStringLiteral("pending")));
        results->setItem(row, 3, new QTableWidgetItem(QStringLiteral("prototype")));
    }

    auto* inspector = new QListWidget(this);
    inspector->setUniformItemSizes(true);
    inspector->addItems({
        QStringLiteral("Type    table"),
        QStringLiteral("Rows    unknown"),
        QStringLiteral("PK      id"),
        QStringLiteral("Source  prototype connector")
    });

    auto* databaseDock = createDock(QStringLiteral("Databases"),
                                    QStringLiteral("odw.view.databases"),
                                    databases);
    auto* queryDock = createDock(QStringLiteral("Query"),
                                 QStringLiteral("odw.view.query"),
                                 queryEditor_);
    auto* resultsDock = createDock(QStringLiteral("Results"),
                                   QStringLiteral("odw.view.results"),
                                   results);
    auto* inspectorDock = createDock(QStringLiteral("Inspector"),
                                     QStringLiteral("odw.view.inspector"),
                                     inspector);

    addDockWidget(Qt::LeftDockWidgetArea, databaseDock);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);
    addDockWidget(Qt::BottomDockWidgetArea, resultsDock);
    addDockWidget(Qt::TopDockWidgetArea, queryDock);

    splitDockWidget(databaseDock, queryDock, Qt::Horizontal);
    splitDockWidget(queryDock, inspectorDock, Qt::Horizontal);
    splitDockWidget(queryDock, resultsDock, Qt::Vertical);

    resizeDocks({databaseDock, queryDock, inspectorDock}, {260, 760, 280}, Qt::Horizontal);
    resizeDocks({queryDock, resultsDock}, {470, 300}, Qt::Vertical);
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

    if (persistTimer_ != nullptr &&
        (event->type() == QEvent::Move ||
         event->type() == QEvent::Resize ||
         event->type() == QEvent::WindowStateChange)) {
        schedulePersist();
    }

    return handled;
}

void MainWindow::schedulePersist() {
    if (persistTimer_ != nullptr) {
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
    persistWorkspace();
    QMainWindow::closeEvent(event);
}

} // namespace odw::ui
