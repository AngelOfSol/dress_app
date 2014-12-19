#version 410

varying vec2 fTexCoord;

uniform sampler2D fontBuffer;

void main(void) 
{
	if(texture(fontBuffer, fTexCoord).a < 0.3)
		discard;
	gl_FragColor = vec4(texture(fontBuffer, fTexCoord));
}