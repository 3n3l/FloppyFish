#ifndef PART_H
#define PART_H

#include <OpenGL/gltypes.h>
#include "glm/ext/vector_float3.hpp"
#include "src/drawables/drawable.h"
#include "src/drawables/floppyMesh.h"

class Part : public Drawable {
   public:
    Part(const std::shared_ptr<FloppyMesh>& partMesh);
    Part(const Part& p);
    ~Part() override;

    void setMeshOffset(const float offset) { _meshOffset = offset; }
    void setWidth(const float width) { _width = width; }
    void setHeight(const float height) { _height = height; }
    void setDepth(const float depth) { _depth = depth; }
    void setY(const float y) { _position.y = y; }
    void setMeshRotation(const float rotation) const { _partMesh->setRotation(rotation); }

    glm::vec3 position() { return _position; }
    float width() const { return _width; }
    float height() const { return _height; }
    float depth() const { return _depth; }

    /**
     * @brief Initialize the sign.
     */
    void init() override;

    /**
     * @brief Draw the sign.
     * @param lightPositions - array holding the light positions.
     * @param moonDirection - vector holding the moon direction.
     */
    void draw(glm::mat4 projection_matrix, GLfloat lightPositions[], glm::vec3 moonDirection) override;

    /**
     * @brief Update the sign.
     * @param elapsedTimeMs - elapsed time since the last update in ms
     * @param modelViewMatrix the mode view matrix of the parent object
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

   private:
    std::shared_ptr<FloppyMesh> _partMesh; /**< Pointer to the mesh of the part */
    glm::vec3 _hitboxColour;               /**< Colour of the hitbox. */
    glm::vec3 _position;                   /**< Current position of the part. */
    float _meshOffset;                     /**< y-Offset of the mesh, used to center this in the hitbox. */
    float _width;                          /**< Width of the hitbox. */
    float _height;                         /**< Height of the hitbox. */
    float _depth;                          /**< Depth of the hitbox. */
};

#endif  // PART_H
