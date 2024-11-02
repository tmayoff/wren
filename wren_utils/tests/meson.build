tests = ['binary_reader', 'string_reader']

foreach test : tests
    test(
        'wren_utils_@0@'.format(test),
        executable(
            'wren_utils_@0@_test'.format(test),
            '@0@.cpp'.format(test),
            dependencies: [wren_utils_dep, boost_test],
            cpp_args: ['-DBOOST_TEST_MODULE=@0@'.format(test)],
        ),
    )
endforeach
