#version 450

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 lightPos;
	vec3 lightDir;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragViewDir;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos; 

// bisognerebbe capire come creare un binding al posto di fare il giro dalla vertex
// ci ho provato gia io ma il codice mi rema un po contro 
layout(location = 4) out vec3 lightPos;
layout(location = 5) out vec3 lightDir;

void main() {
	gl_Position  = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);

	fragPos 	 = (ubo.model * vec4(pos, 1.0)).xyz;
	fragViewDir  = (ubo.view[3]).xyz - (ubo.model * vec4(pos,  1.0)).xyz;
	fragNorm     = (ubo.model * vec4(norm, 0.0)).xyz;

	fragTexCoord = texCoord;
	lightPos = ubo.lightPos;
	lightDir = ubo.lightDir;
}