#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <memory>

#include "glm/ext/vector_float3.hpp"
#include "src/config/config.h"
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

    bool isOutOfBounds() const { return _position.x < -1 - (_width / 2) - Config::obstacleLeftOverhang; }
    glm::vec3 lightPosition() const { return _lightPosition; }

    /**
     * @brief initialize the obstacle.
     */
    void init() override;

    /**
     * @brief draw the obstacle.
     * @param lightPositions - array holding the light positions.
     * @param moonDirection - vector holding the moon direction.
     */
    void draw(glm::mat4 projection_matrix, GLfloat lightPositions[], glm::vec3 moonDirection) override;

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

   private:
    float _initialOffset;     /**< X-offset from the origin. */
    float _height;            /**< Height of the obstacle. */
    float _width;             /**< Width of the obstacle. */
    float _depth;             /**< Depth of the obstacle. */
    Part _upperPart;          /**< Lower part of the Obstacle. */
    Part _lowerPart;          /**< Upper part of the Obstacle. */
    glm::vec3 _position;      /**< Current position ob the obstacle. */
    glm::vec3 _lightPosition; /**< Position of the light source. */
};

#endif  // OBSTACLE_H
