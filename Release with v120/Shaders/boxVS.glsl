#version 410

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 size;

out vec2 gSize;
void main()
{
	gl_Position = vec4(position, 0, 1);
	gSize = size;
}