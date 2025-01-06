#include <OpenGL/gltypes.h>

#include "src/drawables/drawable.h"

class Obstacle : public Drawable {
   public:
    float x;      /**< current x position */
    float width;  /**< width of the obstacle */
    float height; /**< height of the obstacle */
    float depth;  /**< depth of the obstacle */
    float offset; /**< x-offset from the origin */

    Obstacle(std::string texture = "/res/background.png", float offset = 0.0f);
    ~Obstacle();

    /**
     * @brief initialize the obstacle.
     */
    virtual void init();

    /**
     * @brief draw the obstacle.
     */
    virtual void draw(glm::mat4 projection_matrix) const;

    /**
     * @brief update the obstacle.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs);

    /**
     * @brief update the obstacle.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     */
    virtual void reset(float x);

   protected:
    std::string _texture;        /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};
