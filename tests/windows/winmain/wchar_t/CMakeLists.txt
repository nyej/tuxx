project(${PROJECT_BASE_NAME}-wchar_t)

add_definitions(-DTUXX_CHAR_TYPE_IS_WCHAR_T=1)

tuxx_make_tests_targets(${PROJECT_NAME})
