#include <OpenGL/gltypes.h>

#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

class Background {
   public:
    Background(std::string texture = "/res/background.png");
    ~Background();

    /**
     * @brief draw the background.
     */
    virtual void draw(glm::mat4 projection_matrix) const;

    /**
     * @brief initialize the background.
     */
    virtual void init();

   protected:
    GLuint _program;             /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;   /**< The vertex array object containing the vertices */
    glm::mat4 _modelViewMatrix;  /**< The model view matrix to get the object into model view space */
    std::string _texture;        /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};
