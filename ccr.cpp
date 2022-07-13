#include "ccr.h"
#include "ui_ccr.h"
#include <QPainter>
CCR::CCR(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CCR),
    m_serialSettings(new SettingsDialog)
{
    ui->setupUi(this);
    setFixedSize(900, 600);
    initActionsConnections();
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


}

CCR::~CCR()
{
    delete ui;
}

/**
 * @brief CCR::initActionsConnections
 * 连接信号和槽
 */
void CCR::initActionsConnections()
{
    connect(ui->actionexit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionSerialConfig, &QAction::triggered, m_serialSettings, &SettingsDialog::show);
   // connect(ui->actionhomepage, &QAction::triggered, )
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
