cmake_minimum_required(VERSION 3.17)

#############
# 收集源文件 #
#############

# 配置config头文件
configure_file(
    ${CMAKE_TEMPLATES_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.h
    @ONLY
)

# 头文件（可用于在Visual Studio IDE的项目文件树中显示头文件）
file(GLOB_RECURSE HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
# 源文件
file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# UI文件
file(GLOB_RECURSE UI ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
# QRC文件
set(QRC ${RESOURCES_DIR}/app.qrc)
set(PROJECT_SOURCE ${HEADER} ${SRC} ${UI} ${QRC})

###########
# 构建程序 #
###########

qt_standard_project_setup()
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

###########
# 配置依赖 #
###########

# 配置头文件路径
target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# 链接依赖库
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    easy_translate::easy_translate
)

###############
# 配置生成目标 #
###############

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# 指定为GUI应用程序
if(WIN32)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

# 程序是否自动更新语言文件（开发时使用）
option(UPDATE_TRANSLATIONS_FILES "Whether update the translations files. Only for dev." OFF)
target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
    $<$<BOOL:UPDATE_TRANSLATIONS_FILES>:EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES>
)

# 配置应用资源/属性
if(WIN32)
    # Windows
    set(APP_ICON ${APP_ICON_DIR}/app.ico)

    set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    configure_file(
        ${CMAKE_TEMPLATES_DIR}/app.rc.in
        ${RC_FILE}
        @ONLY
    )
    target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
elseif(APPLE)
    # Apple
    set(APP_ICON ${APP_ICON_DIR}/app.icns)

    # Info Plist Strings
    set(
        INFO_PLIST_STRINGS_NAMES
        en.lproj
        zh_CN.lproj
    )
    foreach(INFO_PLIST_STRINGS_NAME IN LISTS INFO_PLIST_STRINGS_NAMES)
        configure_file(
            ${CMAKE_TEMPLATES_DIR}/InfoPlistStrings/${INFO_PLIST_STRINGS_NAME}/InfoPlist.strings.in
            ${CMAKE_CURRENT_BINARY_DIR}/${INFO_PLIST_STRINGS_NAME}/InfoPlist.strings
            @ONLY
        )
    endforeach()
    foreach(INFO_PLIST_STRINGS_NAME IN LISTS INFO_PLIST_STRINGS_NAMES)
        set(INFO_PLIST_STRINGS_FILE ${CMAKE_CURRENT_BINARY_DIR}/${INFO_PLIST_STRINGS_NAME}/InfoPlist.strings)
        set_source_files_properties(
            ${INFO_PLIST_STRINGS_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/${INFO_PLIST_STRINGS_NAME}"
        )
        target_sources(${PROJECT_NAME} PRIVATE ${INFO_PLIST_STRINGS_FILE})
    endforeach()

    # Info Plist
    set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    configure_file(
        ${CMAKE_TEMPLATES_DIR}/Info.plist.in
        ${MACOSX_BUNDLE_INFO_PLIST}
        @ONLY
    )

    # 应用图标
    set_source_files_properties(
        ${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON})

    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST}
    )

    # 语言文件
    file(GLOB LANG_FILES ${LANGUAGES_DIR}/*)
    set_source_files_properties(
        ${LANG_FILES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/languages"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${LANG_FILES})
else()
    # TODO
endif()

#################
# 生成后额外命令 #
#################

# 复制语言文件至输出目录
if(NOT APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${LANGUAGES_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/languages
    )
endif()

# Qt部署（需要保证*deployqt可见）
if(WIN32)
    set(DEPLOYQT windeployqt)
    set(OUTPUT_SUFFIX "exe")
elseif(APPLE)
    set(DEPLOYQT macdeployqt)
    set(OUTPUT_SUFFIX "app")
else()
    # TODO
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${DEPLOYQT}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.${OUTPUT_SUFFIX}
    $<$<PLATFORM_ID:Darwin>:-codesign=->
)

# MacOS DMG打包
option(CREATE_DMG "Create DMG for MacOS, create-dmg is required." OFF)
if(APPLE AND CREATE_DMG)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${SCRIPTS_DIR}/create_dmg.sh
        ${PROJECT_NAME}
        ${APP_ICON}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()
