#version 410

in float gRadius [];

out vec4 fNormal;
out vec4 modelSpacePosition;

uniform mat4 worldMat;
uniform int resolution;

layout(lines) in;
layout(triangle_strip, max_vertices = 128) out;

void main() {
	
	int numSeg =  resolution + 2;
	
	for (float i  = 0; i < numSeg; i += 1)
	{
		float angle = i / (numSeg - 2) * 2 * 3.14;
		
		vec4 mod = vec4(0, cos(angle), sin(angle), 0);
		
		vec4 pos0 = gl_in[0].gl_Position + mod * gRadius[0];
		vec4 pos1 = gl_in[1].gl_Position +  mod * gRadius[1];
		
		vec4 diff = pos1 - pos0;
		
		vec3 opposingPlane = cross(mod.xyz, vec3(1, 0, 0));
		
		fNormal = vec4(normalize(cross(opposingPlane, diff.xyz)), 0.0);
		
		modelSpacePosition = pos0;
		gl_Position = worldMat * (pos0);
		EmitVertex();

		modelSpacePosition = pos1;
		gl_Position = worldMat * (pos1);
		EmitVertex();

	}
		
		EndPrimitive();
}