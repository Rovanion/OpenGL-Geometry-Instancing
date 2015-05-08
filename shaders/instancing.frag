#version 150

in VERTEX
{
	vec3 normal;
	vec2 textureCoordinates;
	vec4 test;
} vertex;

out vec4 outColor;

void main(void){
	outColor = vec4(1, 1, 0, 0);
	if(vertex.normal.y < 0.6){
		outColor = vertex.test;
	}
}
