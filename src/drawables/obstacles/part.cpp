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
    bindVertexArrayObject();

    // Fill position buffer with data.
    std::vector<glm::vec3> positions = {
        glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0),  glm::vec3(-1, 1, 0),
        glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0), glm::vec3(1, 1, 0),
    };
    GLuint positionBuffer = loadPositions(positions);

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &positionBuffer);

    // Initialize mesh.
    _partMesh->init();
}

void Part::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // Move on y-axis.
    _modelViewMatrix = translate(modelViewMatrix, glm::vec3(0, _yCoordinate, 0));

    // Update mesh before scaling part hitbox.
    _partMesh->update(elapsedTimeMs, _modelViewMatrix);

    // Scale to height.
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
