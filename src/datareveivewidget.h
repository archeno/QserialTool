#ifndef DATAREVEIVEWIDGET_H
#define DATAREVEIVEWIDGET_H

#include <QWidget>
#include "serial.h"
namespace Ui {
class DataReveiveWidget;
}

class DataReveiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataReveiveWidget(QWidget *parent = nullptr);
    ~DataReveiveWidget();
private slots:
    void on_checkBoxShowTime_clicked(bool checked);

    void on_btnClearArea_clicked();

    void on_btnRcvClear_clicked();

private:
    void initUi(void);
    void initActions(void);
private:
    Ui::DataReveiveWidget *ui;
    quint64 m_receivedBytes;
    bool m_dataAreaDispalyTime;
};

#endif // DATAREVEIVEWIDGET_H
