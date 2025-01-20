#define GL_SILENCE_DEPRECATION

#include "src/drawables/obstacles/part.h"

#include <QFile>
#include <QOpenGLShaderProgram>
#include <string>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"
#include "src/utils/utils.h"

Part::Part(const std::shared_ptr<FloppyMesh>& partMesh) : _yCoordinate(0) { _partMesh = partMesh; }
Part::Part(Part const& p) : _partMesh(p._partMesh), _yCoordinate(p._yCoordinate) {}
Part::~Part() {}

void Part::init() {
    // Initialize mesh.
    _partMesh->init();

    // Set hitbox colour.
    _hitboxColour = glm::vec3(0.1f, 0.7f, 0.3f);

    // Initialize OpenGL functions, replacing glewInit().
    Drawable::init();

    // Create a program for this class.
    _program = glCreateProgram();

    // Compile shader.
    GLuint vs = compileShader(GL_VERTEX_SHADER, "src/shaders/hitbox.vs.glsl");
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, "src/shaders/hitbox.fs.glsl");

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

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &position_buffer);

    // Unbind vertex array object.
    glBindVertexArray(0);
}

void Part::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // TODO
    _modelViewMatrix = translate(modelViewMatrix, glm::vec3(0, _yCoordinate, 0));

    // Update mesh before scaling part hitbox.
    _partMesh->update(elapsedTimeMs, _modelViewMatrix);

    // TODO
    _modelViewMatrix = scale(_modelViewMatrix, glm::vec3(1, _height, 1));
}

void Part::draw(glm::mat4 projectionMatrix) {
    // Only draw the hitbox quad if the debug-flag is enabled.
    if (Config::showHitbox) {
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

        // Set parameter.
        glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE,
                           glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE,
                           glm::value_ptr(_modelViewMatrix));
        glUniform3fv(glGetUniformLocation(_program, "hitboxColour"), 1, value_ptr(_hitboxColour));

        // Call draw.
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
        glCheckError();

        // Unbind vertex array object.
        glBindVertexArray(0);
        glCheckError();
    }

    // Draw the mesh.
    if (_partMesh != nullptr) {
        _partMesh->draw(projectionMatrix);
    }
}
