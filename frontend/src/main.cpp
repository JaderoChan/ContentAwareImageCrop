#include <qapplication.h>
#include <qimagereader.h>

#include <easy_translate.hpp>

#include <config.h>
#include "logo_icon.h"
#include "settings.h"
#include "dark_palette.h"
#include "main_widget.h"

int main(int argc, char* argv[])
{
    // 设置程序全局属性
    QApplication a(argc, argv);
    QImageReader::setAllocationLimit(QIMAGE_ALLOCATION_LIMIT);
    a.setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    a.setOrganizationName(APP_ORGANIZATION);
    a.setApplicationName(APP_TITLE);
    a.setApplicationVersion(APP_VERSION);
    a.setWindowIcon(getLogoIcon());

    // 设置语言
    {
        Settings settings = loadSettings();
        setLanguage(settings.language);
    }

    // Set to dark palette.
    setDarkPalette();

    MainWidget wgt;
    wgt.show();

    int ret = a.exec();

    // 更新翻译文件（实际上由编译选项 `UPDATE_TRANSLATIONS_FILES` 决定是否真正更新）
    easytr::updateTranslationsFiles();

    return ret;
}
