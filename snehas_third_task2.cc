
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
int main (int argc, char *argv[])
{
    uint32_t nWifi = 5;
    bool verbose = true;
    CommandLine cmd;
    cmd.AddValue ("nWifi", "Number of wifi nodes", nWifi);
    cmd.Parse (argc, argv);
    
    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    
    NodeContainer wifistaNodes;  //setting up the stationary nodes
    wifistaNodes.Create (nWifi); 
    NodeContainer wifiApNode;    //defining up the Access Point at Node 1
    wifiApNode.Create (1);

    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", RectangleValue (Rectangle (-90, 90, -90,90))); //defining the lower left & upper right corners
    
    mobility.Install (wifistaNodes);       //installing the mobility at both the stationary nodes
    mobility.Install (wifiApNode);         //and the access point node

    WifiHelper wifi;
    wifi.SetStandard (WIFI_STANDARD_80211ac);

    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel (YansWifiChannelHelper::Default ().Create ());

    WifiMacHelper wifiMac;
    Ssid ssid = Ssid ("EECE5155");  //defining and setting the SSID to EECE5155

    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "RtsCtsThreshold", UintegerValue(100));
    
    // Set up the AP
    wifiMac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
    NetDeviceContainer apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNode);

    // Set up the STA
    wifiMac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid));
    NetDeviceContainer staDevices = wifi.Install (wifiPhy, wifiMac, wifistaNodes);
   
    InternetStackHelper stack;
    stack.Install (wifiApNode);
    stack.Install (wifistaNodes);
    
    Ipv4AddressHelper address;
    address.SetBase ("192.168.2.0", "255.255.255.0"); //set required IP address to 192.168.2.0 
    Ipv4InterfaceContainer staInterfaces;
    staInterfaces = address.Assign (staDevices);
    address.Assign (apDevice);
    
    UdpEchoServerHelper echoServer (21);   //Server listening on port @ 21
    ApplicationContainer serverApps = echoServer.Install (wifistaNodes.Get (0));
    serverApps.Start (Seconds (1.0)); //set the start time 
    serverApps.Stop (Seconds (10.0)); //set the stop time

   //Defining the Clients
   
   //UDP Echo Client on NODE 3
    UdpEchoClientHelper echoClient3 (staInterfaces.GetAddress (0), 21);
    echoClient3.SetAttribute ("MaxPackets", UintegerValue (2));
    echoClient3.SetAttribute ("Interval", TimeValue (Seconds (2.0))); //set Interval duration of 2 secs
    echoClient3.SetAttribute ("PacketSize", UintegerValue (512));     //set packet size to 512 bytes

    ApplicationContainer clientApps3 = echoClient3.Install (wifistaNodes.Get(3));  //define client 3 @ node 3
    clientApps3.Start (Seconds (3.0));                                             //set start time 
    clientApps3.Stop (Seconds (10.0));                                             //set stop time
    
   //UDP Echo Client on NODE 4
    UdpEchoClientHelper echoClient4 (staInterfaces.GetAddress (0), 21);
    echoClient4.SetAttribute ("MaxPackets", UintegerValue (2));
    echoClient4.SetAttribute ("Interval", TimeValue (Seconds (3.0))); //set Interval duration of 3 secs
    echoClient4.SetAttribute ("PacketSize", UintegerValue (512));     //set packet size to 512 bytes
    
    ApplicationContainer clientApps4 = echoClient4.Install (wifistaNodes.Get(4));  //define client 4 @ node 4
    clientApps4.Start (Seconds (2.0));                                             //set start time 
    clientApps4.Stop (Seconds (10.0));                                             //set stop time
    
    // Set up packet tracer ONLY on Node 4 and on the AP
    wifiPhy.EnablePcap ("Node4", staDevices.Get (4));                            //to capture pcap files at Node 4 thru Wireshark Packet Tracer
    wifiPhy.EnablePcap ("AP", apDevice.Get(0));Simulator::Stop (Seconds (10.0)); //to capture pcap files at AP thru Wireshark Packet Tracer
    
    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
