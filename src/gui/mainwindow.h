#ifndef GLMAINWINDOW_H
#define GLMAINWINDOW_H

#include <QtMultimedia/qaudiodevice.h>
#include <QtMultimedia/qaudiooutput.h>
#include <QtMultimedia/qmediadevices.h>
#include <QtMultimedia/qmediaplayer.h>
#include <QtMultimedia/qsoundeffect.h>
#include <src/drawables/drawable.h>

#include <QElapsedTimer>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWindow>
#include <QTimer>
#include <memory>

#include "glm/ext/vector_float3.hpp"
#include "src/drawables/fishController.h"
#include "src/drawables/floppyMesh.h"
#include "src/drawables/scene/ocean.h"
#include "src/drawables/obstacles/obstacle.h"
#include "src/drawables/scene/skybox.h"

/**
 * @brief The GLWindow class handling the opengl window.
 */
class GLMainWindow : public QOpenGLWindow, protected QOpenGLFunctions_4_1_Core {
    Q_OBJECT

   private slots:
    /**
     * @brief animateGL updates the scene
     *
     * The new positions of all elements are generated.
     * This is not used for drawing.
     */
    void animateGL();

   public:
    /**
     * @brief MainWindow constructor
     * @param parent the parent widget
     */
    GLMainWindow();

    /**
     * @brief show opens the widget
     *
     * If the widget can not start, the program exits
     */
    virtual void show();

    /**
     * @brief initializeGL initializes the context
     *
     * The function initializes GLEW and all drawable objects
     */
    virtual void initializeGL() override;

    /**
     * @brief resizeGL called automatically on resize
     * @param width the new width of the widget
     * @param height the new height of the widget
     */
    virtual void resizeGL(int width, int height) override;

    /**
     * @brief paintGL draws the scene
     */
    virtual void paintGL() override;

   protected:
    /**
     * @brief mousePressEvent automatically called whenever the mouse wheel is used
     * @param event the QWheelEvent containing all relevant data
     */
    virtual void keyPressEvent(QKeyEvent* event) override;

   private:
    glm::mat4 _projectionMatrix;                            /**< Projection Matrix */
    std::shared_ptr<QSoundEffect> _jumpSFX;                 /**< Jump SFX */
    std::shared_ptr<QMediaPlayer> _mediaPlayer;             /**< Media Player used for SFX */
    std::shared_ptr<QAudioOutput> _audioOutput;             /**< AudioOutput used for Sound */
    std::shared_ptr<FloppyMesh> _billMesh;                  /**< Bill the salmon shown in the window */
    std::shared_ptr<FloppyMesh> _secondProp;                /**< A sign to test multi-parts objs */
    std::shared_ptr<Skybox> _skybox;                        /**< A skybox */
    std::shared_ptr<FishController> _billTheSalmon;         /**< Bill the Salmon */
    std::vector<std::shared_ptr<Drawable>> _drawables;      /**< Vector holding pointers to the drawables */
    std::vector<std::shared_ptr<Obstacle>> _obstacles;      /**< Vector holding pointers to the obstacles */
    std::vector<std::shared_ptr<glm::vec3>> _lighPositions; /**< Vector holding pointers to the light positions */
    QTimer _updateTimer;                                    /**< Used for regular frame updates */
    QElapsedTimer _stopWatch;                               /**< Measures time between updates */
};

#endif  // GLMAINWINDOW_H
