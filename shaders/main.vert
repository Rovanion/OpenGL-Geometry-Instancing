#version 150

in vec3 vertNormal;
in vec3 vertPosition;
in vec2 vertTexCoord;
uniform mat4 transform;
uniform mat4 viewFrustum;
uniform mat4 lookMatrix;
out vec4 gl_Position;
out vec3 fragNormal;
out vec3 fragSurface;
out vec2 fragTexCoord;

void main(void)
{
	fragNormal = mat3(transform) * vertNormal;
	fragSurface = mat3(transform) * vertPosition;
	fragTexCoord = vertTexCoord;
	gl_Position = viewFrustum * lookMatrix * transform * vec4(vertPosition, 1.0);
}
