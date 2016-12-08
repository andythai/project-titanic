#version 330

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

smooth in vec3 thePosition;
in vec3 normals;
in vec3 FragPos;

out vec4 outputColor;

uniform DirLight dirLight;
uniform int normal_toggle;
uniform int type;
uniform int ter_color;
uniform mat4 view;
uniform vec3 cam_pos;
uniform vec3 cam_look_at;

void main()
{
	vec3 norm = normalize(normals);
	if (normal_toggle == 0) {

		// Toon shading
		vec3 l = normalize(dirLight.direction);
		/*
		float e_x = -view[3][0];
		float e_y = -view[3][1];
		float e_z = -view[3][2];
		*/
		//vec3 viewPos = vec3(cam_pos);
		//vec3 viewDir = normalize(cam_pos - FragPos);
		vec3 viewDir = vec3(FragPos - cam_pos);

		// Colors
		vec4 grass = vec4(75.0/255.0, (160 + thePosition.y*20)/255.0, 75.0/255.0, 1.0);
		vec4 desert = vec4((160 + thePosition.y*10)/255.0, (160 + thePosition.y*10)/255.0, 80.0/255.0, 1.0) * 0.65f;
		vec4 snow = vec4((160 + thePosition.y * 2)/255.0, (160 + thePosition.y * 2)/255.0, (160 + thePosition.y * 2)/255.0, 1.0);
		vec4 mountain = vec4((75 + thePosition.y * 2)/255.0, (75 + thePosition.y * 2)/255.0, 50/255.0, 1.0);
		vec4 snow_mountain = vec4((40 + thePosition.y * 2)/255.0, (40 + thePosition.y * 2)/255.0, (40 + thePosition.y * 2)/255.0, 1.0);


		if (ter_color == 1) {
			outputColor = mountain;
		}

		else if (ter_color == 2) {
			outputColor = desert;
		}

		else {
			outputColor = snow;
		}

		// Mountain
		if (type == 1) {
			if (thePosition.y > 20) { // MOUNTAIN TIP
				if (ter_color == 1) {
					outputColor = snow;
				}
				else if (ter_color == 2) {
					outputColor = desert;
				}
				else {
					outputColor = snow;
				}
			}
			else if (ter_color == 1) {
				outputColor = mountain;
			}
			else if (ter_color == 3) {
				outputColor = snow_mountain;
			}
		}

		// Oasis
		else if (type == 2) {
			if (thePosition.y < 0) { // BLUE WATER
				outputColor = vec4((100 + thePosition.y*10)/255.0, (100 + thePosition.y*10)/255.0, 200.0/255.0, 1.0);
			}
		}

		// No clue
		else if (type != 1 && type != 2 && type != 3) {
			outputColor = vec4(50.0/255.0, (120 + thePosition.y*10)/255.0, 50.0/255.0, 1.0);
		}

		// Diffuse switch
		float diffuse = max(0.0f, dot(norm, l));
		if (diffuse > 0.75f) {
			// Do nothing
		}
		else if (diffuse > 0.5f) {
			outputColor = outputColor * 0.6f;
		}
		else if (diffuse > 0.25f) {
			outputColor = outputColor * 0.4f;
		}
		else {
			outputColor = outputColor * 0.3f;
		}
		/*
		// Edge silhouette
		float edge = max(0.0f, dot(norm, viewDir));
		if (edge < 0.01f) {
			outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		*/
	}



	else {
		outputColor = vec4(norm, 1.0f);
	}
}