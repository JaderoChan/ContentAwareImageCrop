cmake_minimum_required(VERSION 3.17)

project(ContentAwareImageCropFrontend VERSION 1.0.3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 默认使用Release构建
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 查找Qt库
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

###############
# 配置第三方库 #
###############

include(FetchContent)
set(3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

# 配置Easy Translate库
FetchContent_Declare(
    easy_translate
    SOURCE_DIR ${3RDPARTY_DIR}/easy_translate
)
FetchContent_MakeAvailable(easy_translate)

###############
# 设置程序信息 #
###############

set(APP_ORGANIZATION "JaderoChan")
set(APP_ORGANIZATION_DOMAIN "io.github.jaderochan")
set(APP_TITLE "${PROJECT_NAME}")
set(APP_VERSION_MAJOR ${ContentAwareImageCrop_VERSION_MAJOR})
set(APP_VERSION_MINOR ${ContentAwareImageCrop_VERSION_MINOR})
set(APP_VERSION_PATCH ${ContentAwareImageCrop_VERSION_PATCH})
set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")
set(APP_AUTHOR "頔珞 JaderoChan")
set(APP_CONTACT_EMAIL "c_dl_cn@outlook.com")
set(APP_COPYRIGHT_TEXT "Copyright (c) 2026 JaderoChan")

# 撤销/重做行为的最大步数。此值越大，程序可用步数越多，程序占用的内存也越多。
set(DEFAULT_MAX_RECORD_STEP 10)
# Qt 图像原始数据内存分配大小上限。0 为无限制。
set(QIMAGE_ALLOCATION_LIMIT 0)

###############
# 定义常用路径 #
###############

# CMake文件夹
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# CMake模板文件夹
set(CMAKE_TEMPLATES_DIR ${CMAKE_DIR}/templates)
# 脚本文件夹路径
set(SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
# 语言文件夹路径
set(LANGUAGES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/languages)
# 资源文件夹路径
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
# 应用图标文件夹路径
set(APP_ICON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/icon)

# 重定向目标文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

#############
# 添加子目录 #
#############

add_subdirectory(src)
