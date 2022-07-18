#include "ccr.h"
#include "ui_ccr.h"
#include <QPainter>
#include <QDebug>
#include <QBrush>


CCR::CCR(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CCR),
    m_serialSettings(new SettingsDialog(this)),
    m_dataRcvWidget(new DataReveiveWidget)
{
    ui->setupUi(this);
    resize(900, 600);
    initActionsConnections();
    initUi();
}

CCR::~CCR()
{
    delete ui;
    qDebug()<<"CCR destroied!";
}

CCR &CCR::instance()
{
    static CCR ccr;
    return ccr;
}

/**
 * @brief CCR::initActionsConnections
 * 连接信号和槽
 */
void CCR::initActionsConnections()
{
    connect(ui->actionexit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionSerialConfig, &QAction::triggered, m_serialSettings, &SettingsDialog::show);
    connect(ui->actionconnect, &QAction::triggered, [=]()
    {
        if(Serial::instance().connectDevice())
        {

           m_labSerialStatus.setText(QString(tr("%1 %2 %3 %4 %5 %6")).arg(Serial::instance().portName())
                                   .arg(Serial::instance().baudRate())
                                   .arg(Serial::instance().dataBits())
                                   .arg(Serial::instance().parityList().at(Serial::instance().parityIndex()))
                                   .arg(Serial::instance().stopBits())
                                   .arg(Serial::instance().flowControlList().at(Serial::instance().flowControlIndex())));

            ui->statusbar->addWidget(&m_labSerialStatus);
            ui->actionSerialConfig->setEnabled(false);
        }
        ui->actionconnect->setEnabled(false);
        ui->actiondisconnect->setEnabled(true);
    });
    connect(ui->actiondisconnect, &QAction::triggered,[=]()
    {
        Serial::instance().disconnectDevice();
        ui->actionconnect->setEnabled(true);
        ui->actiondisconnect->setEnabled(false);
        ui->actionSerialConfig->setEnabled(true);
        m_labSerialStatus.setText(tr("disconnected"));
    });
    connect(ui->actionhomepage, &QAction::triggered, this, &CCR::backToHomepage);
    connect(ui->actiondataDisplay, &QAction::triggered, [=](bool checked)
    {
        checked?m_dataRcvWidget->show():m_dataRcvWidget->hide();
    });
}

void CCR::initUi()
{
    ui->labCurrent->setProperty("labtype", "displayvalue");
    ui->labCurrent->setNum(2.36f);
    ui->labVoltage->setProperty("labtype", "displayvalue");
    ui->labIdensity->setProperty("labtype", "displayvalue");
    ui->labRL->setProperty("labtype", "displaystatus");
    ui->labRelayStatus->setProperty("labtype", "displaystatus");
    ui->labLoopStatus->setProperty("labtype", "displaystatus");
    ui->labDeviceStatsu->setProperty("labtype", "displaystatus");
    ui->labA->setProperty("labtype", "displayvalue");
    ui->labV->setProperty("labtype", "displayvalue");

    ui->actionconnect->setEnabled(true);
    ui->actiondisconnect->setEnabled(false);

    this->setWindowTitle(tr("恒流控制器"));
    this->setWindowIcon(QIcon(":/images/tbtn1.png"));

}

/**
 * @brief CCR::paintEvent
 * 绘制背景图片
 */
void CCR::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap(":/images/background.png");
    painter.drawPixmap(rect(), pixmap, QRect());
}
