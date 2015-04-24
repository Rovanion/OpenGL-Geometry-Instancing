#version 150

in vec3 in_Position;
in vec2 in_TexCoord;
uniform mat4 transform;
uniform mat4 projectionMatrix;
uniform mat4 lookMatrix;
out vec2 vert_TexCoord;

void main(void)
{
	vert_TexCoord = in_TexCoord;
	gl_Position = projectionMatrix * lookMatrix * transform  * vec4(in_Position, 1.0);
}
