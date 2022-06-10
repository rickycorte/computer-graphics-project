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

/*
vec4 spot_light(vec3 fragPos, vec3 lightSourcePos, vec3 diffColor, vec3 N) {
	vec3 lightColor = normalize(vec3(253,179,6)); //colore fiamma

	// Spot light direction
	//luce fondomissile, da implementare
	//TODO: evitare di hardcoddare tutti i settings, sarebbero da sposare in un binding
	vec3 dir = normalize(lightSourcePos - fragPos);
	
	// Spot light color
	vec3 missileBottomLightDir = -lightDir; // in teoria nel main lightDir � gi� assegnato a -dirMotoMissile, per� qui � ri-invertito altrimenti la luce � al contrario per qualche arcano motivo 
	float cos_in = 0.96f;
	float cos_out = 0.65f; //toccare cos_in e cos_out mantenendo cos_in > cos_put rompe tutto senza motivo
	float decay_exp = 1.5f;
	float g = 25;

	float coeff = pow(g/length(lightSourcePos - fragPos) ,decay_exp);
	float dimming = clamp((dot(normalize(lightSourcePos - fragPos),missileBottomLightDir)-cos_out)/(cos_in - cos_out),0.0f,1.0f);
	vec3 color = lightColor * coeff * dimming;

	vec3 diffuse = diffColor*(max(dot(N, dir),0.0f));

	return vec4(diffuse*color, 1.f);
}

vec4 point_light(vec3 fragPos, vec3 diffColor, vec3 N){
	vec3 lightPos2 = lightPos; 
	vec3 dir = normalize(lightPos2 - fragPos);
	float g = 10;
	int decay_exp = 1;
	vec3 lightColor = normalize(vec3(253,179,6)); //colore fiamma

	float coeff = pow(g/length(lightPos2 - fragPos) ,decay_exp);
	vec3 color = lightColor * coeff;
	vec3 diffuse = diffColor*(max(dot(N, dir),0.0f));

	return vec4(diffuse*color, 1.f);
}
*/

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
	
	outColor = vec4(clamp(ambient + diffuse + specular, vec3(0.0f), vec3(0.5f)), 1.0f); //questo viene sovraascritto per testare, se viene sommato alla riga sotto la spotlight non si vede
	
	//TODO: non hardcoddare i parametro
	vec3 missile_engine_light = spot_light_color(fragPos, lightPos, -lightDir, 25, 1.5f, 0.96f, 0.65f, normalize(vec3(253.0f/255,179.0f/255,6.0f/255)));

	vec3 top_pos = lightPos + vec3(0, 4, 0); // TODO: non hardcoddare la posizione della cima
	vec3 missile_top_light = point_light_color(fragPos, top_pos, 0.4f, 2.0f, vec3(1,0,0));

	vec3 light_sum = missile_top_light + missile_engine_light;

	outColor = vec4(clamp(diffColor * light_sum , vec3(0.0f), vec3(1.0f)), 1.0f);
}

