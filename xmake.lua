add_rules("mode.debug", "mode.release")
set_languages("c++23")

set_toolchains("gcc")

add_requires("flecs", "conan::tl-expected")
add_requires("spdlog", { system = true })


local boost_inc = os.getenv("BOOST_INCLUDEDIR")

target("wren_utils")
set_kind("static")
add_includedirs("wren_utils/include", boost_inc, { public = true })
add_includedirs("wren_utils/include/wren/utils")
add_files("wren_utils/src/*.cpp")
add_packages("spdlog", "fmt")

-- target("wren_math")
--   set_kind("static")
--   add_includedirs("wren_math/include", { public = true })
--   add_includedirs("wren_math/include/wren/math")
--   add_files("wren_math/src/*.cpp")

-- target("wren_vk")
--   set_kind("static")
--   add_includedirs("wren_vk/include", { public = true })
--   add_includedirs("wren_vk/include/wren/vk")
--   add_files("wren_vk/src/*.cpp")
--   add_deps("wren_math", "wren_utils")

-- target("wren")
--   set_kind("static")
--   add_includedirs("wren/include", { public = true })
--   add_includedirs("wren/include/wren")
--   add_files("wren/src/*.cpp")
--   add_deps("wren_utils", "wren_vk", "wren_math")

-- target("editor")
--   set_kind("binary")
--   add_files("editor/src/*.cpp")
--   add_deps("wren")
