#version 330 core
// This is a sample fragment shader.

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform int on;
uniform Material material;
uniform mat4 view;
uniform vec3 cam_pos;
uniform vec3 cam_look_at;
uniform int toggle_toon;

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 FragPos;
in vec3 Normal;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{
	if (on == 0) { // Toon shading toggle
		vec3 norm = normalize(Normal);
		vec3 l = normalize(dirLight.direction);
		/*
		float e_x = -view[3][0];
		float e_y = -view[3][1];
		float e_z = -view[3][2];
		*/
		//vec3 viewPos = vec3(cam_pos.x, cam_pos.y, cam_pos.z);
		//vec3 viewDir = normalize(viewPos - FragPos);
		vec3 viewDir = normalize(FragPos - cam_pos);
		vec3 result = material.ambient;
		
		// Diffuse switch
		if (toggle_toon == 1) {
			float diffuse = max(0.0f, dot(norm, l));
			if (diffuse > 0.75f) {
				result = result * 0.9f;
			}
			else if (diffuse > 0.5f) {
				result = result * 0.8f; // 0.6
			}
			else if (diffuse > 0.25f) {
				result = result * 0.7f; // 0.4
			}
			else {
				result = result * 0.6f; // 0.3
			}
		}
		

		// Output
		color = vec4(result.r, result.g, result.b, 1.0f);
	}
	else {
		color = vec4(material.ambient.r, material.ambient.g, material.ambient.b, 1.0f);
	}
}
