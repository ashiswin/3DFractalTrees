#version 450

in vec3 pass_Color;
out vec3 out_Color;

void main(void) {
	out_Color = pass_Color;
}
