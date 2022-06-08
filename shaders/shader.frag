#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	const vec3  diffColor = texture(texSampler, fragTexCoord).rgb;
	const vec3  specColor = vec3(1.0f, 1.0f, 1.0f);
	const float specPower = 150.0f;
	const vec3  L = vec3(-0.4830f, 0.8365f, -0.2588f);
	
	vec3 N = normalize(fragNorm);
	vec3 R = -reflect(L, N);
	vec3 V = normalize(fragViewDir);
	
	// Lambert diffuse
	vec3 diffuse  = diffColor * max(dot(N,L), 0.0f);
	// Phong specular
	vec3 specular = specColor * pow(max(dot(R,V), 0.0f), specPower);
	// Hemispheric ambient
	vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * diffColor;
	
	outColor = vec4(clamp(ambient + diffuse + specular, vec3(0.0f), vec3(1.0f)), 1.0f);
}

//pos = viewDirection?
vec4 spot_light(vec3 pos, vec3 diffColor, vec3 N) {
	vec3 lightColor = vec3(253,179,6); //colore fiamma

	// Spot light direction
	//luce fondomissile, da implementare
	vec3 missileBottomLightPos;
	vec3 dir = normalize(missileBottomLightPos - pos);
	
	// Spot light color
	vec3 missileBottomLightDir; //da implementare: -dirMotoMissile 
	float cos_in = 0.9f;
	float cos_out = 0.6f;
	int decay_exp = 2;
	float g = 1.5;
	float coeff = pow(g/length(missileBottomLightPos - pos) ,decay_exp);
	float dimming = clamp((dot(normalize(missileBottomLightPos - pos),missileBottomLightDir)-cos_out)/(cos_in - cos_out),0.0f,1.0f);
	vec3 color = lightColor * coeff * dimming;

	vec3 diffuse = diffColor*(max(dot(N, dir),0.0f));

	return vec4(diffuse*color, 1.f);
}