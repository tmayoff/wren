#pragma once

#include <string_view>

namespace wren::shaders {

const std::string_view kEditorVertShader = R"(
#version 450

layout(set = 0, binding = 0) uniform GLOBALS {
    mat4 view;
    mat4 proj;
} globals;

layout(location = 1) out vec3 near_point;
layout(location = 2) out vec3 far_point;

vec3 grid_plane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 view_inv = inverse(view);
    mat4 proj_inv = inverse(projection);

    vec4 unprojected_point = view_inv * proj_inv * vec4(x, y, z, 1.0);

    return unprojected_point.xyz / unprojected_point.w;
}

void main() {
    vec3 p = grid_plane[gl_VertexIndex].xyz;
    near_point = UnprojectPoint(p.x, p.y, 0.0, globals.view, globals.proj).xyz;
    far_point = UnprojectPoint(p.x, p.y, 1.0, globals.view, globals.proj).xyz;
    
    gl_Position = vec4(p, 1.0);
}
)";

const std::string_view kEditorFragShader = R"(
#version 450

layout(location = 0) out vec4 out_colour;

layout(location = 1) in vec3 near_point;
layout(location = 2) in vec3 far_point;

void main() {
    float t = -near_point.y / (far_point.y - near_point.y);
    // out_colour = vec4(1.0, 0.0, 0.0, 1.0 * float(t > 0));
    out_colour = vec4(1.0, 0.0, 0.0, t);
}

)";

}  // namespace wren::shaders
