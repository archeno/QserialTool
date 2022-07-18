#include "datareveivewidget.h"
#include "ui_datareveivewidget.h"
#include <QDateTime>

DataReveiveWidget::DataReveiveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataReveiveWidget),
    m_receivedBytes(0),
    m_dataAreaDispalyTime(false)
{
    ui->setupUi(this);
    initUi();
    initActions();


}

DataReveiveWidget::~DataReveiveWidget()
{
    delete ui;
}

void DataReveiveWidget::initUi()
{
    this->setWindowTitle(tr("数据报文"));
    this->setWindowIcon(QIcon(":/images/dataReceive.png"));
}

void DataReveiveWidget::initActions()
{
    connect(&Serial::instance(), &Serial::dataReceived, [=](QByteArray data)
    {
        if(m_dataAreaDispalyTime)
        {
            QString time = QTime::currentTime().toString("hh:mm:ss");
            ui->textEdit->append(time);
        }
        ui->textEdit->append(data);
        m_receivedBytes +=data.size();
        ui->lineEditRcvCounts->setText(QString::number(m_receivedBytes));
    });

}

void DataReveiveWidget::on_checkBoxShowTime_clicked(bool checked)
{
     m_dataAreaDispalyTime = checked?true:false;
}

void DataReveiveWidget::on_btnClearArea_clicked()
{
    ui->textEdit->clear();
}

void DataReveiveWidget::on_btnRcvClear_clicked()
{
    m_receivedBytes =0;
    ui->lineEditRcvCounts->setText(QString::number(m_receivedBytes));
}
