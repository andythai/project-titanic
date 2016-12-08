#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texCoord;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 0.1f;
    TexCoords = texCoord;
    ParticleColor = color;
    gl_Position = projection * modelview * vec4((vertex.xyz*scale) + offset, 1.0);
}