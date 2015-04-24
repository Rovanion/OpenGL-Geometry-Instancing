#version 150

in vec2 fragTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void){
	out_Color = texture(texUnit, fragTexCoord);
}
