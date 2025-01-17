#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QOpenGLFunctions_4_1_Core>
#include <string>

#include "glm/ext/matrix_float4x4.hpp"

class Drawable : protected QOpenGLFunctions_4_1_Core {
   public:
    Drawable();
    ~Drawable();
    Drawable(const Drawable&);

    /**
     * @brief initialize the drawable.
     */
    virtual void init();

    /**
     * @brief draw the drawable.
     * @param projection_matrix The current projection matrix
     */
    virtual void draw(glm::mat4 projectionMatrix) {}

    /**
     * @brief update the drawable.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {}

    /**
     * @brief Compile a shader and print warnings/errors if necessary.
     * @param type - GLenum describing the shader type.
     * @param path - string holding the location of the shader.
     * @return the shader handle on success or 0 on failure.
     */
    virtual GLuint compileShader(GLenum type, const std::string& path);

    /**
     * @brief Link the shader program 'prg' and print warnings/errors if necessary.
     * @param program - the program
     * @return the program on success, or 0 on error.
     */
    virtual GLuint linkProgram(GLuint program);

    /**
     * @brief Loads an image as a texture from a given path.
     * @param path - the path to the image file.
     */
    virtual GLuint loadTexture(std::string path);

   protected:
    glm::mat4 _modelViewMatrix;  /**< The model view matrix to get the object into model view space */
    GLuint _program;             /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;   /**< The vertex array object containing the vertices */
    std::string _texturePath;    /**< Path to the texture */
    unsigned int _textureHandle; /**< Handle of the texture */
};

#endif  // DRAWABLE_H
