#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 lightPos;
layout(location = 4) in vec3 lightDir;
layout(location = 5) in vec3 position;

layout(location = 0) out vec4 outColor;

vec4 spot_light(vec3 pos, vec3 diffColor, vec3 N) {
	vec3 lightColor = normalize(vec3(253,179,6)); //colore fiamma

	// Spot light direction
	//luce fondomissile, da implementare
	vec3 missileBottomLightPos = lightPos;
	vec3 dir = normalize(missileBottomLightPos - pos);
	
	// Spot light color
	vec3 missileBottomLightDir = lightDir; // -dirMotoMissile 
	float cos_in = 0.9f;
	float cos_out = 0.1f;
	int decay_exp = 2;
	float g = 25;
	float coeff = pow(g/length(missileBottomLightPos - pos) ,decay_exp);
	float dimming = clamp((dot(normalize(missileBottomLightPos - pos),missileBottomLightDir)-cos_out)/(cos_in - cos_out),0.0f,1.0f);
	vec3 color = lightColor * coeff * dimming;

	vec3 diffuse = diffColor*(max(dot(N, dir),0.0f));

	return vec4(diffuse*color, 1.f);
}

vec4 point_light(vec3 pos, vec3 diffColor, vec3 N){
	vec3 dir = normalize(lightPos - pos);
	float g = 10;
	int decay_exp = 1;
	vec3 lightColor = normalize(vec3(253,179,6)); //colore fiamma

	float coeff = pow(g/length(lightPos - pos) ,decay_exp);
	vec3 color = lightColor * coeff;
	vec3 diffuse = diffColor*(max(dot(N, dir),0.0f));

	return vec4(diffuse*color, 1.f);
}

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
	
	outColor = vec4(clamp(ambient + diffuse + specular, vec3(0.0f), vec3(1.0f)), 1.0f); //+ spot_light(position,diffColor,N);
}

