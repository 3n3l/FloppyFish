#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include "mainwindow.h"

#include <src/drawables/scene/background.h>

#include <QMessageBox>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <cstddef>
#include <glm/glm.hpp>
#include <memory>
#include <random>
#include <vector>
#include <thread>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "src/config/config.h"
#include "src/drawables/fishController.h"
#include "src/drawables/obstacles/obstacle.h"
#include "src/drawables/scene/background.h"
#include "src/drawables/scene/ocean.h"

GLMainWindow::GLMainWindow() : _updateTimer(this) {
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

    // Create all the drawables.
    // NOTE: Order in list is important for culling.
    _billMesh = std::make_shared<FloppyMesh>("res/BillDerLachs.obj", 2.0f, 90.0f),
    _drawables = {
        // TODO: create the fence (ground)
        // std::make_shared<Ground>(Ground("res/ground.png")),
        // std::make_shared<Background>(Background("res/background.png")),

        // The ocean background.
        _ocean = std::make_shared<Ocean>(),
        // Bill the Salmon.
        _billTheSalmon = std::make_shared<FishController>(_billMesh),
    };

    // Skybox.
    _skybox = std::make_shared<Skybox>();

    // Create the in the Config specified amount of obstacles and add it to the drawables.
    float offset = Config::obstacleDistance;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-45.0f, 45.0f);
    for (std::size_t i = 0; i < Config::obstacleAmount; i++) {
        auto upperMesh = std::make_shared<FloppyMesh>("res/Sign.obj", 2.0f, dist(mt));
        auto lowerMesh = std::make_shared<FloppyMesh>("res/Lamp.obj", 1.0f, dist(mt));
        // Create the obstacle itself.
        auto obstacle = std::make_shared<Obstacle>(i * offset, upperMesh, lowerMesh);

        // Push this into _drawables to init, update, draw.
        _drawables.push_back(obstacle);

        // Push this into _obstacles to process lighting and collision.
        _obstacles.push_back(obstacle);
    }

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

    _skybox->init();
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

    // Calculate projection matrix from current resolution, this allows for resizing the window without distortion.
    const float aspect = float(Config::windowWidth) / float(Config::windowHeight);
    _projectionMatrix = glm::perspective(glm::radians(Config::fieldOfVision), aspect, 0.1f, 100.0f);
}

void GLMainWindow::paintGL() {
    // Set up view.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set a background colour.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Draw filled polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Disable culling and set a less strict depth function.
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    // _skybox->draw(_projectionMatrix);
    _ocean->draw(_projectionMatrix);

    // Get the current light positions from the obstacles.
    std::vector<glm::vec3> lightPositions;
    for (auto obstacle : _obstacles) {
        lightPositions.push_back(obstacle->lightPosition());
    }
    glm::vec3 moonDirection = _ocean->getMoonDirection();

    // Draw all drawables.
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    for (auto drawable : _drawables) {
        drawable->draw(_projectionMatrix, lightPositions, moonDirection);
    }
}

void GLMainWindow::animateGL() {
    // Make the context current in case there are glFunctions called.
    makeCurrent();

    // Get the time delta and restart the stopwatch.
    float elapsedTimeMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    _stopWatch.restart();

    // Calculate current model view matrix.
    glm::mat4 modelViewMatrix =
        lookAt(glm::vec3(0.0f, Config::lookAtHeight, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Increment the animation looper if the animation is running.
    const float incrementedLooper = Config::animationLooper + Config::animationSpeed;
    Config::animationLooper = incrementedLooper > 1.0f ? 0.0f : incrementedLooper;

    _skybox->update(elapsedTimeMs, modelViewMatrix);

    // Update all drawables.
    for (auto drawable : _drawables) {
        drawable->update(elapsedTimeMs, modelViewMatrix);
    }

    // Update the window.
    update();
}

void GLMainWindow::keyPressEvent(QKeyEvent *event) {
    const bool isFullscreen = visibility() == FullScreen;
    // Pressing SPACE will make the fish flop or flop the fish idk.
    if (event->key() == Qt::Key_Space) {
        if (!_jumpSFX->isPlaying()) _jumpSFX->play();
        _billTheSalmon->flop();
    }
    // Pressing F in fullscreen mode will reset the window.
    else if (event->key() == Qt::Key_F && isFullscreen) {
        showNormal();
    }
    // Pressing F in normal mode will go to fullscreen mode.
    else if (event->key() == Qt::Key_F && !isFullscreen) {
        showFullScreen();
    }
    // Pressing D will toggle debug mode on/off.
    else if (event->key() == Qt::Key_D) {
        Config::showHitbox = !Config::showHitbox;
    }
    // Pressing ARROW keys will move the camera or change FoV. 0 Resets it again.
    else if (event->key() == Qt::Key_Up) {
        Config::lookAtHeight += 0.01f;
    } else if (event->key() == Qt::Key_Down) {
        Config::lookAtHeight -= 0.01f;
    } else if (event->key() == Qt::Key_Left) {
        Config::fieldOfVision -= 5.0f;
        this->resizeGL(Config::windowWidth, Config::windowHeight);
    } else if (event->key() == Qt::Key_Right) {
        Config::fieldOfVision += 5.0f;
        this->resizeGL(Config::windowWidth, Config::windowHeight);
    } else if (event->key() == Qt::Key_0) {
        Config::lookAtHeight = 0.0f;
        Config::fieldOfVision = 75.0f;
        this->resizeGL(Config::windowWidth, Config::windowHeight);
    }
    // Pressing ESCAPE or Q will quit everything.
    else if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Q) {
        close();
    }
}
