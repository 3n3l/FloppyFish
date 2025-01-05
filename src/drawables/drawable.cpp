#include "src/drawables/drawable.h"

#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/gltypes.h>

#include <QDebug>
#include <QFile>
#include <QOpenGLShaderProgram>
#include <QTextStream>

Drawable::Drawable() : _vertexArrayObject(0) {}
Drawable::~Drawable() {}
