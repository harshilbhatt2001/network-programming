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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int numPackets = 1;     // Number of packets to transmit
int PacketInterval = 1; // Interval between each transmission
int PacketSize = 1024;  // Size of Packet to transmit

int
main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.AddValue("numPackets", "Number of Packets to Echo", numPackets);
    cmd.AddValue("PacketInterval", "Interval between each transmission", PacketInterval);
    cmd.AddValue("PacketSize", "Size of Packet to transmit", PacketSize);
    cmd.Parse (argc, argv);

    // sets the time resolution to one nanosecond
    Time::SetResolution (Time::NS);

    /* 
    enable two logging components that are built into the Echo Client and Echo Server applications
    */

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    NS_LOG_INFO ("Creating Topology");


    // Create Nodes
    NodeContainer nodes;
    nodes.Create(2);

    /* 
    PointToPointHelper is primed and ready to make PointToPointNetDevices and 
    wire PointToPointChannel objects between them. 
    */
    PointToPointHelper pointToPoint;
    //pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    //pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

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
    UdpEchoServerHelper echoServer (9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));


    // Set up a UDP echo client application on the other nodes on same port 9
    UdpEchoClientHelper echoClient (interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(numPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(PacketInterval)));
    echoClient.SetAttribute("PacketSize", UintegerValue(PacketSize));
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));


    // PCAP tracing
    pointToPoint.EnablePcapAll ("point_to_point_echo");

    // ASCII tracing
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("point_to_point_echo.tr"));

    // Run the simulation
    //Simulator::Stop(Seconds(11.0));
    Simulator::Run();
    Simulator::Destroy();

}