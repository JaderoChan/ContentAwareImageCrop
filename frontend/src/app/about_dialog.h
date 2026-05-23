#pragma once

#include <trwidgets/trdialog.h>
#include "ui_about_dialog.h"

class AboutDialog : public TrDialog
{
public:
    explicit AboutDialog(QWidget* parent = nullptr);

protected:
    void updateText() override;

private:
    Ui::AboutDialog ui;
};
