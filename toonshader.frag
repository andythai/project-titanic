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

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 FragPos;
in vec3 Normal;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{
	if (on == 1) { // Use toon shading or not
		vec3 norm = normalize(Normal);
		vec3 l = normalize(dirLight.direction);
		float e_x = -view[3][0];
		float e_y = -view[3][1];
		float e_z = -view[3][2];
		vec3 viewPos = {e_x, e_y, e_z};
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 result = material.ambient;
		
		// Diffuse switch
		float diffuse = max(0.0f, dot(norm, l));
		if (diffuse > 0.75f) {
			// Do nothing
		}
		else if (diffuse > 0.5f) {
			result = result * 0.6f;
		}
		else if (diffuse > 0.25f) {
			result = result * 0.4f;
		}
		else {
			result = result * 0.3f;
		}
		

		// Edge silhouette
		float edge = max(0.0f, dot(norm, viewDir));
		if (edge < 0.01f) {
			result.r = 0.0f;
			result.g = 0.0f;
			result.b = 0.0f; 
		}

		// Output
		color = vec4(result.r, result.g, result.b, 1.0f);
	}
	else {
		color = vec4(material.ambient.r, material.ambient.g, material.ambient.b, 1.0f);
	}
}