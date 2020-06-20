/**
 * @author Harshil Bhatt
 * @create date 2020-06-18 23:02:15
 * @desc 
 *      Place the file in ./scratch/ directory and build using
 *      >> ./waf --run scratch/<filename>
 *      Run the following for more info on command-line arguments
 *      >> ./waf --run "scratch/<filename> --help"
 * 
 *      NETWORK TOPOLOGY
 * 
 *      n0    n1    n2    n3
 *      |     |     |     |
 *      ===================
 *              LAN
 *      
 *      UDP flows from n0 to n1 and back
 *      DropTail queues 
 *      Tracing of queues and packet receptions to file "udp-echo.tr"
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

#include "Data.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RealtimeUdpEcho");

void init           (Data &data, int argc, char *argv[]);
void ConfigureNodes (Data &data, NodeContainer &nodes);
void CSMAChannel    (Data &data, NodeContainer &nodes, 
                    NetDeviceContainer &devices, CsmaHelper &csma);
void AssignIp       (Ipv4Address NetworkAddress, Ipv4Mask SubnetMask, 
                    NetDeviceContainer &devices, Ipv4InterfaceContainer &interface);
void SetupApp       (Data &data, NodeContainer &nodes, Ipv4InterfaceContainer &interface);
void Analyse        (Data &data, CsmaHelper &csma);
void Simulate       ();


int
main (int argc, char *argv[])
{

    LogComponentEnable("RealtimeUdpEcho", LOG_LEVEL_INFO);
        

    Data data;
    init (data, argc, argv);

    NodeContainer nodes;
    ConfigureNodes (data, nodes);
    
    NetDeviceContainer devices;
    CsmaHelper csma;
    CSMAChannel (data, nodes, devices, csma);
    
    Ipv4Address NetworkAddress = "10.1.1.0";
    Ipv4Mask SubnetMask        = "255.255.255.0";
    Ipv4InterfaceContainer interface;
    AssignIp (NetworkAddress, SubnetMask, devices, interface);

    SetupApp(data, nodes, interface);

    Analyse (data, csma);

    Simulate();
}


void 
init(Data &data, int argc, char *argv[])
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
    cmd.AddValue("tracing", "Enable tracing", data.trace);
    cmd.AddValue("netanim", "Enable NetAnim", data.netanim);
    cmd.Parse(argc, argv);

}   


void
ConfigureNodes (Data &data, NodeContainer &nodes)
{
    /* Create nodes and install internet */
    NS_LOG_INFO ("Creating nodes.");
    nodes.Create (data.numNodes);
    NS_LOG_INFO ("Installing internet.");
    InternetStackHelper internet;
    internet.Install (nodes);
    

}

void
CSMAChannel (Data &data, NodeContainer &nodes, NetDeviceContainer &devices, CsmaHelper &csma)
{
    NS_LOG_INFO ("Creating Channels.");
    csma.SetChannelAttribute ("DataRate", DataRateValue(DataRate(data.dataRate)));
    csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (data.delay)));
    csma.SetDeviceAttribute ("Mtu", UintegerValue (data.mtu));
    devices = csma.Install(nodes);
}


void 
AssignIp (Ipv4Address NetworkAddress, Ipv4Mask SubnetMask, NetDeviceContainer &devices, 
         Ipv4InterfaceContainer &interface)
{
    NS_LOG_INFO ("Assigning IP address.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase (NetworkAddress, SubnetMask);
    interface = ipv4.Assign(devices);
}

void 
SetupApp (Data &data, NodeContainer &nodes, Ipv4InterfaceContainer &interface)
{
    NS_LOG_INFO ("Creating Apps.");
    uint16_t port = 9;

    UdpEchoServerHelper server (port);
    ApplicationContainer app = server.Install (nodes.Get(1));
    app.Start (Seconds(1.0));
    app.Stop (Seconds(10.0));

    UdpEchoClientHelper client (interface.GetAddress(1), port);
    client.SetAttribute ("MaxPackets", UintegerValue(data.maxPacketCount));
    client.SetAttribute ("Interval", TimeValue(Seconds(data.interval)));
    client.SetAttribute ("PacketSize", UintegerValue(data.packetSize));
    app = client.Install (nodes.Get(0));
    app.Start (Seconds (2.0));
    app.Stop (Seconds(10.0));
}

void
Analyse (Data &data, CsmaHelper &csma)
{
    if (data.trace)
    {
    AsciiTraceHelper ascii;
    csma.EnableAsciiAll (ascii.CreateFileStream ("trace/realtime-udp-echo/realtime-udp-echo.tr"));
    csma.EnablePcapAll ("trace/realtime-udp-echo/realtime-udp-echo", false);
    }

    if (data.netanim)
    {
        AnimationInterface anim ("NetAnim_Simulation_Files/realtime-udp-echo.xml");
    }
}


void
Simulate ()
{
    Simulator::Stop (Seconds (11.0));
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");
}