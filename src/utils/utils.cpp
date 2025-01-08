#include "src/utils/utils.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLVersionFunctionsFactory>

#include "src/utils/image.h"
namespace Utils {

void _glCheckError(const char *file, int line) {
    QOpenGLFunctions_4_1_Core *gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(QOpenGLContext::currentContext());
    GLenum errorCode;
    while ((errorCode = gl->glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        qDebug() << error << " | " << file << " (" << line << ")";
    }
}

GLuint compileShader(GLenum type, const std::string &path) {
    QOpenGLFunctions_4_1_Core *gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(QOpenGLContext::currentContext());

    QFile f(path.c_str());
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Could not open file: " << path;
    }
    QTextStream in(&f);
    std::string src = in.readAll().toStdString();

    GLuint shader = gl->glCreateShader(type);
    const GLchar *glsrc = src.c_str();
    gl->glShaderSource(shader, 1, &glsrc, NULL);
    gl->glCompileShader(shader);

    std::string message;
    GLint error, length;
    gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &error);
    gl->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        char *tmplog = new char[length];
        gl->glGetShaderInfoLog(shader, length, NULL, tmplog);
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

GLuint linkProgram(GLuint program) {
    QOpenGLFunctions_4_1_Core *gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(QOpenGLContext::currentContext());
    gl->glLinkProgram(program);

    std::string log;
    GLint e, l;
    gl->glGetProgramiv(program, GL_LINK_STATUS, &e);
    gl->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &l);
    if (l > 0) {
        char *tmplog = new char[l];
        gl->glGetProgramInfoLog(program, l, NULL, tmplog);
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

GLuint loadTexture(std::string path) {
    QOpenGLFunctions_4_1_Core *gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(QOpenGLContext::currentContext());
    ImageTexture image(path);

    GLuint textureID;

    gl->glActiveTexture(GL_TEXTURE0);

    // Generate and bind texture.
    // Allocate one texture, and assign the openGL handle (akin to a pointer).
    gl->glGenTextures(1, &textureID);

    // Makes all following texture methods work on the bound texture.
    gl->glBindTexture(GL_TEXTURE_2D, textureID);

    // Assign image data.
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     image.getData());

    // Set texture parameters.
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    // Retrieve maximum supported anisotropy level, and set it.
    GLfloat maxAnisotropy;
    gl->glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

    return textureID;
}

}  // namespace Utils
