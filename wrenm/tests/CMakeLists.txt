list(APPEND WRENM_TESTS vector matrix)

foreach(test IN LISTS WRENM_TESTS)
  add_executable("test_${test}" "${test}.cpp")
  target_link_libraries("test_${test}"
                        PRIVATE wrenm Catch2::Catch2WithMain)
  add_test(NAME "${test}" COMMAND "test_${test}")
endforeach()
