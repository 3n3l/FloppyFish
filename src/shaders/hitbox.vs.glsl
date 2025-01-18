#version 410 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Get position from vertex array object.
layout (location = 0) in vec3 position;


void main(void) {

    // Calculate position in model view projection space.
    vec4 worldPosition = vec4(modelview_matrix * vec4(position, 1));

    // Set the position in homogeneous coordinates.
    gl_Position = projection_matrix * worldPosition;
}
