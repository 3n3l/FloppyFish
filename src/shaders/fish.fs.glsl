#version 410 core

// Get properties from vertex shader.
smooth in vec2 vTexCoords;

// Get texture from linked program.
uniform sampler2D backgroundTexture;

// Send color to screen.
layout(location = 0) out vec4 fcolor;

void main(void) {
    // Set fragment color.
    // fcolor = texture(backgroundTexture, vec2(vTexCoords.s, -vTexCoords.t));
    fcolor = vec4(0.5, 0, 0.5, 1);
}