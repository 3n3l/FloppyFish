#ifndef FISH_H
#define FISH_H

#include "floppyMesh.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"

class FishController : public Drawable {
   public:
    FishController(const std::shared_ptr<FloppyMesh>& billMesh);
    ~FishController() override;

    void flop() { _verticalVelocity = Config::verticalVelocity; }
    float height() { return _height; }
    float width() { return _width; }
    float x() { return _x; }
    float y() { return _y; }

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
    void draw(glm::mat4 projectionMatrix, std::vector<glm::vec3> lightPositions) override;

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
    float _height{};                       /**< Height of the fish. */
    float _width{};                        /**< Width of the fish. */
    float _y{};                            /**< Y-coordinate of the fish. */
    float _x{};                            /**< X-coordinate of the fish. */
    float _verticalVelocity;               /**< The current velocity of the fish. */
    glm::vec3 _hitboxColour{};             /**< The colour of the hitbox. */
    std::shared_ptr<FloppyMesh> _billMesh; /**< Pointer to the mesh of Bill */
};

#endif  // FISH_H
