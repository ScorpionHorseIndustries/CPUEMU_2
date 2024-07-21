cmake_minimum_required(VERSION 3.10)
project(cpuemu VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(SOURCE_FILES
    src/main.cpp
    src/assembler.cpp
    src/cpu.cpp
    src/window.cpp
    src/funcs.cpp
)
set(SFML_DIR "S:\\dev\\cpplibs\\SFML-2.6.1\\lib\\cmake\\SFML")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics window system REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


#S:\dev\cpplibs\SFML-2.6.1\include

if (SFML_FOUND)
    target_link_libraries(${PROJECT_NAME}
        sfml-graphics
        sfml-window
        sfml-system
        opengl32
        winmm
        gdi32
    )

endif()