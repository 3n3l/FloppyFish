#version 410 core

// Get properties from vertex shader.
smooth in vec2 vTexCoords;

// Get texture from linked program.
uniform sampler2D backgroundTexture;
uniform float animationLooper;

// Send colour to screen.
layout (location = 0) out vec4 fColour;

void main(void) {
    // Set fragment colour.
    fColour = texture(backgroundTexture, vec2(vTexCoords.s + animationLooper, -vTexCoords.t));
}
