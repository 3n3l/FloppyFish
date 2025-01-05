#include "src/objects/triangle.h"

#include <vector>

#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/gltypes.h>

#include <QDebug>
#include <QFile>
#include <QOpenGLShaderProgram>
#include <QTextStream>
#include <string>

#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "src/utils/utils.h"

Triangle::Triangle() : _vertexArrayObject(0) {}
Triangle::~Triangle() {}

void Triangle::init() {
    // Create a program for this class.
    _program = glCreateProgram();

    // Compile shader.
    GLuint vs = Utils::compileShader(GL_VERTEX_SHADER, "src/shaders/triangle.vs.glsl");
    GLuint fs = Utils::compileShader(GL_FRAGMENT_SHADER, "src/shaders/triangle.fs.glsl");

    // Attach shader to the program.
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    // Link program.
    _program = Utils::linkProgram(_program);

    // Generate triangle.
    std::vector<glm::vec3> positions = {
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0),
    };

    // Set up a vertex array object for the geometry.
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);

    // Fill vertex array object with data.
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &positionBuffer);
}

void Triangle::draw(glm::mat4 projectionMatrix) const {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Load program.
    glUseProgram(_program);
    glCheckError();

    // Bind vertex array object.
    glBindVertexArray(_vertexArrayObject);
    glCheckError();

    // Call draw.
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glCheckError();

    // Unbind vertex array object.
    glBindVertexArray(0);
    glCheckError();
}
