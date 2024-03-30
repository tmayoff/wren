#include "wren_reflect/parser.hpp"

#include <shaderc/shaderc.h>
#include <shaderc/status.h>
#include <spdlog/spdlog.h>

#include <catch2/catch_test_macros.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv/1.2/spirv.hpp>
#include <wren_reflect/spirv.hpp>

std::string_view const triangle_vert_shader = R"(
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

  auto const compile_result = compiler.CompileGlslToSpv(
      triangle_vert_shader.data(),
      shaderc_shader_kind::shaderc_glsl_vertex_shader, "tests.glsl");
  REQUIRE(compile_result.GetCompilationStatus() ==
          shaderc_compilation_status_success);

  auto parser =
      std::make_shared<wren::reflect::Parser>(wren::reflect::spirv_t{
          compile_result.begin(), compile_result.end()});

  auto const entry_points = parser->entry_points();

  REQUIRE(entry_points.size() == 1);
  auto const entry_point = entry_points.front();

  REQUIRE(entry_point.name == "main");
  REQUIRE(entry_point.exeuction_model == spv::ExecutionModelVertex);
}

TEST_CASE("Vertex Inputs") {
  std::string_view const vertex_inputs = R"(
#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}  
)";

  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  auto const compile_result = compiler.CompileGlslToSpv(
      vertex_inputs.data(),
      shaderc_shader_kind::shaderc_glsl_vertex_shader, "tests.glsl");
  REQUIRE(compile_result.GetCompilationStatus() ==
          shaderc_compilation_status_success);

  auto parser =
      std::make_shared<wren::reflect::Parser>(wren::reflect::spirv_t{
          compile_result.begin(), compile_result.end()});

  REQUIRE(std::find_if(parser->op_names().begin(),
                       parser->op_names().end(), [](auto const pair) {
                         return pair.second == "inPosition";
                       }) != parser->op_names().end());

  for (auto const& n : parser->op_names()) {
    spdlog::info("Named types: {} {}", n.first, n.second);
  }

  REQUIRE(!parser->types().empty());

  for (auto const& type : parser->types()) {
    spdlog::info("ID: {} type: {} parent: {}", type.id,
                 spv::to_string(type.type), type.parent_type);
    if (type.storage_class.has_value()) {
      spdlog::info("\tstorage class: {}",
                   spv::to_string(type.storage_class.value()));
    }
  }

  REQUIRE(false);
}
