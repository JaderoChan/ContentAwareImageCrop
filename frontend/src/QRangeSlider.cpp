#include "QRangeSlider.hpp"

#include <stdexcept>

#include <QPainter>

QRangeSlider::QRangeSlider(QWidget* parent)
    : QWidget(parent)
{
    lowValue_ = minimum_;
    highValue_ = maximum_;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

unsigned int QRangeSlider::minimum() const
{
    return minimum_;
}

unsigned int QRangeSlider::maximum() const
{
    return maximum_;
}

unsigned int QRangeSlider::lowValue() const
{
    return lowValue_;
}

unsigned int QRangeSlider::highValue() const
{
    return highValue_;
}

unsigned int QRangeSlider::step() const
{
    return step_;
}

void QRangeSlider::setStep(unsigned int step)
{
    step_ = step;
}

// Added orientation to support the promote option of a Horizontal Slider in QT designer.
void QRangeSlider::setOrientation(Qt::Orientation orientation)
{
    if (orientation != Qt::Horizontal)
        throw std::invalid_argument("Invalid Orientation. Horizontal is the only available orientation.");
}

void QRangeSlider::setBarDraggable(bool enable)
{
    barDraggable_ = enable;
}

QSize QRangeSlider::sizeHint() const
{
    return QSize(100 * HANDLE_SIZE + 2 * PADDING, 2 * HANDLE_SIZE + 2 * PADDING);
}

QSize QRangeSlider::minimumSizeHint() const
{
    return QSize(2 * HANDLE_SIZE + 2 * PADDING, 2 * HANDLE_SIZE);
}

void QRangeSlider::setMinimum(unsigned int minimum)
{
    if (minimum_ != minimum)
    {
        minimum_ = minimum;

        if (minimum_ >= maximum_)
        {
            setMaximum(minimum_ + 1);
            setLowValue(minimum_);
            setHighValue(maximum_);
        }
        else if (minimum_ >= highValue_)
        {
            setLowValue(minimum_);
            setHighValue(minimum_ + 1);
        }
        else if (minimum_ > lowValue_)
        {
            setLowValue(minimum_);
        }

        update();
        emit minimumChanged(minimum_);
        emit rangeChanged(minimum_, maximum_);
    }
}

void QRangeSlider::setMaximum(unsigned int maximum)
{
    if (maximum_ != maximum)
    {
        maximum_ = maximum;

        if (maximum_ <= minimum_)
        {
            setMinimum(maximum_ - 1);
            setLowValue(minimum_);
            setHighValue(maximum_);
        }
        else if (maximum_ <= lowValue_)
        {
            setLowValue(highValue_ - 1);
            setHighValue(maximum_);
        }
        else if (maximum_ < highValue_)
        {
            setHighValue(maximum_);
        }

        update();
        emit maximumChanged(maximum_);
        emit rangeChanged(minimum_, maximum_);
    }
}

void QRangeSlider::setLowValue(unsigned int lowValue)
{
    if (lowValue_ != lowValue)
    {
        lowValue_ = lowValue;

        if (lowValue_ >= maximum_)
            lowValue_ = maximum_ - 1;

        if (lowValue_ < minimum_)
            lowValue_ = minimum_;

        if (lowValue_ >= highValue_)
            setHighValue(lowValue_ + 1);

        update();
        emit lowValueChanged(lowValue_);
    }
}

void QRangeSlider::setHighValue(unsigned int highValue)
{
    if (highValue_ != highValue)
    {
        highValue_ = highValue;

        if (highValue_ > maximum_)
            highValue_ = maximum_;

        if (highValue_ <= minimum_)
            highValue_ = minimum_ + 1;

        if (highValue_ <= lowValue_)
            setLowValue(lowValue_ - 1);

        update();
        emit highValueChanged(highValue_);
    }
}

void QRangeSlider::setRange(unsigned int minimum, unsigned int maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
}

void QRangeSlider::mousePressEvent(QMouseEvent* e)
{
    // Check if event was on slider.
    if (e->position().y() >= (height() - SLIDER_HEIGHT - HANDLE_SIZE) / 2 &&
        e->position().y() <= (height() - SLIDER_HEIGHT + HANDLE_SIZE) / 2)
    {
        double mouseX = e->position().x() < 0 ? 0 : e->position().x();
        unsigned int mouseValue = (mouseX / width()) * (maximum_ - minimum_) + minimum_;
        lastMouseValue_ = mouseValue;

        if (getLowHandleRect().contains(e->pos()))
            handleClicked_ = 0;
        else if (getHighHandleRect().contains(e->pos()))
            handleClicked_ = 1;
        else if (getRangeRect().contains(e->pos()))
            handleClicked_ = 2;
    }
}

void QRangeSlider::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e);

    lastMouseValue_ = -1;
    handleClicked_ = -1;
}

void QRangeSlider::mouseMoveEvent(QMouseEvent* e)
{
    if (lastMouseValue_ != -1 && handleClicked_ != -1)
    {
        double mouseX = e->position().x() < 0 ? 0 : e->position().x();
        unsigned int mouseValue = (mouseX / width()) * (maximum_ - minimum_) + minimum_;

        if (handleClicked_ == 0)
        {
            setLowValue(mouseValue);
        }
        else if (handleClicked_ == 1)
        {
            setHighValue(mouseValue);
        }
        else if (barDraggable_ && handleClicked_ == 2)
        {
            int deltaValue = (mouseValue - lastMouseValue_);
            if (deltaValue < 0)
            {
                // Check for underflow
                setLowValue(lowValue_ + deltaValue > lowValue_ ? minimum_ : lowValue_ + deltaValue);
                setHighValue(highValue_ + deltaValue);
            }
            else if (deltaValue > 0)
            {
                setLowValue(lowValue_ + deltaValue);
                // Check for overflow
                setHighValue(highValue_ + deltaValue < highValue_ ? maximum_ : highValue_ + deltaValue);
            }
        }

        lastMouseValue_ = mouseValue;
    }
}

void QRangeSlider::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    // Draw background
    painter.setPen(QPen(Qt::GlobalColor::darkGray, 0.8));
    painter.setBrush(QBrush(QColor(Qt::GlobalColor::lightGray)));
    painter.drawRoundedRect(getBackgroundRect(), 2, 2);

    // Draw range
    painter.setBrush(QBrush(QColor(0x1E, 0x90, 0xFF)));
    painter.drawRect(getRangeRect());

    // Draw lower handle
    painter.setBrush(QBrush(QColor(Qt::GlobalColor::white)));
    painter.drawRoundedRect(getLowHandleRect(), 2, 2);

    // Draw higher handle
    painter.drawRoundedRect(getHighHandleRect(), 2, 2);

    painter.end();
}

QRectF QRangeSlider::getBackgroundRect() const
{
    return QRectF(
        PADDING,
        (height() - SLIDER_HEIGHT) / 2,
        width() - 2 * PADDING,
        SLIDER_HEIGHT);
}

QRectF QRangeSlider::getRangeRect() const
{
    return QRectF(
        PADDING + ((width() - 2 * PADDING) * (lowValue_ - minimum_) / (maximum_ - minimum_)),
        (height() - SLIDER_HEIGHT) / 2,
        (width() - 2 * PADDING) * (highValue_ - lowValue_) / (maximum_ - minimum_),
        SLIDER_HEIGHT);
}

QRectF QRangeSlider::getLowHandleRect() const
{
    return QRectF(
        PADDING + ((width() - 2 * PADDING) * (lowValue_ - minimum_) / (maximum_ - minimum_)),
        (height() - HANDLE_SIZE) / 2,
        HANDLE_SIZE,
        HANDLE_SIZE);
}

QRectF QRangeSlider::getHighHandleRect() const
{
    return QRectF(
        PADDING + ((width() - 2 * PADDING) * (highValue_ - minimum_) / (maximum_ - minimum_)) - HANDLE_SIZE,
        (height() - HANDLE_SIZE) / 2,
        HANDLE_SIZE,
        HANDLE_SIZE);
}
