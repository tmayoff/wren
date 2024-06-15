tests = ['vector']
foreach test : tests
    test(
        'wren_math_@0@'.format(test),
        executable(
            'wren_math_@0@_test'.format(test),
            '@0@.cpp'.format(test),
            dependencies: [wrenm_dep, boost_test],
            cpp_args: ['-DBOOST_TEST_MODULE=@0@'.format(test)],
        ),
    )
endforeach
