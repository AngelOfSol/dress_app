#version 410

layout (location = 0) in vec2 position;
layout (location = 1) in float fNum;
uniform mat4 worldMat;

out float num;

void main()
{
	gl_Position = worldMat * vec4(position, 0, 1);
	num = fNum;
}