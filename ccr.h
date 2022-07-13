#ifndef CCR_H
#define CCR_H

#include <QMainWindow>
#include "settingsdialog.h"
namespace Ui {
class CCR;
}

class CCR : public QMainWindow
{
    Q_OBJECT

public:
    explicit CCR(QWidget *parent = nullptr);
    ~CCR();
 //   QMainWindow * m_mainPage;
private:
    Ui::CCR *ui;
    SettingsDialog *m_serialSettings;
    void initActionsConnections(void);
     void paintEvent(QPaintEvent *)Q_DECL_OVERRIDE;
};

#endif // CCR_H
