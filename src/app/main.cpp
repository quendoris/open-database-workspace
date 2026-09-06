#include "../ui/MainWindow.hpp"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("ODW"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("odw.local"));
    QCoreApplication::setApplicationName(QStringLiteral("Open Database Workspace"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.0.0"));

    odw::ui::MainWindow window;
    window.show();

    return app.exec();
}
