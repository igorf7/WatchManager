#ifndef MESSVIEWER_H
#define MESSVIEWER_H
#include "mainwindow.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QObject>

class MessViewer : public QObject
{
    Q_OBJECT
public:
    explicit MessViewer(QObject *parent = nullptr);
    ~MessViewer();

    QWidget *messWidget = nullptr;

private:
    static const quint8 msgNumber = 4;
    QLineEdit *readMsg[msgNumber], *writeMsg[msgNumber];
    QPushButton *readBtn[msgNumber], *writeBtn[msgNumber];

    void setMessage(quint8 num);
    void getMessage(quint8 num);

signals:
    void createAndSend(PacketType_t pt, QByteArray &payload);

private slots:
    void onWriteBnt_0_clicked();
    void onWriteBnt_1_clicked();
    void onWriteBnt_2_clicked();
    void onWriteBnt_3_clicked();
    void onReadBnt_0_clicked();
    void onReadBnt_1_clicked();
    void onReadBnt_2_clicked();
    void onReadBnt_3_clicked();
};

#endif // MESSVIEWER_H
