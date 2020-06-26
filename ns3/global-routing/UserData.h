#include "ns3/string.h"

class Data
{
public:
    int numNodes           = 4;
    int packetSize         = 210;
    std::string dataRate   = "448kb/s";
    bool EnableFlowMonitor = false;    
};