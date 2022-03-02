project(DirectX12Framework
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

message(STATUS "Add external include directory: ${CMAKE_CURRENT_SOURCE_DIR}/external")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external")

message(STATUS "Link d3d12.lib")
target_link_libraries(${PROJECT_NAME} d3d12.lib)

message(STATUS "Link dxgi.lib")
target_link_libraries(${PROJECT_NAME} dxgi.lib) 