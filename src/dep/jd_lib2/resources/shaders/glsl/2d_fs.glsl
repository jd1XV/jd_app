#version 430
in vec3 fs_uvw;
in vec4 fs_col;

out vec4 out_col; 
uniform sampler2DArray tex;

void main() {
    vec4 color = texture(tex, fs_uvw);
    out_col = fs_col * color;
}


