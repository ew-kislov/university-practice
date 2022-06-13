#version 330 core

out vec4 color;

uniform sampler2D u_texture;

in vec2 var_texture_coord;

void main()
{
	color = texture(u_texture, var_texture_coord);
}