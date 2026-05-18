#include "dark_palette.h"

#include <qapplication.h>
#include <qimage.h>
#include <qpalette.h>
#include <qproxystyle.h>
#include <qstylefactory.h>

// 重写禁用态图标生成：将像素 RGB 降至原来的 35%，保留 alpha。
// 默认算法对白色图标效果不明显，此实现使白色图标在禁用时变为明显的深灰色。
class DarkStyle : public QProxyStyle
{
public:
    explicit DarkStyle()
        : QProxyStyle(QStyleFactory::create("Fusion")) {}

    QPixmap generatedIconPixmap(
        QIcon::Mode iconMode,
        const QPixmap& pixmap,
        const QStyleOption* opt) const override
    {
        if (iconMode == QIcon::Disabled)
        {
            QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
            for (int y = 0; y < img.height(); ++y)
            {
                auto* line = reinterpret_cast<QRgb*>(img.scanLine(y));
                for (int x = 0; x < img.width(); ++x)
                {
                    const QRgb px = line[x];
                    line[x] = qRgba(
                        qRed(px)   * 35 / 100,
                        qGreen(px) * 35 / 100,
                        qBlue(px)  * 35 / 100,
                        qAlpha(px)
                    );
                }
            }

            return QPixmap::fromImage(img);
        }

        return QProxyStyle::generatedIconPixmap(iconMode, pixmap, opt);
    }
};

void setDarkPalette()
{
    qApp->setStyle(new DarkStyle());

    QPalette darkPalette;

    // 正常态
    darkPalette.setColor(QPalette::Window,          QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText,      Qt::white);
    darkPalette.setColor(QPalette::Base,            QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase,   QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase,     Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,     Qt::white);
    darkPalette.setColor(QPalette::Text,            Qt::white);
    darkPalette.setColor(QPalette::Button,          QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText,      Qt::white);
    darkPalette.setColor(QPalette::BrightText,      Qt::red);
    darkPalette.setColor(QPalette::Link,            QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight,       QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    // 禁用态
    darkPalette.setColor(QPalette::Disabled, QPalette::Window,          QColor(53, 53, 53));
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText,      QColor(100, 100, 100));
    darkPalette.setColor(QPalette::Disabled, QPalette::Base,            QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Disabled, QPalette::AlternateBase,   QColor(53, 53, 53));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text,            QColor(100, 100, 100));
    darkPalette.setColor(QPalette::Disabled, QPalette::Button,          QColor(45, 45, 45));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText,      QColor(100, 100, 100));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight,       QColor(60, 60, 60));
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(100, 100, 100));

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}
