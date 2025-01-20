#version 410 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Get position from vertex array object.
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

// Send texture coordinates to fragment shader.
smooth out vec2 vTexCoords;

void main(void) {
    // Pass the generated texture coordinates to the FS.
    vTexCoords = texCoords;

    // Calculate position in model view projection space.
    vec4 worldPosition = vec4(modelview_matrix * vec4(position, 1));

    // Set the position in homogeneous coordinates,
    // and force the background to be in the back at all times.
    vec4 pointPosition = vec4(projection_matrix * worldPosition);
    gl_Position = vec4(pointPosition.x, pointPosition.y, 0.99f, pointPosition.w);
}
