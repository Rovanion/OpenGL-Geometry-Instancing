#version 150

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragSurface;
uniform mat4 transform;
uniform sampler2D texUnit;
out vec4 out_Color;

const vec3 light = vec3(1, 0.5, -1);

void main(void){
	float shade = clamp(dot(light, normalize(fragNormal)), 0, 1);
	out_Color = texture(texUnit, fragTexCoord) * vec4(shade, shade, shade, 1.0);
}
