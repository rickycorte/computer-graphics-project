#version 450

layout(set=0, binding = 0) uniform LightBufferObject {
	vec3 globalLightDir;
	vec3 globalLightColor;

	vec3 ambientTopColor;
	vec3 ambientBottomColor;

	vec3 pointlightPos;
	vec3 pointlightColor;
	vec3 pointlightSettings;

	vec3 spotlightPos;
	vec3 spotlightDir;
	vec3 spotlightColor;
	vec4 spotlightSettings;

	vec4 specularSettings;
} lbo;

layout(set=1, binding = 1) uniform sampler2D texSampler;


layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

// distace = g
// beta = exponential decay factor

vec3 point_light_color(vec3 fragPos, vec3 lightPos, float dist, float beta, vec3 lightColor) {

	float decay = dist / length(lightPos - fragPos);
	decay = pow(decay, beta);
	return decay * lightColor;
}

vec3 spot_light_color(vec3 fragPos, vec3 lightPos, vec3 lightDir, float dist, float beta, float cos_in, float cos_out, vec3 lightColor) 
{
	vec3 coeff = point_light_color(fragPos, lightPos, dist, beta, lightColor); // covers light color * coeff part of formula
	float dimming = clamp((dot(normalize(lightPos - fragPos), lightDir) - cos_out) / (cos_in - cos_out), 0.0f, 1.0f);
	
	return coeff * dimming;
}

void main() {
	const vec3  diffColor = texture(texSampler, fragTexCoord).rgb;

	const vec3 L = lbo.globalLightDir; // directional light dir
	
	const vec3 N = normalize(fragNorm);
	const vec3 R = -reflect(L, N);
	const vec3 V = normalize(fragViewDir);
	
	/*********************************************************************************/
	// color

	// Lambert diffuse
	vec3 diffuse  = diffColor * max(dot(N,L), 0.0f);
	// Phong specular
	vec3 specular = lbo.specularSettings.xyz * pow(max(dot(R,V), 0.0f), lbo.specularSettings.w);

	// Hemispheric ambient
	vec3 ambient = (.5f * (1 + N * lbo.ambientTopColor) + .5f * (1 - N * lbo.ambientBottomColor)) * diffColor;
	
	/*********************************************************************************/
	// light

	vec3 directional_color = lbo.globalLightColor;

	vec3 missile_engine_light = spot_light_color(
		fragPos,
		lbo.spotlightPos,
		lbo.spotlightDir,
		lbo.spotlightSettings.x,
		lbo.spotlightSettings.y,
		lbo.spotlightSettings.z,
		lbo.spotlightSettings.w,
		lbo.spotlightColor
	);
	
	vec3 missile_top_light = lbo.pointlightSettings.z * point_light_color(
		fragPos,
		lbo.pointlightPos,
		lbo.pointlightSettings.x,
		lbo.pointlightSettings.y,
		lbo.pointlightColor
	);

	vec3 light = missile_top_light + missile_engine_light + directional_color;

	/*********************************************************************************/
	// output

	outColor = vec4(clamp((diffuse + specular + ambient) * light, vec3(0.0f), vec3(1.0f)), 1.0f);
}

