#ifndef REMOTEVIEWER_H
#define REMOTEVIEWER_H

#include <QObject>

#include "mainwindow.h"

class RemoteViewer : public QObject
{
    Q_OBJECT
public:
    explicit RemoteViewer(QObject *parent = nullptr);
    ~RemoteViewer();

    QWidget *remoteWidget = nullptr;

signals:
    void createAndSend(PacketType_t pt, QByteArray &payload);

private slots:
    void onSendButton_clicked();
};

#endif // REMOTEVIEWER_H
