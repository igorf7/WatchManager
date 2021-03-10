#include "messviewer.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

/**
 * @brief Class constructor
 * @param parent
 */
MessViewer::MessViewer(QObject *parent) : QObject(parent)
{
    messWidget = new QWidget; // Главный контейнер вкладки "Сообщения"
    QVBoxLayout *messlayout = new QVBoxLayout; // Компоновщик

    /* Создать групбоксы и виджеты в них */
    QGroupBox *readGroupBox = new QGroupBox(messWidget);
    QGroupBox *writeGroupBox = new QGroupBox(messWidget);
    readGroupBox->setTitle("Чтение сообщений из устройства");
    writeGroupBox->setTitle("Запись сообщений в устройство");

    QHBoxLayout *readMsgHLayout[msgNumber];
    QHBoxLayout *writeMsgHLayout[msgNumber];
    QVBoxLayout *readMsgVLayout = new QVBoxLayout;
    QVBoxLayout *writeMsgVLayout = new QVBoxLayout;

    QRegExp regExp(".{26}"); // размер сообщения до 26 символов
    QRegExpValidator *sizeValidator = new QRegExpValidator(regExp, this);

    for(int i = 0; i < msgNumber; i++){
        readMsg[i] = new QLineEdit();
        readMsg[i]->setReadOnly(true);
        readBtn[i] = new QPushButton("Прочитать");
        writeMsg[i] = new QLineEdit();
        writeMsg[i]->setValidator(sizeValidator);
        writeBtn[i] = new QPushButton("Записать");
        readMsgHLayout[i] = new QHBoxLayout;
        writeMsgHLayout[i] = new QHBoxLayout;
        readMsgHLayout[i]->addWidget(readMsg[i]);
        readMsgHLayout[i]->addWidget(readBtn[i]);
        writeMsgHLayout[i]->addWidget(writeMsg[i]);
        writeMsgHLayout[i]->addWidget(writeBtn[i]);
        readMsgVLayout->addLayout(readMsgHLayout[i]);
        writeMsgVLayout->addLayout(writeMsgHLayout[i]);
    }
    readGroupBox->setLayout(readMsgVLayout);
    writeGroupBox->setLayout(writeMsgVLayout);

    /* Добавить групбоксы в главный контейнер вкладки "Сообщения" */
    messlayout->addStretch();
    messlayout->addWidget(readGroupBox);
    messlayout->addStretch();
    messlayout->addWidget(writeGroupBox);
    messlayout->addStretch();
    messWidget->setLayout(messlayout);

    connect(writeBtn[0], SIGNAL(clicked()), this, SLOT(onWriteBnt_0_clicked()));
    connect(writeBtn[1], SIGNAL(clicked()), this, SLOT(onWriteBnt_1_clicked()));
    connect(writeBtn[2], SIGNAL(clicked()), this, SLOT(onWriteBnt_2_clicked()));
    connect(writeBtn[3], SIGNAL(clicked()), this, SLOT(onWriteBnt_3_clicked()));

    connect(readBtn[0], SIGNAL(clicked()), this, SLOT(onReadBnt_0_clicked()));
    connect(readBtn[1], SIGNAL(clicked()), this, SLOT(onReadBnt_1_clicked()));
    connect(readBtn[2], SIGNAL(clicked()), this, SLOT(onReadBnt_2_clicked()));
    connect(readBtn[3], SIGNAL(clicked()), this, SLOT(onReadBnt_3_clicked()));

    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
MessViewer::~MessViewer()
{
    qDebug() << "By-by from" << this;
}

void MessViewer::onReadBnt_0_clicked()
{
    getMessage(0);
}

void MessViewer::onReadBnt_1_clicked()
{
    getMessage(1);
}

void MessViewer::onReadBnt_2_clicked()
{
    getMessage(2);
}

void MessViewer::onReadBnt_3_clicked()
{
    getMessage(3);
}

/**
 * @brief MessViewer::onWriteBnt_0_clicked
 */
void MessViewer::onWriteBnt_0_clicked()
{
    setMessage(0);
}

/**
 * @brief MessViewer::onWriteBnt_1_clicked
 */
void MessViewer::onWriteBnt_1_clicked()
{
    setMessage(1);
}

/**
 * @brief MessViewer::onWriteBnt_2_clicked
 */
void MessViewer::onWriteBnt_2_clicked()
{
    setMessage(2);
}

/**
 * @brief MessViewer::onWriteBnt_3_clicked
 */
void MessViewer::onWriteBnt_3_clicked()
{
    setMessage(3);
}

/**
 * @brief MessViewer::getMessage
 * @param num
 */
void MessViewer::getMessage(quint8 num)
{
    QByteArray payload;

    payload.insert(sizeof(RF_Header_t), GET_MSG);
    payload.append(num);
    emit createAndSend(CMD_PACKET, payload);
}

/**
 * @brief MessViewer::setMessage
 * @param num
 */
void MessViewer::setMessage(quint8 num)
{
    QByteArray payload;

    payload.insert(sizeof(RF_Header_t), SET_MSG);
    payload.append(num);
    payload.append(writeMsg[num]->text());
    emit createAndSend(CMD_PACKET, payload);
}
