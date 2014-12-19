#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in float vRadius;

uniform mat4 worldMat;

out float gRadius;

void main()
{
	gl_Position = vec4(position, 1);
	gRadius = vRadius;
}