cmake_minimum_required(VERSION 3.17)

# 收集源文件
file(
    GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# 生成
qt_standard_project_setup()
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 配置依赖
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ContentAwareImageCropBackend
)
