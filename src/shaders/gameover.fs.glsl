#version 410 core

in vec2 frag_tex_coord;
uniform sampler2D tex_sampler;

out vec4 f_colour;

void main() {
    f_colour = texture(tex_sampler, frag_tex_coord);
}
