cmake_minimum_required(VERSION 3.17)

project(ContentAwareImageCropBackend VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集源文件
file(
    GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)
file(
    GLOB_RECURSE SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

set(PROJECT_SOURCES ${HEADERS} ${SRCS})

# 生成静态库
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

# 配置依赖
target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 导出库
file(GLOB BASE_CPP_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/base/*.hpp)
set(
    EXPORT_HEADERS ${BASE_CPP_HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/content_aware_image_crop.hpp
)
