#include <QSurfaceFormat>
#include <QtWidgets/QApplication>

#include "gui/mainwindow.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Seed the random number generator with the current time.
    std::srand(std::time(nullptr));

    // Set gl format.
    QSurfaceFormat glFormat;
    glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    glFormat.setSwapInterval(::getenv("COREGL_FPS") ? 0 : 1);
    glFormat.setVersion(4, 1);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    glFormat.setSamples(8);
    glFormat.setColorSpace(QColorSpace::NamedColorSpace::SRgb);
    glFormat.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(glFormat);

    // Load the main window.
    GLMainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
