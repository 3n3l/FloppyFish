#version 410 core

// Get position from vertex array object.
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

// Send texture coordinates to fragment shader.
smooth out vec2 vTexCoords;

void main(void) {
    // Pass the generated texture coordinates to the FS.
    vTexCoords = texCoords;

    // Set the position in homogeneous coordinates.
    gl_Position = vec4(position, 1.0);
}
