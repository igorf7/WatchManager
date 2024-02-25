#include "remoteview.h"
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

/**
 * @brief RemoteView::RemoteView
 * @param parent
 */
RemoteView::RemoteView(QObject *parent) : QObject(parent)
{
    remoteWidget = new QWidget; // Main container of the "Remote control" tab
    QVBoxLayout *remotelayout = new QVBoxLayout; // Container layout

    QGroupBox *remoteGroupBox = new QGroupBox("", remoteWidget);
    QVBoxLayout* remGrBxLayout = new QVBoxLayout; // GroupBox layout

    QPushButton *sendButton = new QPushButton(" Remote control button ", remoteGroupBox);
    sendButton->setToolTip("Duplicates a button on the remote control");
    connect(sendButton, SIGNAL(clicked()), this, SLOT(onSendButton_clicked()));

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(sendButton);
    btnLayout->addStretch();

    remGrBxLayout->addStretch();
    remGrBxLayout->addLayout(btnLayout);
    remGrBxLayout->addStretch();
    remoteGroupBox->setLayout(remGrBxLayout);

    remotelayout->addWidget(remoteGroupBox);
    remoteWidget->setLayout(remotelayout);

    qDebug() << "Hello from" << this;
}

/**
 * @brief RemoteView::~RemoteView
 */
RemoteView::~RemoteView()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief RemoteView::onSendButton_clicked
 */
void RemoteView::onSendButton_clicked()
{
    QByteArray payload;

    payload.insert(sizeof(TRFHeader), DISPLAY_CMD);
    emit createAndSend(CMD_PACKET, payload);
}
