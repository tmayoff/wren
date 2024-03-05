tests = ['parser']

deps = [catch2, wren_reflect_dep, shaderc]

foreach test : tests
    test(test, executable(test, '@0@.cpp'.format(test), dependencies: deps))
endforeach
