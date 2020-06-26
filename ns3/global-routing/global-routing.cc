/**
 * @harshilbhatt2001 [Harshil Bhatt]
 * @create date 2020-06-21 21:52:55
 * @desc
 *		 Network topology
 *		
 *		  n0
 *		     \ 5 Mb/s, 2ms
 *		      \          1.5Mb/s, 10ms
 *		       n2 -------------------------n3
 *		      /
 *		     / 5 Mb/s, 2ms
 *		   n1
 *		
 *		 - all links are point-to-point links with indicated one-way BW/delay
 *		 - CBR/UDP flows from n0 to n3, and from n3 to n1
 *		 - FTP/TCP flow from n0 to n3, starting at time 1.2 to time 1.35 sec.
 *		 - UDP packet size of 210 bytes, with per-packet interval 0.00375 sec.
 *		   (i.e., DataRate of 448,000 bps)
 *		 - DropTail queues 
 *		 - Tracing of queues and packet receptions to file "simple-global-routing.tr"
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>


#include "global-routing.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("GlobalRouting");



int 
main (int argc, char *argv[])
{
	Data data;
	NodeContainer nodes = Init (data, argc, argv);

	NodeContainer n0n2 = CreateContainer (nodes, 0, 2, data);
	NodeContainer n1n2 = CreateContainer (nodes, 1, 2, data);
	NodeContainer n3n2 = CreateContainer (nodes, 3, 1, data);

	Ipv4InterfaceContainer i0i2 = Internet (nodes, n0n2, "10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = Internet (nodes, n1n2, "10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i2 = Internet (nodes, n3n2, "10.1.3.0", "255.255.255.0");

	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	OnOffApp(data, nodes, 0, 3, i3i2, 1.0, 10.0);
	OnOffApp(data, nodes, 3, 1, i1i2, 1.0, 10.0);

	FlowMonitorHelper flowmonHelper;
	if (data.EnableFlowMonitor)
	{
		flowmonHelper.InstallAll ();
	}

	Simulator::Run();

	if (data.EnableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("simple-global-routing.flowmon", false, false);
    }

	Simulator::Destroy();
}


NodeContainer
Init (Data data, int argc, char *argv[])
{
	Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (data.packetSize));
	Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (data.dataRate));

	CommandLine cmd;
	cmd.AddValue ("flowmonitor", "Enable Flow Monitor", data.EnableFlowMonitor);
	cmd.Parse(argc, argv);

	NS_LOG_INFO ("Creating Nodes.");
	NodeContainer nodes;
	nodes.Create (4);
	return (nodes);
}


NodeContainer
CreateContainer (NodeContainer nodes, int a, int b, Data data)
{
	if ((a >= data.numNodes) || (b >= data.numNodes) || (a=b))
	{
		NS_LOG_ERROR ("Cannot create node container.");
	} 
	NodeContainer nanb = NodeContainer (nodes.Get(a), nodes.Get(b));
	return nanb;
}


NetDeviceContainer
CreateChannel (NodeContainer nanb, std::string dataRate, std::string delay)
{
	static bool firstCall = true;
	if (firstCall)
	{
		NS_LOG_INFO ("Creating Channels.");
		firstCall = false;
	}
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue(dataRate));
	p2p.SetChannelAttribute ("Delay", StringValue(delay));
	NetDeviceContainer dadb = p2p.Install (nanb);
	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll (ascii.CreateFileStream ("trace/global-routing/simple-global-routing.tr"));
	p2p.EnablePcapAll ("trace/global-routing/simple-global-routing");
	return dadb;
}


Ipv4InterfaceContainer
AssignIP (NetDeviceContainer dadb, Ipv4Address NetworkAddress, Ipv4Mask SubnetMask)
{
	static bool firstCall = true;
	if (firstCall)
	{
		NS_LOG_INFO ("Assigning IP Addresses");
		firstCall = false;
	}
	Ipv4AddressHelper ipv4;
	ipv4.SetBase (NetworkAddress, SubnetMask);
	Ipv4InterfaceContainer iaib = ipv4.Assign(dadb);
	return iaib;
}


Ipv4InterfaceContainer
Internet (NodeContainer nodes, NodeContainer nanb, Ipv4Address NetworkAddress, Ipv4Mask SubnetMask)
{
	static bool firstCall = true;
	if (firstCall)
	{
		InternetStackHelper internet;
		internet.Install (nodes);
		firstCall = false;
	}
	
	NetDeviceContainer dadb = CreateChannel (nanb, "5Mbps", "2ms");

	Ipv4InterfaceContainer iaib = AssignIP (dadb, NetworkAddress, SubnetMask);

	return iaib;
}


void
OnOffApp (Data data, NodeContainer nodes, int numSource, int numSink, Ipv4InterfaceContainer iaib, double appStart, double appStop)
{
	static bool firstCall = true;
	if (firstCall)
	{
		NS_LOG_INFO ("Creating Apps.");
		firstCall = false;
	}
	
	uint16_t port = 9;
	OnOffHelper onoff ("ns3::UdpSocketFactory",
						Address (InetSocketAddress (iaib.GetAddress(0), port)));
	onoff.SetConstantRate (DataRate (data.dataRate));
	ApplicationContainer apps = onoff.Install (nodes.Get(numSource));
	apps.Start (Seconds(appStart));						
	apps.Start (Seconds(appStop));						
	
	PacketSinkHelper sink ("ns3::UdpSocketFactory", 
							Address (InetSocketAddress (Ipv4Address::GetAny(), port)));
	apps = sink.Install (nodes.Get(numSink));
	apps.Start (Seconds(appStart));						
	apps.Start (Seconds(appStop));						
}





