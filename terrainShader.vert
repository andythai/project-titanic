#version 330

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inIndices;

smooth out vec3 thePosition; // Interpolate position among fragments
out vec3 normals;
out vec3 FragPos;

void main()
{
  gl_Position = projection * modelview * vec4(inPosition, 1.0);
  thePosition = inPosition;
  normals = mat3(transpose(inverse(model))) * inNormal;
  FragPos = vec3(model * vec4(inPosition, 1.0f));
}