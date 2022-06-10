#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 4) in vec3 lightPos;
layout(location = 5) in vec3 lightDir;

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
	const vec3  specColor = vec3(1.0f, 1.0f, 1.0f);
	const float specPower = 150.0f;

	const vec3 L = vec3(-0.4830f, 0.8365f, -0.2588f); // directional light dir
	
	vec3 N = normalize(fragNorm);
	vec3 R = -reflect(L, N);
	vec3 V = normalize(fragViewDir);
	
	// Lambert diffuse
	vec3 diffuse  = diffColor * max(dot(N,L), 0.0f);
	// Phong specular
	vec3 specular = specColor * pow(max(dot(R,V), 0.0f), specPower);

	// Hemispheric ambient
	// TODO: no hardcode (sono due blu del cielo)
	vec3 ambTopColor = vec3(60.0f/255, 70.0f/255, 201.0f/255);
	vec3 ambBottomColor = vec3(29.0f/255, 34.0f/255, 97.0f/255);

	vec3 ambient = (.5f * (1 + N * ambTopColor) + .5f * (1 - N * ambBottomColor)) * diffColor;
	
	//outColor = vec4(clamp(ambient + diffuse + specular, vec3(0.0f), vec3(0.5f)), 1.0f); //questo viene sovraascritto per testare, se viene sommato alla riga sotto la spotlight non si vede
	
	//TODO: non hardcoddare i parametro
	vec3 missile_engine_light = spot_light_color(fragPos, lightPos, -lightDir, 25, 1.5f, 0.96f, 0.65f, normalize(vec3(253.0f/255,179.0f/255,6.0f/255)));

	vec3 directional_color = vec3(.1f);
	vec3 top_pos = lightPos + vec3(0, 4, 0); // TODO: non hardcoddare la posizione della cima, btw non va quando il razzo ruota per ovvie ragioni :#
	vec3 missile_top_light = 50 * point_light_color(fragPos, top_pos, 0.5f, 4.0f, vec3(1,0,0)); // front multiplier is "intensity"

	vec3 light_sum = missile_top_light + missile_engine_light + directional_color;

	outColor = vec4(clamp((diffuse + specular + ambient) * light_sum , vec3(0.0f), vec3(1.0f)), 1.0f);
}

