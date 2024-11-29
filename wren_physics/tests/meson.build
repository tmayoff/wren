tests = ['raycast']
foreach test : tests
    test(
        'wren_physics_@0@'.format(test),
        executable(
            'wren_physics_@0@_test'.format(test),
            '@0@.cpp'.format(test),
            dependencies: [wren_physics_dep, wren_dep, boost_test],
            cpp_args: ['-DBOOST_TEST_MODULE=@0@'.format(test)],
        ),
    )
endforeach
