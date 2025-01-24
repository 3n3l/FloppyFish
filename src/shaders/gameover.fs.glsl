#version 410 core

in vec2 fragTexCoord;
uniform sampler2D texSampler;

out vec4 color;

void main() {
    color = texture(texSampler, fragTexCoord);
}
