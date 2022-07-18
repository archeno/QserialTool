#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QPainter>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");
SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("串口参数设置"));


    //绑定事件
    connect(&Serial::instance(), &Serial::availablePortsChanged, [=]()
    {
        qDebug()<<"receive signal availablePortsChanged";
       ui->serialPortInfoListBox->clear();
       ui->serialPortInfoListBox->addItems(Serial::instance().portList().keys());
    });
    //显示串口信息
    connect(ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo);
    //应用配置
    connect(ui->btnApply, &QPushButton::clicked,
            this, &SettingsDialog::apply);

    fillPortsInfo();
    fillPortsParameters();
    //更新设置
    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
    qDebug()<<"SettingsDialog destroyed1";
}
SettingsDialog::Settings SettingsDialog::settings() const
{
    return m_currentSettings;
}
void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->clear();
    ui->baudRateBox->addItems(Serial::instance().baudRateList());
    ui->baudRateBox->setCurrentIndex(1);

    ui->dataBitsBox->addItems(Serial::instance().dataBitsList());
    ui->dataBitsBox->setCurrentIndex(3);
    ui->stopBitsBox->addItems(Serial::instance().stopBitsList());
    ui->parityBox->addItems(Serial::instance().parityList());
    ui->flowControlBox->addItems(Serial::instance().flowControlList());
}
void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    ui->serialPortInfoListBox->addItems(Serial::instance().portList().keys());
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;
    QSerialPortInfo list = Serial::instance().portList().values().at(idx);
    ui->descriptionLabel->setText(tr("Description: %1").arg(list.description()));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.manufacturer()));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.serialNumber()));
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::showDialog()
{
    this->show();
}

void SettingsDialog::updateSettings()
{
    m_currentSettings.name = ui->serialPortInfoListBox->currentText();

    Serial::instance().setPortIndex(ui->serialPortInfoListBox->currentIndex());
    Serial::instance().setBaudRate(ui->baudRateBox->currentText().toInt());
    Serial::instance().setDataBits(ui->dataBitsBox->currentIndex());
    Serial::instance().setStopBits(ui->stopBitsBox->currentIndex());
    Serial::instance().setParity(ui->parityBox->currentIndex());
    Serial::instance().setFlowControl(ui->flowControlBox->currentIndex());

//    qDebug()<<Serial::instance().baudRate()<<Serial::instance().portIndex()
//           <<Serial::instance().dataBits()<<Serial::instance().stopBits()
//           <<Serial::instance().parity()<<Serial::instance().flowControl();
}

void SettingsDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap(":/images/background.png");
    painter.drawPixmap(rect(), pixmap, QRect());
}
