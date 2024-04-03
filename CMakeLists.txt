cmake_minimum_required(VERSION 3.28)

project(wren)

option(ENABLE_PROFILING "Enables profiling with tracy" Off)
option(ENABLE_TESTING "Enables testing framework" Off)
option(SKIP_DOWNLOADS "Skips downloading packages" Off)

find_package(PkgConfig REQUIRED)
list(APPEND CMAKE_MODULE_PATH
     "${PROJECT_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if(NOT SKIP_DOWNLOADS)
  include(cmake/CPM.cmake)
  cpmaddpackage(
    NAME
    Format.cmake
    VERSION
    1.7.3
    GITHUB_REPOSITORY
    TheLartians/Format.cmake
    OPTIONS
    "FORMAT_SKIP_CMAKE NO")
endif()

if(ENABLE_PROFILING)
  add_compile_options("-DTRACY_ENABLE")
endif()
add_compile_options("$<$<CONFIG:DEBUG>:-DWREN_DEBUG>")
add_compile_options("-DVULKAN_HPP_NO_EXCEPTIONS")
add_compile_options("-DVULKAN_HPP_ASSERT_ON_RESULT(x)")
add_compile_options("-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
add_compile_options("-DVULKAN_HPP_NAMESPACE=_VK_")
add_compile_options("-DVK_NS=_VK_")

include(CTest)
include(FetchContent)
if(ENABLE_TESTING)
  enable_testing()
endif()

find_package(SPIRV-Headers REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(tl-expected REQUIRED)
find_package(Fontconfig REQUIRED)
pkg_check_modules(VulkanMemoryAllocator REQUIRED)
pkg_check_modules(shaderc REQUIRED shaderc)

find_package(Tracy QUIET)
if(NOT $Tracy_FOUND)
  pkg_check_modules(tracy REQUIRED)
  set(TRACY_DEP tracy)
else()
  set(TRACY_DEP Tracy::TracyClient)
endif()

find_package(Catch2 3 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SPDLOG_INCLUDE_DIRS})

add_subdirectory(wren_utils)
add_subdirectory(wren_text)
add_subdirectory(wren_vk)
add_subdirectory(wrenm)
add_subdirectory(wren_reflect)
add_subdirectory(wren)
add_subdirectory(wren_gui)
add_subdirectory(editor)
