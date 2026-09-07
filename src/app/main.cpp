#include "../ui/MainWindow.hpp"
#include "../ui/theme/VisualTheme.hpp"

#include <kddockwidgets/KDDockWidgets.h>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QTimer>

namespace {

QSize parseSize(const QString& text) {
    const QStringList parts = text.toLower().split(QLatin1Char('x'));
    if (parts.size() != 2) {
        return {};
    }

    bool widthOk = false;
    bool heightOk = false;
    const int width = parts.at(0).toInt(&widthOk);
    const int height = parts.at(1).toInt(&heightOk);
    if (!widthOk || !heightOk || width < 320 || height < 240) {
        return {};
    }
    return QSize(width, height);
}

} // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("ODW"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("odw.local"));
    QCoreApplication::setApplicationName(QStringLiteral("Open Database Workspace"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.0.0"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Open Database Workspace playground"));
    parser.addHelpOption();
    parser.addVersionOption();

    const QCommandLineOption screenshotOption(
        QStringList{QStringLiteral("s"), QStringLiteral("screenshot")},
        QStringLiteral("Capture the main ODW window to a PNG file and exit."),
        QStringLiteral("path"));
    const QCommandLineOption sizeOption(
        QStringList{QStringLiteral("size")},
        QStringLiteral("Set the initial window size as WIDTHxHEIGHT."),
        QStringLiteral("WIDTHxHEIGHT"));
    const QCommandLineOption scenarioOption(
        QStringList{QStringLiteral("scenario")},
        QStringLiteral("Apply a deterministic presentation scenario before capture."),
        QStringLiteral("name"),
        QStringLiteral("default"));
    const QCommandLineOption freshWorkspaceOption(
        QStringList{QStringLiteral("fresh-workspace")},
        QStringLiteral("Do not restore or persist the local workspace state."));

    parser.addOption(screenshotOption);
    parser.addOption(sizeOption);
    parser.addOption(scenarioOption);
    parser.addOption(freshWorkspaceOption);
    parser.process(app);

    KDDockWidgets::initFrontend(KDDockWidgets::FrontendType::QtWidgets);
    odw::ui::theme::apply(app, odw::ui::theme::defaultDarkCyan());

    const bool screenshotMode = parser.isSet(screenshotOption);
    const bool restorePersistentState = !screenshotMode && !parser.isSet(freshWorkspaceOption);

    odw::ui::MainWindow window(restorePersistentState);
    const QSize requestedSize = parseSize(parser.value(sizeOption));
    if (requestedSize.isValid()) {
        window.resize(requestedSize);
    }

    window.show();
    window.applyPresentationScenario(parser.value(scenarioOption));

    if (screenshotMode) {
        const QString outputPath = QFileInfo(parser.value(screenshotOption)).absoluteFilePath();
        QDir().mkpath(QFileInfo(outputPath).absolutePath());

        QTimer::singleShot(450, &app, [&app, &window, outputPath] {
            const bool saved = window.grab().save(outputPath, "PNG");
            QCoreApplication::exit(saved ? 0 : 3);
        });
    }

    return app.exec();
}
