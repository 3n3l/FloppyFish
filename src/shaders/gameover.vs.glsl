#version 410 core

layout (location = 0) in vec3 position;   // Vertex position
layout (location = 2) in vec2 tex_coord;   // Texture coordinates

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

out vec2 frag_tex_coord;

void main() {
    // Pass the texture coordinates to the fragment shader.
    frag_tex_coord = tex_coord;

    // Apply the transformation matrices and set the vertex position.
    vec4 vcPos = vec4(projection_matrix * modelview_matrix * vec4(position, 1.0));

    gl_Position = vec4(vcPos.x, vcPos.y, -0.9f, vcPos.w);
}
