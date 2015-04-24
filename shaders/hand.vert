#version 150

in vec3 vertNormal;
in vec3 vertPosition;
in vec2 vertTextureCoordinate;
uniform mat4 transform;
uniform mat4 lookMatrix;
uniform mat4 viewFrustum;
out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTextureCoordnate;


void main(void){
	fragNormal = vertNormal;
	fragPosition = vertPosition;
	fragTextureCoordnate = vertTextureCoordinate;
	gl_Position = viewFrustum * lookMatrix * transform  * vec4(vertPosition, 1.0);
}
