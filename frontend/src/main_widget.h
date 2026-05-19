#pragma once

#include <qevent.h>
#include <qimage.h>
#include <qstring.h>
#include <qsize.h>
#include <qgraphicsscene.h>
#include <qgraphicsitem.h>

#include <trwidgets/trwidget.h>
#include <config.h>
#include "linked_list.h"
#include "ui_main_widget.h"

class MainWidget : public TrWidget
{
public:
    explicit MainWidget(QWidget* parent = nullptr);
    explicit MainWidget(const QString& filename, QWidget* parent = nullptr);

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

protected:
    void updateText() override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    using StepRecords = LinkedList<QImage>;

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

    // 仅更新与 Current Image （当前 Step 图像） 相关的变量值。
    void updateCurrentImageVariables(const QImage& image);
    // 将图像计入 Step Record，更新当前图像相关变量，更新图像显示和 UI 显示。
    void addNewImage(const QImage& image);

    bool importImage(const QString& filename);
    bool exportImage(const QString& filename);

    Ui::MainWidget ui;

    QGraphicsScene* scene_ = nullptr;
    QGraphicsPixmapItem* pixmapItem_ = nullptr;
    QGraphicsRectItem* dimOverlayLeft_ = nullptr;
    QGraphicsRectItem* dimOverlayRight_ = nullptr;
    QGraphicsLineItem* rangeHintLineLow_ = nullptr;
    QGraphicsLineItem* rangeHintLineHigh_ = nullptr;

    QImage originImage_;
    QImage currentImage_;
    // 步骤记录，用于操作撤销与重做。
    size_t maxRecordSteps_ = DEFAULT_MAX_RECORD_STEP;
    StepRecords records_;

    QString filename_;
    size_t filesize_ = 0;
    // 原始图像尺寸
    QSize originImageSize_ = {0, 0};
    // 当前 Step 图像尺寸
    QSize currentImageSize_ = {0, 0};
    // 预估结果图像尺寸（当前 Step 图像裁切后的大小）
    QSize resultImageSize_ = {0, 0};

    // 裁切范围
    size_t cropRangeLow_ = 0;
    size_t cropRangeHigh_ = 0;
    // 裁切值
    size_t cropValue_ = 0;

    bool isCropping_ = false;

    QString lastOpenDirectory_;
};
