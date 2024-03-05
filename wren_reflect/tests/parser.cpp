#include "wren_reflect/parser.hpp"

#include <shaderc/shaderc.h>
#include <shaderc/status.h>

#include <catch2/catch_test_macros.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv/1.2/spirv.hpp>
#include <wren_reflect/spirv.hpp>

std::string_view triangle_vert_shader = R"(
#version 450

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
)";

TEST_CASE("Entry Points") {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  const auto compile_result = compiler.CompileGlslToSpv(
      triangle_vert_shader.data(),
      shaderc_shader_kind::shaderc_glsl_vertex_shader, "tests.glsl");
  REQUIRE(compile_result.GetCompilationStatus() ==
          shaderc_compilation_status_success);

  auto parser =
      std::make_shared<wren::reflect::Parser>(wren::reflect::spirv_t{
          compile_result.begin(), compile_result.end()});

  const auto entry_points = parser->entry_points();

  REQUIRE(entry_points.size() == 1);
  const auto entry_point = entry_points.front();

  REQUIRE(entry_point.name == "main");
  REQUIRE(entry_point.exeuction_model == spv::ExecutionModelVertex);
}
