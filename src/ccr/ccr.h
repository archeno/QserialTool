#ifndef CCR_H
#define CCR_H

#include <QMainWindow>
#include "settingsdialog.h"
#include <QLabel>
#include "datareveivewidget.h"
namespace Ui {
class CCR;
}

class CCR : public QMainWindow
{
    Q_OBJECT

public:
    explicit CCR(QWidget *parent = nullptr);
    ~CCR();
    static  CCR &instance(void);
    QMainWindow * m_homepage;
Q_SIGNALS:
    void backToHomepage(void);

private:
    Ui::CCR *ui;
    SettingsDialog *m_serialSettings;
    DataReveiveWidget *m_dataRcvWidget;
    QLabel m_labSerialStatus;
    void initActionsConnections(void);
    void initUi(void);
     void paintEvent(QPaintEvent *)Q_DECL_OVERRIDE;
};

#endif // CCR_H
