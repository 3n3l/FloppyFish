#include <OpenGL/gltypes.h>
#define GL_SILENCE_DEPRECATION

#include <QFile>
#include <QOpenGLShaderProgram>

#include "src/drawables/drawable.h"
#include "src/utils/imageTexture.h"

Drawable::Drawable() : _modelViewMatrix(1.0f), _vertexArrayObject(0) {}
Drawable::Drawable(Drawable const &d) : _modelViewMatrix(1.0f), _vertexArrayObject(0) {}
Drawable::~Drawable() {}

void Drawable::init() {
    // Initialize OpenGL functions, replacing glewInit().
    initializeOpenGLFunctions();
}

GLuint Drawable::compileShader(GLenum type, const std::string &path) {
    QFile f(path.c_str());
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Could not open file: " << path;
    }
    QTextStream in(&f);
    std::string src = in.readAll().toStdString();

    GLuint shader = glCreateShader(type);
    const GLchar *glsrc = src.c_str();
    glShaderSource(shader, 1, &glsrc, NULL);
    glCompileShader(shader);

    std::string message;
    GLint error, length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &error);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        char *tmplog = new char[length];
        glGetShaderInfoLog(shader, length, NULL, tmplog);
        /*kill_crlf(tmplog);*/
        message = std::string(tmplog);
        delete[] tmplog;
    } else {
        message = std::string("");
    }

    std::string shaderName = type == GL_VERTEX_SHADER ? "VS" : "FS";
    if (error && message.length() > 0) {
        qDebug() << "OpenGL WARNING: " << shaderName << "\n" << message << "\n";
    } else if (error != GL_TRUE) {
        qDebug() << "OpenGL ERROR: " << shaderName << "\n" << message << "\n";
        shader = 0;
    }
    return shader;
}

GLuint Drawable::linkProgram(GLuint program) {
    glLinkProgram(program);

    std::string log;
    GLint e, l;
    glGetProgramiv(program, GL_LINK_STATUS, &e);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &l);
    if (l > 0) {
        char *tmplog = new char[l];
        glGetProgramInfoLog(program, l, NULL, tmplog);
        /*kill_crlf(tmplog);*/
        log = std::string(tmplog);
        delete[] tmplog;
    } else {
        log = std::string("");
    }

    if (e && log.length() > 0) {
        qDebug() << "OpenGL program '%s': WARNING:\n" << log;
    } else if (e != GL_TRUE) {
        qDebug() << "OpenGL program '%s': ERROR:\n" << log;
        program = 0;
    }
    return program;
}

GLuint Drawable::loadTexture(std::string path, TextureType type) {
    ImageTexture image(path);

    GLuint textureID;

    glActiveTexture(GL_TEXTURE0);

    // Generate and bind texture.
    // Allocate one texture, and assign the openGL handle (akin to a pointer).
    glGenTextures(1, &textureID);
    // Makes all following texture methods work on the bound texture.
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Assign image data.
    switch (type) {
        case Monochrome:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         image.getData());
            break;

        case RGB:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         image.getData());
            break;

        case NormalMap:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         image.getData());
            break;

        case SRGB:
        default:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, image.getWidth(), image.getHeight(), 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, image.getData());
            break;
    }

    // Set texture parameters.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Make magnification use nearest, in order to preserve pixel look.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Retrieve maximum supported anisotropy level, and set it.
    GLfloat maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

    return textureID;
}

GLuint Drawable::loadPositions(std::vector<glm::vec3> positions) {
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    return positionBuffer;
}

GLuint Drawable::loadTextureCoordinates(std::vector<glm::vec2> coordinates) {
    GLuint textureCoordinateBuffer;
    glGenBuffers(1, &textureCoordinateBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
    // Multiply with 2, as it is only vec2.
    glBufferData(GL_ARRAY_BUFFER, coordinates.size() * 2 * sizeof(float), coordinates.data(), GL_STATIC_DRAW);
    // Use indices of 2.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    return textureCoordinateBuffer;
}

GLuint Drawable::loadNormals(std::vector<glm::vec3> normals) {
    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    return normalBuffer;
}

GLuint Drawable::loadIndices(std::vector<unsigned int> indices) {
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    return indexBuffer;
}

void Drawable::bindVertexArrayObject() {
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);
}
