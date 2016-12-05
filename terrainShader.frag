#version 330

smooth in vec3 thePosition;
in vec3 normals;
out vec4 outputColor;

uniform int normal_toggle;
uniform int type;

void main()
{
  if (normal_toggle == 0) {
    if (type == 1) {
      if (thePosition.y > 20) {
        outputColor = vec4((160 + thePosition.y * 2)/255.0, (160 + thePosition.y * 2)/255.0, (160 + thePosition.y * 2)/255.0, 1.0);
      }
      else {
        outputColor = vec4((75 + thePosition.y * 2)/255.0, (75 + thePosition.y * 2)/255.0, 50/255.0, 1.0);
      }
    }
    else if (type == 2) {
      if (thePosition.y < 0) {
        outputColor = vec4((100 + thePosition.y*10)/255.0, (100 + thePosition.y*10)/255.0, 200.0/255.0, 1.0);
      }
      else {
        outputColor = vec4((160 + thePosition.y*10)/255.0, (160 + thePosition.y*10)/255.0, 80.0/255.0, 1.0);
      }
    }
    else if (type == 3) {
      outputColor = vec4(75.0/255.0, (160 + thePosition.y*20)/255.0, 75.0/255.0, 1.0);
    }
    else {
      outputColor = vec4(50.0/255.0, (120 + thePosition.y*10)/255.0, 50.0/255.0, 1.0);
    }
  }
  else {
    outputColor = vec4(normals.x, normals.y, normals.z, 1.0f);
  }
}