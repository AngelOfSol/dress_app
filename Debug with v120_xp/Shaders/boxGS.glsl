#version 410

in vec2 gSize [];

uniform mat4 transformMat;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main() {
	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(gSize[0].xy, 0, 0));
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(0, gSize[0].y, 0, 0));
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(gSize[0].x, 0, 0, 0));
	EmitVertex();
	
	gl_Position = transformMat * gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}