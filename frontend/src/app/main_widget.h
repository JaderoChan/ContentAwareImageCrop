#pragma once

#include <qevent.h>
#include <qimage.h>
#include <qstring.h>
#include <qsize.h>
#include <qgraphicsscene.h>
#include <qgraphicsitem.h>
#include <qthread>

#include <config.h>
#include <trwidgets/trwidget.h>
#include <common/linked_list.h>
#include "crop_image_worker.h"
#include "ui_main_widget.h"

class MainWidget : public TrWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = nullptr);
    explicit MainWidget(const QString& filename, QWidget* parent = nullptr);
    ~MainWidget();

    bool importImage(bool showMessageBoxOnError);
    bool exportImage(bool showMessageBoxOnError);

    bool setMaxRecordSteps(size_t steps);

    bool setCropRange(size_t low, size_t high);
    bool setCropValue(size_t value);

    // 开始内容感知图像裁切。
    void startCrop(bool highlightLowEnergyLine);

    void clockwiseImage();
    void anticlockwiseImage();
    void horizontalFlipImage();
    void verticalFlipImage();

    bool undo();
    bool redo();

    QImage originImage() const { return getCurrent() ? getCurrent()->originImage : QImage(); }
    QImage currentImage() const { return getCurrent() ? getCurrent()->currentImage : QImage(); }

    QString filename() const { return getCurrent() ? getCurrent()->filename : QString(); }
    size_t filesize() const { return getCurrent() ? getCurrent()->filesize : 0; }

    size_t cropRangeLow() const { return getCurrent() ? getCurrent()->cropRangeLow : 0; }
    size_t cropRangeHigh() const { return getCurrent() ? getCurrent()->cropRangeHigh : 0; }
    size_t cropValue() const { return getCurrent() ? getCurrent()->cropValue : 0; }

    QSize originImageSize() const { return originImage().isNull() ? QSize(0, 0) : originImage().size(); }
    QSize currentImageSize() const { return currentImage().isNull() ? QSize(0, 0) : currentImage().size(); }
    QSize resultImageSize() const { return QSize(currentImage().width() - cropValue(), currentImage().height()); }

signals:
    void startWork(CropImageParameters parameters);
    void stopWork();

protected:
    void updateText() override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    void onOneCropped(const QImage& image, size_t progress);
    void onCropFinished(const QImage& image);

private:
    struct RecordStep
    {
        RecordStep() = default;
        RecordStep(
            const QImage& originImage, const QImage& currentImage,
            const QString& filename = QString(), size_t filesize = 0,
            size_t cropRangeLow = 0, size_t cropRangeHigh = 0, size_t cropValue = 0)
            : originImage(originImage), currentImage(currentImage),
            filename(filename), filesize(filesize),
            cropRangeLow(cropRangeLow), cropRangeHigh(cropRangeHigh), cropValue(cropValue) {}

        // QImage 是隐式共享类之一，所以同一图片的多个步骤的 originImage 不会占用额外内存。
        // 有关隐式共享的信息可以参见 https://doc.qt.io/qt-6/implicit-sharing.html。
        QImage originImage;
        QImage currentImage;
        QString filename;
        size_t filesize = 0;

        // 裁切范围
        size_t cropRangeLow = 0;
        size_t cropRangeHigh = 0;
        // 裁切值
        size_t cropValue = 0;
    };

    // 更新图像画面的显示。
    void updateDisplayedImage(const QImage& image);
    // 缩放图像（小于视图窗口尺寸的图像将按原大小居中显示，大于视图窗口尺寸的图像将等比缩放至合适大小）。
    void updateViewTransform();
    // 更新裁切范围提示线的位置与可见性。
    void updateRangeHintLines();

    void updateFileInfoUi();
    void updateImageSizeHintUi();
    void updateRangeRelatedUi();
    void updateValueRalatedUi();
    void updateUndoRedoUi();
    // 包含了 updateUndoRedoUi 函数。
    void updateProgressBarAndButtonUi();
    // 上面所有 UI 更新函数的集合。
    void updateAllUi();

    // 将图像计入 Step Record，更新与当前图像相关的变量，更新图像显示和 UI 显示。
    void addNewImage(const QImage& image);

    bool importImage(const QString& filename);
    bool exportImage(const QString& filename);

    const RecordStep* getCurrent() const { return records_.current() ? &records_.current()->value() : nullptr; }
    RecordStep* getCurrent() { return records_.current() ? &records_.current()->value() : nullptr; }

    Ui::MainWidget ui;

    QGraphicsScene* scene_ = nullptr;
    QGraphicsPixmapItem* pixmapItem_ = nullptr;
    QGraphicsRectItem* dimOverlayLeft_ = nullptr;
    QGraphicsRectItem* dimOverlayRight_ = nullptr;
    QGraphicsLineItem* rangeHintLineLow_ = nullptr;
    QGraphicsLineItem* rangeHintLineHigh_ = nullptr;

    // 步骤记录，用于操作撤销与重做。
    size_t maxRecordSteps_ = DEFAULT_MAX_RECORD_STEP;
    LinkedList<RecordStep> records_;

    bool isCropping_ = false;
    // 裁切时暂存的左侧与右侧图像（裁切范围以外的部分）。
    QImage cropLeftPart_;
    QImage cropRightPart_;
    CropImageWorker worker_;
    QThread workerThread_;

    QString lastOpenDirectory_;
};
