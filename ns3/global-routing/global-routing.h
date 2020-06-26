#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "UserData.h"


using namespace ns3;


NodeContainer Init               (Data data, int argc, char *argv[]);
NodeContainer CreateContainer    (NodeContainer nodes, int a, int b, Data data);
NetDeviceContainer CreateChannel (NodeContainer nanb, std::string dataRate, std::string delay);
Ipv4InterfaceContainer AssignIP  (NetDeviceContainer dadb, Ipv4Address NetworkAddress, Ipv4Mask SubnetMask);
Ipv4InterfaceContainer Internet  (NodeContainer nodes, NodeContainer nanb, 
                                 Ipv4Address NetworkAddress, Ipv4Mask SubnetMask);
void OnOffApp                    (Data data, NodeContainer nodes, int numSource, int numSink, 
                                 Ipv4InterfaceContainer iaib, double appStart, double appStop);
