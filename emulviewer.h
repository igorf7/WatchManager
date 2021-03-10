#ifndef EMULVIEWER_H
#define EMULVIEWER_H
#include "mainwindow.h"

#include <QObject>

/* Parameters structure */
//#pragma pack(push, 1)
typedef struct
{
    float temperature,
           pressure,
           hummidity;
}Params_t;
//#pragma pack(pop)

/**
 * @brief Class constructor
 * @param parent
 */
class EmulViewer : public QObject
{
    Q_OBJECT
public:
    explicit EmulViewer(QObject *parent = nullptr);
    ~EmulViewer();

    static quint8 getSensorId()
    {
        return sensorId;
    }

    QWidget *emulWidget = nullptr;

private:
    static quint8 sensorId;
    QLineEdit *tempLineEdit;
    QLineEdit *pressLineEdit;
    QLineEdit *hummLineEdit;

signals:
    void createAndSend(PacketType_t pt, QByteArray &payload);

private slots:
    void onSendPrmButton_clicked();
    void onValueChanged(int);
};

#endif // EMULVIEWER_H
