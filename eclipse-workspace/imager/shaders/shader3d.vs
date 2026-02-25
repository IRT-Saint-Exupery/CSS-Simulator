#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec4 LightIntensity;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 sunDir;	// Normalized sunPos

void main()
{
	vec4 LightPos = vec4(sunDir,0);
//	float d = max(dot(model * vec4(aNormal,0),LightPos),0.2);
//	float d = max(sqrt(dot(model * vec4(aNormal,0),LightPos)),0.2);  // La racine sert à diminuer moins vite la luminosité sur les bords.
        float d = max(sqrt(sqrt(sqrt(dot(model * vec4(aNormal,0),LightPos)))),0.2);
	LightIntensity = vec4(d,d,d,1);
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
