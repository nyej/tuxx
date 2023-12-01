project(${PROJECT_BASE_NAME}-char)

add_definitions(-DTUXX_CHAR_TYPE_IS_CHAR)

tuxx_make_tests_targets(${PROJECT_NAME})
