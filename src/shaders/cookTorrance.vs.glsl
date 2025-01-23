#version 410 core

// NOTE: this must be the same as Config::obstacleAmount
#define NUM_LIGHTS 5

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// Lighting.
uniform vec3 light_position[NUM_LIGHTS];
smooth out float vLightDistance[NUM_LIGHTS];
smooth out vec3 vLightDir[NUM_LIGHTS];

// Get position from vertex array object.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

// Send texture coordinates and lighting values to fragment shader.
smooth out vec2 vTexCoords;
smooth out vec3 vNormal;
smooth out vec3 vView;

void main(void)
{
    // Calculate position in model view projection space.
    vec4 worldPosition = vec4(modelview_matrix * vec4(position, 1.0f));

    // Pass the generated texture coords to the FS.
    vTexCoords = texCoords;

    // Normal per vertex.
    vNormal = normalize(vec3(inverse(transpose(modelview_matrix)) * vec4(normal, 0)));

    // View vector / direction. Due to the camera being at 0 in the view-space it is 0-Position.
    vView = normalize(-(vec3(worldPosition.x, worldPosition.y, worldPosition.z) / worldPosition.w));

    // Point-light.
    for (int i = 0; i < NUM_LIGHTS; i++) {
        // Direction towards the light, aka -Omega_in.
        vLightDir[i] = normalize(light_position[i] - worldPosition.xyz / worldPosition.w);
        // Distance for light attenuation.
        vLightDistance[i] = length(light_position[i] - worldPosition.xyz / worldPosition.w);
    }

    gl_Position = projection_matrix * worldPosition;
}
