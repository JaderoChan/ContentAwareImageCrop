#pragma once

#include <trwidgets/trdialog.h>
#include "ui_settings_dialog.h"

class SettingsDialog : public TrDialog
{
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

protected:
    void updateText() override;

private:
    Ui::SettingsDialog ui;
};
