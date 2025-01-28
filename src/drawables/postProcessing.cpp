#include "postProcessing.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "src/config/config.h"
#include "src/drawables/drawable.h"
#include "src/utils/imageTexture.h"
#include "src/utils/utils.h"

PostProcessingQuad::PostProcessingQuad()
    : Drawable(), _textureColourBuffer(0), _depthStencilBuffer(0), _frameBufferObject(0) {}

void PostProcessingQuad::init() {
    // Initialize OpenGL functions.
    Drawable::init();

    // Create a program for this class.
    _program = glCreateProgram();

    // Compile shader.
    GLuint vs = Drawable::compileShader(GL_VERTEX_SHADER, "src/shaders/postProcessing.vs.glsl");
    GLuint fs = Drawable::compileShader(GL_FRAGMENT_SHADER, "src/shaders/postProcessing.fs.glsl");

    // Attach shader to the program.
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    // Link program.
    _program = Drawable::linkProgram(_program);

    // Fill position buffer with data.
    std::vector positions = {
        glm::vec3(-1, -1, 1.0),
        glm::vec3(1, -1, 1.0),
        glm::vec3(1, 1, 1.0),
        glm::vec3(-1, 1, 1.0),
    };

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

    // Fill the texture coordinates buffer with data.
    std::vector<glm::vec2> texcoords = {
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        glm::vec2(0, 1),
    };

    GLuint texture_coordinate_buffer;
    glGenBuffers(1, &texture_coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texture_coordinate_buffer);
    // Size of two, due to coordinates s and t.
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    // Use index '2', and size of two again.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Unbind vertex array object.
    glBindVertexArray(0);
    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &texture_coordinate_buffer);

    // Check for errors.
    glCheckError();

    // Initialize the framebuffer.
    glGenFramebuffers(1, &_frameBufferObject);
    glGenFramebuffers(1, &_postProcessFrameBufferObject);
}

void PostProcessingQuad::bind() {
    // Bind framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferObject);
}

void PostProcessingQuad::unbind() {
    // Unbind framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessingQuad::blitBuffers() {
    const unsigned int width = Config::windowWidth * Config::resolutionScale;
    const unsigned int height = Config::windowHeight * Config::resolutionScale;
    // Make it so the multisampling FBO is read while the post-processing FBO is drawn.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _frameBufferObject);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _postProcessFrameBufferObject);
    glCheckError();
    // Conclude the multisampling and copy it to the post-processing FBO.
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glCheckError();
}

void PostProcessingQuad::destroy() { glDeleteFramebuffers(1, &_frameBufferObject); }

void PostProcessingQuad::resetBufferTextures(int width, int height) {
    bind();
    // Create colour buffer.
    glGenTextures(1, &_textureColourBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _textureColourBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Config::samples, GL_RGB, width, height, GL_TRUE);

    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCheckError();
    // Clamping prevents the edges bleeding.
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to the framebuffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _textureColourBuffer, 0);
    glCheckError();

    // Create depth and stencil buffer.
    glGenTextures(1, &_depthStencilBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _depthStencilBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Config::samples, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);

    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Clamping prevents the edges bleeding.
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach renderbuffer to the framebuffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, _depthStencilBuffer,
                           0);

    glCheckError();

    // Unbind texture.
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    unbind();

    // Due to multisample textures not working for postprocessing, use a separate texture.
    glBindFramebuffer(GL_FRAMEBUFFER, _postProcessFrameBufferObject);

    glGenTextures(1, &_postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, _postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Clamping prevents the edges bleeding.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _postProcessingTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is incomplete: " << std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER))
                  << std::endl;
    }

    unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}

void PostProcessingQuad::draw() {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }
    // Load program.
    glUseProgram(_program);

    // Bin vertex array object.
    glBindVertexArray(_vertexArrayObject);

    // Bind texture.
    glBindTexture(GL_TEXTURE_2D, _postProcessingTexture);
    glCheckError();

    // Parameters.
    // uniform gamma from Config.
    glUniform1f(glGetUniformLocation(_program, "gamma"), Config::gamma);

    // Call draw.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    // Un-bind vertex array object.
    glBindVertexArray(0);

    // Check for errors.
    glCheckError();
}
