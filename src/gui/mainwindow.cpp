#include "mainwindow.h"
#include <cstddef>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "src/drawables/obstacles/obstacle.h"

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <OpenGL/gl.h>
#include <src/drawables/background.h>

#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <glm/glm.hpp>

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

    // Create all the drawables.
    _drawables = {
        // TODO: create the fish (character)
        // std::make_shared<Fish>(Fish("res/fish.png")),
        // TODO: create the fence (ground)
        // std::make_shared<Ground>(Ground("res/ground.png")),
        std::make_shared<Background>(Background("res/background.png"))
    };

    // Create the in the Config specified amount of obstacles and add it to the drawables.
    float offset = Config::obstacleDistance;
    for (std::size_t i = 0; i < Config::obstacleAmount; i++) {
        _drawables.push_back(std::make_shared<Obstacle>(Obstacle("res/background.png", i * offset)));
    }
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

    // Initialize all drawables.
    for (auto drawable : _drawables) {
        drawable->init();
    }
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

    // TODO: projection computation is not working right now?!
    const float fovy = glm::radians(60.0f);
    const float aspect = float(Config::windowWidth) / float(Config::windowHeight);
    glm::mat4 projectionMatrix = glm::perspective(fovy, aspect, 0.1f, 100.0f);

    // Draw filled polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw all drawables.
    for (auto drawable : _drawables) {
        drawable->draw(projectionMatrix);
    }
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
    const float incrementedLooper = Config::animationLooper + Config::animationSpeed;
    Config::animationLooper = incrementedLooper > 1.0f ? 0.0f : incrementedLooper;

    // Update all drawables.
    for (auto drawable : _drawables) {
        drawable->update(elapsedTimeMs, modelViewMatrix);
    }

    // Update the window.
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
