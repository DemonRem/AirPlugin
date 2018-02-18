# HSMP

## Haversine Message Protocol

 **HSMP** stands for **Haversine Message Protocol**, a norm defined by 
 Haversine and used to exchange data messages synchronously and 
 asynchronously between nodes, implementing a CAN style BUS over IP 
 networks, and allowing for simulators to exchange data  with air apps 
 of the Haversine family.
 
 This set of files is used in both apps and flight simulator 
 plugins to implement the data exchange. 
 
 The protocol is in constant evolution and updates are signaled by the
 library version number HSMP_PKT_PROTO_VER defined in HSMPNET.h
 New additions to the protocol generate a new version but versions tend
 to be backwards compatible so for example a 4.2 app can talk to a 4.6 
 plugin although it may not be able to understand all its messages. 
 
 
 
## Network

The plugin uses the HSMP protocol which comprises UDP messages being exchanged between X-Plane and Apps. 

Each app has its own UDP port; AirTrack uses 2424, AirFMC uses 2425 air AirEFB uses 2426. 

X-Plane instances attempt to bind to port 59000 but can also bind to another port if this one isn't available. 

Initially the plugins send UDP packets to the multicast address 239.224.224.1 to each of the apps ports. This provides for auto-configuration. If the apps are listening they will receive these packets sent once per second and will present the 
option of connecting to that simulator to the user. Once selected, a unicast communication UDP session is established between the individual app and the individual simulator. 

For networks that don't support multicast, the user can also configure the IP address of the simulator on the app (AirFMC only) or configure the IP address of the app device in the plugin. 

