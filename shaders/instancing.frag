#version 150


in vec3 vertNormal;
in vec2 vertTextureCoordinates;
in vec4 vertTest;

out vec4 outColor;

void main(void){
	outColor = vec4(1, 1, 0, 0);
	if(vertNormal.y < 0.6){
		outColor = vertTest;
	}
}
