# ns-3_Wi-Fi-Network_Simulations
This project focuses on the simulation of Wi-Fi Networks, explores the Ad-Hoc &amp; Infrastructure Modes in Wireless Local Area Networks. 

Before starting, you need to read Section 7.3 in the ns-3 Tutorial*.

## Part 1: Wireless Local Area Network – Ad-hoc Mode 

Define a Wireless Local Area Network (WLAN) operating in Ad-hoc Mode with 5 nodes. Nodes move by following a 2D random walk in a rectangular area defined by the lower-left corner (x=-90 m, y=-90 m) and the upper-right corner (x=90 m, y=90 m).

TIP: Change third.cc so your network only has Wifi nodes of type ns3::AdhocWiFiMac.

Consider the following specifications:
- Channel: Default wireless channel in ns-3
- Physical Layer: Default parameters in IEEE 802.11ac standard
- Link Layer: Standard MAC without quality of service control   o Remember: the network should operate in ad-hoc mode
- Network Layer: Standard IPv4, Address range: 192.168.1.0/24 (Assume that all the nodes behave as ideal routers and can exchange their routing tables in the background)
- Transport Layer: UDP
- Application Layer: UDP Echo Server at Node 0 ▪ Listening on port 20
  - UDP Echo Client at Node 3: ▪ Sends 2 UDP Echo packets to the server at times 1s and 2s
  - UDP Echo Client at Node 4: ▪ Sends 2 UDP Echo packets to the server at times 2s and 4s
  - Packet size: 512 bytes
- Additional parameters: Set up a packet tracer ONLY on Node 1


### References:
- ns-3 Tutorial*: https://www.nsnam.org/docs/tutorial/html/index.html
- WiFi model in ns3, visit the user documentation for the WiFi module: https://www.nsnam.org/docs/release/3.42/models/html/wifi.html
- API documentation page for the WiFi model, visit ns-3 Doxygen: https://www.nsnam.org/docs/doxygen/df/d5e/group__wifi.html
