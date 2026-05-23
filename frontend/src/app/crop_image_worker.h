#pragma once

#include <atomic>

#include <qimage.h>
#include <qobject.h>
#include <qsize.h>

struct CropImageParameters
{
    // 裁切图。
    QImage image;
    // 裁切范围左边界（含）。
    int cropRangeLow{0};
    // 裁切范围右边界（不含）。
    int cropRangeHigh{-1};  // -1 表示使用图像完整宽度（即 img.cols）
    // 裁切量。
    size_t cropValue{0};

    // 画面更新周期。
    size_t cropUpdateT{0};  // 0 为每次裁切后都更新画面。

    // 反馈图中是否高亮裁切线。
    bool isHighlightLine{true};
    // 是否对裁切线使用抗锯齿。
    bool isAntialiasingLine{true};
    // 在裁切前是否通过缩放限制图像尺寸。
    bool isLimitImageSize{true};
    // 裁切线的高亮颜色。
    QColor highlightLineColor{"#ffff00"};
    // 图像尺寸限制。（仅在 isLimitImageSize 为真时可用）
    QSize imageLimitedSize{640, 640};
};

class CropImageWorker : public QObject
{
    Q_OBJECT

public:
    explicit CropImageWorker(QObject* parent = nullptr);

    void startCropWork(CropImageParameters parameters);
    void startMakeEnergyImageWork(QImage image);
    void stopWork();

signals:
    void cropUpdated(const QImage& image, size_t progress);
    void workFinished(const QImage& image);

private:
    std::atomic<bool> shouldClose_{false};
};
