#define GL_SILENCE_DEPRECATION
#define GLM_ENABLE_EXPERIMENTAL

#include <src/drawables/drawable.h>

#include <QFile>
#include <QOpenGLShaderProgram>
#include <string>
#include <utility>
#include <vector>

#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "gameover.h"
#include "src/config/config.h"
#include "src/utils/imageTexture.h"
#include "src/utils/utils.h"

Gameover::Gameover() : _program(0), _vertexArrayObject(0), _verticeAmount(0), _textureHandle(0) {}

void Gameover::init() {
    Drawable::init();

    // Create program for this class
    _program = glCreateProgram();

    // Compile and attach shaders
    GLuint vs = Drawable::compileShader(GL_VERTEX_SHADER, "src/shaders/gameover.vs.glsl");
    GLuint fs = Drawable::compileShader(GL_FRAGMENT_SHADER, "src/shaders/gameover.fs.glsl");
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    _program = Drawable::linkProgram(_program);

    // Define vertices and texture coordinates for a 2D quad
    std::vector<glm::vec3> positions = {
        {-1.0f, 1.0f, 0.0f},   // top-left
        {-1.0f, -1.0f, 0.0f},  // bottom-left
        {1.0f, -1.0f, 0.0f},   // bottom-right
        {1.0f, 1.0f, 0.0f}     // top-right
    };

    std::vector<glm::vec2> texcoords = {
        {0.0f, 0.0f},  // top-left
        {0.0f, 1.0f},  // bottom-left
        {1.0f, 1.0f},  // bottom-right
        {1.0f, 0.0f}   // top-right
    };

    // Define indices for the quad (since OpenGL uses indices to reference the vertices)
    std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

    // Create the vertex array object (VAO)
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);

    // Position buffer
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Texture coordinates buffer
    GLuint texture_coordinate_buffer;
    glGenBuffers(1, &texture_coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texture_coordinate_buffer);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Index buffer (defines the triangles)
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Unbind vertex array object
    glBindVertexArray(0);
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &texture_coordinate_buffer);
    glDeleteBuffers(1, &index_buffer);

    // Load texture
    this->loadTexture();
}

void Gameover::draw(glm::mat4 projection_matrix) {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Use shader program
    glUseProgram(_program);

    // Bind the vertex array object
    glBindVertexArray(_vertexArrayObject);

    // Set uniform matrices for projection and modelview
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE,
                       glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE,
                       glm::value_ptr(_modelViewMatrix));

    // Activate and bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
    glUniform1i(glGetUniformLocation(_program, "texSampler"), 0);

    // Draw the quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind vertex array object
    glBindVertexArray(0);

    // Check for errors
    glCheckError();
}


void Gameover::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // Update modelview matrix
    _modelViewMatrix = modelViewMatrix;
}


void Gameover::loadTexture() {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture data
    ImageTexture image("res/GameOver-1.png");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image.getData());

    glGenerateMipmap(GL_TEXTURE_2D);
    _textureHandle = textureID;
}
