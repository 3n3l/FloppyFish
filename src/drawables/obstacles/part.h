#ifndef PART_H
#define PART_H

#include <OpenGL/gltypes.h>

#include "src/drawables/drawable.h"

class Part : public Drawable {
   public:
    Part(std::string texture = "/res/sign.png");
    ~Part();

    /**
     * @brief Initialize the sign.
     */
    virtual void init() override;

    /**
     * @brief Draw the sign.
     */
    virtual void draw(glm::mat4 projection_matrix) const override;

    /**
     * @brief Reset this part of the obstacle.
     * @param nx - new x-coordinate
     * @param ny - new y-coordinate
     * @param height - new height
     */
    virtual void reset(float nx, float ny, float height);

    /**
     * @brief Update the sign.
     * @param elapsedTimeMs - elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix);

   private:
    unsigned int _textureHandle; /**< handle of the texture */
    std::string _texture;        /**< path of the texture */
    float _height;               /**< height of the sign */
    float _y;                    /**< y-coordinate of the sign */
};

#endif  // PART_H
