#version 430
layout (location = 0) in vec3 vert_xyz;
layout (location = 1) in vec3 vert_uvw;
layout (location = 2) in vec4 vert_col;
layout (location = 3) in uint texunit;

out vec3 fs_uvw;
out vec4 fs_col;
flat out uint fs_texunit;

uniform vec2 screen_conv;

void main(){
    gl_Position.x = (vert_xyz.x * screen_conv.x) - 1;
    gl_Position.y = 1 - (vert_xyz.y * screen_conv.y);
    gl_Position.z = 0;
    gl_Position.w = 1;
    fs_uvw = vert_uvw;
    fs_col = vert_col;
    fs_texunit = texunit;
}