#include <OpenGL/gl.h>
#include <OpenGL/gltypes.h>

#include <QDebug>
#include <QFile>

namespace Utils {

void _glCheckError(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        qDebug() << error << " | " << file << " (" << line << ")";
    }
}

GLuint compileShader(GLenum type, const std::string &path) {
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

GLuint linkProgram(GLuint program) {
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

}  // namespace Utils
