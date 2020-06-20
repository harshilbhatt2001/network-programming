/**
 * @author Harshil Bhatt
 * @create date 2020-06-18 21:01:10
 * @desc 
 */

#include "ns3/string.h"


class UserData
{
public:
    /**
     * payloadSize    = 1472
     * dataRate       = "100Mbps"
     * tcpVariant     = "TcpNewReno";
     * phyRate        = "HtMcs7";
     * SimulationTime = 10;
     * pcaptracing    = false;
     * netanim        = false;
     */

    uint32_t payloadSize   = 1472;
    std::string dataRate   = "100Mbps";
    std::string tcpVariant = "TcpNewReno";      // https://en.wikipedia.org/wiki/TCP_congestion_control#TCP_New_Reno
    std::string phyRate    = "HtMcs7";
    double SimulationTime  = 10;
    bool pcaptracing       = false;
    bool netanim           = false;
};
