#pragma once

#include <QMainWindow>

class QDockWidget;
class QTextEdit;
class QTimer;
class QWidget;

namespace odw::ui {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    static constexpr int kLayoutStateVersion = 1;

    QDockWidget* createDock(const QString& title, const QString& objectName, QWidget* content);
    void buildDemoWorkspace();
    void wireDurableState();
    void schedulePersist();
    void persistWorkspace();
    void restoreWorkspace();
    [[nodiscard]] QString workspaceStatePath() const;

    QTextEdit* queryEditor_ = nullptr;
    QTimer* persistTimer_ = nullptr;
};

} // namespace odw::ui
