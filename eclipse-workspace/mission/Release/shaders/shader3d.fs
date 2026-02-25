#version 410 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 LightIntensity;

// texture samplers
uniform sampler2D texture;

void main()
{
	FragColor = texture2D(texture, TexCoord) * LightIntensity;
}