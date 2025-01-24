#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "src/drawables/drawable.h"
#include "src/drawables/obstacles/part.h"

class Obstacle : public Drawable {
   public:
    /**
     *
     * @param offset The offset applied to the obstacle.
     * @param upperPartMesh The mesh for the upper part of the obstacle.
     * @param lowerPartMesh The mesh for the lower part of the obstacle.
     */
    Obstacle(float offset = 0.0f, const std::shared_ptr<FloppyMesh>& upperPartMesh = nullptr,
             const std::shared_ptr<FloppyMesh>& lowerPartMesh = nullptr);
    ~Obstacle() override;
    Obstacle(const Obstacle&);

    bool isOutOfBounds() const { return _xCoordinate < -1 - (_width / 2); }

    /**
     * @brief initialize the obstacle.
     */
    void init() override;

    /**
     * @brief draw the obstacle.
     */
    void draw(glm::mat4 projection_matrix) override;

    /**
     * @brief update the obstacle.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     * @param modelViewMatrix The model view matrix to use for drawing.
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * @brief reset child parts.
     */
    virtual void reset();

    void getBounds(float& bx, float& by, float& bwidth, float& bheight) const;

   private:
    Part _upperPart;    /**< Lower part of the Obstacle. */
    Part _lowerPart;    /**< Upper part of the Obstacle. */
    float _offset;      /**< X-offset from the origin. */
    float _height;      /**< Height of the obstacle. */
    float _width;       /**< Width of the obstacle. */
    float _depth;       /**< Depth of the obstacle. */
    float _xCoordinate; /**< Current x position. */
};

#endif  // OBSTACLE_H
