#ifndef OCEAN_H
#define OCEAN_H

#include "src/drawables/drawable.h"

class Ocean : public Drawable {
   public:
    Ocean();

    /**
     * @brief draw the ocean.
     */
    void draw(glm::mat4 projectionMatrix) override;

    /**
     * @brief initialize the ocean.
     */
    void init() override;

    /**
     * @brief update Updates the object's position, rotation etc.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     * @param modelViewMatrix the mode view matrix of the parent object
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

   protected:
    GLuint _program;            /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;  /**< The vertex array object containing the vertices */
    GLuint _verticeAmount;      /**< The amount of vertices used to draw the triangle */
    glm::mat4 _modelViewMatrix; /**< The model view matrix to get the object into model view space */

    GLuint _textureHandle; /**< Texture handle (memory location of texture). */

    float _subsequentRotation;      /**< The subsequent rotation around the Y-axis applied to the mesh. */
    float _subsequentRotationSpeed; /**< The subsequent rotation speed around the Y-axis applied to the mesh. */

    /**
     * @brief loadTexture loads the textures for the ocean
     */
    void loadTexture();
};

#endif  // OCEAN_H
