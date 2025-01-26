#version 410 core

// Get position from vertex array object.
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 tex_coords;

smooth out vec2 v_tex_coords;

void main(void)
{
    v_tex_coords = tex_coords;

    // Just pass the position to the FS.
    gl_Position = vec4(position, 1.0f);
}
