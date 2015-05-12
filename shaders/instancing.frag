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
	if(vertex.textureCoordinates == vec2(0.0, 0.0))
		outColor = vertex.test;
	else
		outColor = texture(texUnit, vertex.textureCoordinates);
}
