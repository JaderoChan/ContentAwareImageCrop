cmake_minimum_required(VERSION 3.17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 构建测试程序
option(BUILD_TEST "Whether build test program." ON)
if(BUILD_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()
