#include "mainwindow.h"

#include "glm/ext/matrix_float4x4.hpp"

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <OpenGL/gl.h>

#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <glm/glm.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "src/config/config.h"

GLMainWindow::GLMainWindow() : QOpenGLWindow(), QOpenGLFunctions(), _updateTimer(this), _stopWatch() {
    // Set to the preconfigured size.
    setWidth(Config::windowWidth);
    setHeight(Config::windowHeight);

    // Set the title.
    setTitle("Floppy Fish");

    setSurfaceType(QWindow::OpenGLSurface);

    // Update the scene periodically.
    QObject::connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(animateGL()));
    _updateTimer.start(18);
    _stopWatch.start();

    // Create the scrolling background.
    _background = Background("res/background.png");
}

void GLMainWindow::show() {
    QOpenGLWindow::show();

    // Check for a valid context.
    if (!isValid() || !context()->isValid() || context()->format().majorVersion() != 4) {
        QMessageBox(QMessageBox::Critical, "Error", "Cannot get a valid OpenGL context.", QMessageBox::Ok).exec();
        close();
    }
}

void GLMainWindow::initializeGL() {
    // Initialize OpenGL funtions, replacing glewInit().
    initializeOpenGLFunctions();

    // Make sure the context is current.
    makeCurrent();

    // Enable multisampling for AA;
    glEnable(GL_MULTISAMPLE);

    // Enable alpha blending and selecting blend function.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    _background.init();
}

void GLMainWindow::resizeGL(int width, int height) {
    // Update the viewport.
    glViewport(0, 0, width, height);

    // Store the resolution in the config.
    Config::windowWidth = width;
    Config::windowHeight = height;
}

void GLMainWindow::paintGL() {
    // Set up view.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Set a background color.
    glClearColor(0, 0, 0, 0.5f);

    // Calculate projection matrix from current resolution, this allows for resizing the window without distortion.
    const float fovy = glm::radians(60.0f);
    const float aspect = float(Config::windowWidth) / float(Config::windowHeight);
    glm::mat4 projectionMatrix = glm::perspective(fovy, aspect, 0.1f, 100.0f);

    // Draw filled polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    _background.draw(projectionMatrix);
}

void GLMainWindow::animateGL() {
    // Make the context current in case there are glFunctions called.
    makeCurrent();

    // Get the time delta and restart the stopwatch.
    float elapsedTimeMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    _stopWatch.restart();

    // Calculate current model view matrix.
    glm::mat4 modelViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Increment the animation looper if the animation is running.
    const float incrementedLooper = Config::animationLooper + 0.0003f * Config::animationSpeed;
    Config::animationLooper = incrementedLooper > 1.0f ? 0.0f : incrementedLooper;

    _background.update(elapsedTimeMs, modelViewMatrix);

    // Update the widget.
    update();
}

void GLMainWindow::keyPressEvent(QKeyEvent *event) {
    const bool isFullscreen = visibility() == QWindow::FullScreen;
    // Pressing SPACE will make the fish flop or flop the fish idk.
    if (event->key() == Qt::Key_Space) {
        // TODO: make the fish flop!
        qDebug() << "FLOPPY FISH";
    }
    // Pressing F in fullscreen mode will reset the window.
    else if (event->key() == Qt::Key_F && isFullscreen) {
        showNormal();
    }
    // Pressing F in normal mode will go to fullscreen mode.
    else if (event->key() == Qt::Key_F && !isFullscreen) {
        showFullScreen();
    }
    // Pressing ESCAPE or Q will quit everything.
    else if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Q) {
        close();
    }
}
