/**
 * @author  Harshil Bhatt
 * @create date 2020-06-19 18:57:25
 * @desc
 */


#include "ns3/string.h"


class Data
{
public:
    std::string phyMode = "DsssRate1Mbps";
    double distance     = 500;      // m
    uint32_t packetSize = 1000;     // bytes
    uint32_t numPackets = 1;
    uint32_t numNodes   = 25;       // by default, 5x5
    uint32_t sinkNode   = 0;
    uint32_t sourceNode = 24;
    double interval     = 1.0;      // seconds
    bool verbose        = false;
    bool tracing        = false;
};


