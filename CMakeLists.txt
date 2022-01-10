# cmake according to https://github.com/alaingalvan/directx12-seed/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)
project(DirectX12Raytracing
        VERSION 1.0.0.0
        LANGUAGES C CXX
)

# =============================================================

# Sources

file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# build exe
add_executable(${PROJECT_NAME} ${FILE_SOURCES})