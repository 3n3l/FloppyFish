#include "src/utils/utils.h"

#include <QFile>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLVersionFunctionsFactory>

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

void geom_cube(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texcoords,
               std::vector<unsigned int> &indices) {
    static const float p[] = {
        -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f,  // front
        -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  // back
        +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f,  // right
        -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f,  // left
        -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f,  // top
        -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f   // bottom
    };
    static const float n[] = {
        0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f,  // front
        0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f,  // back
        +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,   // right
        -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,   // left
        0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,  0.0f,  +1.0f, 0.0f,   // top
        0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f    // bottom
    };
    static const float t[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // front
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // back
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // right
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // left
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // top
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom
    };
    static const unsigned int i[] = {
        0,  1,  3,  1,  2,  3,   // front face
        4,  5,  7,  5,  6,  7,   // back face
        8,  9,  11, 9,  10, 11,  // right face
        12, 13, 15, 13, 14, 15,  // left face
        16, 17, 19, 17, 18, 19,  // top face
        20, 21, 23, 21, 22, 23,  // bottom face
    };
    static const glm::vec3 *vp = reinterpret_cast<const glm::vec3 *>(p);
    static const glm::vec3 *vn = reinterpret_cast<const glm::vec3 *>(n);
    static const glm::vec2 *vt = reinterpret_cast<const glm::vec2 *>(t);
    positions.assign(vp, vp + sizeof(p) / sizeof(glm::vec3));
    normals.assign(vn, vn + sizeof(n) / sizeof(glm::vec3));
    texcoords.assign(vt, vt + sizeof(t) / sizeof(glm::vec2));
    indices.assign(i, i + sizeof(i) / sizeof(unsigned int));
}

}  // namespace Utils
