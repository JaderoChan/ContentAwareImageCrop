#pragma once

#include <qvalidator.h>

/**
 * @brief 严格的整数验证器
 * @note 如果为输入框设置此验证器：
 *
 * - 当用户输入非法数据时会拒绝输入。
 *
 * - 当用户清空输入框时，使用回退值 fallback 代替空框。
 *
 * - 可通过 setDynamic* 方法设置动态边界。
 */
class StrictIntValidator : public QIntValidator
{
public:
    StrictIntValidator(int bottom, int top, const int* fallback, QObject* parent = nullptr);

    void setDynamicBottom(const int* ptr);
    void setDynamicTop(const int* ptr);

    State validate(QString& input, int& pos) const override;
    void fixup(QString& input) const override;

private:
    const int* fallback_      = nullptr;
    const int* dynamicTop_    = nullptr;
    const int* dynamicBottom_ = nullptr;
};
