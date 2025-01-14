#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <OpenGL/gltypes.h>

#include "src/drawables/drawable.h"
#include "src/drawables/obstacles/part.h"

class Obstacle : public Drawable {
   public:
    Obstacle(std::string texture = "/res/background.png", float offset = 0.0f);
    ~Obstacle();

    bool isOutOfBounds() const { return _x < -1 - (_width / 2); }

    /**
     * @brief initialize the obstacle.
     */
    virtual void init();

    /**
     * @brief draw the obstacle.
     */
    virtual void draw(glm::mat4 projection_matrix) const;

    /**
     * @brief update the obstacle.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix);

    /**
     * @brief reset child parts.
     */
    virtual void reset();

   private:
    unsigned int _textureHandle; /**< handle of the texture */
    std::string _texture;        /**< path of the texture */
    Part _upperPart;             /**< lower part of the Obstacle */
    Part _lowerPart;             /**< upper part of the Obstacle */
    float _offset;               /**< x-offset from the origin */
    float _height;               /**< height of the obstacle */
    float _width;                /**< width of the obstacle */
    float _depth;                /**< depth of the obstacle */
    float _x;                    /**< current x position */
};

#endif  // OBSTACLE_H
