#ifndef DEFINE_PACKET_H
#define DEFINE_PACKET_H

#pragma pack(push, 1)

typedef struct TEST_PACKET {
    unsigned short PacketLength;
    unsigned char Command;
    unsigned char Status:1;
    unsigned char Spare2_1:1;
    unsigned char Spare2_2:1;
    unsigned char Spare2_3:1;
    unsigned char DataValid:1;
    unsigned char Spare2_5:1;
    unsigned char TargetCondition1:1;
    unsigned char TargetCondition2:1;
    unsigned char Spare3[2];
    unsigned char Power; // 5
    unsigned char Spare6;
    unsigned char Watchdog;
} TEST_PACKET;

#pragma pack(pop)

#endif // DEFINE_PACKET_H

