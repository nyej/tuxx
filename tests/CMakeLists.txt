if (UNIX)
    add_compile_options(-pthread)
    link_libraries(pthread)
endif()

add_definitions(-DTUXX_UNDER_TEST)
set(TEST_ADD_CASES_CPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test_add_cases.cpp)
set(TEST_ASSERTS_CPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test_asserts.cpp)

add_subdirectory(unix)
add_subdirectory(windows)
