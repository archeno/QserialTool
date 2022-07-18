#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QSettings>
#include <QMap>

class Serial : public QObject
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    Serial(Serial &&) = delete;
    Serial(const Serial &) = delete;
    Serial &operator=(Serial &&) = delete;
    Serial &operator=(const Serial &) = delete;
    ~Serial();

Q_SIGNALS:
    void portChanged();
    void parityChanged();
    void baudRateChanged();
    void dataBitsChanged();
    void stopBitsChanged();
    void portIndexChanged();
    void flowControlChanged();
    void baudRateListChanged();
    void autoReconnectChanged();
    void baudRateIndexChanged();
    void availablePortsChanged();
    void connectionError(const QString &name);
    void dataReceived(const QByteArray &data);

public:
    static Serial &instance();
    void close();
    bool isOpen() const;
    bool isWritable()const;
    bool isReadable() const;
    bool configurationOk() const;
    quint64 write(const QByteArray &data) ;
    bool open(const QIODevice::OpenMode mode) ;
    void disconnectDevice();
    bool connectDevice();
    QString portName() const;
    QSerialPort *port() const;
    bool autoReconnect() const;

    quint8 portIndex() const;
    quint8 parityIndex() const;
    quint8 displayMode() const;
    quint8 dataBitsIndex() const;
    quint8 stopBitsIndex() const;
    quint8 flowControlIndex() const;    
    QMap<QString, QSerialPortInfo> portList() const;
    QStringList parityList() const;
    QStringList baudRateList() const;
    QStringList dataBitsList() const;
    QStringList stopBitsList() const;
    QStringList flowControlList() const;

    qint32 baudRate() const;
    QSerialPort::Parity parity() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::StopBits stopBits() const;
    QSerialPort::FlowControl flowControl() const;

public Q_SLOTS:

    void setBaudRate(const qint32 rate);
    void setParity(const quint8 parityIndex);
    void setPortIndex(const quint8 portIndex);
    void appendBaudRate(const QString &baudRate);
    void setDataBits(const quint8 dataBitsIndex);
    void setStopBits(const quint8 stopBitsIndex);
    void setAutoReconnect(const bool autoreconnect);
    void setFlowControl(const quint8 flowControlIndex);
private Q_SLOTS:
    void onReadyRead();
    void readSettings();
    void writeSettings();
    void refreshSerialDevices();
    void handleError(QSerialPort::SerialPortError error);
private:
    QSerialPort *m_port;
    QTimer m_timerFreshPorts;
    bool m_autoReconnect;
    int m_lastSerialDeviceIndex;
    QSettings m_settings;
    qint32 m_baudRate;
    QSerialPort::Parity m_parity;
    QSerialPort::DataBits m_dataBits;
    QSerialPort::StopBits m_stopBits;
    QSerialPort::FlowControl m_flowControl;

    quint8 m_portIndex;
    quint8 m_parityIndex;
    quint8 m_dataBitsIndex;
    quint8 m_stopBitsIndex;
    quint8 m_flowControlIndex;

    QMap<QString , QSerialPortInfo> m_portList;
   // QStringList m_portList;
    QStringList m_baudRateList;
    QVector<QSerialPortInfo> validPorts() const;
signals:

};

#endif // SERIAL_H
