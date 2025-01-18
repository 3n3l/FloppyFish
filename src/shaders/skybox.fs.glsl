#version 410 core

uniform samplerCube skyboxTexture;

// Send color to screen.
layout (location = 0) out vec4 fcolour;

smooth in vec3 direction;

void main(void)
{
    // Modifier to make the stars have more contrast.
    float modifier = 1.5f;
    // Modifier to make the stars darker.
    float factor = 1.0f;

    vec4 texColor = texture(skyboxTexture, direction);
    fcolour = vec4(pow(texColor.r, modifier) * factor,
    pow(texColor.g, modifier) * factor,
    pow(texColor.b, modifier) * factor,
    texColor.a);
}
