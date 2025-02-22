tests = ['reflect']

shaders = custom_target(
    command: [
        slangc,
        '-o', '@OUTPUT@',
        '-target', 'spirv',
        '-source-embed-language', 'cpp',
        '-source-embed-name', 'triangle',
        '-source-embed-style', 'default',
        '-preserve-params', '@INPUT@',
    ],
    input: ['triangle.slang'],
    output: ['triangle.hpp'],
)

foreach test : tests
    test(
        'wren_reflect_@0@'.format(test),
        executable(
            'wren_reflect_@0@_test'.format(test),
            '@0@.cpp'.format(test),
            shaders,
            dependencies: [wren_reflect_dep, boost_test],
            cpp_args: ['-DBOOST_TEST_MODULE=@0@'.format(test)],
        ),
    )
endforeach
