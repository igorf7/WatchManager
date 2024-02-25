#include "monitorview.h"

#include <QApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

/**
 * @brief Class constructor
 * @param parent
 */
MonitorView::MonitorView(QObject *parent) : QObject(parent)
{
    monitorWidget = new QWidget; // Main container of the "Sensors" tab

    QStringList prmName;
    prmName << "<H2>Temperature,</H2>\n<H2>°C</H2>"
            << "<H2>Pressure,</H2>\n<H2>mmHg</H2>"
            << "<H2>Humidity,</H2>\n<H2>%</H2>"
            << "<H2>RSSI, dBm</H2>";

    for (int i = 0; i < sensNum; i++) {
        tempLcd[i] = new QLCDNumber;
        tempLcd[i]->setSmallDecimalPoint(true);
        tempLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        tempLcd[i]->setMinimumWidth(150);
        tempLcd[i]->setDigitCount(5);
        pressLcd[i] = new QLCDNumber;
        pressLcd[i]->setSmallDecimalPoint(true);
        pressLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        pressLcd[i]->setMinimumWidth(150);
        pressLcd[i]->setDigitCount(5);
        hummLcd[i] = new QLCDNumber;
        hummLcd[i]->setSmallDecimalPoint(true);
        hummLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        hummLcd[i]->setMinimumWidth(150);
        hummLcd[i]->setDigitCount(5);
        rssiLcd[i] = new QLCDNumber;
        rssiLcd[i]->setSmallDecimalPoint(true);
        rssiLcd[i]->setSegmentStyle(QLCDNumber::Flat);
        rssiLcd[i]->setMinimumWidth(150);
        rssiLcd[i]->setDigitCount(5);
    }

    QGridLayout *prmLayout = new QGridLayout;

    for (auto i = 0; i < 4; ++i) {
        QLabel *prmLabel = new QLabel(prmName.at(i));
        prmLabel->setAlignment(Qt::AlignCenter);
        prmLabel->setStyleSheet("color: rgb(136, 136, 136);"
                                 "background-color: rgb(230, 235, 220)");

        prmLayout->addWidget(prmLabel, i, 0);
    }

    prmLayout->addWidget(tempLcd[0], 0, 1);
    prmLayout->addWidget(tempLcd[1], 0, 2);

    prmLayout->addWidget(pressLcd[0], 1, 1);
    prmLayout->addWidget(pressLcd[1], 1, 2);

    prmLayout->addWidget(hummLcd[0], 2, 1);
    prmLayout->addWidget(hummLcd[1], 2, 2);

    prmLayout->addWidget(rssiLcd[0], 3, 1);
    prmLayout->addWidget(rssiLcd[1], 3, 2);

    /*  */
    monitorWidget->setLayout(prmLayout);

    csvDir = (QDir::currentPath() + "/CSV");

    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
MonitorView::~MonitorView()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief MonitorView::onShowParams
 * @param packet
 */
void MonitorView::onShowParams(const QByteArray &packet)
{
    TRFPacket *rx_packet = (TRFPacket*)packet.data();
    TParameter prm;

    memcpy(&prm, rx_packet->data, sizeof(TParameter));

    float t_normalized = (int)(prm.temperature * 10.0) / 10.0;
    float p_normalized = (int)(prm.pressure * 10.0) / 10.0;
    float h_normalized = (int)(prm.humidity * 10.0) / 10.0;

    quint16 n = (rx_packet->header.snd_addr & 0x00FF) - 1;

    // tempLcd[n]->display(qRound(prm.temperature));
    // pressLcd[n]->display(qRound(prm.pressure));
    // hummLcd[n]->display(qRound(prm.humidity));
    tempLcd[n]->display(t_normalized);
    pressLcd[n]->display(p_normalized);
    hummLcd[n]->display(h_normalized);
    rssiLcd[n]->display(prm.rssi);

    getFileName(n, filename);

    csvFile.setFileName(filename);

    if (!csvFile.exists()) this->createCsvFile();

    if (csvFile.open(QIODevice::Append)) {
        /* Write data to file */
        QTextStream ts(&csvFile);
        ts.setDevice(&csvFile);
        ts << QDate::currentDate().toString("yyyy.MM.dd") << sep
           << QTime::currentTime().toString("hh:mm:ss") << sep
           << t_normalized << sep << p_normalized<< sep << h_normalized
           << sep << Qt::endl;
        csvFile.close();
    }

    // if (csvFile.open(QIODevice::Append | QIODevice::Text)) {
    //     /* Write data to file */
    //     QTextStream ts(&csvFile);
    //     ts.setDevice(&csvFile);
    //     ts << QDate::currentDate().toString("yyyy.MM.dd") << sep
    //        << QTime::currentTime().toString("hh:mm:ss") << sep
    //        << QString::number(prm.temperature, 'f', 1) << sep
    //        << QString::number(prm.pressure, 'f', 1) << sep
    //        << QString::number(prm.humidity, 'f', 1) << sep << Qt::endl;
    //     csvFile.close();
    // }
}

/**
 * @brief MonitorView::createCsvFile
 */
void MonitorView::createCsvFile()
{
    QDir folder = csvDir;

    if (!folder.exists()) folder.mkdir(csvDir);

    csvFile.setFileName(filename);

    if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        /* Write header */
        QTextStream ts(&csvFile);
        ts.setDevice(&csvFile);
        ts << "Date" << sep << "Time" << sep
           << "Temperature" << sep << "Pressure" << sep << "Humidity" << sep << Qt::endl;
        csvFile.close();
    }
}

/**
 * @brief MainWindow::getFileName
 * @param fname
 */
void MonitorView::getFileName(quint16 n, QString &fname)
{
    fname = (csvDir + "/sensor" + QString::number(n) +
            QDate::currentDate().toString("_yyyyMMdd").append(".csv"));
}

/**
 * @brief MonitorView::onConfirmCmd
 * @param cmd
 */
void MonitorView::onConfirmCmd(quint8 cmd)
{
    switch (cmd)
    {
    case SET_RECEIVE:
        emit writeStatusBar("Sniffer in receive mode");
        break;
    case SET_SLEEP:
        for (int i = 0; i <sensNum; i++) {
            tempLcd[i]->display(0);
            pressLcd[i]->display(0);
            hummLcd[i]->display(0);
            rssiLcd[i]->display(0);
        }
        emit writeStatusBar("Sniffer in sleep mode");
        break;
    default:
        break;
    }
}

/**
 * @brief MonitorView::onCurrentChanged
 * @param index
 */
void MonitorView::onCurrentChanged(int index)
{
    QByteArray payload;

    if (index != MONITOR) {
        payload.insert(sizeof(TRFHeader), SET_SLEEP);
    }
    else {
        payload.insert(sizeof(TRFHeader), SET_RECEIVE);
    }
    emit createAndSend(CONNECT, payload);
}

/**
 * @brief MonitorView::sendResetCmd
 */
void MonitorView::sendResetCmd()
{
    QByteArray payload;

    payload.insert(sizeof(TRFHeader), RESET_CMD);
    emit createAndSend(CONNECT, payload);
}
