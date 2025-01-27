#version 410 core

uniform sampler2D colour_buffer;
uniform float gamma;

// Send colour to screen.
layout (location = 0) out vec4 f_colour;

smooth in vec2 v_tex_coords;

vec3 aces_tonemap(vec3 colour) {
    mat3 m1 = mat3(
    0.59719, 0.07600, 0.02840,
    0.35458, 0.90834, 0.13383,
    0.04823, 0.01566, 0.83777
    );
    mat3 m2 = mat3(
    1.60475, -0.10208, -0.00327,
    -0.53108, 1.10813, -0.07276,
    -0.07367, -0.00605, 1.07602
    );
    vec3 v = vec3(m1 * colour);
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / (2.2 / gamma)));
}

void main(void)
{
    vec4 tex_colour = texture(colour_buffer, vec2(v_tex_coords.x, v_tex_coords.y));
    vec3 aces_colour = aces_tonemap(tex_colour.rgb);
    f_colour = vec4(aces_colour, 1.0f);
    f_colour.a = 1.0f;
    //        f_colour.rgb = pow(tex_colour.rgb, vec3(1.0 / 2.2));
    //    f_colour = tex_colour;
}

