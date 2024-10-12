tests = ['ecs']
foreach test : tests
    test(
        'ecs_@0@'.format(test),
        executable(
            'ecs_@0@_test'.format(test),
            '@0@.cpp'.format(test),
            dependencies: [ecs_dep, boost_test],
            cpp_args: ['-DBOOST_TEST_MODULE=@0@'.format(test)],
        ),
    )
endforeach
