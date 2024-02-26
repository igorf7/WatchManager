#ifndef MONITORVIEW_H
#define MONITORVIEW_H
#include "mainwindow.h"

#include <QObject>
#include <QLCDNumber>
#include <QDate>
#include <QFile>
#include <QDir>

/**
 * @brief Class constructor
 * @param parent
 */
class MonitorView : public QObject
{
    Q_OBJECT
public:
    explicit MonitorView(QObject *parent = nullptr);
    ~MonitorView();

    QWidget *monitorWidget = nullptr;

signals:
    void createAndSend(TPacketType pt, QByteArray &payload);
    void writeStatusBar(const QString &str);

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
    QFile csvFile;
    QString csvDir;
    QString filename;
    char sep = ',';

    void sendResetCmd();
    void createCsvFile();
    void getFileName(quint16 n, QString &fname);
};

#endif // MONITORVIEW_H
