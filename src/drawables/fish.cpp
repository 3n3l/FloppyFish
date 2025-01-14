#include "src/drawables/fish.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"

#define GL_SILENCE_DEPRECATION

#include <QDebug>
#include <QFile>
#include <QOpenGLShaderProgram>
#include <QTextStream>

Fish::Fish(std::string texturePath, float x, float y, float height, float width)
    : Drawable(), _texturePath(texturePath), _x(x), _y(y), _height(height), _width(width) {}
Fish::Fish(Fish const& f) : _texturePath(f._texturePath), _x(f._x), _y(f._y), _height(f._height), _width(f._width) {}
Fish::~Fish() {}

void Fish::init() {
    // Initialize OpenGL funtions.
    Drawable::init();

    // Create a program for this class.
    _program = glCreateProgram();

    // Create texture handle.
    _textureHandle = loadTexture(_texturePath);

    // Compile shader.
    GLuint vs = compileShader(GL_VERTEX_SHADER, "src/shaders/fish.vs.glsl");
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, "src/shaders/fish.fs.glsl");

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
    std::vector<glm::vec3> positions = {
        glm::vec3(-1, -1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, 1, 0),
        glm::vec3(1, -1, 0),
    };
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Fill the texture coordinates buffer with data.
    std::vector<glm::vec2> texcoords = {
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
}

void Fish::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    _y -= Config::gravity;  // Apply gravity.
    _modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(_x, _y, 0));
    _modelViewMatrix = glm::scale(_modelViewMatrix, glm::vec3(_width, _height, 1));
}

void Fish::draw(glm::mat4 projectionMatrix) {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Load program.
    glUseProgram(_program);

    // Bind vertex array object.
    glBindVertexArray(_vertexArrayObject);

    // Set parameter.
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE,
                       glm::value_ptr(_modelViewMatrix));

    // Set the background texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
    glUniform1i(glGetUniformLocation(_program, "backgroundTexture"), 0);

    // Call draw.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    // Unbind vertex array object.
    glBindVertexArray(0);
}

void Fish::getBounds(float& bx, float& by, float& bwidth, float& bheight) const {
    bx = _x;
    by = _y;
    bwidth = _width;
    bheight = _height;
}
