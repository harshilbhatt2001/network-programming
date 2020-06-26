/**
 * @harshilbhatt2001 [Harshil Bhatt]
 * @create date 2020-06-22 22:47:06
 * @desc 
 *      Point to Point network in a Grid Topology
 */


#include "p2p-grid.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/string.h"
#include "ns3/vector.h"
#include "ns3/log.h"
#include "ns3/ipv6-address-generator.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("P2P-Grid");

PointToPointGrid_Helper::PointToPointGrid_Helper (uint32_t nRows, uint32_t nCols, 
                                                  PointToPointHelper pointToPoint)
    : m_xSize (nCols), m_ySize (nRows)
{
    if (m_xSize < 1 || m_ySize < 1)
    {
        NS_FATAL_ERROR ("Need more nodes for grid.");
    }

    InternetStackHelper internet;

    for (uint32_t y = 0; y < nRows; y++)
    {
        NodeContainer rowNodes;
        NetDeviceContainer rowDevices;
        NetDeviceContainer colDevices;

        for (uint32_t x = 0; x < nCols; x++)
        {
            rowNodes.Create(1);
            
            // Install p2p links across the row
            if (x > 0)
            {
                rowDevices.Add (pointToPoint.Install (rowNodes.Get(x-1), rowNodes.Get(x)));
            }
            // Install vertical p2p links 
            if (y > 0)
            {
                colDevices.Add (pointToPoint.Install (m_nodes.at(y-1).Get(x), rowNodes.Get(x)));
            }
        }
        
        m_nodes.push_back (rowNodes);
        m_rowDevices.push_back (rowDevices);

        if (y > 0)
        {
            m_colDevices.push_back (colDevices);
        }
    }
}



PointToPointGrid_Helper::~PointToPointGrid_Helper ()
{
}

void
PointToPointGrid_Helper::InstallInternet (InternetStackHelper internet)
{
    for (uint32_t i = 0; i < m_nodes.size(); ++i)
    {
        NodeContainer rowNodes = m_nodes[i];
        for (uint32_t j = 0; j < rowNodes.GetN(); ++j)
        {
            internet.Install (rowNodes.Get(j));
        }
    }
}

void
PointToPointGrid_Helper::AssignIpv4Adress (Ipv4AddressHelper rowIp, Ipv4AddressHelper colIp)
{
    for (uint32_t i = 0; i < m_nodes.size(); ++i)
    {
        Ipv4InterfaceContainer rowInterfaces;
        NetDeviceContainer rowContainer = m_rowDevices[i];
        for (uint32_t j = 0; j < rowContainer.GetN(); ++j)
        {
            rowInterfaces.Add (rowIp.Assign (rowContainer.Get(j)));
            rowInterfaces.Add (rowIp.Assign (rowContainer.Get(j+1)));
            rowIp.NewNetwork();
        }
        m_rowInterfaces.push_back (rowInterfaces);
    }

    for (uint32_t i = 0; i < m_nodes.size(); ++i)
    {
        Ipv4InterfaceContainer colInterfaces;
        NetDeviceContainer colContainer = m_colDevices[i];
        for (uint32_t j = 0; j < colContainer.GetN(); ++j)
        {
            colInterfaces.Add (colIp.Assign (colContainer.Get(j)));
            colInterfaces.Add (colIp.Assign (colContainer.Get(j+1)));
            colIp.NewNetwork();
        }
        m_colInterfaces.push_back (colInterfaces);
    }
}


void
PointToPointGrid_Helper::BoundingBox (double ulx, double uly,
                                      double lrx, double lry)
{
    double xDist;
    double yDist;
    if (lrx > ulx)
    {
        xDist = lrx - ulx;
    }
    else
    {
        xDist = ulx - lrx;
    }
    if (lry > uly)
    {
        yDist = lry - uly;
    }
    else
    {
        yDist = uly - lry;
    }

    double xAdder = xDist / m_xSize;
    double yAdder = yDist / m_ySize;
    double yLoc   = yDist / 2;
    for (uint32_t i = 0; i < m_ySize; ++i)
    {
        double xLoc = xDist / 2;
        for (uint32_t j = 0; j < m_xSize; ++j)
        {
            Ptr<Node> node = GetNode(i, j);
            Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
            if (loc==0)
            {
                loc = CreateObject<ConstantPositionMobilityModel> ();
                loc->AggregateObject (loc);
            }
            Vector locVector (xLoc, yLoc, 0);
            loc->SetPosition (locVector);

            xLoc += xAdder;
        }
        yLoc += yAdder;
    }
}    

Ptr<Node> 
PointToPointGrid_Helper::GetNode (uint32_t row, uint32_t col)
{
    if (row > m_nodes.size() - 1 ||
        col > m_nodes.at(row).GetN() - 1)
        {
            NS_FATAL_ERROR ("Selected Node out of bounds.");
        }
    return ((m_nodes.at (row)).Get (col));
}

Ipv4Address
PointToPointGrid_Helper::GetIpv4Adress (uint32_t row, uint32_t col)
{
    if (row > m_nodes.size() - 1 ||
        col > m_nodes.at(row).GetN() - 1)
        {
            NS_FATAL_ERROR ("Selected Node out of bounds.");
        }
    
    if (col == 0)
    {
        return (m_rowInterfaces.at(row)).GetAddress(0);
    }
    else
    {
        return (m_rowInterfaces.at(row)).GetAddress((2*col)-1);
    }
}
