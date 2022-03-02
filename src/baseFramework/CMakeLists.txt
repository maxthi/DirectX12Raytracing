# cmake according to https://github.com/alaingalvan/directx12-seed/blob/master/CMakeLists.txt
project(BaseFramework
        VERSION 1.0.0.0
        LANGUAGES C CXX
)

# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# build library
add_library(${PROJECT_NAME} STATIC ${FILE_SOURCES})
