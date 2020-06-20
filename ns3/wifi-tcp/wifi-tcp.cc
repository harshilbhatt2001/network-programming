/**
 * @author Harshil Bhatt
 * @create date 2020-06-17 00:36:53
 * @desc
 *      Place the file in ./scratch/ directory and build using
 *      >> ./waf --run scratch/<filename>
 *      Run the following for more info on command-line arguments
 *      >> ./waf --run "scratch/<filename> --help"
 * 
 *      This is a simple example to test TCP over 802.11n (with MPDU aggregation enabled).
 * 
 *      Network topology:
 *
 *        Ap    STA
 *        *      *
 *        |      |
 *        n1     n2
 *
 *      In this example, an HT station sends TCP packets to the access point.
 *      We report the total throughput received during a window of 100ms.
 *      The user can specify the application data rate and choose the variant
 *      of TCP i.e. congestion control algorithm to use.
 * 
 *      ERRORS:
 *   FIXED  1. TcpVariant not working properly.
 *   FIXED  2. Mobility Model not working
 *          3. NetAnim Simulation not working
 *          4. Throughput = ZERO
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

#include "UserData.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("wifi-tcp");

void CalculateThroughput();
void init              (UserData &userData, int argc, char *argv[]);
void SetupTCP          (std::string &tcpVariant, TypeId &tcpTid, uint32_t &payloadSize);
void SetupWIFI         (std::string &phyRate, WifiMacHelper &wifiMac, 
                       WifiHelper &wifiHelper, YansWifiPhyHelper &wifiPhy);
void ConfigureNodes    (Ptr<Node> &apWifiNode, Ptr<Node> &staWifiNode, NetDeviceContainer &apDevice, 
                       NetDeviceContainer &staDevice, WifiHelper &wifiHelper, WifiMacHelper &wifiMac, 
                       YansWifiPhyHelper &wifiPhy, NodeContainer &networkNodes);
void SetupMobility     (Ptr<Node> &apWifiNode, Ptr<Node> &staWifiNode);
void InstallInternet   (Ipv4Address networkAddress, Ipv4Mask SubnetMask, NodeContainer &networkNodes, 
                       NetDeviceContainer &apDevice, NetDeviceContainer &staDevice, Ipv4InterfaceContainer &apInterface,
                       Ipv4InterfaceContainer &staInterface);
void TcpRxAP           (Ptr<Node> &apWifiNode, int port, ApplicationContainer &sinkApp);
void TcpTxSta          (Ptr<Node> &staWifiNode, int port, Ipv4InterfaceContainer &apInterface, 
                       uint32_t &payloadSize, std::string &dataRate, ApplicationContainer &serverApp);
void StartApp          (ApplicationContainer &sinkApp, ApplicationContainer &serverApp);
void tracing           (UserData &userData, YansWifiPhyHelper &wifiPhy, NetDeviceContainer &apDevice, 
                       NetDeviceContainer &staDevice);
void AverageThroughput (UserData &userData);




Ptr<PacketSink> sink;
uint64_t lastTotalRx = 0;



int 
main (int argc, char *argv[])
{   
    UserData userData;
    init(userData, argc, argv);

    TypeId tcpTid;
    SetupTCP (userData.tcpVariant, tcpTid, userData.payloadSize);
    WifiMacHelper wifiMac;
    WifiHelper wifiHelper;
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    SetupWIFI (userData.phyRate, wifiMac, wifiHelper, wifiPhy);
    Ptr<Node> apWifiNode;
    Ptr<Node> staWifiNode;
    NodeContainer networkNodes;
    NetDeviceContainer apDevice; 
    NetDeviceContainer staDevice;
    ConfigureNodes(apWifiNode, staWifiNode, apDevice, staDevice, wifiHelper, wifiMac, wifiPhy, networkNodes);
    SetupMobility(apWifiNode, staWifiNode);
    Ipv4InterfaceContainer apInterface; 
    Ipv4InterfaceContainer staInterface;
    InstallInternet("10.1.1.0", "255.255.255.0", networkNodes, apDevice, staDevice, apInterface, staInterface);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    int port = 12345; 
    ApplicationContainer sinkApp;
    ApplicationContainer serverApp;
    TcpRxAP (apWifiNode, port, sinkApp);
    TcpTxSta (staWifiNode, port, apInterface, userData.payloadSize, userData.dataRate, serverApp);
    
    StartApp (sinkApp, serverApp);
    Simulator::Schedule (Seconds(1.1), &CalculateThroughput);
    
    tracing(userData, wifiPhy, apDevice, staDevice);
    
    Simulator::Stop (Seconds (userData.SimulationTime + 1));
    Simulator::Run();
    // Simulator::Destroy(); is called in AverageThroughput()
    AverageThroughput(userData);
    return 0;
}


void
CalculateThroughput ()
{
    Time now = Simulator::Now ();
    double cur = (sink -> GetTotalRx () - lastTotalRx) * (double) 8 / 1e5; // Convert Application RX Packets to MBits.
    std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
    lastTotalRx = sink -> GetTotalRx ();
    Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}


void 
AverageThroughput(UserData &userData)
{
    double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * userData.SimulationTime));
    
    Simulator::Destroy();
    if (averageThroughput < 50)
    {
        NS_LOG_ERROR ("Obtained throughput is not in the expected boundaries!");
        exit (1);
    }
    std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;
}


void 
init(class UserData &userData, int argc, char *argv[])
{
    
    /**
     * payloadSize    = 1472
     * dataRate       = "100Mbps"
     * tcpVariant     = "TcpNewReno"
     * phyRate        = "HtMcs7"
     * SimulationTime = 10
     * pcaptracing    = false
     * netanim        = false
     */

    CommandLine cmd;
    cmd.AddValue("payloadSize",    "Payload size in bytes",      userData.payloadSize);
    cmd.AddValue("dataRate",       "Application Data Rate",      userData.dataRate);
    cmd.AddValue ("tcpVariant",    "Transport protocol to use: TcpNewReno, "
                  "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                  "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat ", userData.tcpVariant);
    cmd.AddValue("phyRate",        "Physical layer bitrate",     userData.phyRate);
    cmd.AddValue("SimulationTime", "Simulation Time in seconds", userData.SimulationTime);
    cmd.AddValue("pcaptracing",    "Enable pcap tracing",        userData.pcaptracing);
    cmd.AddValue("netanim",        "Enable NetAnim",             userData.netanim);
    cmd.Parse(argc, argv);
    std::cout << "Init Done" << std::endl;
}     


void 
SetupTCP(std::string &tcpVariant, TypeId &tcpTid, uint32_t &payloadSize)
{   
    tcpVariant = std::string ("ns3::") + tcpVariant;

    if (tcpVariant.compare ("ns3::tcpWestwoodPlus") == 0)
    {
        // TcpWestwoodplus is not TypeId name; we need TcpWestwood
        Config::SetDefault ("ns3::TcplProtocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId()));
        // default is WESTWOOD; set to WESTWOODPLUS 
        Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
    else
    {
        TypeId tcpTid;
        NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found.");
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    }

    // Configure TCP options
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));
    std::cout << "TCP Setup" << std::endl; 
}


void
SetupWIFI (std::string &phyRate, WifiMacHelper &wifiMac, WifiHelper &wifiHelper, YansWifiPhyHelper &wifiPhy)
{
    wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);

    /* Setup legacy channel */
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

    /* Setup Physical Later */
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
    wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                        "DataMode", StringValue(phyRate),
                                        "ControlMode", StringValue("HtMcs0"));
    std::cout << "Wifi Setup" << std::endl;
    
}


void
ConfigureNodes (Ptr<Node> &apWifiNode, Ptr<Node> &staWifiNode, NetDeviceContainer &apDevice, NetDeviceContainer &staDevice, WifiHelper &wifiHelper, WifiMacHelper &wifiMac,  YansWifiPhyHelper &wifiPhy, NodeContainer &networkNodes)
{
    networkNodes.Create(2);
    apWifiNode = networkNodes.Get(0);
    staWifiNode = networkNodes.Get(1);

    /* Configure AP */
    Ssid ssid  = Ssid ("network");
    wifiMac.SetType ("ns3::ApWifiMac",
                     "Ssid", SsidValue (ssid));

    apDevice = wifiHelper.Install (wifiPhy, wifiMac, apWifiNode);  

    /* Configure STA */
    wifiMac.SetType ("ns3::StaWifiMac",
                     "Ssid", SsidValue (ssid));

    staDevice = wifiHelper.Install (wifiPhy, wifiMac, staWifiNode);
    std::cout << "Nodes Configured" << std::endl;

}   

void
SetupMobility (Ptr<Node> &apWifiNode, Ptr<Node> &staWifiNode)
{
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

    positionAlloc -> Add (Vector (0.0, 0.0, 0.0));
    positionAlloc -> Add (Vector (1.0, 1.0, 0.0));

    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (apWifiNode);
    mobility.Install (staWifiNode);
    std::cout << "Mobility Setup" << std::endl;
}


void 
InstallInternet (Ipv4Address networkAddress, Ipv4Mask SubnetMask, NodeContainer &networkNodes, NetDeviceContainer &apDevice, NetDeviceContainer &staDevice,
                 Ipv4InterfaceContainer &apInterface, Ipv4InterfaceContainer &staInterface)
{
    InternetStackHelper internet;
    internet.Install(networkNodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase (networkAddress, SubnetMask);

    apInterface = ipv4.Assign(apDevice);
    staInterface = ipv4.Assign(staDevice);
    std::cout << "Internet Installed" << std::endl;
}


void
TcpRxAP (Ptr<Node> &apWifiNode, int port, ApplicationContainer &sinkApp)
{
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), port));
    sinkApp = sinkHelper.Install(apWifiNode);
    sink = StaticCast<PacketSink> (sinkApp.Get(0));
    std::cout << "TCP Receiver Setup at AP" << std::endl;
}


void
TcpTxSta (Ptr<Node> &staWifiNode, int port, Ipv4InterfaceContainer &apInterface, uint32_t &payloadSize, std::string &dataRate,
          ApplicationContainer &serverApp)
{

    OnOffHelper server ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), port));
    server.SetAttribute("PacketSize", UintegerValue(payloadSize));
    server.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
    serverApp = server.Install(staWifiNode);
    std::cout << "TCP Transmitter Setup at STA" << std::endl;
}

void 
StartApp (ApplicationContainer &sinkApp, ApplicationContainer &serverApp)
{
    sinkApp.Start(Seconds(0.0));
    std::cout << "sinkApp Started" << std::endl;
    serverApp.Start(Seconds(1.0));
    std::cout << "serverApp Started" << std::endl;
}


void
tracing (UserData &userData, YansWifiPhyHelper &wifiPhy, NetDeviceContainer &apDevice, NetDeviceContainer &staDevice)
{
    if (userData.pcaptracing)
    {
        wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
        wifiPhy.EnablePcap ("trace/AccessPoint", apDevice);
        wifiPhy.EnablePcap ("trace/Station", staDevice);
        std::cout << "pcap generated" << std::endl;
    }

    if (userData.netanim)
    {
        AnimationInterface anim ("NetAnim_Simulation_Files/wifi-tcp.xml");
        std::cout << "xml generated" << std::endl;
    }

}

