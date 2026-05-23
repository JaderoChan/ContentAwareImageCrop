#pragma once

#include <trwidgets/trdialog.h>
#include "settings.h"
#include "ui_settings_dialog.h"

class SettingsDialog : public TrDialog
{
public:
    explicit SettingsDialog(const Settings& settings, QWidget* parent = nullptr);

    Settings execForSettings();

protected:
    void updateText() override;

private:
    Ui::SettingsDialog ui;
};
