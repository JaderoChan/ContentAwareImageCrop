#pragma once

#include <qevent.h>
#include <qimage.h>
#include <qstring.h>
#include <qsize.h>
#include <qgraphicsscene.h>
#include <qgraphicsitem.h>

#include <trwidgets/trwidget.h>
#include "linked_list.h"
#include "ui_main_widget.h"

class MainWidget : public TrWidget
{
public:
    explicit MainWidget(QWidget* parent = nullptr);
    explicit MainWidget(const QString& filename, QWidget* parent = nullptr);

    bool importImage(bool showMessageBoxOnError);
    bool exportImage(bool showMessageBoxOnError);

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

    void onResetRangeButtonClicked();
    void onResetValueButtonClicked();

private:
    using StepRecords = LinkedList<QImage>;

    void updateDisplayedImage(const QImage& image);
    void updateViewTransform();

    void updateFileInfoUi();
    void updateImageSizeUi();
    void updateRangeSliderUi();
    void updateSliderUi();
    void updateUndoRedoUi();
    void updateProgressBarAndButtonUi();
    void updateAllUi();

    void updateCurrentImageVariables(const QImage& image);
    void setToNewImage(const QImage& image);

    bool importImage(const QString& filename);
    bool exportImage(const QString& filename);

    Ui::MainWidget ui;

    QGraphicsScene* scene_ = nullptr;
    QGraphicsPixmapItem* pixmapItem_ = nullptr;

    QImage originImage_;
    QImage currentImage_;
    StepRecords records_;

    QString filename_;
    size_t filesize_ = 0;
    QSize originImageSize_ = {0, 0};
    QSize currentImageSize_ = {0, 0};
    QSize resultImageSize_ = {0, 0};

    size_t cropRangeLow_ = 0;
    size_t cropRangeHigh_ = 0;
    size_t cropValue_ = 0;

    bool isCropping_ = false;

    QString lastOpenDirectory_;
};
