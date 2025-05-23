cmake_minimum_required (VERSION 3.16)

project(panacea)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_COLOR_DIAGNOSTICS ON)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06)
FetchContent_MakeAvailable(cpr)

add_subdirectory(depends/xen)
add_subdirectory(src)

include(cmake/EmbedFiles.cmake)
embed_files("${PROJECT_SOURCE_DIR}/assets/shaders/*.*" "${CMAKE_BINARY_DIR}/assets/shaders" panacea Shaders)