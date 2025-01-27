#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include "mainwindow.h"

#include <src/drawables/background.h>

#include <QFontDatabase>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <cstddef>
#include <glm/glm.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "src/config/config.h"
#include "src/drawables/fishController.h"
#include "src/drawables/obstacles/obstacle.h"
#include <src/drawables/collisionchecker.h>
#include <iostream>

GLMainWindow::GLMainWindow() : QOpenGLWindow(), QOpenGLFunctions_4_1_Core(), _updateTimer(this), _stopWatch() {
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
    _billMesh = std::make_shared<FloppyMesh>("res/BillDerLachs.obj", glm::vec3(0.0f, -0.1f, 0.0f), 0.1f, 90.0f,
                                             Config::debugRotation),
    _drawables = {
        // TODO: create the fence (ground)
        // std::make_shared<Ground>(Ground("res/ground.png")),
        // std::make_shared<Background>(Background("res/background.png")),
        // Bill the Salmon.
        _billTheSalmon = std::make_shared<FishController>(_billMesh),
    };
    // Skybox.
    _skybox = std::make_shared<Skybox>();

    // Create the in the Config specified amount of obstacles and add it to the drawables.
    float offset = Config::obstacleDistance;
    for (std::size_t i = 0; i < Config::obstacleAmount; i++) {
        _drawables.push_back(std::make_shared<Obstacle>(
            Obstacle(i * offset,
                     std::make_shared<FloppyMesh>("res/Sign.obj", glm::vec3(0.5f, -1.4f, 0.0f), 0.1f, 45.0f,
                                                  Config::debugRotation),
                     std::make_shared<FloppyMesh>("res/Lamp.obj", glm::vec3(0.5f, -0.2f, 0.0f), 0.1f, 45.0f,
                                                  Config::debugRotation))));
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
    _gameover.init();

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
    
     QPainter painter(this);

     //Load Font from a file
    _fontId = QFontDatabase::addApplicationFont("res/UnifrakturMaguntia-Regular.ttf");
    if (_fontId == -1) {
        // Haandle error if font couldnt be loaded
        qWarning("Font failed to load.");
        return;
    }

    // Get font family name from the loaded font
    QString fontFamily = QFontDatabase::applicationFontFamilies(_fontId).at(0);


    if (_gameIsOver) {
        // Set projection matrix for 2D rendering
        glm::mat4 projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

        // Draw the game over screen
        _gameover.draw(projectionMatrix);

        // context for the drawing
        painter.beginNativePainting();

        // Display the final score on the game-over screen
        painter.setPen(Qt::red);            
        painter.setFont(QFont(fontFamily, 36));  
        painter.drawText(width() / 2 - 100, height() / 6, QString("Game Over!"));
        painter.setFont(QFont(fontFamily, 24));  
        painter.drawText(width() / 2 - 100, height() / 6 + 40, QString("Final Score: %1").arg(Config::_score));

         // End native painting
        painter.endNativePainting();
    } else {
         // Disable culling and set a less strict depth function.
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        _skybox->draw(_projectionMatrix);

        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        // Draw all drawables.
        for (auto drawable : _drawables) {
            drawable->draw(_projectionMatrix);
        }
  
        painter.beginNativePainting();  

        painter.setPen(Qt::red);            
        painter.setFont(QFont(fontFamily, 24));  
        painter.drawText(width() / 2 - 100, height() / 6, QString("Score: %1").arg(Config::_score));

        painter.endNativePainting(); 
    }
}

void GLMainWindow::animateGL() {
    // Make the context current in case there are glFunctions called.
    makeCurrent();

    // Get the time delta and restart the stopwatch.
    float elapsedTimeMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    _stopWatch.restart();

     // If the animation is paused (collision happened), stop updating.
    if (_gameIsOver) {
        return;
    }

    // Calculate current model view matrix.
    glm::mat4 modelViewMatrix =
        glm::lookAt(glm::vec3(0.0f, Config::lookAtHeight, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Set model-view matrix (no transformations, just the screen in place)
    glm::mat4 _modelViewMatrix = glm::mat4(1.0f);

    // Increment the animation looper if the animation is running.
    const float incrementedLooper = Config::animationLooper + Config::animationSpeed;
    Config::animationLooper = incrementedLooper > 1.0f ? 0.0f : incrementedLooper;

    _skybox->update(elapsedTimeMs, modelViewMatrix);

    // Update the model-view matrix
    _gameover.update(elapsedTimeMs, _modelViewMatrix);

    // Update all drawables and check for collision
     for (auto drawable : _drawables) {
        drawable->update(elapsedTimeMs, modelViewMatrix);
         // check if collision is true
        auto draw = std::dynamic_pointer_cast<Obstacle>(drawable);
         if (draw != nullptr && CollisionChecker::checkCollision(_billTheSalmon, draw)) {
            // Freeze the game on collision
             _gameIsOver = true;
             break;                
         }
     }
    // Update the window.
    update();
}

void GLMainWindow::keyPressEvent(QKeyEvent *event) {
    const bool isFullscreen = visibility() == QWindow::FullScreen;
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
