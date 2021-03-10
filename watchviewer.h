#ifndef WATCHVIEWER_H
#define WATCHVIEWER_H

#include "mainwindow.h"

#include <QObject>
#include <QGroupBox>
#include <QLCDNumber>

class WatchViewer : public QObject
{
    Q_OBJECT
public:
    explicit WatchViewer(QObject *parent = nullptr);
    ~WatchViewer();

    QWidget* watchWidget = nullptr;

private:
    QByteArray dataBuffer;
    QComboBox *timeZoneBox;
    QLCDNumber *timeLcd;
    QLabel *currDate;

    tabsEnum_t myTabIndex = CLOCK;

protected:
    qint32 showTimeTimer;
    quint32 timerPeriod = 100;
    void timerEvent(QTimerEvent *event);

signals:
    void createAndSend(PacketType_t pt, QByteArray &payload);

private slots:
    void onAutoSetButton_clicked();
    void onCurrentChanged(int index);
};

#endif // WATCHVIEWER_H
