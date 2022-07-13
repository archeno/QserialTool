#include "utilities.h"
#include <QAbstractButton>
/**
 * Shows a macOS-like message box with the given properties
 */
int Misc::Utilities::showMessageBox(const QString &text, const QString &informativeText,
                                    const QString &windowTitle,
                                    const QMessageBox::StandardButtons &bt)
{
    // Get app icon
    QPixmap icon;
   icon.load(":/images/icon-small@1x.png");

    // Create message box & set options
    QMessageBox box;
    box.setIconPixmap(icon);
    box.setStandardButtons(bt);
    box.setWindowTitle(windowTitle);
    box.setText("<h3>" + text + "</h3>");
    box.setInformativeText(informativeText);

    // Add button translations
    if (bt & QMessageBox::Ok)
        box.button(QMessageBox::Ok)->setText(tr("Ok"));
    if (bt & QMessageBox::Save)
        box.button(QMessageBox::Save)->setText(tr("Save"));
    if (bt & QMessageBox::SaveAll)
        box.button(QMessageBox::SaveAll)->setText(tr("Save all"));
    if (bt & QMessageBox::Open)
        box.button(QMessageBox::Open)->setText(tr("Open"));
    if (bt & QMessageBox::Yes)
        box.button(QMessageBox::Yes)->setText(tr("Yes"));
    if (bt & QMessageBox::YesToAll)
        box.button(QMessageBox::YesToAll)->setText(tr("Yes to all"));
    if (bt & QMessageBox::No)
        box.button(QMessageBox::No)->setText(tr("No"));
    if (bt & QMessageBox::NoToAll)
        box.button(QMessageBox::NoToAll)->setText(tr("No to all"));
    if (bt & QMessageBox::Abort)
        box.button(QMessageBox::Abort)->setText(tr("Abort"));
    if (bt & QMessageBox::Retry)
        box.button(QMessageBox::Retry)->setText(tr("Retry"));
    if (bt & QMessageBox::Ignore)
        box.button(QMessageBox::Ignore)->setText(tr("Ignore"));
    if (bt & QMessageBox::Close)
        box.button(QMessageBox::Close)->setText(tr("Close"));
    if (bt & QMessageBox::Cancel)
        box.button(QMessageBox::Cancel)->setText(tr("Cancel"));
    if (bt & QMessageBox::Discard)
        box.button(QMessageBox::Discard)->setText(tr("Discard"));
    if (bt & QMessageBox::Help)
        box.button(QMessageBox::Help)->setText(tr("Help"));
    if (bt & QMessageBox::Apply)
        box.button(QMessageBox::Apply)->setText(tr("Apply"));
    if (bt & QMessageBox::Reset)
        box.button(QMessageBox::Reset)->setText(tr("Reset"));
    if (bt & QMessageBox::RestoreDefaults)
        box.button(QMessageBox::RestoreDefaults)->setText(tr("Restore defaults"));

    // Show message box & return user decision to caller
    return box.exec();
}

///**
// * Displays the about Qt dialog
// */
//void Misc::Utilities::aboutQt()
//{
//    qApp->aboutQt();
//}
