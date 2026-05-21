#pragma once

#include <atomic>

#include <qimage.h>
#include <qobject.h>
#include <qsize.h>

struct CropImageParameters
{
    // 裁切图。
    QImage image;
    // 裁切量。
    size_t cropValue{0};
    // 图像尺寸限制。（仅在 isLimitImageSize 为真时可用）
    QSize limitImageSize{640, 640};
    // 裁切线的高亮颜色。
    QColor highlightLineColor{"#ffff00"};
    // 在裁切前是否通过缩放限制图像尺寸。
    bool isLimitImageSize{true};
    // 反馈图中是否高亮裁切线。
    bool isHighlightLine{true};
    // 是否对裁切线使用抗锯齿。
    bool isAntialiasingLine{true};
};

class CropImageWorker : public QObject
{
    Q_OBJECT

public:
    explicit CropImageWorker(QObject* parent = nullptr);

    void startWork(CropImageParameters parameters);
    void stopWork();

signals:
    // 每裁切线一次进行反馈。
    void oneCropped(const QImage& image, size_t progress);
    void cropFinished(const QImage& image);

private:
    std::atomic<bool> shouldClose_{false};
};
