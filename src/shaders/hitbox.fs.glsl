#version 410 core

// Get texture from linked program.
uniform vec3 hitboxColour;

// Send color to screen.
layout (location = 0) out vec4 fColour;

void main(void) {
    // Set fragment colour.
    fColour = vec4(hitboxColour, 1.0f);
}
