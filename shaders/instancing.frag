#version 150

in VERTEX
{
	vec3 normal;
	vec2 textureCoordinates;
	vec4 test;
} vertex;

uniform sampler2D texUnit;

out vec4 outColor;

void main(void){
	outColor = vertex.test;
	//	outColor = texture(texUnit, vertex.textureCoordinates);
}
