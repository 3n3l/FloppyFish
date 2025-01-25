#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "src/config/config.h"
#define GL_SILENCE_DEPRECATION

#include <QFile>
#include <QOpenGLShaderProgram>

#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "src/drawables/drawable.h"
#include "src/drawables/fishController.h"

FishController::FishController(const std::shared_ptr<FloppyMesh>& billMesh) {
    _width = 0.05f;
    _height = 0.05f;
    _hitboxColour = glm::vec3(0.1f, 0.4f, 0.9f);
    _billMesh = billMesh;
    _verticalVelocity = 0.0f;
}
FishController::~FishController() {}

void FishController::init() {
    // Initialize mesh.
    _billMesh->init();

    // Initialize OpenGL functions.
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
        glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0),  glm::vec3(-1, 1, 0),
        glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0), glm::vec3(1, 1, 0),
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

void FishController::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // Slowly revert velocity back to lower velocity bound.
    if (_verticalVelocity >= Config::velocityBound) {
        _verticalVelocity += Config::verticalAcceleration;
    }

    // Update y-coordinate with the current velocity.
    _y += _verticalVelocity;

    // Translate to the updated y-coordinate.
    _modelViewMatrix = translate(modelViewMatrix, glm::vec3(_x, _y, 0));

    // Let's do a simple linear interpolation to translate between current velocity and the angle.
    // Maps from [velocityBound, verticalVelocity] to [lowerAngle, upperAngle].
    float x0 = Config::velocityBound, x1 = Config::verticalVelocity, x = _verticalVelocity;
    float f0 = Config::lowerAngle, f1 = Config::upperAngle;
    float rotation = f0 * ((x1 - x) / (x1 - x0)) + f1 * ((x - x0) / (x1 - x0));
    _modelViewMatrix = glm::rotate(_modelViewMatrix, glm::radians(rotation), glm::vec3(0, 0, 1));

    // Update mesh before scaling the hitbox.
    _billMesh->update(elapsedTimeMs, _modelViewMatrix);

    // Apply the scaling of the hitbox quad.
    _modelViewMatrix = scale(_modelViewMatrix, glm::vec3(_width, _height, 1.0));
}

void FishController::draw(glm::mat4 projectionMatrix) {
    // Draw the mesh.
    if (_billMesh != nullptr) {
        _billMesh->draw(projectionMatrix);
    }

    // Only draw the hitbox quad if the debug-flag is enabled.
    if (Config::showHitbox) {
        if (_program == 0) {
            qDebug() << "Program not initialized.";
            return;
        }

        // Load program.
        glUseProgram(_program);

        // Bind vertex array object.
        glBindVertexArray(_vertexArrayObject);

        // Set parameters.
        glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE,
                           glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE,
                           glm::value_ptr(_modelViewMatrix));
        glUniform3fv(glGetUniformLocation(_program, "hitboxColour"), 1, value_ptr(_hitboxColour));

        // Call draw.
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

        // Unbind vertex array object.
        glBindVertexArray(0);
    }
}

void FishController::getBounds(float& boundX, float& boundY, float& boundWidth, float& boundHeight) const {
    boundX = _x;
    boundY = _y;
    boundWidth = _width;
    boundHeight = _height;
}
