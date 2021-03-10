#include "watchviewer.h"

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
WatchViewer::WatchViewer(QObject *parent) : QObject(parent)
{
    watchWidget = new QWidget(); // Главный контейнер вкладки "Часы"
    QVBoxLayout *watchlayout = new QVBoxLayout; // Компоновщик

    /* Групбокс автоматической установки даты/времени */
    QGroupBox *auGroupBox = new QGroupBox(watchWidget);
    auGroupBox->setTitle("Установка даты и времени");
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
    QLabel *timeZoneLabel = new QLabel("Часовой пояс");
    QPushButton *autoSetButton = new QPushButton(" Синхронизировать ");
    connect(autoSetButton, SIGNAL(clicked()), this, SLOT(onAutoSetButton_clicked()));

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

    /* Добавляем виджеты в контейнер вкладки "Часы" */
    watchlayout->addWidget(auGroupBox);
    watchlayout->addLayout(calendarLayot);
    watchWidget->setLayout(watchlayout);

    showTimeTimer = this->startTimer(timerPeriod);

    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
WatchViewer::~WatchViewer()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief WatchViewer::timerEvent
 * @param event
 */
void WatchViewer::timerEvent(QTimerEvent *event)
{
    static QDate dateNow;
    static QTime timeNow;
    static QLocale dtLocale(QLocale::system());

    if (event->timerId() == showTimeTimer) {
        dateNow = QDate::currentDate();
        timeNow = QTime::currentTime();
        timeLcd->display(timeNow.toString("hh:mm:ss"));
        currDate->setText(dtLocale.dayName(dateNow.dayOfWeek(), dtLocale.LongFormat)
                          + ", "
                          + dateNow.toString(Qt::SystemLocaleLongDate));
    }
}

/**
 * @brief autoSetButton slot
 */
void WatchViewer::onAutoSetButton_clicked()
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
    payload.insert(sizeof(RF_Header_t), SET_DATETIME);
    payload.append(static_cast<char>(timestamp>>24));
    payload.append(static_cast<char>(timestamp>>16));
    payload.append(static_cast<char>(timestamp>>8));
    payload.append(static_cast<char>(timestamp>>0));

    emit createAndSend(CMD_PACKET, payload);
}

/**
 * @brief WatchViewer::onCurrentChanged
 * @param index
 */
void WatchViewer::onCurrentChanged(int index)
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
