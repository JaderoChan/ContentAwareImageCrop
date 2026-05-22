#include "crop_image_worker.h"

#include <cstring>

#include <energy_mat.hpp>
#include <utilities.hpp>

CropImageWorker::CropImageWorker(QObject* parent) : QObject(nullptr) {}

namespace converter
{

static QImage toQImage(const Image& image)
{
    if (image.isEmpty())
        return QImage();
    size_t bytesPerLine = image.cols * 3;
    return QImage(image.data(), image.cols, image.rows, bytesPerLine, QImage::Format_RGB888).copy();
}

static Image toImage(const QImage& qimg)
{
    constexpr int channels = 3;

    QImage src = (qimg.format() == QImage::Format_RGB888) ? qimg : qimg.convertToFormat(QImage::Format_RGB888);
    Image img(src.height(), src.width(), channels);
    // Copy each scanline individually to strip any row-alignment padding Qt may add.
    for (int row = 0; row < src.height(); ++row)
        std::memcpy(img.data() + row * src.width() * channels, src.constScanLine(row), src.width() * channels);
    return img;
}

static QColor toQColor(const RgbColor& color)
{
    return QColor(color.r, color.g, color.b);
}

static RgbColor toRgbColor(const QColor& color)
{
    return RgbColor{
        static_cast<uint8_t>(color.red()),
        static_cast<uint8_t>(color.green()),
        static_cast<uint8_t>(color.blue())};
}

} // namespace converter

void CropImageWorker::startCropWork(CropImageParameters parameters)
{
    shouldClose_.store(false);

    const auto&
    [qimg, cropValue, cropRangeLow, cropRangeHigh, cropUpdateT,
     limitSize, highlightColor, isLimit, isHighlight, isAntialiasing] = parameters;

    Image img = converter::toImage(qimg);

    int colLow  = cropRangeLow;
    int colHigh = (cropRangeHigh < 0) ? img.cols : cropRangeHigh;

    size_t counter = 0;
    for (size_t i = 0; i < cropValue && !shouldClose_; ++i)
    {
        if (colHigh <= colLow)
            break;

        std::vector<IPos> line;
        if (isLimit && (img.rows > limitSize.height() || img.cols > limitSize.width()))
        {
            Image scaled = limitImageScale(img, limitSize.width(), limitSize.height());
            // 将范围边界映射到缩放图坐标
            double ratio = static_cast<double>(scaled.cols) / img.cols;
            int scaledLow  = std::clamp(static_cast<int>(std::round(colLow  * ratio)), 0, scaled.cols - 1);
            int scaledHigh = std::clamp(static_cast<int>(std::round(colHigh * ratio)), 0, scaled.cols);

            if (scaledHigh > scaledLow)
            {
                line = fetchMinimumEnergyLine(scaled, scaledLow, scaledHigh);
                if (!line.empty())
                    line = mapLineToOriginalSize(line, img.rows, img.cols, scaled.rows, scaled.cols);
            }
            else
            {
                line = fetchMinimumEnergyLine(img, colLow, colHigh);
            }
        }
        else
        {
            line = fetchMinimumEnergyLine(img, colLow, colHigh);
        }

        if (line.empty())
            break;

        if (counter == cropUpdateT) // Reduce the interface update frequency to prevent UI lag.
        {
            if (isHighlight)
            {
                Image highlighted = highlightLine(img, line, converter::toRgbColor(highlightColor), isAntialiasing);
                emit cropUpdated(converter::toQImage(highlighted), i + 1);
            }
            else
            {
                emit cropUpdated(converter::toQImage(img), i + 1);
            }
        }

        counter++;
        counter = (counter > cropUpdateT ? 0 : counter);
        img = removeLine(img, line);

        colHigh--;
    }

    emit workFinished(converter::toQImage(img));
}

void CropImageWorker::startMakeEnergyImageWork(QImage image)
{
    Image img = converter::toImage(image);
    auto energyMat = createEnergyMat(img);
    energyMat = normalizeEnergyMat(energyMat);
    img = energyMatToGrayImage(energyMat);

    QImage result = QImage(img.data(), img.cols, img.rows, img.cols, QImage::Format_Grayscale8).copy();
    emit workFinished(result);
}

void CropImageWorker::stopWork()
{
    shouldClose_.store(true);
}
