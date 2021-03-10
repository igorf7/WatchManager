#include "emulviewer.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <cstring>
#include <QDebug>

quint8 EmulViewer::sensorId = 2;

/**
 * @brief Class constructor
 * @param parent
 */
EmulViewer::EmulViewer(QObject *parent) : QObject(parent)
{
    emulWidget = new QWidget; // Главный контейнер вкладки "Эмулятор"
    QVBoxLayout *emullayout = new QVBoxLayout; // Компоновщик

    QDoubleValidator *tempValidator = new QDoubleValidator(this);
    QDoubleValidator *pressValidator = new QDoubleValidator(this);
    QDoubleValidator *hummValidator = new QDoubleValidator(this);
    tempValidator->setRange(-100.0, 100.0, 1);
    tempValidator->setNotation(QDoubleValidator::StandardNotation);
    tempValidator->setLocale(QLocale::English);
    pressValidator->setRange(0.0, 999.0, 1);
    pressValidator->setNotation(QDoubleValidator::StandardNotation);
    pressValidator->setLocale(QLocale::English);
    hummValidator->setRange(0.0, 100.0, 1);
    hummValidator->setNotation(QDoubleValidator::StandardNotation);
    hummValidator->setLocale(QLocale::English);

    QGroupBox *emulGroupBox = new QGroupBox("Эмуляция датчиков", emulWidget);
    QFormLayout *prmLayout = new QFormLayout;
    tempLineEdit = new QLineEdit;
    pressLineEdit = new QLineEdit;
    hummLineEdit = new QLineEdit;
    QSpinBox *sensIdSpinBox = new QSpinBox;
    sensIdSpinBox->setRange(2, 5);
    sensIdSpinBox->setAlignment(Qt::AlignCenter);
    sensIdSpinBox->setMaximumWidth(40);
    connect(sensIdSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(onValueChanged(int)));
    sensIdSpinBox->setValue(sensorId);

    tempLineEdit->setMaximumWidth(80);
    tempLineEdit->setAlignment(Qt::AlignCenter);
    tempLineEdit->setValidator(tempValidator);
    tempLineEdit->setText("0.0");
    pressLineEdit->setMaximumWidth(80);
    pressLineEdit->setAlignment(Qt::AlignCenter);
    pressLineEdit->setValidator(pressValidator);
    pressLineEdit->setText("0.0");
    hummLineEdit->setMaximumWidth(80);
    hummLineEdit->setAlignment(Qt::AlignCenter);
    hummLineEdit->setValidator(hummValidator);
    hummLineEdit->setText("0.0");
    //QCheckBox *repeatChekBox = new QCheckBox("Цикл");
    QPushButton * sendPrmButton = new QPushButton("Отправить");
    sendPrmButton->setMaximumWidth(80);

    prmLayout->addRow(tr("&Температура:"), tempLineEdit);
    prmLayout->addRow(tr("&Давление:"), pressLineEdit);
    prmLayout->addRow(tr("&Влажность:"), hummLineEdit);
    prmLayout->addRow("", new QLabel("")); // Пустая строка

    QVBoxLayout *v1Layout = new QVBoxLayout;
    QHBoxLayout *h1Layout = new QHBoxLayout;
    h1Layout->addWidget(new QLabel("Датчик №"));
    h1Layout->addWidget(sensIdSpinBox);
    h1Layout->addStretch();
    h1Layout->addWidget(sendPrmButton);
    h1Layout->addStretch();
    //h1Layout->addWidget(repeatChekBox);
    h1Layout->addStretch();
    v1Layout->addLayout(prmLayout);
    v1Layout->addLayout(h1Layout);
    emulGroupBox->setLayout(v1Layout);

    emullayout->addStretch();
    emullayout->addWidget(emulGroupBox);
    emullayout->addStretch();

    /**/
    emulWidget->setLayout(emullayout);

    connect(sendPrmButton, SIGNAL(clicked()), this, SLOT(onSendPrmButton_clicked()));

    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
EmulViewer::~EmulViewer()
{
    qDebug() << "By-by from" << this;
}

void EmulViewer::onSendPrmButton_clicked()
{
    bool Ok;
    QByteArray payload;
    char tmp[12];

    Params_t *prm = reinterpret_cast<Params_t*>(tmp);

    float value = tempLineEdit->text().toFloat(&Ok);
    prm->temperature = value;
    value = pressLineEdit->text().toFloat(&Ok);
    prm->pressure = value;
    value = hummLineEdit->text().toFloat(&Ok);
    prm->hummidity = value;
    payload.insert(sizeof(RF_Header_t), tmp, 12);

    emit createAndSend(BME280_PACKET, payload);
}

void EmulViewer::onValueChanged(int value)
{
    this->sensorId = static_cast<quint8>(value);
}
