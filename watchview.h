#ifndef WATCHVIEW_H
#define WATCHVIEW_H

#include "mainwindow.h"

#include <QObject>
#include <QGroupBox>
#include <QLCDNumber>

class WatchView : public QObject
{
    Q_OBJECT
public:
    explicit WatchView(QObject *parent = nullptr);
    ~WatchView();

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
    void createAndSend(TPacketType pt, QByteArray &payload);

private slots:
    void onAutoSetButtonClicked();
    void onCurrentChanged(int index);
};

#endif // WATCHVIEW_H
