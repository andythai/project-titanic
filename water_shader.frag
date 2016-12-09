#version 330 core
// This is a sample fragment shader.

const float Eta = 0.15;
float kDistortion = 0.015;
float kReflection = 0.01;


// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;

in vec3 v_incident;
in vec3 worldNormal;
in vec3 eyeNormal;
in vec4 pos;
in vec3 worldPosition;

in float passedTime;
in vec2 v_texCoord;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

// Uniform
uniform vec3 camPos;
uniform samplerCube skybox;
uniform sampler2D dudv;
uniform sampler2D normalMap;
uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform vec3 LightDirection;
uniform bool show_reflect;
uniform bool show_refract;


//light
vec4 tangent = vec4(1.0, 0.0, 0.0, 0.0);
vec4 lightNormal = vec4(0.0, 1.0, 0.0, 0.0);
vec4 biTangent = vec4(0.0, 0.0, 1.0, 0.0);

vec4 baseColor = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{

/*
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
    //color = vec4(0.0f, 0.0f, 1.0f, sampleExtraOutput);
	//vec3 worldIncident = normalize(v_incident);
	vec3 eye = normalize(worldPosition - camPos);

	//vec3 refraction = refract(eye, worldNormal, Eta);
	vec3 reflection = reflect(eye, vec3(0.0, 1.0, 0.0));

	//vec4 refractionColor = texture(skybox, refraction);
	vec4 reflectionColor = texture(skybox, reflection);

	//float fresnel = Eta + (1.0 - Eta)*pow(max(0.0, 1.0- dot (-worldIncident, worldNormal)), 5.0);

	//color = mix(refractionColor, reflectionColor, fresnel);
	color = reflectionColor;
	//color = refractionColor;
	//color.a = 0.5;
*/	

	// Light Tangent Space //
    vec4 lightDirection = normalize(vec4(LightDirection.xyz, 1.0));
    vec4 lightTanSpace = normalize(vec4(dot(lightDirection, tangent), dot(lightDirection, biTangent), dot(lightDirection, lightNormal), 1.0));

	// Fresnel Term //
    vec4 distOffset = texture(dudv, v_texCoord + vec2(passedTime)) * kDistortion;
    vec4 normal = texture(normalMap, vec2(v_texCoord + distOffset.xy));
    normal = normalize(normal * 2.0 - 1.0);
    normal.a = 0.81;

	vec4 lightReflection = normalize(reflect(-1 * lightTanSpace, normal));
    vec4 invertedFresnel = vec4(dot(normal, lightReflection));
    vec4 fresnelTerm = 1.0 - invertedFresnel;

	// Reflection //
    vec4 dudvColor = texture(dudv, vec2(v_texCoord + distOffset.xy));
    dudvColor = normalize(dudvColor * 2.0 - 1.0) * kReflection;

	vec3 eye = normalize(worldPosition - camPos);
	vec3 refraction = refract(eye, vec3(0.0, 1.0, 0.0), 0.9);
	if (!show_refract) {
		refraction = vec3(0.0f, 0.0f, 0.0f);
	}

	vec3 reflection = reflect(eye, vec3(0.0, 1.0, 0.0));
	if (!show_reflect) {
		reflection = vec3(0.0f, 0.0f, 0.0f);
	}

	// Projection Coordinates
    vec4 tmp = vec4(1.0 / pos.w);
    vec4 projCoord = vec4(reflection,1.0);
    projCoord += vec4(1.0);
    projCoord *= vec4(0.5);
    projCoord += dudvColor;
    projCoord = clamp(projCoord, 0.0, 1.0);

	//vec4 reflectionColor = mix(texture(skybox, vec3(projCoord.x, projCoord.y, projCoord.z)), vec4(1.0,1.0,1.0,1.0), 0.3);
	vec4 reflectionColor = texture(skybox, reflection);
    reflectionColor *= fresnelTerm;

	vec4 projCoord2 = vec4(refraction, 1.0f);
    projCoord2 += vec4(1.0);
    projCoord2 *= vec4(0.5);
    projCoord2 += dudvColor;
    projCoord2 = clamp(projCoord2, 0.0, 1.0);

	 // Refraction //
    vec4 refractionColor = texture(skybox, vec3(-projCoord2.x, projCoord2.y, projCoord2.z));
    vec4 depthValue = vec4(0.1, 0.1, 0.1, 1.0);
    vec4 invDepth = 1.0 - depthValue;

	refractionColor *= invertedFresnel * invDepth;
    refractionColor += depthValue * invertedFresnel;
	refractionColor = texture(skybox, vec3(refraction));
	refractionColor *= invertedFresnel * invDepth;
    refractionColor += depthValue * invertedFresnel;

	//color = mix(reflectionColor, refractionColor, 0.2);
	color = (reflectionColor + refractionColor);
	//color = reflectionColor;
	//color = refractionColor;
	//color = vec4(v_texCoord.x,0.0, v_texCoord.y, 1.0);
}
