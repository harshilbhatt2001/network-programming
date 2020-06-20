/**
 * @author Harshil Bhatt
 * @create date 2020-06-19 16:23:09
 * @desc
 */

#include "ns3/string.h"
#include <bits/types.h>

typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;

 class Data
 {
public:
    int numNodes            = 4;
    int dataRate            = 5e6;
    int delay               = 2;
    int mtu                 = 1400;
    uint32_t packetSize     = 1024;
    uint32_t maxPacketCount = 500;
    double interval         = 0.01;
    double SimulationTime   = 10;
    bool trace              = false;
    bool netanim            = false;
 };
