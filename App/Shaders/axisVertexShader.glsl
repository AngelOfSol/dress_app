#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 worldMat;

out vec4 fColor;

void main()
{
	gl_Position = worldMat * vec4(position, 1);
	fColor = color;
}