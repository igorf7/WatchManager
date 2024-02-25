#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

class ComPort : public QObject
{
    Q_OBJECT
public:
    explicit ComPort(QObject *parent = nullptr);
    ~ComPort();
    QSerialPort serialPort;

signals:
    void portError(const QString &msg);
    void quitComPort();
    void portOpened(const QString &port);
    void portClosed();
    void parsingPacket(const QByteArray &packet);

public slots:
    void onPortStart();
    void onDisconnectClicked();
    void onConnectClicked(const QString &port, int br);
    void onSendDataPacket(const QByteArray &packet);

private slots:
    void onReadyRead();
};

#endif // COMPORT_H
//eof
