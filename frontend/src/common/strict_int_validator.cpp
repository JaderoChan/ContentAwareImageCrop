#include "strict_int_validator.h"

StrictIntValidator::StrictIntValidator(int bottom, int top, const int* fallback, QObject* parent)
    : QIntValidator(bottom, top, parent), fallback_(fallback) {}

void StrictIntValidator::setDynamicBottom(const int* ptr)
{
    dynamicBottom_ = ptr;
}

void StrictIntValidator::setDynamicTop(const int* ptr)
{
    dynamicTop_ = ptr;
}

QValidator::State StrictIntValidator::validate(QString& input, int& pos) const
{
    State state = QIntValidator::validate(input, pos);

    if (state == Intermediate)
    {
        bool ok;
        const int val = input.toInt(&ok);
        if (ok && (val < bottom() || val > top()))
            return Invalid;
    }
    else if (state == Acceptable)
    {
        bool ok;
        const int val = input.toInt(&ok);
        if (ok)
        {
            if (dynamicBottom_ && val < static_cast<int>(*dynamicBottom_))
                return Intermediate;
            if (dynamicTop_   && val > static_cast<int>(*dynamicTop_))
                return Intermediate;
        }
    }

    return state;
}

void StrictIntValidator::fixup(QString& input) const
{
    if (!fallback_)
        return;

    int val = static_cast<int>(*fallback_);
    if (dynamicBottom_) val = qMax(val, static_cast<int>(*dynamicBottom_));
    if (dynamicTop_)    val = qMin(val, static_cast<int>(*dynamicTop_));
    input = QString::number(val);
}
