#define GL_SILENCE_DEPRECATION

#include "src/drawables/drawable.h"

#include <QFile>
#include <QOpenGLShaderProgram>

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

GLuint Drawable::loadTexture(std::string path) {
    ImageTexture image(path);

    GLuint textureID;

    glActiveTexture(GL_TEXTURE0);

    // Generate and bind texture.
    // Allocate one texture, and assign the openGL handle (akin to a pointer).
    glGenTextures(1, &textureID);

    // Makes all following texture methods work on the bound texture.
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Assign image data.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image.getData());

    // Set texture parameters.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Retrieve maximum supported anisotropy level, and set it.
    GLfloat maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

    return textureID;
}
