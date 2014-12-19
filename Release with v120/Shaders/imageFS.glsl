#version 410

varying vec2 fTexel;

uniform sampler2D textureBuffer;

void main(void) 
{
	gl_FragColor = vec4(texture(textureBuffer, fTexel));
}