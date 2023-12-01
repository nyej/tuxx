if (UNIX)
    add_compile_options(-pthread)
    link_libraries(pthread)
endif()

add_definitions(-DTUXX_UNDER_TEST)

list(
    APPEND tuxx_tests
    test_add_cases
    test_asserts
    test_utils
)

set(base_dir ${CMAKE_CURRENT_SOURCE_DIR})

function(tuxx_make_tests_targets project_name)
    foreach(tst ${tuxx_tests})
        set(test_case_name ${project_name}-${tst})
        add_executable(${test_case_name} ${base_dir}/${tst}.cpp ${base_dir}/tuxx_tests__.cpp)
        add_test(NAME ${test_case_name} COMMAND ${test_case_name})
    endforeach()
endfunction()

add_subdirectory(unix)
add_subdirectory(windows)
