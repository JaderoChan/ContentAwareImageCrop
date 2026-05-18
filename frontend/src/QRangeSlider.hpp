#ifndef Q_RANGE_SLIDER_HPP
#define Q_RANGE_SLIDER_HPP

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QRect>

class QRangeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit QRangeSlider(QWidget* parent = nullptr);

    unsigned int minimum() const;
    unsigned int maximum() const;
    unsigned int lowValue() const;
    unsigned int highValue() const;

    unsigned int step() const;
    void setStep(unsigned int step);

    void setOrientation(Qt::Orientation orientation);
    void setBarDraggable(bool enable);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void setMinimum(unsigned int minimum);
    void setMaximum(unsigned int maximum);
    void setLowValue(unsigned int lowValue);
    void setHighValue(unsigned int highValue);
    void setRange(unsigned int minimum, unsigned int maximum);

signals:
    void minimumChanged(unsigned int minimum);
    void maximumChanged(unsigned int maximum);
    void lowValueChanged(unsigned int lowValue);
    void highValueChanged(unsigned int highValue);
    void rangeChanged(unsigned int minimum, unsigned int maximum);

private:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

    QRectF getBackgroundRect() const;
    QRectF getRangeRect() const;
    QRectF getLowHandleRect() const;
    QRectF getHighHandleRect() const;

    // Minimum range value
    unsigned int minimum_ = 0;
    // Maximum range value
    unsigned int maximum_ = 100;

    // value of low handle
    unsigned int lowValue_;
    // Value of high handle
    unsigned int highValue_;

    // Step value
    unsigned int step_ = 1;

    // Value of the last mouse click
    int lastMouseValue_ = -1;

    // Whether the slider bar can be drag
    bool barDraggable_ = false;

    // Handle of the last mouse click. 0 is low handle, 1 is high handle and 2 is slider bar
    int handleClicked_ = -1;

    // Painter constants
    // Slider height in pixels
    static constexpr unsigned int SLIDER_HEIGHT = 5;
    // Slider handle size in pixels
    static constexpr unsigned int HANDLE_SIZE = 13;
    // Left and Right padding in pixels
    static constexpr unsigned int PADDING = 1;
};

#endif // Q_RANGE_SLIDER_HPP
