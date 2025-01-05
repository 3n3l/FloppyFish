#include <OpenGL/gltypes.h>

#include "src/drawables/drawable.h"

class Background : public Drawable {
   public:
    Background(std::string texture = "res/background.png");
    ~Background();

    /**
     * @brief initialize the background.
     */
    virtual void init() override;

    /**
     * @brief draw the background.
     * @param projection_matrix The current projection matrix
     */
    virtual void draw(glm::mat4 projection_matrix) const override;

   protected:
    GLuint _program;             /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;   /**< The vertex array object containing the vertices */
    glm::mat4 _modelViewMatrix;  /**< The model view matrix to get the object into model view space */
    std::string _texture;        /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};
