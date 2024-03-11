add_executable(wren_editor src/main.cpp)
target_link_libraries(wren_editor PUBLIC wren)

install(TARGETS wren_editor)
