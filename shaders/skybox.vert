#version 150

in vec3 vertPosition;
in vec2 vertTexCoord;
uniform mat4 transform;
uniform mat4 projectionMatrix;
uniform mat4 lookMatrix;
out vec2 fragTexCoord;

void main(void)
{
	fragTexCoord = vertTexCoord;
	gl_Position = projectionMatrix * lookMatrix * transform  * vec4(vertPosition, 1.0);
}
