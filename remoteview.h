#ifndef REMOTEVIEW_H
#define REMOTEVIEW_H

#include <QObject>

#include "mainwindow.h"

class RemoteView : public QObject
{
    Q_OBJECT
public:
    explicit RemoteView(QObject *parent = nullptr);
    ~RemoteView();

    QWidget *remoteWidget = nullptr;

signals:
    void createAndSend(TPacketType pt, QByteArray &payload);

private slots:
    void onSendButton_clicked();
};

#endif // REMOTEVIEW_H
