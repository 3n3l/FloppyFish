#ifndef PART_H
#define PART_H

#include <OpenGL/gltypes.h>

#include "src/drawables/drawable.h"

class Part : public Drawable {
   public:
    Part(std::string texture = "/res/sign.png");
    ~Part();
    Part(const Part&);

    void setHeight(float height) { _height = height; }
    float height() { return _height; }
    void setY(float y) { _y = y; }

    /**
     * @brief Initialize the sign.
     */
    virtual void init() override;

    /**
     * @brief Draw the sign.
     */
    virtual void draw(glm::mat4 projection_matrix) override;

    /**
     * @brief Update the sign.
     * @param elapsedTimeMs - elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

   private:
    unsigned int _textureHandle; /**< handle of the texture */
    std::string _texture;        /**< path of the texture */
    float _height;               /**< height of the sign */
    float _y;                    /**< y-coordinate of the sign */
};

#endif  // PART_H
