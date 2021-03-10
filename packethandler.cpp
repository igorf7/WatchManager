#include "packethandler.h"
#include "mainwindow.h"
#include "emulviewer.h"
#include "comport.h"

#include <QMessageBox>
#include <QDebug>

/**
 * @brief Class constructor
 */
PacketHandler::PacketHandler(QObject *parent) : QObject(parent)
{
    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
PacketHandler::~PacketHandler()
{
    qDebug() << "By-by from" << this;
}

/**
 * @brief PacketHandler::onCreateAndSend
 * @param pt -
 * @param payload -
 */
void PacketHandler::onCreateAndSend(PacketType_t pt, QByteArray &payload)
{
    RF_Header_t *rfHeader =
            reinterpret_cast<RF_Header_t*>(payload.data());

    switch (pt)
    {
    case CMD_PACKET:
        rfHeader->dst_addr = MainWindow::getAddress();
        rfHeader->snd_addr = (RF_REMOTE_TYPE << 8) | 0x01;
        break;
    case ACK_PACKET:
        break;
    case CONNECT:
        rfHeader->dst_addr = (RF_REMOTE_TYPE << 8) | 0x01;
        rfHeader->snd_addr = 0;
        break;
    default:
        rfHeader->snd_addr = 0;
        break;
    }

    rfHeader->type = pt;

    emit sendDataPacket(payload);
}

/**
 * @brief PacketHandler::onParsingPacket
 * @param packet
 */
void PacketHandler::onParsingPacket(const QByteArray &packet)
{
    quint16 id;
    RF_Packet_t *rx_packet = (RF_Packet_t*)packet.data();

    switch (rx_packet->header.dst_addr)
    {
    case 0: // To WatchManager from Remote via UART

        switch (rx_packet->data[0])
        {
        case GET_ADDRESS: // соединение установлено
            memcpy(&id, &rx_packet->data[1], sizeof(id));
            if (id == 0x0B01) {   // пульт
                emit connectionEstablished(id);
            }
            break;
        case SET_RECEIVE:
            emit confirmCmd(rx_packet->data[0]);
            break;
        case SET_SLEEP:
            emit confirmCmd(rx_packet->data[0]);
            break;
        default:
            break;
        }
        break;
    case 0x0B01: // To Remote from devices via RF
        break;
    case 0x0901: // To Base from devices via RF
        if (rx_packet->header.type == BME280_PACKET) {
            emit showParams(packet);
        }
        break;
    default:
        break;
    }
}
