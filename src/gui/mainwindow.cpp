#include "mainwindow.h"

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

    // Create the drawables.
    _drawables = {std::make_shared<Background>(Background("res/background.png"))};

    // Create the obstacles.
    float offset = Config::obstacleOffset;
    for (unsigned int i = 0; i < Config::obstacleAmount; i++) {
        _obstacles.push_back(std::make_shared<Obstacle>(Obstacle("res/background.png", i * offset)));
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

    // Initialize all obstacles.
    for (auto obstacle : _obstacles) {
        obstacle->init();
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
    glClearColor(0.5f, 0.63f, 0.74f, 1.0f);

    // Calculate projection matrix from current resolution, this allows for resizing the window without distortion.
    /*const float fovy = glm::radians(60.0f);*/
    /*const float aspect = float(Config::windowWidth) / float(Config::windowHeight);*/
    /*glm::mat4 projectionMatrix = glm::perspective(fovy, aspect, 0.1f, 100.0f);*/
    // TODO: projection computation is not working right now.
    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    // Draw filled polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw all drawables.
    for (auto drawable : _drawables) {
        drawable->draw(projectionMatrix);
    }

    // Draw all obstacles.
    for (auto obstacle : _obstacles) {
        obstacle->draw(projectionMatrix);
    }
}

void GLMainWindow::animateGL() {
    // Make the context current in case there are glFunctions called.
    makeCurrent();

    // Get the time delta and restart the stopwatch.
    float elapsedTimeMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    _stopWatch.restart();

    // Calculate current model view matrix.
    glm::mat4 modelViewMatrix = glm::lookAt(glm::vec3(0), glm::vec3(0), glm::vec3(0, 1, 0));

    // Increment the animation looper if the animation is running.
    const float incrementedLooper = Config::animationLooper + 0.0003f * Config::animationSpeed;
    Config::animationLooper = incrementedLooper > 1.0f ? 0.0f : incrementedLooper;

    // Update the widget.
    update();

    // Update all drawables.
    for (auto drawable : _drawables) {
        drawable->update(elapsedTimeMs);
    }

    // Iterate over all obstacles and sort list by x-values in place.
    for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
        std::shared_ptr<Obstacle> obstacle = *it;
        if (obstacle->x <= -((1 / obstacle->width) + 1)) {
            // Reset the obstacle based on the last element of the list.
            std::shared_ptr<Obstacle> last = _obstacles.back();
            /*float offset = 1 + (1 / last->width);*/
            obstacle->reset(last->x + Config::obstacleOffset);

            // Move newly reset obstacle to the end of the list.
            _obstacles.splice(_obstacles.end(), _obstacles, it);
        }
        obstacle->update(elapsedTimeMs);
    }

    // Iterate over all obstacles and reset if out of bounds.
    /*for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {*/
    /*    std::shared_ptr<Obstacle> obstacle = *it;*/
    /*    obstacle->update(elapsedTimeMs);*/
    /*    if (obstacle->x > -1 - (1 / obstacle->width)) {*/
    /*        std::shared_ptr<Obstacle> previous = *std::prev(it);*/
    /*        float offset = 1 + (1 / previous->width);*/
    /*        obstacle->reset(previous->x + offset);*/
    /*    }*/
    /*}*/
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
