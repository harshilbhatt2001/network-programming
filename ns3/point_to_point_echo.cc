/**
 * @author Harshil Bhatt
 * @create date 2020-06-11 22:28:52
 * @desc 
 *      Place the file in ./scratch/ directory and build using
 *      >> ./waf --run scratch/<filename>
 *      Run the following for user input via command line
 *      >> ./waf --run "scratch/point_to_point_echo --PrintHelp" 
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("PointToPoint");

int
main (int argc, char *argv[])
{
    int MaxPackets = 1;
    int Interval = 1;
    int PacketSize = 1024;
    bool netanim = false;
    bool tracing = false;
    int port = 9090;

    CommandLine cmd;
    cmd.AddValue("MaxPackets", "No of packets sent", MaxPackets);
    cmd.AddValue("Interval", "Interval between each transmission (s)", Interval);
    cmd.AddValue("PacketSize", "Size of Packets (bytes)", PacketSize);
    cmd.AddValue("netanim", "Enable netanim simulation", netanim);
    cmd.AddValue("tracing", "Enable Pcap tracing", tracing);
    cmd.AddValue("port", "Select port", port);
    cmd.Parse (argc, argv);

    // sets the time resolution to one nanosecond
    Time::SetResolution (Time::NS);

    /* 
    enable two logging components that are built into the Echo Client and Echo Server applications
    */

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);


    // Create Nodes
    NodeContainer nodes;
    nodes.Create(2);

    /* 
    PointToPointHelper is primed and ready to make PointToPointNetDevices and 
    wire PointToPointChannel objects between them. 
    */
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    /*
    We need to have a list of all of the NetDevice objects that are created, 
    so we use a NetDeviceContainer to hold them
    */
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Install Internet Stack (TCP, UDP, IP...)
    InternetStackHelper stack;
    stack.Install(nodes);

    // Associate nodes with IP addresses.
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);


    // Set up a UDP echo server application on one of the nodes on port 9
    UdpEchoServerHelper echoServer (port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));


    // Set up a UDP echo client application on the other nodes on same port 9
    UdpEchoClientHelper echoClient (interfaces.GetAddress(1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(MaxPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(Interval)));
    echoClient.SetAttribute("PacketSize", UintegerValue(PacketSize));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));


    if (tracing)
    {
        pointToPoint.EnablePcapAll("trace/peer-to-peer");
    }


    if (netanim)
    {
        AnimationInterface anim("NetAnim_Simulation_Files/peer-to-peer.xml");
        anim.SetConstantPosition(nodes.Get(0), 40.0, 40,0);
        anim.SetConstantPosition(nodes.Get(1), 80.0, 40,0);
    }

    // Run the simulation
    //Simulator::Stop(Seconds(11.0));
    Simulator::Run();
    Simulator::Destroy();

}