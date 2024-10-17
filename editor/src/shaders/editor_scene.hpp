#pragma once

#include <string_view>

namespace wren::shaders {

const std::string_view kEditorVertShader = R"(
#version 450

layout(binding = 0) uniform GLOBALS {
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
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = grid_plane[gl_VertexIndex];
    near_point = UnprojectPoint(p.x, p.y, 0.0, globals.view, globals.proj);
    far_point = UnprojectPoint(p.x, p.y, 1.0, globals.view, globals.proj);
    gl_Position = vec4(p, 1.0);
}
)";

const std::string_view kEditorFragShader = R"(
#version 450

layout(location = 0) out vec4 out_color;

layout(location = 1) in vec3 near_point;
layout(location = 2) in vec3 far_point;

vec4 grid(vec3 frag_pos_3D, float scale) {
    vec2 coord = frag_pos_3D.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(frag_pos_3D.x > -0.1 * minimumx && frag_pos_3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(frag_pos_3D.z > -0.1 * minimumz && frag_pos_3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}

void main() {
    float t = -near_point.y / (far_point.y - near_point.y);
    vec3 frag_pos_3d = near_point + t * (far_point - near_point);
    out_color = grid(frag_pos_3d, 10) * float(t > 0);
}

)";

}  // namespace wren::shaders
