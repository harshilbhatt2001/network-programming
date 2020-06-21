/**
 * @author Harshil Bhatt
 * @create date 2020-06-19 17:21:06
 * @desc
 * 
 * 
 *      This program configures a grid (default 5x5) of nodes on an
 *      802.11b physical layer, with
 *      802.11b NICs in adhoc mode, and by default, sends one packet of 1000
 *      (application) bytes to node 1.
 *      
 *      The default layout is like this, on a 2-D grid.
 *      
 *      n20  n21  n22  n23  n24
 *      n15  n16  n17  n18  n19
 *      n10  n11  n12  n13  n14
 *      n5   n6   n7   n8   n9
 *      n0   n1   n2   n3   n4
 *      
 *      the layout is affected by the parameters given to GridPositionAllocator;
 *      by default, GridWidth is 5 and numNodes is 25..
 *      
 *      There are a number of command-line options available to control
 *      the default behavior.  The list of available command-line options
 *      can be listed with the following command:
 *      >> ./waf --run "wifi-simple-adhoc-grid --help"
 *      
 *      Note that all ns-3 attributes (not just the ones exposed in the below
 *      script) can be changed at command line; see the ns-3 documentation.
 * 
 *      ERROR:
 *          1.Build Failed 
 *      
 *      TO-DO:  
 *          1. enable an IP-level trace that shows forwarding events only
 *
 *      
 */

#include "ns3/core-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/internet-stack-helper.h"

#include "wifi-simple-adhoc-grid.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdHocGrid");

void init                   (Data &data, int argc, char *argv[], NodeContainer &nodes);
void SetupWifiNIC           (Data &data, NodeContainer &nodes, NetDeviceContainer &devices,
                            YansWifiPhyHelper &wifiPhy);
void SetupMobility          (Data &data, NodeContainer &nodes);                      
void Routing                (Data &data, NodeContainer &nodes, NetDeviceContainer &devices,
                            OlsrHelper &olsr);
void AssignIP               (Ipv4Address NetworkAddress, Ipv4Mask SubnetMask, NetDeviceContainer &devices, 
                            Ipv4InterfaceContainer &interface);
void Recv                   (Data &data, NodeContainer &nodes, TypeId &tid, int port);
void Send                   (Data &data, NodeContainer &nodes, TypeId &tid, 
                            int port, Ipv4InterfaceContainer &interface);
void tracing                (Data &data, OlsrHelper &olsr, YansWifiPhyHelper &wifiPhy, 
                            NetDeviceContainer &devices);
void ReceivePacket          (Ptr<Socket> socket);                            
static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, 
                            Time pktInterval);

int 
main (int argc, char *argv[])
{
    Data data;
    NodeContainer nodes;
    init (data, argc, argv, nodes);
    NetDeviceContainer devices;
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    SetupWifiNIC (data, nodes, devices, wifiPhy);
    SetupMobility (data, nodes);
    OlsrHelper olsr;
    Routing (data, nodes, devices, olsr);
    
    tracing (data, olsr, wifiPhy, devices);
    
    Simulator::Schedule (Seconds (30.0), &GenerateTraffic,
                        data.sourceNode, data.packetSize, data.numPackets, data.interval);

    NS_LOG_UNCOND ("Testing from node " << data.sourceNode << " to " << data.sinkNode << " with grid distance " << data.distance);

    Simulator::Stop (Seconds (33.0));
    Simulator::Run();
    Simulator::Destroy();


}


void
init (Data &data, int argc, char *argv[], NodeContainer &nodes)
{
    CommandLine cmd;

    cmd.AddValue ("phyMode",    "Wifi Phy mode",                     data.phyMode);
    cmd.AddValue ("distance",   "distance (m)",                      data.distance);
    cmd.AddValue ("packetSize", "Size of packet Sent",               data.packetSize);
    cmd.AddValue ("numPackets", "Number of packets sent",            data.numPackets);
    cmd.AddValue ("interval",   "Interval (s) b/w packets",          data.interval);
    cmd.AddValue ("verbose",    "Turn on all WifiNetDevice Logging", data.verbose);
    cmd.AddValue ("tracing",    "Turn on ascii and pcap tracing",    data.tracing);
    cmd.AddValue ("numNodes",   "Number of Nodes",                   data.numNodes);
    cmd.AddValue ("sinkNode",   "Receiver node number",              data.sinkNode);
    cmd.AddValue ("sourceNode", "Sender node number",                data.sourceNode);

    cmd.Parse(argc, argv);
    Time interPacketInterval = Seconds (data.interval); 
    // Fix non-unicast data rate to be the same as that of unicast
    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (data.phyMode));

    nodes.Create (data.numNodes);

}

void
SetupWifiNIC (Data &data, NodeContainer &nodes, NetDeviceContainer &devices,
              YansWifiPhyHelper &wifiPhy)
{
    WifiHelper wifi;
    if (data.verbose)
    {
        wifi.EnableLogComponents ();
    }
    
    wifiPhy.Set ("RxGain", DoubleValue(-10.0));     // set gain = 0
    wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);

    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
    wifiPhy.SetChannel (wifiChannel.Create());

    WifiMacHelper wifiMac;
    wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                  "DataMode", StringValue (data.phyMode),
                                  "ControlMode", StringValue (data.phyMode));

    wifiMac.SetType ("ns3::AdhocWifiMac");
    devices = wifi.Install (wifiPhy, wifiMac, nodes);
}

void
SetupMobility (Data &data, NodeContainer &nodes)
{
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX",       DoubleValue (0.0),
                                   "MinY",       DoubleValue (0.0),
                                   "DeltaX",     DoubleValue (data.distance),
                                   "DeltaY",     DoubleValue (data.distance),
                                   "GridWidth",  UintegerValue (5),
                                   "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);                                   
}



void
Routing (Data &data, NodeContainer &nodes, NetDeviceContainer &devices,
        OlsrHelper &olsr)
{
    Ipv4StaticRoutingHelper staticRouting;

    Ipv4ListRoutingHelper list;
    list.Add (staticRouting, 0);
    list.Add (olsr, 10);

    InternetStackHelper internet;
    internet.SetRoutingHelper (list);
    internet.Install (nodes);
    Ipv4Address NetworkAddress = "10.1.1.0";
    Ipv4Mask SubnetMask = "255.255.255.0";
    Ipv4InterfaceContainer interface;
    AssignIP (NetworkAddress, SubnetMask, devices, interface);

    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    int port = 80; 
    Recv (data, nodes, tid, port);
    Send (data, nodes, tid, port, interface);
}

void
AssignIP (Ipv4Address NetworkAddress, Ipv4Mask SubnetMask, NetDeviceContainer &devices, 
          Ipv4InterfaceContainer &interface)
{
    Ipv4AddressHelper ipv4;
    NS_LOG_INFO ("Assigning IP Address.");
    ipv4.SetBase (NetworkAddress, SubnetMask);
    interface = ipv4.Assign (devices);
}

void 
Recv (Data &data, NodeContainer &nodes, TypeId &tid, int port)
{
    Ptr<Socket> recvSink = Socket::CreateSocket (nodes.Get(data.sinkNode), tid);
    InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), port);
    recvSink -> Bind (local);
    recvSink -> SetRecvCallback (MakeCallback (&ReceivePacket));
}

void
Send (Data &data, NodeContainer &nodes, TypeId &tid, int port, Ipv4InterfaceContainer &interface)
{
    Ptr<Socket> source = Socket::CreateSocket (nodes.Get(data.sourceNode), tid);
    InetSocketAddress remote = InetSocketAddress (interface.GetAddress (data.sinkNode, 0), port);
    source -> Connect (remote);
}

void
tracing (Data &data, OlsrHelper &olsr, YansWifiPhyHelper &wifiPhy, NetDeviceContainer &devices)
{
    if (data.tracing)
    {
        AsciiTraceHelper ascii;
        wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("trace/wifi-simple-adhoc-grid.tr"));
        wifiPhy.EnablePcap ("trace/wifi-simple-adhoc-grid", devices);
        // Trace routing tables
        Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("trace/wifi-simple-adhoc-grid.routes", std::ios::out);
        olsr.PrintRoutingTableAllEvery (Seconds (2), routingStream);
        Ptr<OutputStreamWrapper> neighborStream = Create<OutputStreamWrapper> ("trace/wifi-simple-adhoc-grid.neighbors", std::ios::out);
        olsr.PrintNeighborCacheAllEvery (Seconds (2), neighborStream);
    }
}

void 
ReceivePacket (Ptr<Socket> socket)
{
    while (socket->Recv())
    {
        NS_LOG_UNCOND ("Received one packet!");
    }
}

static void
GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval)
{
    if (pktCount > 0)
    {
        socket -> Send (Create<Packet> (pktSize));
        Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, pktCount-1, pktInterval);
    }
    else
    {
        socket -> Close();
    }
    
}
