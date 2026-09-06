#pragma once

#include <QMainWindow>

class QCloseEvent;
class QDockWidget;
class QEvent;
class QString;
class QTextEdit;
class QTimer;
class QWidget;

namespace odw::ui {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(bool restorePersistentState = true, QWidget* parent = nullptr);

    void applyPresentationScenario(const QString& scenario);

protected:
    bool event(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    static constexpr int kLayoutStateVersion = 1;

    QDockWidget* createDock(const QString& title, const QString& objectName, QWidget* content);
    void buildPlaygroundWorkspace();
    void wireDurableState();
    void schedulePersist();
    void persistWorkspace();
    void restoreWorkspace();
    [[nodiscard]] QString workspaceStatePath() const;

    QTextEdit* queryEditor_ = nullptr;
    QTimer* persistTimer_ = nullptr;
    QDockWidget* resultsDock_ = nullptr;
    QDockWidget* inspectorDock_ = nullptr;
    QDockWidget* automationDock_ = nullptr;
    QDockWidget* vcsDock_ = nullptr;
    QDockWidget* logsDock_ = nullptr;
    bool persistenceEnabled_ = true;
};

} // namespace odw::ui
