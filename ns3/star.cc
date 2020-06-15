/**
 * @author Harshil Bhatt
 * @create date 2020-06-15 21:15:18
 * @desc 
 *      Place the file in ./scratch/ directory and build using
 *      >> ./waf --run scratch/<filename>
 *      Run the following for user input via command line
 *      >> ./waf --run "scratch/<filename> --PrintHelp"
 * 
 *      Network topology (default)
 *          n2 n3 n4   
 *           \ | /     
 *            \|/      
 *       n1--- n0---n5 
 *            /|\      
 *           / | \     
 *          n8 n7 n6   
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Star-Topo");

int 
main (int argc, char *argv[])
{
    Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(137));
    Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("14kb/s"));

    bool tracing = false;
	bool netanim = false;
    uint32_t nSpokes = 8;
    
    CommandLine cmd;
	cmd.AddValue("tracing", "Enable Pcap tracing", tracing);
	cmd.AddValue("netanim", "Enable NetAnim", netanim);
    cmd.AddValue("nSpokes", "Number of nodes in star", nSpokes);
    cmd.Parse(argc, argv);

    NS_LOG_INFO("Building Star Topology.");
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    PointToPointStarHelper star (nSpokes, pointToPoint);
    NS_LOG_INFO("Finished Builing Star Topology.");

    NS_LOG_INFO("Installing Internet on All Nodes.");
    InternetStackHelper internet;
    star.InstallStack(internet);
    NS_LOG_INFO("Finished Installing Internet.");

    NS_LOG_INFO("Assigning IP address.");
    star.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0", "255.255.255.0"));
    NS_LOG_INFO("Finished Assigning IP address.");


    // Create packet sink on hub to recieve packets
    NS_LOG_INFO("Creating Applications.");
    uint16_t port = 4000;
    Address hubLocalAddress = (InetSocketAddress(Ipv4Address::GetAny(), port));
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
    ApplicationContainer hubApp = packetSinkHelper.Install(star.GetHub());
    hubApp.Start(Seconds(1.0));
    hubApp.Stop(Seconds(10.0)); 

    // Create OnOff app to send TCP to hub 
    OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address());
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer spokeApps;
    for (uint32_t i = 0; i < star.SpokeCount(); ++i)
    {
        AddressValue remoteAddress (InetSocketAddress(star.GetHubIpv4Address(i), port));
        onOffHelper.SetAttribute("Remote", remoteAddress);
        spokeApps.Add(onOffHelper.Install(star.GetSpokeNode(i)));
    }
    spokeApps.Start(Seconds(1.0));
    spokeApps.Stop(Seconds(1.0));
    NS_LOG_INFO("Finished Creating Applications.");

    NS_LOG_INFO("Enabling Global Static Routing.");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_INFO("Finished Enabling Global Static Routing.");

	if (tracing)
	{
    	pointToPoint.EnablePcapAll ("trace/star/star");
	}

	// Not Working
	if (netanim)
	{
		AnimationInterface anim("NetAnim_Simulation_Files/star/star.xml");
	}

    NS_LOG_INFO ("Run Simulation.");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");
    return 0;
}