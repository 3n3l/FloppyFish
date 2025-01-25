#ifndef PART_H
#define PART_H

#include "glm/ext/vector_float3.hpp"
#include "src/drawables/drawable.h"
#include "src/drawables/floppyMesh.h"

class Part : public Drawable {
   public:
    Part(const std::shared_ptr<FloppyMesh>& partMesh);
    Part(const Part& p);
    ~Part() override;

    void setMeshOffset(float offset) { _meshOffset = offset; }
    void setHeight(float height) { _height = height; }
    void setY(float y) { _position.y = y; }

    glm::vec3 position() { return _position; }
    float height() { return _height; }

    /**
     * @brief Initialize the sign.
     */
    void init() override;

    /**
     * @brief Draw the sign.
     */
    void draw(glm::mat4 projection_matrix, std::vector<glm::vec3> lightPositions) override;

    /**
     * @brief Update the sign.
     * @param elapsedTimeMs - elapsed time since the last update in ms
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

   private:
    std::shared_ptr<FloppyMesh> _partMesh; /**< Pointer to the mesh of the part */
    glm::vec3 _hitboxColour;               /**< Colour of the hitbox. */
    glm::vec3 _position;                   /**< Current position of the part. */
    float _meshOffset;                     /**< y-Offset of the mesh, used to center this in the hitbox. */
    float _height;                         /**< Height of the sign */
};

#endif  // PART_H
