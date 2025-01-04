#include <OpenGL/gltypes.h>

#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

class Background {
   public:
    Background(std::string texture = "/res/background.png");
    ~Background();

    /**
     * @brief draw this triangle.
     */
    virtual void draw(glm::mat4 projection_matrix) const;

    /**
     * @brief initialize this triangle.
     */
    virtual void init();

   protected:
    GLuint _program;             /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;   /**< The vertex array object containing the vertices */
    GLuint _verticeAmount;       /**< The amount of vertices used to draw the triangle */
    glm::mat4 _modelViewMatrix;  /**< The model view matrix to get the object into model view space */
    std::string _texture;        /**< path of the texture */
    unsigned int _textureHandle; /**< handle of the texture */
};
