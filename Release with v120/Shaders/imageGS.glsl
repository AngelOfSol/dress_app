#version 410

out vec2 fTexel;

uniform mat4 transformMat;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main() {
	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(1, 1, 0, 0));
	fTexel = vec2(1, 1);
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(0, 1, 0, 0));
	fTexel = vec2(0, 1);
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(1, 0, 0, 0));
	fTexel = vec2(1, 0);
	EmitVertex();
	
	gl_Position = transformMat * gl_in[0].gl_Position;
	fTexel = vec2(0, 0);
	EmitVertex();


	EndPrimitive();
}