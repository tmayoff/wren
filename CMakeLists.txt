cmake_minimum_required(VERSION 3.28)

project(wren)

find_package(PkgConfig REQUIRED)
list(APPEND CMAKE_MODULE_PATH
     "${PROJECT_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_compile_options("$<$<CONFIG:DEBUG>:-DWREN_DEBUG>")
add_compile_options("-DVULKAN_HPP_NO_EXCEPTIONS")
add_compile_options("-DVULKAN_HPP_ASSERT_ON_RESULT(x)")

include(CTest)
include(FetchContent)
enable_testing()

find_package(SPIRV-Headers REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(tl-expected REQUIRED)
pkg_check_modules(VulkanMemoryAllocator REQUIRED)
pkg_check_modules(shaderc REQUIRED shaderc)

find_package(Tracy QUIET)
if (NOT $Tracy_FOUND)
     pkg_check_modules(tracy REQUIRED)
     set(TRACY_DEP tracy)
else()
     set(TRACY_DEP Tracy::TracyClient)
endif()

find_package(Catch2 3 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SPDLOG_INCLUDE_DIRS})

add_subdirectory(wrenm)
add_subdirectory(wren_reflect)
add_subdirectory(wren)
add_subdirectory(editor)
