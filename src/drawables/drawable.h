#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <OpenGL/gltypes.h>

#include <string>

#include "glm/ext/matrix_float4x4.hpp"

class Drawable {
   public:
    Drawable();
    ~Drawable();

    /**
     * @brief initialize the drawable.
     */
    virtual void init() {}

    /**
     * @brief draw the drawable.
     * @param projection_matrix The current projection matrix
     */
    virtual void draw(glm::mat4 projectionMatrix) const {}

    /**
     * @brief update the drawable.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {}

   protected:
    glm::mat4 _modelViewMatrix;  /**< The model view matrix to get the object into model view space */
    GLuint _program;             /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;   /**< The vertex array object containing the vertices */
    std::string _texture;        /**< Path to the texture */
    unsigned int _textureHandle; /**< Handle of the texture */
};

#endif  // DRAWABLE_H
