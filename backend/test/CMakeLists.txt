cmake_minimum_required(VERSION 3.17)

project(ContentAwareImageCropBackend_Test VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集源文件
file(
    GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 生成
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 配置依赖
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    ContentAwareImageCropBackend
)

# Temp
if(WIN32)
    set(QTDEPLOY windeployqt)
    set(TARGET_SUFFIX ".exe")
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${QTDEPLOY} ${CMAKE_BINARY_DIR}/backend/${PROJECT_NAME}${TARGET_SUFFIX}
    )
elseif(APPLE)
    set(QTDEPLOY macdeployqt)
    set(TARGET_SUFFIX "")
endif()
