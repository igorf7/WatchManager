#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "packethandler.h"
#include "comport.h"
#include "remoteview.h"
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

signals:
    void connectClicked(const QString &port, int br);
    void disconnectClicked();
    void sendDataPacket(const QByteArray &packet);
    void createAndSend(TPacketType pt, QByteArray &payload);

public slots:
    void onPortError(const QString &msg);
    void onPortOpened(const QString &port);
    void onPortClosed();
    void onWriteStatusBar(const QString &str);
    void onConnectionEstablished(quint16 id);

protected:
    qint32 connTimeout;
    //int portNumber;
    void timerEvent(QTimerEvent *event);

private:
    QTabWidget *mainTabWidget;
    QComboBox *portComboBox;
    QComboBox *brComboBox;
    QPushButton *connPushButton = nullptr;
    QList<QString> portList;
    QThread *threadPort = nullptr;
    QString portName;

    static quint16 deviceAddress;
    PacketHandler *packHandler;

    bool isPortOpened = false;

    void updatePortList();

private slots:
    void onConnPushButtonClicked();
    void onBrChanged(int index);
};
#endif // MAINWINDOW_H
