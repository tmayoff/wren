add_library(wren_text src/font.cpp)
target_include_directories(
  wren_text PUBLIC include ${Fontconfig_INCLUDE_DIRS})
target_include_directories(wren_text PRIVATE include/wren_text)
target_link_libraries(wren_text PUBLIC wren_utils fontconfig spdlog::spdlog)
