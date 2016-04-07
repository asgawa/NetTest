#ifndef DEFINE_PACKET_H
#define DEFINE_PACKET_H

#pragma pack(push, 1)
typedef struct TEST_PACKET {
    unsigned short length;
    char packetId;
    char targetId;
    char data[32];
    char heartbeat;
} TEST_PACKET;

#pragma pack(pop)

#endif // DEFINE_PACKET_H

