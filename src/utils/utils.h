#ifndef UTILS_H
#define UTILS_H

#include <QOpenGLFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Utils {

/**
 * @brief Checks for ALL OpenGL errors and prints them as clear sentences.
 * @param file - the file this was called from.
 * @param line - the line this was called from
 */
void _glCheckError(const char* file, int line);

/**
 * Generates a cube.
 * @param positions of the generated cube.
 * @param normals of the generated cube.
 * @param texcoords of the generated cube.
 * @param indices of the generated cube.
 */
void geom_cube(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texcoords,
               std::vector<unsigned int>& indices);

}  // namespace Utils

// A macro to print the OpenGL error, the file and the line this macro was called from.
#define glCheckError() Utils::_glCheckError(__FILE__, __LINE__)

#endif
