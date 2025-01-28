#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "drawable.h"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

class Gameover : public Drawable {
   public:
    Gameover();

    /**
     * @brief draw the Game over picture.
     */
    void draw(glm::mat4 projectionMatrix);

    /**
     * @brief initialize the Game over.
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
};

#endif  // GAMEOVER_H
