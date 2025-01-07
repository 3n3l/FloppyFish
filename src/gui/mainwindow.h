#ifndef GLMAINWINDOW_H
#define GLMAINWINDOW_H

#include <src/drawables/drawable.h>
#include <src/drawables/obstacle.h>

#include <QElapsedTimer>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QTimer>

/**
 * @brief The GLWindow class handling the opengl window.
 */
class GLMainWindow : public QOpenGLWindow, protected QOpenGLFunctions {
    Q_OBJECT

   private:
    QTimer _updateTimer;                               /**< Used for regular frame updates */
    QElapsedTimer _stopWatch;                          /**< Measures time between updates */
    std::vector<std::shared_ptr<Drawable>> _drawables; /**< Vector holding pointers to the drawables */

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
    explicit GLMainWindow();

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
    std::list<std::shared_ptr<Obstacle>> _obstacles; /**< doubly linked list of obstacles */
};

#endif  // GLMAINWINDOW_H
