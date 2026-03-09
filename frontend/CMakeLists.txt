cmake_minimum_required(VERSION 3.17)

project(ContentAwareImageCropFrontend VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找Qt库
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

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
