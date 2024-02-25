#include "watchview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDateEdit>
#include <QTimeEdit>
#include <QList>
#include <QLabel>
#include <QTimerEvent>
#include <QDebug>

/**
 * @brief Class constructor
 * @param parent
 */
WatchView::WatchView(QObject *parent) : QObject(parent)
{
    watchWidget = new QWidget(); // Main container of the "Clock" tab
    QVBoxLayout *watchlayout = new QVBoxLayout; // Layout

    /* Groupbox for automatic date/time setting */
    QGroupBox *auGroupBox = new QGroupBox(watchWidget);
    auGroupBox->setTitle("Setting the date and time on the device");
    QHBoxLayout *agbLayout = new QHBoxLayout;
    timeZoneBox = new QComboBox;
    QList<QString> timeZoneList;
    timeZoneList << "UTC + 00:00" << "UTC + 01:00" << "UTC + 02:00" << "UTC + 03:00"
                 << "UTC + 04:00" << "UTC + 05:00" << "UTC + 06:00" << "UTC + 07:00"
                 << "UTC + 08:00" << "UTC + 09:00" << "UTC + 10:00" << "UTC + 11:00"
                 << "UTC + 12:00" << "UTC - 01:00" << "UTC - 02:00" << "UTC - 03:00"
                 << "UTC - 04:00" << "UTC - 05:00" << "UTC - 06:00" << "UTC - 07:00"
                 << "UTC - 08:00" << "UTC - 09:00" << "UTC - 10:00" << "UTC - 11:00"
                 << "UTC - 12:00";
    timeZoneBox->addItems(timeZoneList);
    timeZoneBox->setCurrentIndex(3);
    QLabel *timeZoneLabel = new QLabel("Timezone");
    QPushButton *autoSetButton = new QPushButton(" Synchronize ");
    connect(autoSetButton, SIGNAL(clicked()), this, SLOT(onAutoSetButtonClicked()));

    timeLcd = new QLCDNumber();
    timeLcd->setSmallDecimalPoint(true);
    timeLcd->setSegmentStyle(QLCDNumber::Flat);
    timeLcd->setDigitCount(8);
    timeLcd->display("");

    currDate = new QLabel();
    currDate->setAlignment(Qt::AlignCenter);

    agbLayout->addStretch();
    agbLayout->addWidget(autoSetButton);
    agbLayout->addStretch();
    agbLayout->addWidget(timeZoneLabel);
    agbLayout->addWidget(timeZoneBox);
    agbLayout->addStretch();
    auGroupBox->setLayout(agbLayout);

    QVBoxLayout *calendarLayot = new QVBoxLayout;
    calendarLayot->addWidget(timeLcd);
    calendarLayot->addWidget(currDate);

    /* Adding widgets to the container of the "Clock" tab */
    watchlayout->addWidget(auGroupBox);
    watchlayout->addLayout(calendarLayot);
    watchWidget->setLayout(watchlayout);

    showTimeTimer = this->startTimer(timerPeriod);

    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
WatchView::~WatchView()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief WatchView::timerEvent
 * @param event
 */
void WatchView::timerEvent(QTimerEvent *event)
{
    static QDate dateNow;
    static QTime timeNow;

    if (event->timerId() == showTimeTimer) {
        dateNow = QDate::currentDate();
        timeNow = QTime::currentTime();
        timeLcd->display(timeNow.toString("hh:mm:ss"));
        currDate->setText(dateNow.toString(Qt::TextDate));
    }
}

/**
 * @brief autoSetButton slot
 */
void WatchView::onAutoSetButtonClicked()
{
    qint64 timestamp;
    QByteArray payload;
    qint32 timeShift = 0;

    if (timeZoneBox->currentIndex() <= 12) {
        timeShift = timeZoneBox->currentIndex() * 3600 + 1;
    }
    else {
        timeShift = -3600 * (timeZoneBox->currentIndex() - 12) + 1;
    }
    timestamp = QDateTime::currentSecsSinceEpoch() + timeShift;
    payload.insert(sizeof(TRFHeader), SET_DATETIME);
    payload.append(static_cast<char>(timestamp>>24));
    payload.append(static_cast<char>(timestamp>>16));
    payload.append(static_cast<char>(timestamp>>8));
    payload.append(static_cast<char>(timestamp>>0));

    emit createAndSend(CMD_PACKET, payload);
}

/**
 * @brief WatchView::onCurrentChanged
 * @param index
 */
void WatchView::onCurrentChanged(int index)
{
    if (index == myTabIndex) {
        showTimeTimer = this->startTimer(timerPeriod);
    }
    else {
        if (showTimeTimer != 0) {
            this->killTimer(showTimeTimer);
            showTimeTimer = 0;
        }
    }
}
