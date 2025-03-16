#include "wren/mesh_loader.hpp"

#include <spdlog/spdlog.h>

#include <boost/algorithm/string/split.hpp>
#include <span>
#include <wren/utils/binary_reader.hpp>
#include <wren/utils/filesystem.hpp>

namespace wren {

auto load_glb_mesh(const std::filesystem::path& glb_path) -> Mesh {
  // gltf::load_mesh(glb_path);

  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  return Mesh{vertices, indices};
}

auto load_stl_mesh(const std::filesystem::path& stl_path) -> Mesh {
  auto data = utils::fs::read_file_to_bin(stl_path);
  std::span s(data);
  utils::BinaryReader reader(std::as_bytes(s));

  // Skip header
  reader.skip(80);

  const auto triangles = reader.read<uint32_t>();

  std::vector<Vertex> vertices;
  vertices.reserve(triangles * 3);
  std::vector<uint16_t> indices;
  size_t newest_index = 0;

  for (auto i = 0; i < triangles; ++i) {
    const math::Vec3f normal{reader.read_list<float, 3>()};

    std::array verts = {reader.read_list<float, 3>(),
                        reader.read_list<float, 3>(),
                        reader.read_list<float, 3>()};

    reader.skip(2);

    for (const auto& v : verts) {
      auto it =
          std::ranges::find_if(vertices, [vec = v, normal](const auto& v) {
            return v.pos == math::Vec3f{vec} && v.normal == normal;
          });

      if (it == vertices.end()) {
        // New vertex
        vertices.emplace_back(math::Vec3f{v}, normal, math::Vec4f{1.0F});
        indices.push_back(newest_index++);
      } else {
        // Old vertex
        indices.push_back(std::ranges::distance(vertices.begin(), it));
      }
    }
  }

  return Mesh{vertices, indices};
}

auto load_mesh(const std::filesystem::path& mesh_path) -> expected<Mesh> {
  spdlog::debug("Loading mesh: {}", mesh_path.string());
  auto ext = mesh_path.extension().string();

  if (ext.contains(".glb")) {
    return load_glb_mesh(mesh_path);
  }

  if (ext.contains(".stl")) {
    return load_stl_mesh(mesh_path);
  }

  return std::unexpected(
      make_error_code(MeshLoaderError::ExtensionNotSupported));
}

}  // namespace wren
