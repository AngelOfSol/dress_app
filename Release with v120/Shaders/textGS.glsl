#version 410

in vec2 gTexCoord [];

out vec2 fTexCoord;

uniform float textWidth;
uniform float textHeight;
uniform float textureWidth;
uniform float textureHeight;

uniform mat4 transformMat;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main() {
	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(textWidth, textHeight, 0, 0));
	fTexCoord = gTexCoord[0] + vec2(textureWidth, textureHeight);
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(0, textHeight, 0, 0));
	fTexCoord = gTexCoord[0] + vec2(0, textureHeight);
	EmitVertex();

	gl_Position = transformMat * (gl_in[0].gl_Position + vec4(textWidth, 0, 0, 0));
	fTexCoord = gTexCoord[0] + vec2(textureWidth, 0);
	EmitVertex();
	
	gl_Position = transformMat * gl_in[0].gl_Position;
	fTexCoord = gTexCoord[0] + vec2(0, 0);
	EmitVertex();


	EndPrimitive();
}