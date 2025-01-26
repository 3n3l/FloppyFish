#include "glm/gtc/type_ptr.hpp"
#define GL_SILENCE_DEPRECATION
#define GLM_ENABLE_EXPERIMENTAL

#include <src/drawables/drawable.h>

#include <QFile>
#include <QOpenGLShaderProgram>
#include <string>
#include <vector>

#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "skybox.h"
#include "src/config/config.h"
#include "src/utils/imageTexture.h"
#include "src/utils/utils.h"

Skybox::Skybox() { _subsequentRotationSpeed = Config::skyRotation; }

void Skybox::init() {
    // Initialize OpenGL functions.
    Drawable::init();

    // Create a program for this class.
    _program = glCreateProgram();

    // Compile shader.
    GLuint vs = Drawable::compileShader(GL_VERTEX_SHADER, "src/shaders/skybox.vs.glsl");
    GLuint fs = Drawable::compileShader(GL_FRAGMENT_SHADER, "src/shaders/skybox.fs.glsl");

    // Attach shader to the program.
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    // Link program.
    _program = Drawable::linkProgram(_program);

    // Create vectors (dynamic arrays).
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned int> indices;

    // Create cube.
    Utils::geom_cube(positions, normals, texcoords, indices);
    _verticeAmount = indices.size();

    // Set up a vertex array object for the geometry.
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);

    // Fill vertex array object with position data.
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Fill vertex array object with normal data.
    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
    // Use index '1'.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Fill the texture coordinates buffer with data.
    GLuint texture_coordinate_buffer;
    glGenBuffers(1, &texture_coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texture_coordinate_buffer);
    // Size of two, due to coordinates s and t.
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    // Use index '2', and size of two again.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Unbind vertex array object.
    glBindVertexArray(0);
    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &texture_coordinate_buffer);
    glDeleteBuffers(1, &index_buffer);

    // Check for errors.
    glCheckError();

    // Save the number of vertices for drawing.
    // Multiplied by because every index will be used thrice.
    _verticeAmount = indices.size() * 3;

    // Load texture.
    this->loadTexture();
}

void Skybox::draw(glm::mat4 projection_matrix) {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Load program.
    glUseProgram(_program);

    // Bin vertex array object.
    glBindVertexArray(_vertexArrayObject);

    // Set parameter.
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE, value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE, value_ptr(_modelViewMatrix));

    // Activate and bind texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureHandle);

    // Call draw.
    glDrawElements(GL_TRIANGLES, _verticeAmount, GL_UNSIGNED_INT, 0);

    // Unbin vertex array object.
    glBindVertexArray(0);

    // Check for errors.
    glCheckError();
}

void Skybox::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // Update the model-view matrix.
    _subsequentRotation += _subsequentRotationSpeed * elapsedTimeMs;
    _subsequentRotation = _subsequentRotation >= 360.0f ? 0.0f : _subsequentRotation;
    _modelViewMatrix = rotate(modelViewMatrix, glm::radians(_subsequentRotation), glm::vec3(0.0f, -0.2f, -1.0f));
}

void Skybox::loadTexture() {
    auto star_images = std::vector<std::string>();
    star_images = {
        "res/starsPX.png", "res/starsNX.png", "res/starsPY.png",
        "res/starsNY.png", "res/starsPZ.png", "res/starsNZ.png",
    };

    GLuint textureID;
    glActiveTexture(GL_TEXTURE0);
    // Generate and bind texture.
    // Allocate one texture, and assign the openGL handle (akin to a pointer).
    glGenTextures(1, &textureID);
    // Makes all following texture methods work on the bound texture.
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Set texture parameters.
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Assign image data.
    for (int i = 0; i < 6; i++) {
        // Iterate throughout the 6 sides of the cubemap.
        ImageTexture image(star_images[i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image.getData());
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    _textureHandle = textureID;
}
