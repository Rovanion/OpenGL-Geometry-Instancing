#version 150

in vec3 in_Position;
in vec2 in_TexCoord;
in vec3 in_Normal;
uniform mat4 transform;
uniform mat4 projectionMatrix;
uniform mat4 lookMatrix;
out vec4 gl_Position;
out vec2 vert_TexCoord;
out vec3 vert_normal;
out vec3 vert_surface;

void main(void)
{
	vert_normal = mat3(transform) * in_Normal;
	vert_surface = mat3(transform) * in_Position;
	vert_TexCoord = in_TexCoord;
	gl_Position = projectionMatrix * lookMatrix * transform * vec4(in_Position, 1.0);
}
