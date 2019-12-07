#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform Uniform {
    mat4 mvp;
} uni;

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_border_inner;

layout(location = 0) out vec4 out_fill_color;
layout(location = 1) out vec2 out_border_inner;

void main() {
    gl_Position = uni.mvp * vec4(in_position, 0.0, 1.0);
    out_fill_color = in_color;
    out_border_inner = in_border_inner;
}
