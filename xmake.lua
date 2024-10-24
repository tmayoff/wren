add_rules("mode.debug", "mode.release")
set_languages("c++23")

-- set_toolchains("clang")

add_undefines('SDL_VIDEO_DRIVER_X11')

add_requires("flecs", "vulkan-memory-allocator", "vulkan", "shaderc", "spirv-reflect", "tracy", "backward-cpp", "toml++")
add_requires("imgui 1.91.1-docking", { config = { vulkan = true, sdl2 = true, } })
add_requires("spdlog", "fmt", "pkgconfig::SPIRV-Headers", "sdl2", { system = true })

local boost_inc = os.getenv("BOOST_INCLUDEDIR")

target("wren_utils")
set_kind("static")
add_includedirs("wren_utils/include", boost_inc, { public = true })
add_includedirs("wren_utils/include/wren/utils")
add_files("wren_utils/src/*.cpp")
add_packages("spdlog", "fmt", { public = true })

target("wren_math")
set_kind("static")
add_includedirs("wren_math/include", { public = true })
add_includedirs("wren_math/include/wren/math")
add_files("wren_math/src/*.cpp")

-- target("wren_reflect")
--   set_kind("static")
--   add_includedirs("wren_reflect/include", spirv_inc, { public = true })
--   add_includedirs("wren_reflect/include/wren/reflect")
--   add_files("wren_reflect/src/*.cpp")
-- add_packages("SPIRV-Headers", { public = true })

target("wren_vk")
set_kind("static")
add_includedirs("wren_vk/include", { public = true })
add_includedirs("wren_vk/include/wren/vk")
add_files("wren_vk/src/*.cpp")
add_deps("wren_math", "wren_utils", { public = true })
add_defines("VULKAN_HPP_NO_EXCEPTIONS",
  "VUKLAN_HPP_ASSERT_ON_RESULT(x)",
  "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1", { public = true })
add_packages("vulkan", "vulkan-memory-allocator", "shaderc", "spirv-reflect", { public = true })

target("wren")
set_kind("static")
add_includedirs("wren/include", { public = true })
add_includedirs("wren/include/wren")
add_files("wren/src/*.cpp", "wren/src/assets/*.cpp", "wren/src/scene/*.cpp", "wren/src/utils/*.cpp")
add_deps("wren_utils", "wren_vk", "wren_math")
add_packages("flecs", "sdl2", "tracy", "toml++", { public = true })

target("editor")
set_kind("binary")
add_files("editor/src/*.cpp")
add_deps("wren")
add_defines("WREN_BUILD_ASSETS_DIR=\"$(projectdir)/editor/assets\"")
add_packages("imgui", "backward-cpp", "sdl2")
