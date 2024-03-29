#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <QObject>

/* Device types */
typedef enum{
    METEO_WATCH_TYPE = (quint8)9,
    RF_BME280_TYPE,
    RF_REMOTE_TYPE
}TDeviceType;

typedef enum  { /* Packet types */
    BME280_PACKET = static_cast<char>(1),
    CMD_PACKET,
    ACK_PACKET,
    DIA_PACKET,
    CONNECT
}TPacketType;

/* Cmd code */
typedef enum{
    DISPLAY_CMD = static_cast<char>(7),
    SET_MSG,
    GET_MSG,
    SET_DATETIME,
    GET_DATETIME,
    DUMMY_CMD,
    GET_ADDRESS,
    SET_RECEIVE,
    SET_SLEEP,
    RESET_CMD
}TCmdCode;

/* Packet header structure */
#pragma pack(push, 1)
typedef struct
{
    quint16 dst_addr,
            snd_addr;
    quint8 type;
} TRFHeader;
#pragma pack(pop)

/* Packet structure */
#pragma pack(push, 1)
typedef struct
{
    TRFHeader header;
    quint8 data[64];
} TRFPacket;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    float temperature,
          pressure,
          humidity;
    qint8 rssi;
} TParameter;
#pragma pack(pop)

class PacketHandler : public QObject
{
    Q_OBJECT
public:
    explicit PacketHandler(QObject *parent = nullptr);
    ~PacketHandler();

signals:
    void sendDataPacket(const QByteArray &packet);
    void showParams(const QByteArray &packet);
    void confirmCmd(quint8 cmd);
    void connectionEstablished(quint16 id);

public slots:
    void onCreateAndSend(TPacketType pt, QByteArray &payload);
    void onParsingPacket(const QByteArray &packet);
};

#endif // PACKETHANDLER_H
