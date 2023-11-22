project(windows)

set(PROJECT_BASE_NAME ${PROJECT_NAME})

if (NOT WINDOWS)
    # Attempt to emulate Unix behavior
    add_definitions(-DTESTLIB_IS_WINDOWS=1)
endif()

add_subdirectory(console)
add_subdirectory(winmain)
