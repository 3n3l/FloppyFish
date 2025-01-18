#version 410 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Get position from vertex array object.
layout (location = 0) in vec3 position;

// Send direction to sample cubemap to fragment shader.
smooth out vec3 direction;

void main(void)
{
    // Calculate position in model view projection space.
    vec4 positionVC = vec4(modelview_matrix * vec4(100.0f * position, 1.0f));
    // Set the z to w (1.0), so the skybox is always at the far-plane.

    direction = vec3(inverse(modelview_matrix) * vec4(positionVC));

    positionVC = projection_matrix * positionVC;
    gl_Position = vec4(positionVC.x, positionVC.y, positionVC.w, positionVC.w);
}
