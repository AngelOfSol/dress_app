#version 410

uniform mat4 worldMat;

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

void main() {
	gl_Position = worldMat * (gl_in[0].gl_Position);
	EmitVertex();

	gl_Position = worldMat * (gl_in[0].gl_Position + vec4(0.1, 1, 0, 0));
	EmitVertex();

	gl_Position = worldMat * (gl_in[0].gl_Position + vec4(-0.1, 1, 0, 0));
	EmitVertex();

	EndPrimitive();
}