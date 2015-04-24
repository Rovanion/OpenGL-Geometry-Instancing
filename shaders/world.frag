#version 150

in vec2 vert_TexCoord;
in vec3 vert_normal;
in vec3 vert_surface;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];
uniform mat4 lookMatrix;
uniform vec3 cameraPos;

uniform mat4 transform;
uniform sampler2D texUnit;
uniform sampler2D maskrosen;

out vec4 out_Color;

const vec3 light = vec3(0.58, 0.58, 0.58);

void main(void){
	float shade;
	shade = clamp(dot(light, normalize(vert_normal)), 0, 1);
	vec3 light = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < 4; i++) {
		vec3 reflectedLightDirection;
		if (isDirectional[i])
			reflectedLightDirection = reflect(-lightSourcesDirPosArr[i], vert_normal);
		else
			reflectedLightDirection = reflect(lightSourcesDirPosArr[i] - vert_surface, vert_normal);

		vec3 eyeDirection = normalize(vert_surface - cameraPos);


		float specularStrength = 0.0;
		if(dot(lightSourcesDirPosArr[i], vert_normal) > 0.0) {
			specularStrength = dot(reflectedLightDirection, eyeDirection);
			specularStrength = max(specularStrength, 0.01);
			specularStrength = pow(specularStrength, specularExponent[i]);
		}

		light += specularStrength * lightSourcesColorArr[i] * 0.5 + shade * 0.5;
	}
	light = clamp(light, 0, 1);

	out_Color = texture(texUnit, vert_TexCoord) * vec4(light, 1.0);
}
