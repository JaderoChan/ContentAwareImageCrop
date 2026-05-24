#include "settings_dialog.h"

#include "qvalidator.h"

SettingsDialog::SettingsDialog(const Settings& settings, QWidget* parent)
    : TrDialog(parent)
{
    ui.setupUi(this);

    // Set the language combo box.
    for (int lang = LANG_FIRST, i = 0; lang <= LANG_LAST; ++lang)
    {
        ui.languageComboBox->addItem("", lang);
        if (static_cast<Language>(lang) == settings.language)
            ui.languageComboBox->setCurrentIndex(i);
        i++;
    }

    // Set the update level combo box.
    size_t updateTs[] = {0, 1, 3, 7};
    for (size_t i = 0, count = sizeof(updateTs) / sizeof(size_t); i < count; ++i)
    {
        ui.updateLevelComboBox->addItem("", static_cast<qulonglong>(updateTs[i]));
        if (updateTs[i] == settings.cropUpdateT)
            ui.updateLevelComboBox->setCurrentIndex(i);
    }

    ui.highlighLineCheckBox->setChecked(settings.isHighlightLine);
    ui.highlighLineColorRSpinBox->setValue(settings.highlightColor.red());
    ui.highlighLineColorGSpinBox->setValue(settings.highlightColor.green());
    ui.highlighLineColorBSpinBox->setValue(settings.highlightColor.blue());
    ui.highlighLineColorRSpinBox->setRange(0, 255);
    ui.highlighLineColorGSpinBox->setRange(0, 255);
    ui.highlighLineColorBSpinBox->setRange(0, 255);

    ui.limitImageSizeCheckBox->setChecked(settings.isLimitImageSize);
    ui.imageLimitedWidthLineEdit->setText(QString::number(settings.imageLimitedSize.width()));
    ui.imageLimitedHeightLineEdit->setText(QString::number(settings.imageLimitedSize.height()));
    ui.imageLimitedWidthLineEdit->setValidator(new QIntValidator(128, 2048, this));
    ui.imageLimitedHeightLineEdit->setValidator(new QIntValidator(128, 2048, this));

    ui.antialiasingLineCheckBox->setChecked(settings.isAntialiasingLine);

    ui.maxRecordStepsSpinBox->setValue(settings.maxRecordSteps);

    updateText();
}

Settings SettingsDialog::execForSettings()
{
    exec();

    Settings settings;

    settings.language = static_cast<Language>(ui.languageComboBox->currentData().toInt());
    settings.cropUpdateT = ui.updateLevelComboBox->currentData().toULongLong();

    settings.isHighlightLine = ui.highlighLineCheckBox->isChecked();
    int r = ui.highlighLineColorRSpinBox->value();
    int g = ui.highlighLineColorGSpinBox->value();
    int b = ui.highlighLineColorBSpinBox->value();
    settings.highlightColor = QColor(r, g, b);

    settings.isLimitImageSize = ui.limitImageSizeCheckBox->isChecked();
    int width  = ui.imageLimitedWidthLineEdit->text().toInt();
    int height = ui.imageLimitedHeightLineEdit->text().toInt();
    settings.imageLimitedSize = QSize(width, height);

    settings.isAntialiasingLine = ui.antialiasingLineCheckBox->isChecked();

    settings.maxRecordSteps = ui.maxRecordStepsSpinBox->value();

    return settings;
}

void SettingsDialog::updateText()
{
    setWindowTitle(EASYTR("Settings"));

    ui.languageTextLabel->setText(EASYTR("Language"));
    for (int i = 0; i < ui.languageComboBox->count(); ++i)
    {
        Language lang = static_cast<Language>(ui.languageComboBox->itemData(i).toInt());
        ui.languageComboBox->setItemText(i, EASYTR(getLanguageStringId(lang).toStdString()));
    }

    ui.updateLevelTextLabel->setText(EASYTR("Update Level"));
    ui.updateLevelTextLabel->setToolTip(EASYTR("UpdateLevel.Tooltip"));
    for (size_t i = 0; i < ui.updateLevelComboBox->count(); ++i)
    {
        size_t updateT = ui.updateLevelComboBox->itemData(i).toULongLong();
        QString text = (
            updateT == 0 ?
            EASYTR("Immediately (Update per 1 times)") :
            QString(EASYTR("Update per %1 times")).arg(updateT + 1));
        ui.updateLevelComboBox->setItemText(i, text);
    }

    ui.highlighLineCheckBox->setText(EASYTR("Highlight Crop Line"));
    ui.highlighLineCheckBox->setToolTip(EASYTR("HighlightLine.Tooltip"));

    ui.limitImageSizeCheckBox->setText(EASYTR("Limit Image Size"));
    ui.limitImageSizeCheckBox->setToolTip(EASYTR("LimitImageSize.ToolTip"));
    ui.widthTextLabel->setText(EASYTR("Width"));
    ui.heightTextLabel->setText(EASYTR("Height"));

    ui.antialiasingLineCheckBox->setText(EASYTR("Antialiasing Crop Line"));
    ui.antialiasingLineCheckBox->setToolTip(EASYTR("AntialiasingLine.ToolTip"));

    ui.maxRecordStepsTextLabel->setText(EASYTR("Max Record Steps"));
    ui.maxRecordStepsTextLabel->setToolTip(EASYTR("MaxRecordSteps.Tooltip"));
}
