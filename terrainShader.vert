#version 330

uniform mat4 projection;
uniform mat4 modelview;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inIndices;

smooth out vec3 thePosition; // Interpolate position among fragments
out vec3 normals;

void main()
{
  gl_Position = projection * modelview * vec4(inPosition, 1.0);
  thePosition = inPosition;
  normals = inNormal;
}