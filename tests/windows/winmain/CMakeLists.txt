project(${PROJECT_BASE_NAME}-winmain)

set(PROJECT_BASE_NAME ${PROJECT_NAME})

add_definitions(-DTESTLIB_USE_WINMAIN=1)

add_subdirectory(char)
add_subdirectory(wchar_t)
