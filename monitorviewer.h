#ifndef MONITORVIEWER_H
#define MONITORVIEWER_H
#include "mainwindow.h"

#include <QObject>
#include <QLCDNumber>

/* Parameters structure */
typedef struct
{
    float temperature,
           pressure,
           hummidity;
}Params_t;

/**
 * @brief Class constructor
 * @param parent
 */
class MonitorViewer : public QObject
{
    Q_OBJECT
public:
    explicit MonitorViewer(QObject *parent = nullptr);
    ~MonitorViewer();

    QWidget *monitorWidget = nullptr;

signals:
    void createAndSend(PacketType_t pt, QByteArray &payload);

public slots:
    void onShowParams(const QByteArray &packet);
    void onConfirmCmd(quint8 cmd);
    void onCurrentChanged(int index);

private:
    static const quint8 sensNum = 2;
    QLCDNumber *tempLcd[sensNum];
    QLCDNumber *pressLcd[sensNum];
    QLCDNumber *hummLcd[sensNum];
    QLCDNumber *rssiLcd[sensNum];

    tabsEnum_t myTabIndex = MONITOR;

    void sendResetCmd();

private slots:
};

#endif // MONITORVIEWER_H
