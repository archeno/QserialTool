#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QMessageBox>
namespace Misc {
class Utilities : public QObject
{
    Q_OBJECT
public:
   // explicit Utilities(QObject *parent = nullptr);
    static int showMessageBox(const QString &text,
                              const QString &informativeText = "",
                              const QString &windowTitle = "",
                              const QMessageBox::StandardButtons &bt = QMessageBox::Ok);
signals:
//       static void aboutQt();
};
}
#endif // UTILITIES_H
