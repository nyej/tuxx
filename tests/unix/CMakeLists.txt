project(unix)

set(PROJECT_BASE_NAME ${PROJECT_NAME})

if (NOT UNIX)
    # Attempt to emulate Unix behavior
    add_definitions(-DTESTLIB_IS_UNIX=1)
endif()

add_subdirectory(char)
add_subdirectory(wchar_t)
