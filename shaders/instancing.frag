#version 150

in VERTEX
{
	vec3 normal;
	vec2 textureCoordinates;
	vec4 test;
} vertex;

out vec4 outColor;

void main(void){
	outColor = vertex.test;
}
