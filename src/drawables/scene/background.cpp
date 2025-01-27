#define GL_SILENCE_DEPRECATION

#include "src/drawables/scene/background.h"

#include <QFile>
#include <QOpenGLShaderProgram>
#include <string>

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"
#include "src/utils/utils.h"

Background::Background(std::string texturePath) : Drawable(), _textureHandle(0), _texturePath(texturePath) {}
Background::Background(Background const &b) : _textureHandle(b._textureHandle), _texturePath(b._texturePath) {}
Background::~Background() {}

void Background::init() {
    // Initialize OpenGL funtions, replacing glewInit().
    Drawable::init();

    // Create a program for this class.
    _program = glCreateProgram();

    // Create texture handle.
    _textureHandle = loadTexture(_texturePath);

    // Compile shader.
    GLuint vs = compileShader(GL_VERTEX_SHADER, "src/shaders/background.vs.glsl");
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, "src/shaders/background.fs.glsl");

    // Attach shader to the program.
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    // Link program.
    _program = linkProgram(_program);

    // Set up a vertex array object for the geometry.
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);

    // Fill position buffer with data.
    // TODO: I have no idea why 0.6?!
    std::vector positions = {
        glm::vec3(-1, -0.6, 0),
        glm::vec3(-1, 0.6, 0),
        glm::vec3(1, 0.6, 0),
        glm::vec3(1, -0.6, 0),
    };
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Fill the texture coordinates buffer with data.
    std::vector texcoords = {
        glm::vec2(-1, -1),
        glm::vec2(-1, 1),
        glm::vec2(1, 1),
        glm::vec2(1, -1),
    };
    GLuint texture_coordinate_buffer;
    glGenBuffers(1, &texture_coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texture_coordinate_buffer);
    // Size of two, due to coordinates s and t.
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    // Use index '2', and size of two again.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    // Use index '2', and size of two again.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Unbind vertex array object.
    glBindVertexArray(0);
    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &texture_coordinate_buffer);

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Check for an OpenGL error in this method.
    glCheckError();
}

void Background::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) { _modelViewMatrix = modelViewMatrix; }

void Background::draw(glm::mat4 projectionMatrix) {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Load program.
    glUseProgram(_program);

    // Bind vertex array object.
    glBindVertexArray(_vertexArrayObject);

    // Set parameter.
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, value_ptr(_modelViewMatrix));

    // Set the background texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
    glUniform1i(glGetUniformLocation(_program, "backgroundTexture"), 0);

    // Repeat the background texture horizontally.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // Stretch the background texture vertically.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Value that goes from 0.0 to 1.0 and resets again.
    glUniform1f(glGetUniformLocation(_program, "animationLooper"), Config::animationLooper);

    // Call draw.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Check for an OpenGL error in this method.
    glCheckError();
}
