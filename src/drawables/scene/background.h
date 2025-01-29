#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "src/drawables/drawable.h"

class Background : public Drawable {
   public:
    Background(std::string texture = ":/res/background.png");
    Background(const Background&);
    ~Background() override;

    /**
     * @brief initialize the background.
     */
    void init() override;

    /**
     * Update the background.
     * @param elapsedTimeMs - elapsed time since the last update in ms.
     * @param modelViewMatrix - transformation into view coordinates.
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * Draw the background.
     * @param projectionMatrix - transformation into NDC.
     */
    void draw(glm::mat4 projectionMatrix);

   protected:
    std::string _texturePath;    /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};

#endif  // BACKGROUND_H
