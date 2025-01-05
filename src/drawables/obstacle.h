#include <OpenGL/gltypes.h>

#include "src/drawables/drawable.h"

class Obstacle : public Drawable {
   public:
    Obstacle(std::string texture = "/res/background.png");
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

   protected:
    std::string _texture;        /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
    float _width;                /**< width of the obstacle */
    float _height;               /**< height of the obstacle */
    float _depth;                /**< depth of the obstacle */
};
