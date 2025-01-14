#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "src/drawables/drawable.h"

class Background : public Drawable {
   public:
    Background(std::string texture = "res/background.png");
    Background(const Background&);
    ~Background();

    /**
     * @brief initialize the background.
     */
    virtual void init() override;

    /**
     * @brief update the drawable.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * @brief draw the background.
     * @param projection_matrix The current projection matrix
     */
    virtual void draw(glm::mat4 projection_matrix) override;

   protected:
    std::string _texturePath;    /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};

#endif  // BACKGROUND_H
