#include "mainwindow.h"

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

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

    setSurfaceType(OpenGLSurface);

    // Update the scene periodically.
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(animateGL()));
    _updateTimer.start(18);
    _stopWatch.start();

    // Create a triangle to be displayed in the center.

    _billTheSalmon =
        std::make_shared<FloppyMesh>("res/BillDerLachs.obj", glm::vec3(0.0f, -0.3f, 0.0f), 0.1f, 90.0f, 0.05f);
    _secondProp = std::make_shared<FloppyMesh>("res/Lamp.obj", glm::vec3(0.5f, 0.3f, 0.0f), 0.1f, 45.0f, 0.05f);

    _skybox = std::make_shared<Skybox>();

    // TODO: Initialize the media player.
    _mediaPlayer = std::make_shared<QMediaPlayer>();
    const auto devices = QMediaDevices::audioOutputs();
    _audioOutput = std::make_shared<QAudioOutput>(devices.first());

    _audioOutput->setVolume(100);
    _mediaPlayer->setAudioOutput(_audioOutput.get());

    _mediaPlayer->setSource(QUrl::fromLocalFile("res/FloppyJumpOST_v1.wav"));
    _mediaPlayer->play();

    _jumpSFX = std::make_shared<QSoundEffect>();
    _jumpSFX->setSource(QUrl::fromLocalFile("res/FloppyJumpSFX.wav"));
    _jumpSFX->setVolume(100);
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
    // Initialize OpenGL functions, replacing glewInit().
    initializeOpenGLFunctions();

    // Make sure the context is current.
    makeCurrent();

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable multisampling for AA;
    glEnable(GL_MULTISAMPLE);

    // Enable alpha blending and selecting blend function.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    _billTheSalmon->init();
    _secondProp->init();
    _skybox->init();
}

void GLMainWindow::resizeGL(int width, int height) {
    // Update the viewport.
    glViewport(0, 0, width, height);

    // Store the resolution in the config.
    Config::windowWidth = width;
    Config::windowHeight = height;

    // Calculate projection matrix from current resolution, this allows for resizing the window without distortion.
    const float aspect = float(Config::windowWidth) / float(Config::windowHeight);
    _projectionMatrix = glm::perspective(glm::radians(Config::fieldOfVision), aspect, 0.1f, 100.0f);
}

void GLMainWindow::paintGL() {
    // Set up view.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set a background color.
    glClearColor(0.5f, 0.63f, 0.74f, 1.0f);

    // Draw filled polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Disable culling and set a less strict depth function.
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    _skybox->draw(_projectionMatrix);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    _billTheSalmon->draw(_projectionMatrix);
    _secondProp->draw(_projectionMatrix);
}

void GLMainWindow::animateGL() {
    // Make the context current in case there are glFunctions called.
    makeCurrent();

    // Get the time delta and restart the stopwatch.
    float timeElapsedMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    _stopWatch.restart();

    // Calculate current model view matrix.
    glm::mat4 modelViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    _skybox->update(timeElapsedMs, modelViewMatrix);
    _billTheSalmon->update(timeElapsedMs, modelViewMatrix);
    _secondProp->update(timeElapsedMs, modelViewMatrix);

    // Update the widget.
    update();
}

void GLMainWindow::keyPressEvent(QKeyEvent *event) {
    const bool isFullscreen = visibility() == QWindow::FullScreen;
    // Pressing SPACE will make the fish flop or flop the fish idk.
    if (event->key() == Qt::Key_Space) {
        // TODO: make the fish flop!
        if (!_jumpSFX->isPlaying()) _jumpSFX->play();
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
