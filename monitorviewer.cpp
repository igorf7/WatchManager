#include "monitorviewer.h"

#include <QApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

//quint8 MonitorViewer::sensorId = 2;

/**
 * @brief Class constructor
 * @param parent
 */
MonitorViewer::MonitorViewer(QObject *parent) : QObject(parent)
{
    monitorWidget = new QWidget; // Главный контейнер вкладки "Мониторинг датчиков"

    QFont startBtnFont;
    startBtnFont.setBold(true);

    QGridLayout *prmLayout = new QGridLayout;

    QLabel *tempLabel = new QLabel("<H2>Температура, °C</H2>");
    QLabel *pressLabel = new QLabel("<H2>Давление,</H2>"
                                    "<H2>мм. рт. ст.</H2>");
    QLabel *hummLabel = new QLabel("<H2>Влажность, %</H2>");
    QLabel *rssiLabel = new QLabel("<H2>RSSI, dBm</H2>");

    tempLabel->setAlignment(Qt::AlignCenter);
    tempLabel->setStyleSheet("color: rgb(136, 136, 136);"
                             "background-color: rgb(230, 235, 220)");

    pressLabel->setAlignment(Qt::AlignCenter);
    pressLabel->setStyleSheet("color: rgb(136, 136, 136);"
                             "background-color: rgb(230, 235, 220)");

    hummLabel->setAlignment(Qt::AlignCenter);
    hummLabel->setStyleSheet("color: rgb(136, 136, 136);"
                             "background-color: rgb(230, 235, 220)");

    rssiLabel->setAlignment(Qt::AlignCenter);
    rssiLabel->setStyleSheet("color: rgb(136, 136, 136);"
                             "background-color: rgb(230, 235, 220)");

    for (int i = 0; i < sensNum; i++) {
        tempLcd[i] = new QLCDNumber;
        tempLcd[i]->setSmallDecimalPoint(true);
        tempLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        tempLcd[i]->setDigitCount(4);
        pressLcd[i] = new QLCDNumber;
        pressLcd[i]->setSmallDecimalPoint(true);
        pressLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        pressLcd[i]->setDigitCount(4);
        hummLcd[i] = new QLCDNumber;
        hummLcd[i]->setSmallDecimalPoint(true);
        hummLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        hummLcd[i]->setDigitCount(4);
        rssiLcd[i] = new QLCDNumber;
        rssiLcd[i]->setSmallDecimalPoint(true);
        rssiLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        rssiLcd[i]->setDigitCount(4);
    }

    prmLayout->addWidget(tempLabel, 0, 0);
    prmLayout->addWidget(tempLcd[0], 0, 1);
    prmLayout->addWidget(tempLcd[1], 0, 2);
    prmLayout->addWidget(pressLabel, 1, 0);
    prmLayout->addWidget(pressLcd[0], 1, 1);
    prmLayout->addWidget(pressLcd[1], 1, 2);
    prmLayout->addWidget(hummLabel, 2, 0);
    prmLayout->addWidget(hummLcd[0], 2, 1);
    prmLayout->addWidget(hummLcd[1], 2, 2);
    prmLayout->addWidget(rssiLabel, 3, 0);
    prmLayout->addWidget(rssiLcd[0], 3, 1);
    prmLayout->addWidget(rssiLcd[1], 3, 2);

    /*  */
    monitorWidget->setLayout(prmLayout);

    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
MonitorViewer::~MonitorViewer()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief MonitorViewer::onShowParams
 * @param packet
 */
void MonitorViewer::onShowParams(const QByteArray &packet)
{
    int //temperature_int,
        pressure_int,
        humidity_int;

    RF_Packet_t *rx_packet = (RF_Packet_t*)packet.data();
    Parameter_t prm;

    memcpy(&prm, rx_packet->data, sizeof(Parameter_t));

    quint16 id = (rx_packet->header.snd_addr & 0x00FF) - 1;

    //temperature_int = qRound(prm.temperature);
    pressure_int = qRound(prm.pressure);
    humidity_int = qRound(prm.humidity);

    tempLcd[id]->display(prm.temperature);
    pressLcd[id]->display(pressure_int);
    hummLcd[id]->display(humidity_int);
    //hummLcd[id]->display(prm.humidity);
    rssiLcd[id]->display(prm.rssi);

    if (MainWindow::getBeeperStatus()) {
        QApplication::beep();
    }
}

/**
 * @brief MonitorViewer::onConfirmCmd
 * @param cmd
 */
void MonitorViewer::onConfirmCmd(quint8 cmd)
{
    switch (cmd)
    {
    case SET_RECEIVE:
        break;
    case SET_SLEEP:
        for (int i = 0; i <sensNum; i++) {
            tempLcd[i]->display(0);
            pressLcd[i]->display(0);
            hummLcd[i]->display(0);
            rssiLcd[i]->display(0);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief MonitorViewer::onCurrentChanged
 * @param index
 */
void MonitorViewer::onCurrentChanged(int index)
{
    QByteArray payload;

    if (index != MONITOR) {
        payload.insert(sizeof(RF_Header_t), SET_SLEEP);
    }
    else {
        payload.insert(sizeof(RF_Header_t), SET_RECEIVE);
    }
    emit createAndSend(CONNECT, payload);
}

void MonitorViewer::sendResetCmd()
{
    QByteArray payload;

    payload.insert(sizeof(RF_Header_t), RESET_CMD);
    emit createAndSend(CONNECT, payload);
}
