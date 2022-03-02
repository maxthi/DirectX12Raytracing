# cmake according to https://github.com/alaingalvan/directx12-seed/blob/master/CMakeLists.txt
project(SimpleTriangle
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
add_executable(${PROJECT_NAME} ${FILE_SOURCES})

message(STATUS "${PROJECT_NAME} link BaseFramework")
target_link_libraries (${PROJECT_NAME} BaseFramework)

message(STATUS "${PROJECT_NAME} link DirectX12Framework")
target_link_libraries (${PROJECT_NAME} DirectX12Framework)