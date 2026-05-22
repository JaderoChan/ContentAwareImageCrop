#include "main_widget.h"

#include <qbrush.h>
#include <qcolor.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qlist.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qpen.h>
#include <qshortcut.h>
#include <qtransform.h>
#include <qurl.h>

#include <common/strict_int_validator.h>

// 裁切范围提示线颜色。
constexpr QColor RANGE_HINT_LINE_COLOR(0, 102, 255);

void popupMessageBox(
    QMessageBox::Icon icon, const QString& title, const QString& text,
    const QString& buttonText, QMessageBox::ButtonRole buttonRole = QMessageBox::AcceptRole,
    QWidget* parent = nullptr)
{
    QMessageBox msgBox(icon, title, text, QMessageBox::NoButton, parent);
    msgBox.addButton(buttonText, buttonRole);
    msgBox.exec();
}

void popupInformation(const QString& title, const QString& text, const QString& buttonText, QWidget* parent = nullptr)
{ popupMessageBox(QMessageBox::Information, title, text, buttonText, QMessageBox::AcceptRole, parent); }

void popupWarning(const QString& title, const QString& text, const QString& buttonText, QWidget* parent = nullptr)
{ popupMessageBox(QMessageBox::Warning, title, text, buttonText, QMessageBox::AcceptRole, parent); }

MainWidget::MainWidget(QWidget* parent)
    : TrWidget(parent)
{
    ui.setupUi(this);

    // Init graphics view
    scene_ = new QGraphicsScene(this);
    pixmapItem_ = scene_->addPixmap(QPixmap());

    // 初始化图像裁切范围外的黑色遮罩。
    QBrush dimBrush(QColor(0, 0, 0, 120));
    dimOverlayLeft_ = scene_->addRect(QRectF(), Qt::NoPen, dimBrush);
    dimOverlayRight_ = scene_->addRect(QRectF(), Qt::NoPen, dimBrush);
    dimOverlayLeft_->setVisible(false);
    dimOverlayRight_->setVisible(false);

    // 初始化裁切范围提示线。
    QPen hintPen(RANGE_HINT_LINE_COLOR);
    hintPen.setCosmetic(true);
    rangeHintLineLow_ = scene_->addLine(0, 0, 0, 0, hintPen);
    rangeHintLineHigh_ = scene_->addLine(0, 0, 0, 0, hintPen);
    rangeHintLineLow_->setVisible(false);
    rangeHintLineHigh_->setVisible(false);

    ui.graphicsView->setScene(scene_);
    ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.graphicsView->viewport()->setAcceptDrops(true);
    ui.graphicsView->viewport()->installEventFilter(this);

    // 初始化工作线程
    worker_.moveToThread(&workerThread_);
    connect(this, &MainWidget::startCropWork, &worker_, &CropImageWorker::startCropWork);
    connect(this, &MainWidget::startMakeEnergyImageWork, &worker_, &CropImageWorker::startMakeEnergyImageWork);
    connect(&worker_, &CropImageWorker::cropUpdated, this, &MainWidget::onOneCropped);
    connect(&worker_, &CropImageWorker::workFinished, this, &MainWidget::onWorkFinished);
    workerThread_.start();

    // Set shortcut
    ui.clockwiseButton->setShortcut(QKeySequence(QKeyCombination(Qt::Key_BracketRight)));
    ui.anticlockwiseButton->setShortcut(QKeySequence(QKeyCombination( Qt::Key_BracketLeft)));
    ui.undoButton->setShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_Z)));
    ui.redoButton->setShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_Y)));
    ui.exportButton->setShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_E)));

    QShortcut* importImg = new QShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_O)),
    this, [this]() { importImage("", true); });
    QShortcut* addOne = new QShortcut(QKeySequence(QKeyCombination(Qt::Key_Up)),
    this, [this]() { setCropValue(cropValue() + 1); });
    QShortcut* decreaseOne = new QShortcut(QKeySequence(QKeyCombination(Qt::Key_Down)),
    this, [this]() { setCropValue(cropValue() - 1); });
    QShortcut* addTen = new QShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_Up)),
    this, [this]()
    {
        const size_t maxVal = cropRangeHigh() - cropRangeLow();
        setCropValue(cropValue() + 10 >= maxVal ? maxVal : cropValue() + 10);
    });
    QShortcut* decreaseTen = new QShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_Down)),
    this, [this]() { setCropValue(cropValue() < 10 ? 0 : (cropValue() - 10)); });
    QShortcut* crop = new QShortcut(QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_Return)),
    this, [this]() { onCropButtonClicked(); });
    QShortcut* makeEnergyImage = new QShortcut(QKeySequence(QKeyCombination(Qt::SHIFT, Qt::Key_Return)),
    this, [this]() { startMakeEnergyImage(); });

    // Connects
    connect(ui.clockwiseButton, &QPushButton::clicked, this, &MainWidget::clockwiseImage);
    connect(ui.anticlockwiseButton, &QPushButton::clicked, this, &MainWidget::anticlockwiseImage);
    connect(ui.horFlipButton, &QPushButton::clicked, this, &MainWidget::horizontalFlipImage);
    connect(ui.verFlipButton, &QPushButton::clicked, this, &MainWidget::verticalFlipImage);

    connect(ui.resetRangeButton, &QPushButton::clicked, this, [this]()
    { setCropRange(0, currentImageSize().width()); });
    connect(ui.resetValueButton, &QPushButton::clicked, this, [this]()
    { setCropValue(0); });

    connect(ui.undoButton, &QPushButton::clicked, this, &MainWidget::undo);
    connect(ui.redoButton, &QPushButton::clicked, this, &MainWidget::redo);

    connect(ui.rangeSlider, &QRangeSlider::valueChanged, this, [this](unsigned int low, unsigned int high)
    { setCropRange(low, high); });
    connect(ui.rangeLowLineEdit, &QLineEdit::textEdited, this, [this](const QString& low)
    { setCropRange(low.toUInt(), cropRangeHigh()); });
    connect(ui.rangeHighLineEdit, &QLineEdit::textEdited, this, [this](const QString& high)
    { setCropRange(cropRangeLow(), high.toUInt()); });

    connect(ui.valueSlider, &QSlider::valueChanged, this, [this](int value)
    { setCropValue(static_cast<size_t>(value)); });
    connect(ui.valueLineEdit, &QLineEdit::textEdited, this, [this](const QString& value)
    { setCropValue(value.toUInt()); });

    connect(ui.cropButton, &QPushButton::clicked, this, &MainWidget::onCropButtonClicked);
    connect(ui.makeEnergyImageButton, &QPushButton::clicked, this, &MainWidget::startMakeEnergyImage);

    connect(ui.differentButton, &QPushButton::pressed, this, [this]()
    { toggleToOriginImageDisplay(true); });
    connect(ui.differentButton, &QPushButton::released, this, [this]()
    { toggleToOriginImageDisplay(false); });

    connect(ui.exportButton, &QPushButton::clicked, this, [this]() { exportImage("", true); });

    // Update UI
    updateAllUi();
    updateText();
}

MainWidget::MainWidget(const QString& filename, QWidget* parent)
    : MainWidget(parent)
{
    importImage(filename, true);
    updateAllUi();
}

MainWidget::~MainWidget()
{
    worker_.stopWork();
    workerThread_.quit();
    workerThread_.wait();
}

bool MainWidget::importImage(QString filename, bool showMessageBoxOnError)
{
    if (isWorking_)
        return false;

    if (filename.isEmpty())
    {
        filename = QFileDialog::getOpenFileName(
            this, EASYTR("Open Image"), lastOpenDirectory_.isEmpty() ? QDir::currentPath() : lastOpenDirectory_,
            QString("%1 (*.png *jpg *.jpeg *.bmp);;%2 (*)").arg(EASYTR("Image Files")).arg(EASYTR("All Files")));
    }

    if (filename.isEmpty())
    {
        if (showMessageBoxOnError)
            popupInformation(EASYTR("Information"), EASYTR("No files are open."), EASYTR("Ok"), this);
        return false;
    }

    lastOpenDirectory_ = QFileInfo(filename).dir().path();

    QImage image;
    if (image.load(filename))
    {
        RecordStep step(image, image, filename, QFileInfo(filename).size(), 0, image.width(), 0);
        records_.insertNext(step);

        updateDisplayedImage(currentImage());
        updateAllUi();

        return true;
    }

    if (showMessageBoxOnError)
        popupWarning(EASYTR("Warning"), EASYTR("Failed to open image file."), EASYTR("Ok"), this);
    return false;
}

bool MainWidget::exportImage(QString filename, bool showMessageBoxOnError)
{
    if (isWorking_)
        return false;

    if (filename.isEmpty())
    {
        filename = QFileDialog::getSaveFileName(
            this, EASYTR("Save Image"), lastOpenDirectory_.isEmpty() ? QDir::currentPath() : lastOpenDirectory_,
            QString("%1 (*.png *jpg *.jpeg *.bmp);;%2 (*)").arg(EASYTR("Image Files")).arg(EASYTR("All Files")));
    }

    if (filename.isEmpty())
    {
        if (showMessageBoxOnError)
            popupInformation(EASYTR("Information"), EASYTR("No files are save."), EASYTR("Ok"), this);
        return false;
    }

    lastOpenDirectory_ = QFileInfo(filename).dir().path();

    if (!records_.current())
    {
        if (showMessageBoxOnError)
            popupWarning(EASYTR("Warning"), EASYTR("Failed to save the image file."), EASYTR("Ok"), this);
        return false;
    }

    QImage image = currentImage();
    return image.save(filename);
}

bool MainWidget::setMaxRecordSteps(size_t steps)
{
    if (steps == 0)
        return false;

    maxRecordSteps_ = steps;
    while (records_.length() > maxRecordSteps_)
        records_.removeHead();

    updateDisplayedImage(currentImage());
    updateAllUi();

    return true;
}

bool MainWidget::setCropRange(size_t low, size_t high)
{
    if (!getCurrent() || low >= high || high > static_cast<size_t>(currentImageSize().width()))
        return false;

    getCurrent()->cropRangeLow = low;
    getCurrent()->cropRangeHigh = high;
    setCropValue(cropValue() > (high - low) ? 0 : cropValue());

    updateRangeRelatedUi();
    return true;
}

bool MainWidget::setCropValue(size_t value)
{
    if (!getCurrent() || cropRangeHigh() <= cropRangeLow() || value > (cropRangeHigh() - cropRangeLow()))
        return false;

    getCurrent()->cropValue = value;

    ui.cropButton->setEnabled(isWorking_ ? true : (records_.current() && cropValue() != 0));
    updateImageSizeHintUi();
    updateValueRalatedUi();
    return true;
}

void MainWidget::startCrop(bool highlightLowEnergyLine)
{
    if (isWorking_ || !getCurrent() || cropValue() == 0)
        return;

    CropImageParameters parameters;
    parameters.image = currentImage();
    parameters.cropRangeLow  = static_cast<int>(cropRangeLow());
    parameters.cropRangeHigh = static_cast<int>(cropRangeHigh());
    parameters.cropValue = cropValue();

    // TODO: add application configure.
    // parameters.cropUpdateT
    // parameters.limitImageSize
    // parameters.highlightLineColor
    // parameters.isLimitImageSize
    // parameters.isHighlightLine
    // parameters.isAntialiasingLine

    isWorking_ = true;
    ui.progressBar->setMinimum(0);
    ui.progressBar->setMaximum(cropValue());
    ui.progressBar->setValue(0);
    updateAllUi();

    emit startCropWork(parameters);
}

void MainWidget::startMakeEnergyImage()
{
    if (isWorking_ || !getCurrent())
        return;

    isWorking_ = true;
    updateAllUi();
    emit startMakeEnergyImageWork(currentImage());
}

void MainWidget::clockwiseImage()
{
    if (isWorking_ || currentImage().isNull())
        return;
    addNewImage(currentImage().transformed(QTransform().rotate(90)));
}

void MainWidget::anticlockwiseImage()
{
    if (isWorking_ || currentImage().isNull())
        return;
    addNewImage(currentImage().transformed(QTransform().rotate(-90)));
}

void MainWidget::horizontalFlipImage()
{
    if (isWorking_ || currentImage().isNull())
        return;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 13, 0))
    addNewImage(currentImage().flipped(Qt::Horizontal));
#else
    addNewImage(currentImage().mirrored(true, false));
#endif
}

void MainWidget::verticalFlipImage()
{
    if (isWorking_ || currentImage().isNull())
        return;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 13, 0))
    addNewImage(currentImage().flipped(Qt::Vertical));
#else
    addNewImage(currentImage().mirrored(false, true));
#endif
}

bool MainWidget::undo()
{
    if (!isWorking_ && records_.current() && records_.current()->hasPrevious())
    {
        records_.moveToPrevious();
        updateDisplayedImage(currentImage());
        updateAllUi();
        return true;
    }
    return false;
}

bool MainWidget::redo()
{
    if (!isWorking_ && records_.current() && records_.current()->hasNext())
    {
        records_.moveToNext();
        updateDisplayedImage(currentImage());
        updateAllUi();
        return true;
    }
    return false;
}

void MainWidget::updateText()
{
    ui.clockwiseButton->setToolTip(EASYTR("Rotate 90 degrees clockwise (])"));
    ui.anticlockwiseButton->setToolTip(EASYTR("Rotate 90 degrees anticlockwise ([)"));
    ui.horFlipButton->setToolTip(EASYTR("Horizontal flip"));
    ui.verFlipButton->setToolTip(EASYTR("Vertical flip"));
    ui.resetRangeButton->setToolTip(EASYTR("Reset slider"));
    ui.resetValueButton->setToolTip(EASYTR("Reset slider"));
    ui.undoButton->setToolTip(EASYTR("Undo (Ctrl + Z)"));
    ui.redoButton->setToolTip(EASYTR("Redo (Ctrl + Y)"));
    ui.differentButton->setToolTip(EASYTR("Compared with the original image (Y)"));

    ui.cropButton->setText(EASYTR("Crop"));
    ui.cropButton->setToolTip(EASYTR("Ctrl + Enter"));
    ui.makeEnergyImageButton->setText(EASYTR("Make Energy Image"));
    ui.makeEnergyImageButton->setToolTip(EASYTR("Shift + Enter"));
    ui.exportButton->setText(EASYTR("Export"));
    ui.exportButton->setToolTip(EASYTR("Ctrl + E"));
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    // 切换原图显示。
    if (ui.differentButton->isEnabled() && event->key() == Qt::Key_Y && !event->isAutoRepeat())
        toggleToOriginImageDisplay(true);
    TrWidget::keyPressEvent(event);
}

void MainWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Y && !event->isAutoRepeat())
        toggleToOriginImageDisplay(false);
    TrWidget::keyReleaseEvent(event);
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
                        importImage(url.toLocalFile(), true);
                }
                return true;
            }
            default:
                break;
        }
    }

    return TrWidget::eventFilter(obj, event);
}

void MainWidget::onOneCropped(const QImage& image, size_t progress)
{
    ui.progressBar->setValue(progress);
    updateDisplayedImage(image);
}

void MainWidget::onWorkFinished(const QImage& image)
{
    isWorking_ = false;
    addNewImage(image);
}

void MainWidget::onCropButtonClicked()
{
    isWorking_ ? worker_.stopWork() : startCrop(true);
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

void MainWidget::updateRangeHintLines()
{
    bool visible = !currentImage().isNull() && !isWorking_;
    dimOverlayLeft_->setVisible(visible);
    dimOverlayRight_->setVisible(visible);
    rangeHintLineLow_->setVisible(visible);
    rangeHintLineHigh_->setVisible(visible);

    if (visible)
    {
        double w = currentImageSize().width();
        double h = currentImageSize().height();
        dimOverlayLeft_->setRect(0, 0, static_cast<int>(cropRangeLow()), h);
        dimOverlayRight_->setRect(cropRangeHigh(), 0, w - cropRangeHigh(), h);
        rangeHintLineLow_->setLine(static_cast<int>(cropRangeLow()), 0, static_cast<int>(cropRangeLow()), h);
        rangeHintLineHigh_->setLine(cropRangeHigh(), 0, cropRangeHigh(), h);
    }
}

static double byteToMB(size_t byte)
{
    constexpr size_t rate = 1024 * 1024;
    return static_cast<double>(byte) / rate;
}

void MainWidget::updateFileInfoUi()
{
    ui.filenameLabel->setText(filename().isEmpty() ? EASYTR("No Image") : filename());
    ui.filenameLabel->setToolTip(filename());
    ui.filesizeLabel->setText(QString("%1 MB").arg(byteToMB(filesize())));
}

void MainWidget::updateImageSizeHintUi()
{
    ui.originSizeLabel->setText(QString("%1 x %2").arg(originImageSize().width()).arg(originImageSize().height()));
    ui.resultSizeLabel->setText(QString("%1 x %2").arg(resultImageSize().width()).arg(resultImageSize().height()));
}

void MainWidget::updateRangeRelatedUi()
{
    const int imgWidth = currentImageSize().width();

    {
        QSignalBlocker blocker(ui.rangeSlider);
        if (imgWidth > 0)
        {
            ui.rangeSlider->setRange(0, imgWidth);
            ui.rangeSlider->setLowValue(cropRangeLow());
            ui.rangeSlider->setHighValue(cropRangeHigh());
        }
    }
    ui.rangeSlider->setEnabled(imgWidth > 0);

    {
        QSignalBlocker lowBlocker(ui.rangeLowLineEdit);
        QSignalBlocker highBlocker(ui.rangeHighLineEdit);
        ui.rangeLowLineEdit->setText(QString::number(cropRangeLow()));
        ui.rangeHighLineEdit->setText(QString::number(cropRangeHigh()));
    }

    if (getCurrent() && imgWidth > 0)
    {
        auto* lowValidator = new StrictIntValidator(0, imgWidth, &getCurrent()->cropRangeLow, this);
        lowValidator->setDynamicTop(&getCurrent()->cropRangeHigh);
        ui.rangeLowLineEdit->setValidator(lowValidator);

        auto* highValidator = new StrictIntValidator(0, imgWidth, &getCurrent()->cropRangeHigh, this);
        highValidator->setDynamicBottom(&getCurrent()->cropRangeLow);
        ui.rangeHighLineEdit->setValidator(highValidator);
    }

    ui.rangeLowLineEdit->setEnabled(imgWidth > 0);
    ui.rangeHighLineEdit->setEnabled(imgWidth > 0);
    ui.resetRangeButton->setEnabled(imgWidth > 0);

    updateRangeHintLines();
}

void MainWidget::updateValueRalatedUi()
{
    size_t cropRange = (cropRangeHigh() > cropRangeLow()) ? (cropRangeHigh() - cropRangeLow()) : 0;

    {
        QSignalBlocker blocker(ui.valueSlider);
        ui.valueSlider->setRange(0, cropRange);
        ui.valueSlider->setValue(cropValue());
    }
    ui.valueLineEdit->setEnabled(cropRange != 0);

    {
        QSignalBlocker blocker(ui.valueLineEdit);
        ui.valueLineEdit->setText(QString::number(cropValue()));
    }
    ui.valueLineEdit->setValidator(new StrictIntValidator(0, cropRange, &getCurrent()->cropValue, this));
    ui.valueLineEdit->setEnabled(cropRange != 0);
    ui.resetValueButton->setEnabled(cropRange != 0);
}

void MainWidget::updateUndoRedoUi()
{
    bool condition = (!isWorking_ && records_.current());
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
        ui.exportButton
    };

    for (const auto& btn : opButtons)
        btn->setEnabled(!isWorking_ && !currentImageSize().isEmpty());
    ui.differentButton->setEnabled(!isWorking_ && getCurrent());
    ui.cropButton->setText(isWorking_ ? EASYTR("Stop") : EASYTR("Crop"));
    ui.cropButton->setEnabled(isWorking_ ? true : (records_.current() && cropValue() != 0));
    ui.makeEnergyImageButton->setEnabled(!isWorking_ && !currentImageSize().isEmpty());
    updateUndoRedoUi();

    ui.progressBar->setVisible(isWorking_);
}

void MainWidget::updateAllUi()
{
    updateFileInfoUi();
    updateImageSizeHintUi();
    updateRangeRelatedUi();
    updateValueRalatedUi();
    updateProgressBarAndButtonUi();
}

void MainWidget::addNewImage(const QImage& image)
{
    if (maxRecordSteps_ == 0)
        return;

    records_.removeAllNext();
    while (records_.length() >= maxRecordSteps_)
        records_.removeHead();

    records_.insertNext(getCurrent() ?
        RecordStep(originImage(), image, filename(), filesize(), 0, image.isNull() ? 0 : image.width(), 0) :
        RecordStep(image, image, QString(), 0, 0, image.isNull() ? 0 : image.width(), 0));

    updateDisplayedImage(currentImage());
    updateAllUi();
}

void MainWidget::toggleToOriginImageDisplay(bool enable)
{
    if (enable)
    {
        updateDisplayedImage(originImage());
        dimOverlayLeft_->setVisible(false);
        dimOverlayRight_->setVisible(false);
        rangeHintLineLow_->setVisible(false);
        rangeHintLineHigh_->setVisible(false);
    }
    else
    {
        updateDisplayedImage(currentImage());
        updateRangeHintLines();
    }
}
