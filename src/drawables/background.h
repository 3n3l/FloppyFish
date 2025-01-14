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
     * Update the background.
     * @param elapsedTimeMs - elapsed time since the last update in ms.
     * @param modelViewMatrix - transformation into view coordinates.
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * Draw the background.
     * @param projectionMatrix - transformation into NDC.
     */
    virtual void draw(glm::mat4 projectionMatrix) override;

   protected:
    std::string _texturePath;    /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};

#endif  // BACKGROUND_H
