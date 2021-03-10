#include "remoteviewer.h"
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

/**
 * @brief RemoteViewer::RemoteViewer
 * @param parent
 */
RemoteViewer::RemoteViewer(QObject *parent) : QObject(parent)
{
    remoteWidget = new QWidget; // Главный контейнер вкладки "Пульт"
    QVBoxLayout *remotelayout = new QVBoxLayout; // Компоновщик главного контейнера

    QGroupBox *remoteGroupBox = new QGroupBox("", remoteWidget);
    QVBoxLayout* remGrBxLayout = new QVBoxLayout; // Компоновщик групбокса

    QPushButton *sendButton = new QPushButton(" Кнопка пульта ", remoteGroupBox);
    sendButton->setToolTip("Дублирует кнопку на пульте");
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
 * @brief RemoteViewer::~RemoteViewer
 */
RemoteViewer::~RemoteViewer()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief RemoteViewer::onSendButton_clicked
 */
void RemoteViewer::onSendButton_clicked()
{
    QByteArray payload;

    payload.insert(sizeof(RF_Header_t), DISPLAY_CMD);
    emit createAndSend(CMD_PACKET, payload);
}
