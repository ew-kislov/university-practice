#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coord;

out vec2 var_texture_coord;

uniform mat4 transform_matrix;

void main(){
    gl_Position = transform_matrix * vec4(position, 1.0);

    var_texture_coord = texture_coord;
}