cmake_minimum_required(VERSION 3.16)
project(cpuemu VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


set(SOURCE_FILES
    src/main.cpp
    src/assembler.cpp
    src/cpu.cpp
    src/window.cpp
    src/funcs.cpp
)
# set(SFML_DIR "S:\\dev\\cpplibs\\SFML-2.6.1\\lib\\cmake\\SFML")
# set(SFML_STATIC_LIBRARIES TRUE)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


#S:\dev\cpplibs\SFML-2.6.1\include


target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    opengl32
    winmm
    gdi32
    )

# if(WIN32)
#     add_custom_command(
#         TARGET ${PROJECT_NAME}
#         COMMENT "Copy OpenAL DLL"
#         PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
#         VERBATIM)
# endif()

