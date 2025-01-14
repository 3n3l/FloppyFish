#ifndef FISH_H
#define FISH_H

#include <string>

#include "glm/ext/matrix_float4x4.hpp"
#include "src/drawables/drawable.h"

class Fish : public Drawable {
   public:
    Fish(std::string texturePath = "res/fish.png", float x = 0.0f, float y = 0.0f, float height = 0.1f,
         float width = 0.1f);
    Fish(const Fish&);
    ~Fish();

    /**
     * Initialize the fish.
     */
    virtual void init() override;

    /**
     * Update the fish.
     * @param elapsedTimeMs - elapsed time since the last update in ms.
     * @param modelViewMatrix - transformation into view coordinates.
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * Draw the fish.
     * @param projectionMatrix - transformation into NDC.
     */
    virtual void draw(glm::mat4 projectionMatrix) override;

    /**
     * @brief Get the bounding box of the fish.
     * @param bx - the x-coordinate of the fish.
     * @param by - the y-coordinate of the fish.
     * @param bwidth - the width of the fish.
     * @param bheight - the height of the fish.
     * @returns the coordinates and dimensions of the fish.
     */
    void getBounds(float& bx, float& by, float& bwidth, float& bheight) const;

   private:
    unsigned int _textureHandle; /**< handle of the texture */
    std::string _texturePath;    /**< path of the texture */
    float _height;               /**< height of the fish */
    float _width;                /**< width of the fish */
    float _y;                    /**< y-coordinate of the fish */
    float _x;                    /**< x-coordinate of the fish */
};

#endif  // FISH_H
