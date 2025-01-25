#version 410 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Get position from vertex array object.
layout (location = 0) in vec3 position;

// Send direction to sample cubemap to fragment shader.
smooth out vec3 direction;

void main(void)
{
    // Just pass the position to the FS.
    gl_Position = vec4(position, 1.0f);
}
