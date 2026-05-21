#include "crop_image_worker.h"

#include <cstring>

#include <energy_mat.hpp>
#include <utilities.hpp>

CropImageWorker::CropImageWorker(QObject* parent) : QObject(nullptr) {}

namespace converter
{

static QImage toQImage(const Image& image)
{
    size_t bytesPerLine = image.cols * 3;
    return QImage(image.data(), image.cols, image.rows, bytesPerLine, QImage::Format_RGB888).copy();
}

static Image toImage(const QImage& qimg)
{
    constexpr int channels = 3;

    QImage src = (qimg.format() == QImage::Format_RGB888) ? qimg : qimg.convertToFormat(QImage::Format_RGB888);
    Image img(src.height(), src.width(), channels);
    // Copy each scanline individually to strip any row-alignment padding Qt may add.
    for (int r = 0; r < src.height(); ++r)
        std::memcpy(img.data() + r * src.width() * channels, src.constScanLine(r), src.width() * channels);
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

void CropImageWorker::startWork(CropImageParameters parameters)
{
    shouldClose_.store(false);

    const auto& [qimg, cropValue, limitSize, highlightColor, isLimit, isHighlight, isAntialiasing] = parameters;

    Image img = converter::toImage(qimg);
    for (size_t i = 0; i < parameters.cropValue && !shouldClose_; ++i)
    {
        std::vector<IPos> line;
        if (isLimit)
        {
            Image scaled = limitImageScale(img, limitSize.width(), limitSize.height());
            line = fetchMinimumEnergyLine(scaled);
            line = mapLineToOriginalSize(line, img.rows, img.cols, scaled.rows, scaled.cols);
        }
        else
        {
            line = fetchMinimumEnergyLine(img);
        }

        if (isHighlight)
        {
            Image highlighted = highlightLine(img, line, converter::toRgbColor(highlightColor), isAntialiasing);
            emit oneCropped(converter::toQImage(highlighted), i + 1);
        }

        img = removeLine(img, line);
    }

    emit cropFinished(converter::toQImage(img));
}

void CropImageWorker::stopWork()
{
    shouldClose_.store(true);
}
