#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "packethandler.h"
#include "comport.h"
#include "remoteviewer.h"
#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

typedef enum tabsEnum {
    CLOCK = 0,
    MONITOR,
    REMOTE
} tabsEnum_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static quint16 getAddress()
    {
        return deviceAddress;
    }

    static bool getBeeperStatus()
    {
        return beeperStatus;
    }

signals:
    void connectClicked(const QString &port);
    void disconnectClicked();
    void sendDataPacket(const QByteArray &packet);
    void createAndSend(PacketType_t pt, QByteArray &payload);

public slots:
    void onPortOpenError(const QString &msg);
    void onPortSendError(const QString &msg);
    void onPortOpened(const QString &port);
    void onPortClosed();
    void onWriteStatusBar(const QString &str, int timeout);
    void onConnectionEstablished(quint16 id);

protected:
    qint32 connTimeout;
    int portNumber;
    void timerEvent(QTimerEvent *event);

private:
    QTabWidget *mainTabWidget;
    QPushButton *connPushButton = nullptr;
    QList<QString> portList;
    QThread *threadPort = nullptr;
    QString portName;

    static quint16 deviceAddress;
    PacketHandler *packHandler;

    bool isPortOpened = false;
    bool isConnected = false;
    static bool beeperStatus;

    void updatePortList();

private slots:
    void onConnPushButton_clicked();
    void onQuit_triggered();
    void onManual_triggered();
    void onAbout_triggered();
    void onBeep_triggered(bool state);
    void onAddressEdited(const QString &text);
};
#endif // MAINWINDOW_H
