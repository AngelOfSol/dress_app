#version 410

in float gRadius [];

out vec4 fNormal;
out vec4 modelSpacePosition;

uniform mat4 worldMat;
uniform int resolution;
uniform int dir;

layout(points) in;
layout(triangle_strip, max_vertices = 84) out;

void main() {

	int numSeg = resolution + 2;
	
	float angle;
	
	float cosAngle;
	float sinAngle;
	
	float prevCos;
	float prevSin;
	
	for (float i  = 0; i < numSeg; i += 1)
	{
		angle = i / (numSeg  - 2) * 2 * 3.14;
		
		cosAngle = cos(angle);
		sinAngle = sin(angle);
		
		vec4 mod = vec4(0, cosAngle, sinAngle, 0);
		
		fNormal = vec4(-1, 0, 0, 0) * dir;
		
		modelSpacePosition = gl_in[0].gl_Position;
		gl_Position = worldMat * gl_in[0].gl_Position;
		EmitVertex();
		
		
		modelSpacePosition = (gl_in[0].gl_Position + mod * gRadius[0]);
		gl_Position = worldMat * (gl_in[0].gl_Position + mod * gRadius[0]);
		EmitVertex();
		
		mod = vec4(0, prevCos,  prevSin, 0);
		modelSpacePosition = (gl_in[0].gl_Position + mod * gRadius[0]);
		gl_Position = worldMat * (gl_in[0].gl_Position + mod * gRadius[0]);
		EmitVertex();
		
		EndPrimitive();
		
		prevCos = cosAngle;
		prevSin = sinAngle;
	}
}