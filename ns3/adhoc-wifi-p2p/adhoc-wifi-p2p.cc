/**
 * @harshilbhatt2001 [Harshil Bhatt]
 * @create date 2020-06-20 22:33:14
 * @desc 
 *      This script configures two nodes on an 802.11b physical layer, with
 *      802.11b NICs in adhoc mode, and by default, sends one packet of 1000
 *      (application) bytes to the other node.  The physical layer is configured
 *      to receive at a fixed RSS (regardless of the distance and transmit
 *      power); therefore, changing position of the nodes has no effect.
 *     
 *      There are a number of command-line options available to control
 *      the default behavior.  The list of available command-line options
 *      can be listed with the following command:
 *      ./waf --run "wifi-simple-adhoc --help"
 */


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
#include "adhoc-wifi-p2p.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("AdhocWifiP2P");



int
main (int argc, char *argv[])
{
    Data data;
    NodeContainer nodes = init(data, argc, argv);
    NetDeviceContainer devices = SetupWifi(data, nodes);
    SetupMobility (nodes);
    Internet (nodes, devices);
    Ptr<Socket> source = SocketConnect (nodes);

    NS_LOG_UNCOND ("Testing " << data.numpackets  << " packet(s) sent with receiver rss " << data.rss );
    Time interPacketInterval = Seconds (data.interval); 
    Simulator::ScheduleWithContext (source->GetNode()->GetId(),
                                    Seconds(1.0), &GenerateTraffic,
                                    source, data.packetSize, data.numpackets, interPacketInterval);
    
    Simulator::Run();
    Simulator::Destroy();
}


NodeContainer
init (Data data, int argc, char *argv[])
{
    CommandLine cmd;
    cmd.AddValue ("phyMode",    "Wifi Phy Mode",                   data.phyMode);
    cmd.AddValue ("rss",        "Received Signal Gain",            data.rss);
    cmd.AddValue ("packetSize", "Application Packet Size",         data.packetSize);
    cmd.AddValue ("numPackets", "Number of Applicatoin Packets",   data.numpackets);
    cmd.AddValue ("interval",   "Interval between packets",        data.interval);
    cmd.AddValue ("verbose",    "Turn on all Wifi Log Components", data.verbose);
    cmd.Parse (argc, argv);

    

    // Fix non-unicast data rate to be the same as that of unicast
    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(data.phyMode));

    NS_LOG_INFO ("Nodes Created.");
    NodeContainer nodes;
    nodes.Create (2);
    return (nodes);
}

NetDeviceContainer 
SetupWifi (Data &data, NodeContainer nodes)
{
    WifiHelper wifi;
    if (data.verbose)
    {
        wifi.EnableLogComponents();
    }
    wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    wifiPhy.Set("RxGain", DoubleValue(0));
    wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel", "Rss", DoubleValue(data.rss));
    wifiPhy.SetChannel(wifiChannel.Create());

    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                  "DataMode",    StringValue(data.phyMode),
                                  "ControlMode", StringValue(data.phyMode));
    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);
    NS_LOG_INFO ("Wifi Setup.");
    wifiPhy.EnablePcap ("trace/wifi-adhoc-p2p/wifi-simple-adhoc", devices);
    return (devices);
}


void
SetupMobility (NodeContainer nodes)
{
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc -> Add (Vector (0.0, 0.0, 0.0));
    positionAlloc -> Add (Vector (5.0, 0.0, 0.0));
    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    NS_LOG_INFO ("Mobility Setup.");
}

void
Internet (NodeContainer nodes, NetDeviceContainer devices)
{
    InternetStackHelper internet;
    internet.Install (nodes);
    Ipv4Address NetworkAddress = "10.1.1.0";
    Ipv4Mask    SubnetMask     = "255.255.255.0";
    Ipv4InterfaceContainer interface = AssignIP (NetworkAddress, SubnetMask, devices);
    NS_LOG_INFO ("Internet Installed.");
}

Ipv4InterfaceContainer
AssignIP (Ipv4Address NetworkAddress, Ipv4Mask SubnetMask, NetDeviceContainer devices)
{
    Ipv4AddressHelper ipv4;
    ipv4.SetBase(NetworkAddress, SubnetMask);
    Ipv4InterfaceContainer interface = ipv4.Assign(devices);
    NS_LOG_INFO ("Assigned IP addresses.");
    return (interface);
}

Ptr<Socket>
SocketConnect (NodeContainer nodes)
{
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    Ptr<Socket> recvSink = Socket::CreateSocket (nodes.Get(0), tid);
    int port = 80;
    InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), port);
    recvSink -> Bind (local);
    recvSink -> SetRecvCallback (MakeCallback (&ReceivePacket));

    Ptr<Socket> source = Socket::CreateSocket (nodes.Get(1), tid);
    InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), port);
    source -> SetAllowBroadcast (true);
    source -> Connect (remote);
    return (source);
}


void 
ReceivePacket (Ptr<Socket> socket)
{
    while (socket -> Recv())
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

