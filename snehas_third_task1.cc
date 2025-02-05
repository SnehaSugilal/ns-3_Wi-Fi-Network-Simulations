
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AdhocNetworkExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    //uint32_t nCsma = 3;
    uint32_t nWifi = 5;
    bool tracing = true;

    CommandLine cmd(__FILE__);
//commenting lines that are not needed from third.cc
    //cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    //cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    //cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.
    if (nWifi > 18)
    {
        std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box"
                  << std::endl;
        return 1;
    }

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

//commenting lines that are not needed from third.cc
    //NodeContainer p2pNodes;
    //p2pNodes.Create(2);
    //PointToPointHelper pointToPoint;
    //pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    //pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    //NetDeviceContainer p2pDevices;
    //p2pDevices = pointToPoint.Install(p2pNodes);
    //NodeContainer csmaNodes;
    //csmaNodes.Add(p2pNodes.Get(1));
    //csmaNodes.Create(nCsma);
    //CsmaHelper csma;
    //csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    //csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
    //NetDeviceContainer csmaDevices;
    //csmaDevices = csma.Install(csmaNodes);

    NodeContainer WiFistaNodes;
    WiFistaNodes.Create(nWifi);
    //NodeContainer wifiApNode = p2pNodes.Get(0);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    WifiHelper wifi;
    
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "RtsCtsThreshold", UintegerValue(100));
    
    NetDeviceContainer WiFiDevices;
    //mac.SetType("ns3::AdhocWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
   
    mac.SetType("ns3::AdhocWifiMac");
    WiFiDevices = wifi.Install(phy, mac, WiFistaNodes);
    
    //Ssid ssid = Ssid("ns-3-ssid");

   //NetDeviceContainer staDevices;
   //mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
   //staDevices = wifi.Install(phy, mac, wifiStaNodes);

   //NetDeviceContainer apDevices;
   //mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
   //apDevices = wifi.Install(phy, mac, wifiApNode);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-90, 90, -90, 90))); //defining the lower left & upper right corners
    mobility.Install(WiFistaNodes);
    
//commenting lines that are not needed from third.cc
   // mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
   // mobility.Install(wifiApNode);

    InternetStackHelper stack;
//commenting lines that are not needed from third.cc    
    // stack.Install(csmaNodes);
    // stack.Install(wifiApNode);
    stack.Install(WiFistaNodes);

    Ipv4AddressHelper address;

    address.SetBase("192.168.1.0", "255.255.255.0");  //set required IP address to 192.168.1.0 
    Ipv4InterfaceContainer WiFiInterfaces;
    WiFiInterfaces = address.Assign(WiFiDevices);

    //address.Assign(staDevices);
    // address.Assign(apDevices);

    UdpEchoServerHelper echoServer(20);  //set the Server port @ 20

    ApplicationContainer serverApps = echoServer.Install(WiFistaNodes.Get(0)); //defining the Server node
    serverApps.Start(Seconds(1.0));  //set the start time 
    serverApps.Stop(Seconds(10.0));  //set the stop time

    UdpEchoClientHelper echoClient(WiFiInterfaces.GetAddress(0), 20);
    echoClient.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));    //set Interval duration of 1 sec
    echoClient.SetAttribute("PacketSize", UintegerValue(512));       //set packet size to 512 bytes

    ApplicationContainer clientApps1 = echoClient.Install(WiFistaNodes.Get(3));  //define client 1 @ node 3
    clientApps1.Start(Seconds(1.0));                                             //set start time 
    clientApps1.Stop(Seconds(3.0));                                              //set stop time

    ApplicationContainer clientApps2 = echoClient.Install(WiFistaNodes.Get(4));  //define client 2 @ node 4
    clientApps2.Start(Seconds(2.0));                                             //set start time 
    clientApps2.Stop(Seconds(6.0));                                              //set stop time
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));

    if (tracing)
    {
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
       // pointToPoint.EnablePcapAll("third");
        phy.EnablePcap("third", WiFiDevices.Get(1));                // this command will generate the pcap files thru Wireshark
      // csma.EnablePcap("third", csmaDevices.Get(0), true);
    }

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
