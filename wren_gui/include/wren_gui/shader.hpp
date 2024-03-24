#pragma once

#include <string_view>

namespace wren::gui {

constexpr std::string_view VERTEX_SHADER = R"(
#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec4 in_colour;

layout(location = 0) out vec4 out_colour;

void main () {
    gl_Position = vec4(in_position, 0.0, 1.0);
    out_colour = in_colour;
}
)";

constexpr std::string_view FRAGMENT_SHADER = R"(
#version 450

layout(location = 0) in vec4 in_colour;

layout(location = 0) out vec4 out_colour;

void main() {
    out_colour = in_colour;
}
)";

}  // namespace wren::gui
