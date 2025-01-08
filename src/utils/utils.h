#ifndef UTILS_H
#define UTILS_H

#include <QOpenGLFunctions>
#include <cstdlib>

namespace Utils {

/**
 * @brief Checks for ALL OpenGL errors and prints them as clear sentences.
 * @param file - the file this was called from.
 * @param line - the line this was called from
 */
void _glCheckError(const char* file, int line);

}  // namespace Utils

// A macro to print the OpenGL error, the file and the line this macro was called from.
#define glCheckError() Utils::_glCheckError(__FILE__, __LINE__)

#endif
