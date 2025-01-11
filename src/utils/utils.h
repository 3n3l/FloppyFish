#ifndef UTILS_H
#define UTILS_H

#include <OpenGL/gltypes.h>

#include <cstdio>
#include <cstdlib>
#include <string>

namespace Utils {
/**
 * @brief Compile a shader and print warnings/errors if necessary.
 * @param type - GLenum describing the shader type.
 * @param path - string holding the location of the shader.
 * @return the shader handle on success or 0 on failure.
 */
GLuint compileShader(GLenum type, const std::string& path);

/**
 * @brief Link the shader program 'prg' and print warnings/errors if necessary.
 * @param program - the program
 * @return the program on success, or 0 on error.
 */
GLuint linkProgram(GLuint program);

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
void geom_cube(
        std::vector<glm::vec3>& positions,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texcoords,
        std::vector<unsigned int>& indices);

}  // namespace Utils

// A macro to print the OpenGL error, the file and the line this macro was called from.
#define glCheckError() Utils::_glCheckError(__FILE__, __LINE__)

#endif
