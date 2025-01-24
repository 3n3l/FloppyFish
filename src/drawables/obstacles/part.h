#ifndef PART_H
#define PART_H

#include "src/drawables/drawable.h"
#include "src/drawables/floppyMesh.h"

class Part : public Drawable {
   public:
    Part(const std::shared_ptr<FloppyMesh>& partMesh);
    ~Part() override;
    Part(const Part& p);

    void setMeshOffset(float offset) { _meshOffset = offset; }
    void setHeight(float height) { _height = height; }
    void setY(float y) { _yCoordinate = y; }

    float height() { return _height; }

    /**
     * @brief Initialize the sign.
     */
    void init() override;

    /**
     * @brief Draw the sign.
     */
    void draw(glm::mat4 projection_matrix) override;

    /**
     * @brief Update the sign.
     * @param elapsedTimeMs - elapsed time since the last update in ms
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

   private:
    float _height;           /**< Height of the sign */
    float _yCoordinate;      /**< y-coordinate of the sign */
    glm::vec3 _hitboxColour; /**< Colour of the hitbox. */
    float _meshOffset;       /**< y-Offset of the mesh, used to center this in the hitbox. */

    std::shared_ptr<FloppyMesh> _partMesh; /**< Pointer to the mesh of the part */
};

#endif  // PART_H
