/**
 * @author Harshil Bhatt
 * @create date 2020-06-12 20:40:14
 * @desc
 *      Place the file in ./scratch/ directory and build using
 *      >> ./waf --run scratch/<filename>
 *      Run the following for user input via command line
 *      >> ./waf --run "scratch/<filename> --PrintHelp"
 * 
 *      Default Network Topology:
 *        Wifi 10.1.3.0
 *                      AP
 *      *    *    *    *
 *      |    |    |    |    10.1.1.0
 *      n5   n6   n7   n0 -------------- n1   n2   n3   n4  <- SERVER
 *           CLIENT      point-to-point  |    |    |    |
 *                                       ================
 *                                         LAN 10.1.2.0
 * 
 *      STA nodes (N5, N6, N7) are MOBILE. 
 * 
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

int numPackets = 1;     // Number of packets to transmit
int PacketInterval = 1; // Interval between each transmission
int PacketSize = 1024;  // Size of Packet to transmit
int SimDuration = 10;   // Duration of Simulation

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WirelessNetworkTopology");

void
CourseChange (std::string context, Ptr<const MobilityModel> model)
{
    Vector position = model->GetPosition ();
    NS_LOG_UNCOND (context <<
        " x = " << position.x << ", y = " << position.y);
}

int 
main (int argc, char *argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nWifi = 3;
    bool tracing = false;
    bool netanim = false;
    
    CommandLine cmd;
    cmd.AddValue("nCsma", "Number of extra \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("nWifi", "Number of Wifi STA devices", nWifi);
    cmd.AddValue("verbose", "Tell echo application to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);
    cmd.AddValue("simDuration", "Duration of Simulation", SimDuration);
    cmd.AddValue("netanim", "Enable netanim simulation", netanim);


    cmd.Parse(argc, argv);

    if (nWifi > 18)
    {
        std::cout << "nWifi should be less than 18 otherwise grid layout exceeds bounding box" << std::endl;
        return 1; 
    }

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }


    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);


    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));
    csmaNodes.Create(nCsma);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nWifi);
    NodeContainer wifiApNode = p2pNodes.Get(0);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();

    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevices;
    apDevices = wifi.Install(phy, mac, wifiApNode);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",       DoubleValue(0.0),
                                  "MinY",       DoubleValue(0.0),
                                  "DeltaX",     DoubleValue(5.0),
                                  "DeltaY",     DoubleValue(10.0),
                                  "GridWidth",  UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(wifiStaNodes);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiApNode);

    InternetStackHelper stack;
    stack.Install(csmaNodes);
    stack.Install(wifiApNode);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign (csmaDevices);

    address.SetBase ("10.1.3.0", "255.255.255.0");
    address.Assign (staDevices);
    address.Assign (apDevices);    

    UdpEchoServerHelper echoServer (9);

    ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));

    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (numPackets));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (PacketSize));

    ApplicationContainer clientApps =
    echoClient.Install (wifiStaNodes.Get (nWifi - 1));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    /*
    The simulation runs indefinitely, hence we need to stop it.
    This is because we asked the wireless access point to generate beacons. 
    It will generate beacons forever, and this will result in simulator events 
    being scheduled into the future indefinitely
    */
    Simulator::Stop (Seconds (SimDuration));

    if (tracing)
    {
        pointToPoint.EnablePcapAll ("trace/wireless_topo/p2p");
        phy.EnablePcap  ("trace/wireless_topo/wireless", apDevices.Get (0));
        csma.EnablePcap ("trace/wireless_topo/csma", csmaDevices.Get (0), true);
    }

    std::ostringstream oss;
    oss <<
    "/NodeList/" << wifiStaNodes.Get (nWifi - 1)->GetId () <<
    "/$ns3::MobilityModel/CourseChange";
    Config::Connect (oss.str (), MakeCallback (&CourseChange));
    
    if (tracing)
    {    
        AnimationInterface anim("NetAnim_Simulation_Files/wireless_topo.xml");
        anim.SetConstantPosition(csmaNodes.Get(0), 10.0, 10.0);
        anim.SetConstantPosition(csmaNodes.Get(1), 10.0, 30.0);
        anim.SetConstantPosition(csmaNodes.Get(2), 10.0, 50.0);
        anim.SetConstantPosition(csmaNodes.Get(3), 10.0, 70.0);

        anim.SetConstantPosition(wifiStaNodes.Get(0), 30.0, 10.0);
        anim.SetConstantPosition(wifiStaNodes.Get(1), 50.0, 10.0);
        anim.SetConstantPosition(wifiStaNodes.Get(2), 70.0, 10.0);

        anim.SetConstantPosition(wifiApNode.Get(0), 100.0, 100.0);
    }

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
