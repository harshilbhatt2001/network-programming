#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-helper.h"
#include "ns3/internet-stack-helper.h"
#include "UserData.h"

using namespace ns3;


NodeContainer init              (Data data, int argc, char *argv[]);
NetDeviceContainer SetupWifi    (Data &data, NodeContainer nodes);
void SetupMobility              (NodeContainer nodes);
void Internet                   (NodeContainer nodes, NetDeviceContainer devices);
Ipv4InterfaceContainer AssignIP (Ipv4Address NetworkAddress, Ipv4Mask SubnetMask, 
                                NetDeviceContainer devices);
Ptr<Socket> SocketConnect       (NodeContainer nodes);
void ReceivePacket              (Ptr<Socket> socket);
static void GenerateTraffic     (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval); 






