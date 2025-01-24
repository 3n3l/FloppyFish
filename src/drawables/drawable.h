#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QOpenGLFunctions_4_1_Core>
#include <string>
#include <vector>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

class Drawable : protected QOpenGLFunctions_4_1_Core {
   public:
    Drawable();
    ~Drawable() override;
    Drawable(const Drawable&);

    // Enumerator to pass with textures.
    enum TextureType { RGB, SRGB, NormalMap, Monochrome };

    /**
     * @brief initialize the drawable.
     */
    virtual void init();

    /**
     * @brief draw the drawable.
     * @param projectionMatrix The current projection matrix
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
     * @param path the path to the texture.
     * @param type the way the texture shall be parsed
     * @return the GLuint pointing to the texture.
     */
    GLuint loadTexture(std::string path, TextureType type = SRGB);

    /**
     * Loads a given positions vector as a VBO into the VAO.
     * @param vector holding the positions.
     * @return the GLuint pointing to the position buffer.
     */
    GLuint loadPositions(std::vector<glm::vec3> positions);

    /**
     * Loads a given texture coordinate vector as a VBO into the VAO.
     * @param vector holding the coordinates.
     * @return the GLuint pointing to the texture coordinate buffer.
     */
    GLuint loadTextureCoordinates(std::vector<glm::vec2> coordinates);

    /**
     * Loads a given normals vector as a VBO into the VAO.
     * @param vector holding the normals.
     * @return the GLuint pointing to the normals buffer.
     */
    GLuint loadNormals(std::vector<glm::vec3> normals);

    /**
     * Loads a given indices vector as a VBO into the VAO.
     * @param vector holding the indices.
     * @return the GLuint pointing to the indices buffer.
     */
    GLuint loadIndices(std::vector<unsigned int> indices);

    /**
     * Binds this VAO to the program.
     */
    void bindVertexArrayObject();

   protected:
    GLuint _program;             /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;   /**< The vertex array object containing the vertices */
    glm::mat4 _modelViewMatrix;  /**< The model view matrix to get the object into model view space */
    unsigned int _textureHandle; /**< Handle of the texture */
    std::string _texturePath;    /**< Path to the texture */
};

#endif  // DRAWABLE_H
