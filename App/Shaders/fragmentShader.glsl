#version 410

in vec4 fNormal;
in vec4 modelSpacePosition;

uniform mat4 worldMat;
uniform vec4 diffuse;
uniform vec4 lightSource[4];
uniform int numLights;
uniform float ambient;
uniform float attenuation;

void main(void) 
{
	vec4 lightAngle;
	
	float diffuseIntensity = 0;
	
	for(int i = 0; i < numLights; i++)
	{
		lightAngle = modelSpacePosition - lightSource[i];
		diffuseIntensity += clamp(dot(fNormal, normalize(lightAngle)), 0.0, 1.0) / dot(lightAngle, lightAngle) * attenuation;
	}
	
	gl_FragColor =  vec4((diffuse * ambient + diffuse * diffuseIntensity).rgb, 5.0);
}