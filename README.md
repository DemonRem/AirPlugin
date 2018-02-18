# AirPlugin

## The X-Plane Haversine Air Plugin

This is the code for the Air plugin to be used with **X-Plane versions 9, 10 and 11** and allow any of these simulators to communicate with Haversine Apps such as AirTrack and AirFMC. 

This code is in constant evolution as new functions and protocol messages are implemented. The plugin version is defined by HSAIRXPL_VERSION in hsxpl.h

Be advised, the code is ugly :D and there may well be much nicer and cleaner ways of structuring things. It does however work well and has very light weight on X-Plane or its framerate. 

For building convenience, the X-PLane SDK files are also included under XPSDK. 

New planes and functions are implemented by testing against their specific datarefs and reading / writing to them. Because different planes have different datarefs and ways of doing things, not all app functions are available with all planes. I try to support as many planes as possible and keep adding support for new ones, and would like to write some new code and make adding new planes easier by the means of a special configuration file dataref syntax. This is however still a plan for a yet to be determined future as my available development time is limited. 

## Network

The plugin uses the HSMP protocol which comprises UDP messages being exchanged between X-Plane and Apps. AirTrack traditionally uses port 2424, AirFMC uses port 2425, AirEFB users port 2426 and the X-Plane plugin attempts to bind to port 59000 by default so messages are exchanged between these. 

The Makefile is my own version which allows cross-compiling. It probably needs to be tweeked if you want to compile the code yourself. 

## Haversine Message Protocol

 **HSMP** stands for **Haversine Message Protocol**, a norm defined by
 Haversine and used to exchange data messages synchronously and
 asynchronously between nodes, implementing a CAN style BUS over IP
 networks, and allowing for simulators to exchange data  with air apps
 of the Haversine family.

 This set of files is used in both apps and flight simulator
 plugins to implement the data exchange.

 The protocol is in constant evolution and updates are signaled by the
 library version number HSMP_PKT_PROTO_VER defined in hsmpnet.h
 New additions to the protocol generate a new version but versions tend
 to be backwards compatible so for example a 4.2 app can talk to a 4.6
 plugin although it may not be able to understand all its messages.

## CPFlight MCP

As I have a CP Flight MCP and EFIS module, and most of the code required to make it work with X-Plane is shared with the code required to make AirTrack work, I've added support for the MCP (in he CPFLIGHT.x files). If you don't need this you can disable the CPFlight code by commenting out -DCPFLIGHT from the Makefile. This is not really needed though as the code doesn't get called if there is no MCP. 

## One Plugin fits all

The principle behind the Haversine Air Plugin is one plugin does all, and allow you to connect multiple and different apps on multiple devices with multiple X-Plane flight simulator with minimum effort. So by installing the plugin you get compatibilitiy with all Apps (plus the CPFlight MCP).

## Building 

To build you will need the HSMP set of files as well:

1) Clone the Air Plugin

git clone git@github.com:haversine-air/AirPlugin.git

2) Change to the AirPlugin directory 

3) Edit the Makefile and adjust the compiler to your own

4) Type "make platform"

5) If the build is successfull, copy the generated HaversineAir folder to the X-Plane Resources/plugins directory

Where platform is one of: 

	make osx64 - For Mac OS X 64 bit build
	make osx32 - For Mac OS X 32 bit build
	make lin64 - For Linux 64 bit build
	make lin32 - For Linux 32 bit build
	make win64 - For Windows 64 bit build
	make win32 - For Windows 32 bit build
	make all   - For all builds

## New code / suggestions / improvements / bugs / support of new planes

Feel free to ask for contributor status and submit pull requests.


