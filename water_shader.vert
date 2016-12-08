#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

const float PI = 3.1415926535;
const float G = 9.81;

layout (location = 0) in vec3 position;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float sampleExtraOutput;

//clipping plane
uniform vec4 reflectPlane;
uniform vec4 refractPlane;

//uniforms on wave!!!!!!
uniform float u_time;
uniform float u_waterPlaneLength;

uniform float amplitudes[4];
uniform float waveLengths[4];
uniform float speeds[4];

uniform vec2 wave_direction[4];

out vec3 v_incident;
out vec3 worldNormal;
out vec3 eyeNormal;

out float pasedTime;
out vec3 worldPosition;
out vec4 pos;
out float passedTime;
out vec2 v_texCoord;

//function prototypes
float wave(int i, float x, float y);
float waveHeight(float x, float y);
float dWavedx(int i, float x, float y);
float dWavedy(int i, float x, float y);
vec3 waveNormal(float x, float y);

void main()
{

    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
    sampleExtraOutput = 1.0f;
	worldPosition = vec3(model * vec4(position.x, position.y, position.z, 1.0));

	//gl_ClipDistance[0] = dot(worldPosition, reflectPlane);

	//time to fuck this wave up
	
	vec4 finalVertex = vec4(position, 1.0f);
	/*
	vec3 finalBitangent = (0.0, 0.0, 0.0);
	vec3 finalNormal = (0.0, 0.0, 0.0);
	vec3 finalTangent = (0.0, 0.0, 0.0);

	for (int i = 0; i < 4; i++)
	{
		vec2 direction = normalize(wave_direction[i]);

		float speed = speed[i];
		float amplitude = amplitudes[i];
		float wavelength = wavelengths[i];

		float frequency = sqrt(G*2.0*PI/wavelength);
		float phase = speed*frequency;
		float alpha = frequency*dot(direction, position.xz)+phase*u_time;
		
		finalVertex.x += amplitude*direction.x*cos(alpha);
		finalVertex.y += amplitude*sin(alpha);
		finalVertex.z += amplitude*direction.y*cos(alpha);
	}

	for (int i = 0; i < 4; i++)
	{
		vec2 direction = normalize(wave_direction[i]);

		float speed = speed[i];
		float amplitude = amplitudes[i];
		float wavelength = wavelengths[i];

		float frequency = sqrt(G*2.0*PI/wavelength);
		float phase = speed*frequency;
		float alpha = frequency * dot(direction, finalVertex.xz) + phase*u_time;
				
		// x direction
		finalBitangent.x += direction.x*direction.x * wavelength * amplitude * sin(alpha);
		finalBitangent.y += direction.x * wavelength * amplitude * cos(alpha);	
		finalBitangent.z += direction.x*direction.y * wavelength * amplitude * sin(alpha);	
	
		// y direction
		finalNormal.x += direction.x * wavelength * amplitude * cos(alpha);
		finalNormal.y += wavelength * amplitude * sin(alpha);
		finalNormal.z += direction.y * wavelength * amplitude * cos(alpha);

		// z direction
		finalTangent.x += direction.x*direction.y * wavelength * amplitude * sin(alpha);
		finalTangent.y += direction.y * wavelength * amplitude * cos(alpha);	
		finalTangent.z += direction.y*direction.y * wavelength * amplitude * sin(alpha);	
	}

	finalTangent.x = -finalTangent.x;
	finalTangent.z = 1.0 - finalTangent.z;
	finalTangent = normalize(finalTangent);

	finalBitangent.x = 1.0 - finalBitangent.x;
	finalBitangent.z = -finalBitangent.z;
	finalBitangent = normalize(finalBitangent);

	finalNormal.x = -finalNormal.x;
	finalNormal.y = 1.0 - finalNormal.y;
	finalNormal.z = -finalNormal.z;
	finalNormal = normalize(finalNormal);

	v_bitangent = finalBitangent;
	v_normal = finalNormal;
	v_tangent = finalTangent;
	*/

	//texcoord
	//v_texCoord = vec2(clamp((finalVertex.x+u_waterPlaneLength*0.5-0.5)/u_waterPlaneLength, 0.0, 1.0), clamp((-finalVertex.z+u_waterPlaneLength*0.5+0.5)/u_waterPlaneLength, 0.0, 1.0));
	//v_texCoord = vec2((finalVertex.x+u_waterPlaneLength*0.5)/u_waterPlaneLength, (u_waterPlaneLength*0.5-finalVertex.z)/u_waterPlaneLength);
	//v_texCoord = vec2(clamp(position.x, 0.0, 1.0),clamp(position.z, 0.0, 1.0));
	v_texCoord = vec2((position.x+1)/2, (1-position.z)/2);
	vec4 vertex = modelview*vec4(position, 1.0f);
	
	// We caculate in world space.
	v_incident = mat3(transpose(inverse(model))) * vec3(vertex);

	/*
	vec4 pos = vec4(position,1.0f);
	pos.z = waveHeight(pos.x, pos.y);
	worldNormal = waveNormal(pos.x, pos.y);
	eyeNormal = mat3(transpose(inverse(model))) * worldNormal;
	//eyeNormal = mat3(transpose(inverse(model))) * vec3(0.0, 1.0, 0.0);
	*/
	pos = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	passedTime = u_time/5.0;
}

float wave(int i, float x, float y) {
    float frequency = 2*PI/waveLengths[i];
    float phase = speeds[i] * frequency;
    float theta = dot(wave_direction[i], vec2(x, y));
    return amplitudes[i] * sin(theta * frequency + u_time * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < 4; ++i)
        height += wave(i, x, y);
    return height;
}

float dWavedx(int i, float x, float y) {
    float frequency = 2*PI/waveLengths[i];
    float phase = speeds[i] * frequency;
    float theta = dot(wave_direction[i], vec2(x, y));
    float A = amplitudes[i] * wave_direction[i].x * frequency;
    return A * cos(theta * frequency + u_time * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*PI/waveLengths[i];
    float phase = speeds[i] * frequency;
    float theta = dot(wave_direction[i], vec2(x, y));
    float A = amplitudes[i] * wave_direction[i].y * frequency;
    return A * cos(theta * frequency + u_time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < 4; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

