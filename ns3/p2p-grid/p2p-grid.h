/**
 * @harshilbhatt2001 [Harshil Bhatt]
 * @create date 2020-06-22 23:03:55
 * 
 *      TODO:
 *          1. Add Ipv6 support
 *     
 */

#ifndef P2P_HELPER_H
#define P2P_HELPER_H


#include <vector>

#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv6-interface-container.h"
#include "ns3/net-device-container.h"


using namespace ns3;

class PointToPointGrid_Helper 
{
public:
    /**
     * 
     * \param nRows total number of rows in grid
     * 
     * \param nCols total number of columns in grid
     * 
     * \param pointToPoint PointToPointHelper which is used
     *                     connect all the nodes together in the grid.
     */
    PointToPointGrid_Helper (uint32_t numRows,
                             uint32_t numCols,
                             PointToPointHelper pointToPoint);

    ~PointToPointGrid_Helper();


    /**
     * 
     * \param row row address of desired node
     * 
     * \param col column address of desired node
     * 
     * \returns Pointer to specified node
     */

    Ptr<Node> GetNode (uint32_t row, uint32_t col);


    /**
     * Returns Ipv4 address at node specified by (row, col)
     * address
     * 
     * \param row row address of desired node
     * 
     * \param col column address of desired node
     * 
     * \returns Ipv4 Adress of one of the interfaces
     */
    Ipv4Address GetIpv4Adress (uint32_t row, uint32_t col);

    
    /**
     * \param internet InternetStackHelper used to install
     *                 internet on every node
     */
    void InstallInternet (InternetStackHelper internet);
    

    /**
     *  Assign Ipv4 Address to row and column interfaces.
     * 
     * \param rowIp Ipv4AdressHelper to install assign Ipv4 adress
     *              to row interface
     * 
     * \param colIp Ipv4AdressHelper to install assign Ipv4 adress
     *              to column interface
     */
    void AssignIpv4Adress (Ipv4AddressHelper rowIp, Ipv4AddressHelper colIp);

    /**
     * Sets up the node canvas locations for every node in the grid,
     * needed for use with the animation interface
     * 
     * \param ulx upper left x value
     * \param uly upper left y value
     * \param lrx lower right x value
     * \param lry lower right y value
     */
    void BoundingBox (double ulx, double uly, double lrx, double lry);

private:
    uint32_t m_xSize;   // no of rows
    uint32_t m_ySize;   // no of cols
    std::vector<NetDeviceContainer> m_rowDevices;   // NetDevices in row
    std::vector<NetDeviceContainer> m_colDevices;   // NetDevices in col
    std::vector<Ipv4InterfaceContainer> m_rowInterfaces;    // Ipv4 interfaces of row
    std::vector<Ipv4InterfaceContainer> m_colInterfaces;    // Ipv4 interfaces of col
    std::vector<NodeContainer> m_nodes;    // all nodes in grid 



};



#endif /* P2P_HELPER_H */