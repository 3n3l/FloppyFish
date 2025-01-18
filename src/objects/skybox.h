#ifndef SKYBOX_H
#define SKYBOX_H

#include <QOpenGLFunctions_4_1_Core>

#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

class Skybox {
   public:
    Skybox();

    /**
     * @brief draw the skybox.
     */
    virtual void draw(glm::mat4 projectionMatrix);

    /**
     * @brief initialize the skybox.
     */
    virtual void init();

    /**
     * @brief update Updates the object's position, rotation etc.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     * @param modelViewMatrix the mode view matrix of the parent object
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix);

   protected:
    GLuint _program;            /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;  /**< The vertex array object containing the vertices */
    GLuint _verticeAmount;      /**< The amount of vertices used to draw the triangle */
    glm::mat4 _modelViewMatrix; /**< The model view matrix to get the object into model view space */

    GLuint _textureHandle; /**< Texture handle (memory location of texture). */

    float _subsequentRotation;      /**< The subsequent rotation around the Y-axis applied to the mesh. */
    float _subsequentRotationSpeed; /**< The subsequent rotation speed around the Y-axis applied to the mesh. */

    /**
     * @brief loadTexture loads the textures for the skybox
     */
    void loadTexture();
};

#endif  // SKYBOX_H
