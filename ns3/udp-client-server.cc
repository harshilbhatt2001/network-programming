/**
 * @author Harshil Bhatt
 * @create date 2020-06-15 17:32:03
 * @desc 
 *      Place the file in ./scratch/ directory and build using
 *      >> ./waf --run scratch/<filename>
 *      Run the following for user input via command line
 *      >> ./waf --run "scratch/<filename> --PrintHelp"
 * 
 *      Network Topology
 *         n0    n1
 *         |     |
 *         =======
 *           LAN
 */


#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpClientServer");

int 
main(int argc, char *argv[])
{
    bool useV6 = false;
    bool tracing = false;
    bool netanim = false;
    uint32_t maxPacketCount = 250;
    float time = 0.05;
    Time interPacketInterval = Seconds(time);
    uint32_t MaxPacketSize = 1024;

    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
    
    CommandLine cmd;
    cmd.AddValue("maxPackets", "Number of Packets", maxPacketCount);
    cmd.AddValue("interval", "Interval between Packets (seconds)", time);
    cmd.AddValue("packetSize", "Size of Packets (bytes)", MaxPacketSize);
    cmd.AddValue("tracing", "Enable Pcap tracing", tracing);
    cmd.AddValue("netanim", "Enale NetAnim", netanim);
    cmd.AddValue("useIpv6", "Use Ipv6", useV6);

    cmd.Parse(argc, argv);

    Address serverAddress;
    
    NS_LOG_INFO("Creating nodes.");
    NodeContainer nodes;
    nodes.Create(2);

    InternetStackHelper internet;
    internet.Install(nodes);

    NS_LOG_INFO("Creating Channel.");
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(DataRate(5000000)));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    csma.SetDeviceAttribute("Mtu", UintegerValue(1400));    // mtu -> maximum transmission unit
    NetDeviceContainer devices = csma.Install(nodes);

    NS_LOG_INFO("Assigning Ip addresses.");
    if (useV6)
    {
        Ipv6AddressHelper ipv6;
        ipv6.SetBase("2001:0000:f00d:cafe::", Ipv6Prefix(64));
        Ipv6InterfaceContainer interface = ipv6.Assign(devices);
        serverAddress = Address(interface.GetAddress(1, 1));  
    }
    else
    {
        Ipv4AddressHelper ipv4;
        ipv4.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interface = ipv4.Assign(devices);
        serverAddress = Address(interface.GetAddress(0));
    }

    NS_LOG_INFO("Creating Applications.");
    uint16_t port = 4000;
    UdpServerHelper server (port);
    ApplicationContainer apps = server.Install(nodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    UdpClientHelper client (serverAddress, port);
    client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
    client.SetAttribute("Interval", TimeValue(interPacketInterval));
    client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
    apps = client.Install(nodes.Get(1));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(10.0));   

    if (tracing)
    {
        csma.EnablePcapAll("trace/udp-client-server/udp-client-server");
    }

    // Doesn't Work
    if (netanim)
    {
        AnimationInterface anim("NetAnim_Simulation_Files/udp-client-server.xml");
        anim.SetConstantPosition(nodes.Get(0), 40.0, 40,0);
        anim.SetConstantPosition(nodes.Get(1), 80.0, 40,0);
    }

    NS_LOG_INFO("Running Simulation");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");
    return 0;   
}