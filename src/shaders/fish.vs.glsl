#version 410 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Get position from vertex array object.
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

// Send texture coordinates to fragment shader.
smooth out vec2 vTexCoords;

void main(void) {
    // Pass the generated texture coordinates to the FS.
    vTexCoords = texCoords;

    // Calculate position in model view projection space.
    vec4 worldPosition = modelview_matrix * vec4(position, 1);

    // Set the position in homogeneous coordinates.
    gl_Position = projection_matrix * worldPosition;
}
