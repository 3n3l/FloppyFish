#version 410 core

uniform sampler2D colour_buffer;

// Send colour to screen.
layout (location = 0) out vec4 f_colour;

smooth in vec2 v_tex_coords;

void main(void)
{
    vec4 tex_colour = texture(colour_buffer, vec2(v_tex_coords.x, v_tex_coords.y));
    f_colour = tex_colour;
}
