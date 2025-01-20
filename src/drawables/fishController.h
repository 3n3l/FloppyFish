#ifndef FISH_H
#define FISH_H

#include "floppyMesh.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "src/drawables/drawable.h"

class FishController : public Drawable {
   public:
    FishController(const std::shared_ptr<FloppyMesh>& billMesh);
    ~FishController() override;

    float xCoordinate() { return _xCoordinate; }
    float yCoordinate() { return _yCoordinate; }
    float width() { return _width; }
    float height() { return _height; }
    static void flop();

    /**
     * Initialize the fish.
     */
    void init() override;

    /**
     * Update the fish.
     * @param elapsedTimeMs - elapsed time since the last update in ms.
     * @param modelViewMatrix - transformation into view coordinates.
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * Draw the fish.
     * @param projectionMatrix - transformation into NDC.
     */
    void draw(glm::mat4 projectionMatrix) override;

    /**
     * @brief Get the bounding box of the fish.
     * @param boundX - the x-coordinate of the fish.
     * @param boundY - the y-coordinate of the fish.
     * @param boundWidth - the width of the fish.
     * @param boundHeight - the height of the fish.
     * @returns the coordinates and dimensions of the fish.
     */
    void getBounds(float& boundX, float& boundY, float& boundWidth, float& boundHeight) const;

   private:
    float _height{};         /**< Height of the fish. */
    float _width{};          /**< Width of the fish. */
    float _yCoordinate{};    /**< Y-coordinate of the fish. */
    float _xCoordinate{};    /**< X-coordinate of the fish. */
    glm::vec3 _hitboxColour{}; /**< The colour of the hitbox. */

    std::shared_ptr<FloppyMesh> _billMesh; /**< Pointer to the mesh of the bill */
};

#endif  // FISH_H
