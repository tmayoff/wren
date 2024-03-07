tests = []

deps = [catch2, wren_reflect_dep, shaderc, spdlog]

foreach test : tests
    test(test, executable(test, '@0@.cpp'.format(test), dependencies: deps))
endforeach
