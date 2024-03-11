cmake_minimum_required(VERSION 3.28)

project(wren)

find_package(PkgConfig REQUIRED)
list(APPEND CMAKE_MODULE_PATH
     "${PROJECT_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(tl-expected REQUIRED)
pkg_check_modules(shaderc REQUIRED shaderc)

include_directories(${SDL2_INCLUDE_DIRS} ${SPDLOG_INCLUDE_DIRS})

add_subdirectory(wrenm)
add_subdirectory(wren_reflect)
add_subdirectory(wren)
add_subdirectory(editor)
