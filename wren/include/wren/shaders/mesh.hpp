#pragma once

#include <string_view>

namespace wren::shaders {

const std::string_view kMeshVertShader = R"(
#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

layout(binding = 0) uniform GLOBALS {
    mat4 view;
    mat4 proj;
} globals;

layout(binding = 1) uniform LOCALS {
    mat4 model;
} locals;

layout(location = 0) out vec3 out_color;

void main() {
    gl_Position = globals.proj * globals.view * vec4(in_position, 0.0, 1.0);
    out_color = in_color;
}
)";

const std::string_view kMeshFragShader = R"(
#version 450

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(in_color, 1.0);
}
     
)";

}  // namespace wren::shaders
