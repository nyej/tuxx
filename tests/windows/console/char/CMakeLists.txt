project(${PROJECT_BASE_NAME}-char)

add_definitions(-DTUXX_CHAR_TYPE_IS_CHAR=1)

set(test_add_cases_name ${PROJECT_NAME}-test_add_cases)
add_executable(${test_add_cases_name} ${TEST_ADD_CASES_CPP_FILE})
add_test(NAME ${test_add_cases_name} COMMAND ${test_add_cases_name})

set(test_asserts_name ${PROJECT_NAME}-test_asserts)
add_executable(${test_asserts_name} ${TEST_ASSERTS_CPP_FILE})
add_test(NAME ${test_asserts_name} COMMAND ${test_asserts_name})
