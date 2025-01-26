#ifndef OCEAN_H
#define OCEAN_H

#include "src/drawables/drawable.h"

class Ocean : public Drawable {
   public:
    Ocean();

    /**
     * @brief draw the ocean.
     */
    void draw(glm::mat4 projectionMatrix);

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

    /**
     * @brief Returns the direction of the moon.
     * @return The direction of the moon as a vector.
     */
    glm::vec3 getMoonDirection() const { return _moonDirection; }

   protected:
    GLuint _program;            /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject;  /**< The vertex array object containing the vertices */
    GLuint _verticeAmount;      /**< The amount of vertices used to draw the triangle */
    glm::mat4 _modelViewMatrix; /**< The model view matrix to get the object into model view space */

    GLuint _textureHandle; /**< Texture handle (memory location of texture). */

    float _elapsedTime;             /**< Elapsed time in ms since last update */
    glm::vec3 _moonDirection;       /**< Direction of the moon (vector) */
    float _subsequentRotation;      /**< The subsequent rotation around the Y-axis applied to the mesh. */
    float _subsequentRotationSpeed; /**< The subsequent rotation speed around the Y-axis applied to the mesh. */
    glm::mat4 _skyRotationMatrix;   /**< Rotation matrix of the skybox. */
    /**
     * @brief loadTexture loads the textures for the ocean
     */
    void loadTexture();
};

#endif  // OCEAN_H
