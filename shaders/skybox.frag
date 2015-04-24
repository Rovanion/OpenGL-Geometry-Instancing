#version 150

in vec2 vert_TexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void){
	out_Color = texture(texUnit, vert_TexCoord);
}
