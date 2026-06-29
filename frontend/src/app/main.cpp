#include <qapplication.h>
#include <qimagereader.h>

#include <easy_translate.hpp>

#include <config.h>
#include <common/dark_palette.h>
#include "logo_icon.h"
#include "settings.h"
#include "main_window.h"

int main(int argc, char* argv[])
{
    // 设置程序全局属性
    QApplication a(argc, argv);
    a.setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    a.setOrganizationName(APP_ORGANIZATION);
    a.setApplicationName(APP_TITLE);
    a.setApplicationVersion(APP_VERSION);
    // macOS 平台下不显式设置程序图标。
#ifndef Q_OS_MAC
    a.setWindowIcon(getLogoIcon());
#endif

    // 设置 Qt 图像内存分配上限
    QImageReader::setAllocationLimit(QIMAGE_ALLOCATION_LIMIT);

    // 设置语言
    {
        Settings settings = loadSettings();
        setLanguage(settings.language);
    }

    // 设置暗色调色板
    setDarkPalette();

    MainWindow wgt;
    wgt.show();

    int ret = a.exec();

    // 更新翻译文件（实际上由编译选项 UPDATE_TRANSLATIONS_FILES 决定是否真正更新）
    easytr::updateTranslationsFiles();

    return ret;
}
