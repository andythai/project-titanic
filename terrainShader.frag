#version 330

smooth in vec3 thePosition;
out vec4 outputColor;

void main()
{
	outputColor = vec4(50.0/255.0, (120 + thePosition.y*10)/255.0, 50.0/255.0, 1.0);
}