
#include "ns3/string.h"

class Data
{
public:
    std::string phyMode  = "DsssRate1Mbps";
    double rss           = -80;     // dBm
    uint32_t packetSize  = 1024;    // bytes
    uint32_t numpackets  = 1;
    double interval      = 1.0;     // seconds
    bool verbose         = false;
};