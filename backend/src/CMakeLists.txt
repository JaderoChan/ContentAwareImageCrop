cmake_minimum_required(VERSION 3.17)

project(ContentAwareImageCropBackend VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集源文件
file(
    GLOB_RECURSE SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# 生成静态库
add_library(${PROJECT_NAME} STATIC ${SRCS})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# 配置头文件路径
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
