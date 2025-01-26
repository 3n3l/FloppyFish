#version 410 core

layout(location = 0) in vec3 position;   // Vertex position
layout(location = 2) in vec2 texCoord;   // Texture coordinates

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

out vec2 fragTexCoord;

void main() {
    gl_Position = projection_matrix * modelview_matrix * vec4(position, 1.0);
    fragTexCoord = texCoord;
}