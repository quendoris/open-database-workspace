#pragma once

#include <kddockwidgets/MainWindow.h>

#include <QHash>
#include <QString>

class QAction;
class QCloseEvent;
class QEvent;
class QTextEdit;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;

namespace KDDockWidgets::QtWidgets {
class DockWidget;
}

namespace odw::ui::views {
class AdaptiveInspector;
}

namespace odw::ui {

class MainWindow final : public KDDockWidgets::QtWidgets::MainWindow {
    Q_OBJECT

public:
    explicit MainWindow(bool restorePersistentState = true, QWidget* parent = nullptr);

    void applyPresentationScenario(const QString& scenario);

protected:
    bool event(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    using DockWidget = KDDockWidgets::QtWidgets::DockWidget;

    DockWidget* createToolDock(const QString& title,
                               const QString& uniqueName,
                               QWidget* content);
    DockWidget* createDocumentDock(const QString& title,
                                   const QString& uniqueName,
                                   QWidget* content);

    void buildWorkspace();
    void buildConnections();
    void registerDocuments();
    void openDocument(const QString& id);
    void inspectObject(const QString& type,
                       const QString& rows,
                       const QString& primaryKey,
                       const QString& source);

    QWidget* makeTableDocument(const QString& objectName, const QString& objectType);
    QWidget* makeQueryDocument();
    QWidget* makeAutomationDocument();

    void wireDockPersistence(DockWidget* dock);
    void wireDurableState();
    void schedulePersist();
    void persistWorkspace();
    void restoreWorkspace();
    [[nodiscard]] QString workspaceStatePath() const;

    QTreeWidget* connectionsTree_ = nullptr;
    views::AdaptiveInspector* inspector_ = nullptr;
    QTextEdit* queryEditor_ = nullptr;
    QTimer* persistTimer_ = nullptr;

    QAction* runAction_ = nullptr;
    QAction* stopAction_ = nullptr;

    DockWidget* connectionsDock_ = nullptr;
    DockWidget* inspectorDock_ = nullptr;
    DockWidget* queryDock_ = nullptr;
    DockWidget* automationDock_ = nullptr;

    QHash<QString, DockWidget*> documents_;
    bool persistenceEnabled_ = true;
};

} // namespace odw::ui
