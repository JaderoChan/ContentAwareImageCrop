#include "about_dialog.h"

#include <config.h>

#define URL_HTML \
"<html><head/><body><p>%1: <a href=\"%2\"><span style=\" \
text-decoration: underline; color:#0066ff;\">%3</span></a></p></body></html>"

AboutDialog::AboutDialog(QWidget* parent)
    : TrDialog(parent)
{
    ui.setupUi(this);
    updateText();
}

void AboutDialog::updateText()
{
    setWindowTitle(EASYTR("About"));

    ui.titleTextLabel->setText(EASYTR(APP_TITLE));
    ui.versionTextLabel->setText(APP_VERSION);
    ui.authorTextLabel->setText(QString("%1: %2 (%3)").arg(EASYTR("Author")).arg(APP_AUTHOR).arg(APP_CONTACT_EMAIL));
    ui.githubUrlTextLabel->setText(QString(URL_HTML).arg(EASYTR("Github URL")).arg(APP_URL).arg(APP_TITLE));
    ui.refLinkingTextLabel->setText(QString(URL_HTML).arg(EASYTR("Reference Linking")).
        arg("https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html").
        arg("Real-world dynamic programming: seam carving"));
}
