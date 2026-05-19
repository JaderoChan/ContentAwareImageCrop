#include "main_widget.h"

#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qlist.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qtransform.h>
#include <qurl.h>

#include <config.h>
#include "strict_int_validator.h"

MainWidget::MainWidget(QWidget* parent)
    : TrWidget(parent)
{
    ui.setupUi(this);

    // Init graphics view
    scene_ = new QGraphicsScene(this);
    pixmapItem_ = scene_->addPixmap(QPixmap());
    ui.graphicsView->setScene(scene_);
    ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.graphicsView->viewport()->setAcceptDrops(true);
    ui.graphicsView->viewport()->installEventFilter(this);

    // Connects
    connect(ui.clockwiseButton, &QPushButton::clicked, this, &MainWidget::clockwiseImage);
    connect(ui.anticlockwiseButton, &QPushButton::clicked, this, &MainWidget::anticlockwiseImage);
    connect(ui.horFlipButton, &QPushButton::clicked, this, &MainWidget::horizontalFlipImage);
    connect(ui.verFlipButton, &QPushButton::clicked, this, &MainWidget::verticalFlipImage);

    connect(ui.resetRangeButton, &QPushButton::clicked, this, &MainWidget::onResetRangeButtonClicked);
    connect(ui.resetValueButton, &QPushButton::clicked, this, &MainWidget::onResetValueButtonClicked);

    connect(ui.undoButton, &QPushButton::clicked, this, &MainWidget::undo);
    connect(ui.redoButton, &QPushButton::clicked, this, &MainWidget::redo);

    // TODO: Hard coding is not allowed! Show message box when error occured.
    connect(ui.exportButton, &QPushButton::clicked, this, [=]() { exportImage(true); });

    // Update UI
    updateAllUi();
    updateText();
}

MainWidget::MainWidget(const QString& filename, QWidget* parent)
    : MainWidget(parent)
{
    importImage(filename);
    updateAllUi();
}

bool MainWidget::importImage(bool showMessageBoxOnError)
{
    QString filename = QFileDialog::getOpenFileName(
        this, EASYTR("Open Image"), lastOpenDirectory_.isEmpty() ? QDir::currentPath() : lastOpenDirectory_,
        QString("%1 (*.png *jpg *.jpeg *.bmp);;%2 (*)").arg(EASYTR("Image Files")).arg(EASYTR("All Files")));

    if (filename.isEmpty())
    {
        if (showMessageBoxOnError)
            QMessageBox::warning(this, EASYTR("Warning"), EASYTR("No files are open."), EASYTR("Ok"));
        return false;
    }

    lastOpenDirectory_ = QFileInfo(filename).dir().path();

    if (!importImage(filename))
    {
        if (showMessageBoxOnError)
            QMessageBox::warning(this, EASYTR("Warning"), EASYTR("Failed to open image file."), EASYTR("Ok"));
        return false;
    }

    return true;
}

bool MainWidget::exportImage(bool showMessageBoxOnError)
{
    QString filename = QFileDialog::getSaveFileName(
        this, EASYTR("Save Image"), lastOpenDirectory_.isEmpty() ? QDir::currentPath() : lastOpenDirectory_,
        QString("%1 (*.png *jpg *.jpeg *.bmp);;%2 (*)").arg(EASYTR("Image Files")).arg(EASYTR("All Files")));

    if (filename.isEmpty())
    {
        if (showMessageBoxOnError)
            QMessageBox::warning(this, EASYTR("Warning"), EASYTR("No files are save."), EASYTR("Ok"));
        return false;
    }

    lastOpenDirectory_ = QFileInfo(filename).dir().path();

    if (!exportImage(filename))
    {
        if (showMessageBoxOnError)
            QMessageBox::warning(this, EASYTR("Warning"), EASYTR("Failed to save the image file."), EASYTR("Ok"));
        return false;
    }

    return true;
}

void MainWidget::startCrop(bool highlightLowEnergyLine)
{
    // TODO
}

void MainWidget::clockwiseImage()
{
    addNewImage(currentImage_.transformed(QTransform().rotate(90)));
}

void MainWidget::anticlockwiseImage()
{
    addNewImage(currentImage_.transformed(QTransform().rotate(-90)));
}

void MainWidget::horizontalFlipImage()
{
    addNewImage(currentImage_.mirrored(true, false));
}

void MainWidget::verticalFlipImage()
{
    addNewImage(currentImage_.mirrored(false, true));
}

bool MainWidget::undo()
{
    if (records_.current() && records_.current()->hasPrevious())
    {
        records_.moveToPrevious();
        QImage image = records_.current()->value();
        updateCurrentImageVariables(image);
        updateDisplayedImage(currentImage_);
        updateAllUi();
        return true;
    }

    return false;
}

bool MainWidget::redo()
{
    if (records_.current() && records_.current()->hasNext())
    {
        records_.moveToNext();
        QImage image = records_.current()->value();
        updateCurrentImageVariables(image);
        updateDisplayedImage(currentImage_);
        updateAllUi();
        return true;
    }

    return false;
}

void MainWidget::updateText()
{
    ui.clockwiseButton->setToolTip(EASYTR("Rotate 90 degrees clockwise"));
    ui.anticlockwiseButton->setToolTip(EASYTR("Rotate 90 degrees anticlockwise"));
    ui.horFlipButton->setToolTip(EASYTR("Horizontal flip"));
    ui.verFlipButton->setToolTip(EASYTR("Vertical flip"));
    ui.undoButton->setToolTip(EASYTR("Undo"));
    ui.redoButton->setToolTip(EASYTR("Redo"));
    ui.differentButton->setToolTip(EASYTR("Compared with the original image"));
}

bool MainWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui.graphicsView->viewport())
    {
        switch (event->type())
        {
            case QEvent::DragEnter:
            {
                auto* e = static_cast<QDragEnterEvent*>(event);
                if (e->mimeData()->hasUrls())
                    e->acceptProposedAction();
                return true;
            }
            case QEvent::DragMove:
            {
                auto* e = static_cast<QDragMoveEvent*>(event);
                if (e->mimeData()->hasUrls())
                    e->acceptProposedAction();
                return true;
            }
            case QEvent::Drop:
            {
                auto* e = static_cast<QDropEvent*>(event);
                if (e->mimeData()->hasUrls())
                {
                    const QUrl url = e->mimeData()->urls().constFirst();
                    if (url.isLocalFile())
                        importImage(url.toLocalFile());
                }
                return true;
            }
            default:
                break;
        }
    }

    return TrWidget::eventFilter(obj, event);
}

void MainWidget::onResetRangeButtonClicked()
{
    cropRangeLow_ = 0;
    cropRangeHigh_ = currentImageSize_.width();
    updateRangeSliderUi();
}

void MainWidget::onResetValueButtonClicked()
{
    cropValue_ = 0;
    updateSliderUi();
}

void MainWidget::resizeEvent(QResizeEvent* event)
{
    TrWidget::resizeEvent(event);
    updateViewTransform();
}

void MainWidget::updateViewTransform()
{
    if (!pixmapItem_ || pixmapItem_->pixmap().isNull())
        return;

    QSize viewSize = ui.graphicsView->viewport()->size();
    QSize imgSize = pixmapItem_->pixmap().size();

    if (imgSize.width() <= viewSize.width() && imgSize.height() <= viewSize.height())
        ui.graphicsView->resetTransform();
    else
        ui.graphicsView->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}

void MainWidget::updateDisplayedImage(const QImage& image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    pixmapItem_->setPixmap(pixmap);
    scene_->setSceneRect(pixmap.rect());
    updateViewTransform();
}

static double byteToMB(size_t byte)
{
    constexpr size_t rate = 1024 * 1024;
    return static_cast<double>(byte) / rate;
}

void MainWidget::updateFileInfoUi()
{
    ui.filenameLabel->setText(filename_.isEmpty() ? EASYTR("No Image") : filename_);
    ui.filenameLabel->setToolTip(filename_);
    ui.filesizeLabel->setText(QString("%1 MB").arg(byteToMB(filesize_)));
}

void MainWidget::updateImageSizeUi()
{
    ui.originSizeLabel->setText(QString("%1 x %2").arg(originImageSize_.width()).arg(originImageSize_.height()));
    ui.resultSizeLabel->setText(QString("%1 x %2").arg(resultImageSize_.width()).arg(resultImageSize_.height()));
}

void MainWidget::updateRangeSliderUi()
{
    ui.rangeSlider->setRange(0, currentImageSize_.width());
    ui.rangeSlider->setLowValue(cropRangeLow_);
    ui.rangeSlider->setHighValue(cropRangeHigh_);
    ui.rangeSlider->setEnabled(currentImageSize_.width() != 0);

    ui.rangeLowLineEdit->setText(QString::number(cropRangeLow_));
    ui.rangeHighLineEdit->setText(QString::number(cropRangeHigh_));

    auto* lowValidator = new StrictIntValidator(0, currentImageSize_.width(), &cropRangeLow_, this);
    lowValidator->setDynamicTop(&cropRangeHigh_);
    ui.rangeLowLineEdit->setValidator(lowValidator);

    auto* highValidator = new StrictIntValidator(0, currentImageSize_.width(), &cropRangeHigh_, this);
    highValidator->setDynamicBottom(&cropRangeLow_);
    ui.rangeHighLineEdit->setValidator(highValidator);

    ui.rangeLowLineEdit->setEnabled(currentImageSize_.width() != 0);
    ui.rangeHighLineEdit->setEnabled(currentImageSize_.width() != 0);
    ui.resetRangeButton->setEnabled(currentImageSize_.width() != 0);
}

void MainWidget::updateSliderUi()
{
    ui.valueSlider->setRange(0, currentImageSize_.width());
    ui.valueSlider->setValue(cropValue_);
    ui.valueLineEdit->setEnabled(currentImageSize_.width() != 0);

    ui.valueLineEdit->setText(QString::number(cropValue_));
    ui.valueLineEdit->setValidator(new StrictIntValidator(0, currentImageSize_.width(), &cropValue_, this));
    ui.valueLineEdit->setEnabled(currentImageSize_.width() != 0);
    ui.resetValueButton->setEnabled(currentImageSize_.width() != 0);
}

void MainWidget::updateUndoRedoUi()
{
    bool condition = (!isCropping_ && records_.current());
    ui.undoButton->setEnabled(condition && records_.current()->hasPrevious());
    ui.redoButton->setEnabled(condition && records_.current()->hasNext());
}

void MainWidget::updateProgressBarAndButtonUi()
{
    QList<QWidget*> opButtons = {
        ui.clockwiseButton,
        ui.anticlockwiseButton,
        ui.horFlipButton,
        ui.verFlipButton,
        ui.differentButton,
        ui.exportButton
    };

    for (const auto& btn : opButtons)
        btn->setEnabled(!isCropping_ && records_.current());
    ui.cropButton->setEnabled(records_.current());
    updateUndoRedoUi();

    ui.progressBar->setVisible(isCropping_);
}

void MainWidget::updateAllUi()
{
    updateFileInfoUi();
    updateImageSizeUi();
    updateRangeSliderUi();
    updateSliderUi();
    updateProgressBarAndButtonUi();
}

void MainWidget::updateCurrentImageVariables(const QImage& image)
{
    currentImage_ = image;
    currentImageSize_ = image.size();
    resultImageSize_ = currentImageSize_;

    cropRangeLow_ = 0;
    cropRangeHigh_ = currentImageSize_.width();
    cropValue_ = 0;
}

void MainWidget::addNewImage(const QImage& image)
{
    records_.removeAllNext();
    records_.insertNext(image);
    records_.moveToNext();

    updateCurrentImageVariables(image);
    updateDisplayedImage(currentImage_);
    updateAllUi();
}

bool MainWidget::importImage(const QString& filename)
{
    QImage image;
    if (image.load(filename))
    {
        originImage_ = image;
        originImageSize_ = image.size();

        filename_ = filename;
        filesize_ = QFileInfo(filename).size();

        records_.clear();
        records_.insertNext(image);
        records_.moveToHead();

        updateCurrentImageVariables(image);
        updateDisplayedImage(currentImage_);
        updateAllUi();

        return true;
    }

    return false;
}

bool MainWidget::exportImage(const QString& filename)
{
    if (records_.current())
    {
        QImage image = records_.current()->value();
        return image.save(filename);
    }

    return false;
}
