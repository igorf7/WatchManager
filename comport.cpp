#include "comport.h"

#include <QDebug>

/**
 * @brief ComPort constructor
 * @param parent
 */
ComPort::ComPort(QObject *parent) :
    QObject(parent)
{
    qDebug() << "Hello from" << this;
}

/**
 * @brief ComPort destructor
 */
ComPort::~ComPort()
{
    onDisconnectClicked();
    emit quitComPort();
    qDebug() << "By-by from" << this;
}

/**
 * @brief Run at class start.
 */
void ComPort:: onPortStart()
{
    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

/**
 * @brief Opens and configures the port
 * @param port - port name
 */
void ComPort:: onConnectClicked(const QString &port)
{
    serialPort.setPortName(port);
    if (!serialPort.isOpen()) {
        if (!serialPort.open(QIODevice::ReadWrite)) {
            if (!QSerialPortInfo::availablePorts().isEmpty())
                emit portOpenError(tr("Порт не доступен. Возможно он занят другим приложением."));
        }
        else {
            serialPort.setBaudRate(QSerialPort::Baud115200);
            serialPort.setDataBits(QSerialPort::Data8);
            serialPort.setStopBits(QSerialPort::OneStop);
            serialPort.setParity(QSerialPort::NoParity);
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
            qDebug() << "Port opened" << serialPort.portName();
            emit portOpened(port);
        }
    }
    else {
        emit portOpenError(tr("Этот порт уже открыт."));
    }
}

/**
 * @brief Closes the port
 */
void ComPort::onDisconnectClicked()
{
    if (serialPort.isOpen()) {
        serialPort.close();
        qDebug() << "Port closed" << serialPort.portName();
        emit portClosed();
    }
}

/**
 * @brief Reads the port when the data is ready
 */
void ComPort::onReadyRead()
{
    static QByteArray rxData;

    while (1) {
        rxData.append(serialPort.readAll());
        if (!serialPort.waitForReadyRead(10)) {
            break;
        }
    }

    if (rxData.size() > 0) {
        emit parsingPacket(rxData);
        rxData.clear();
    }
}

/**
 * @brief Sends data via port
 * @param packet - data packet
 */
void ComPort::onSendDataPacket(const QByteArray &packet)
{   
    if (!serialPort.isOpen()) {
        emit portSendError(tr("Нет соединения"));
    }
    else {
        serialPort.write(packet);
    }
}
//eof
