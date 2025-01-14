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

}  // namespace Utils
