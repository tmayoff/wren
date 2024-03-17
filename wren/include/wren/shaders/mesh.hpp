#pragma once

#include <string_view>
namespace wren::shaders {

std::string_view const MESH_VERT_SHADER = R"(
#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;            
} ubo;

layout(location = 0) out vec3 out_color;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 0.0, 1.0);
    out_color = in_color;
}  
)";

std::string_view const MESH_FRAG_SHADER = R"(
#version 450

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(in_color, 1.0);
}
     
)";

}  // namespace wren::shaders
