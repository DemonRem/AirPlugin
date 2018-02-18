/*-
 * Copyright (c) 2013-2017 Haversine Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * *
 *
 * 
 * Description:    
 * 
 * HSMP stands for Haversine Message Transfer Protocol, a norm defined by 
 * Haversine and used to exchange data messages synchronously and 
 * asynchronously * between nodes, implementing a CAN style BUS over IP 
 * networks, and allowing, * amongst others for simulators to exchange data 
 * with air apps of the * Haversine family.
 *
 * Message Format: 
 *
 * Each HSMP message always comprises an identifier followed by the message 
 * data as follows:
 *
 *     +--------------------------------+--------------------------------+
 *     |  MSG ID (32 bits unsigned int) |      MSG DATA (VAR LENGTH)     |
 *     +--------------------------------+--------------------------------+
 *
 * The identifier (ID) is constructed according to the following format:
 *
 *               1         2         3
 *      12345678901234567890123456789012
 *     +--------------------------------+
 *     |              (MID)             |
 *     |CCCTTTTTSSSSSSSSGGGGGGGGVVVVVVVV|
 *     +--------------------------------+
 *  
 * The MID comprises, from left to right, and from MSB to LSB: 
 * 
 * CCC  : The first 3 bits identify a class of messages out of possible 8
 *        These represent a major group of messages, for example, aerospace, 
 *        marine or automotive. 
 * 
 * TTTTT : The following 5 bits identify the type of data variable it is out 
 *        of possible 32 distinct types.
 * 
 * SSSSSSSS : The following 8 bits contain the length of the data part of the
 *            message required to represent it in 4 byte (32 bit) units. 
 *            Therefore an SSSSSSSS value of 100 implies that the data part of
 *            the message is 400 bytes long.  
 * 
 * GGGGGGGG : The next 8 bits identify the group of data variables, one of 
 *            256 possible for the class.
 * 
 * VVVVVVVV : The last 8 bits identify the variable in question within the
 *            corresponding group and class. 
 * 
 * This file contains the definitions for all currently supported classes, 
 * types of data, groups and variables used in HSMP. 
 * 
 * Envelope format: 
 * 
 * HSMP messages are exchanged on an IP network primarly using UDP
 * packets which are then put on the LAN or WAN and addressed to other nodes. 
 * Because HSMP messages are often small in size and UDP packets can hold
 * several hundred bytes (typically 1500 on an Ethernet segment for example) 
 * multiple messages can be aggregated into a single HSMP container. 
 *
 * Since most communications are expected to occur between little endian
 * systems, the network protocol is defined as little endian and all values
 * are represented as such. Big endian systems wishing to communicate must
 * convert integers, floats and doubles accordingly. 
 * 
 * The container or envelope which comprises a UDP packet  is constructed as 
 * follows: 
 * 
 *     +--------------------------------+
 *     |      HSMP PACKET HEADER        |
 *     +--------------------------------+
 *     |         HSMP MESSAGE           |
 *     +--------------------------------+
 *     |         HSMP MESSAGE           |
 *     +--------------------------------+
 *     |         HSMP MESSAGE           |
 *     +--------------------------------+
 *     |              ...               |
 *     +--------------------------------+
 * 
 * Multiple messages can be carried in a single packet, providing they fit
 * and don't cause the packet to exceed the MTU of the layer 2 media. 
 * 
 * The packet header is defined as follows: 
 * 
 *     +--------------------------------+
 *     |TTTTTTTTVVVVVVVVVVVVVVVVVVVVVVVV|
 *     |  (NODE TYPE / PROTO VERSION)   | : 4 bytes 
 *     +--------------------------------+
 *     |         SEQUENCE NUMBER        | : 4 bytes 
 *     +--------------------------------+
 *     |            ACK NUMBER          | : 4 bytes 
 *     +--------------------------------+
 *     |             FLAGS              | : 4 bytes 
 *     +--------------------------------+
 *     |   DATA SIZE (inc HDR + MSGS)   | : 4 bytes 
 *     +--------------------------------+
 *     |       NUMBER OF MESSAGES       | : 4 bytes
 *     +--------------------------------+
 *     |        NODE ID  (optional)     | : 16 bytes
 *     +--------------------------------+
 *     |       AUTH SHA1 (optional)     | : 20 bytes
 *     +--------------------------------+
 *
 * NODE TYPE is formed by a byte identifying the type of node it is. 
 * It does not identify the node itself and this is usually achieved 
 * by its IP address and UDP port number. 
 * 
 * PROTO VERSION is embedded in the first integer's 3 LSBs and contains
 * the version of the protocol in use whereby version 3.5.1 would be 
 * encoded as 0x00030501 with each digit in its own byte. 
 * 
 * SEQUENCE NUMBER is an ever increasing integer containing the packet number.
 * This number reverts to 0 after 4294967295 (2^32 - 1). 
 * 
 * ACK NUMBER can be used to acknowledge the reception of a given sequence
 * number. 
 * 
 * SEQUENCE AND ACK numbers are not currently implemented. Their fields
 * are reserved for future use allowing for delivery control and shall be
 * developed with the help of appropriate flags.
 * 
 * FLAGS is a generic purpose field for signaling multiple things. 
 * 
 * DATA SIZE has the total number of bytes in the packet with hdr + msgs
 *
 * NUMBER OF MESSAGES naturaly contains the number of messages present in the
 * packet. 
 * 
 * NODE ID and AUTH SHA1 are for implementing message authentication in a
 * future version intended for WAN communications over the Internet. 
 * They are optionally only present if the corresponding flag is set. 
 * 
 * 
 */

#ifndef __HSMPMSG_H__
#define __HSMPMSG_H__

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

/* Some type definitions for windows */
#if defined(_WIN32)
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int socklen_t;
#endif

/* ################### HSMP PACKETS ############################### */

typedef struct hsmp_pkt_hdr_s {
	uint32_t	idver;
	uint32_t	seqno;
	uint32_t	ackno;
	uint32_t	flags;
	uint32_t	dsize;
	uint32_t	nomsgs;
} hsmp_pkt_hdr_t;

typedef struct hsmp_pkt_s {
	hsmp_pkt_hdr_t	hdr;
	void		*data;      /* Not a real pointer, just an offset reference */
} hsmp_pkt_t;

#define HSMP_MESSAGE_SIZE_FOR_MID(a)       ((((a>>16)&0xFF)*4)+4)
#define HSMP_MESSAGE_DATA_SIZE_FOR_MID(a)  ((((a>>16)&0xFF)*4))

typedef struct hsmp_auth_pkt_hdr_s {
	uint32_t	idver;
	uint32_t	seqno;
	uint32_t	ackno;
	uint32_t	flags;
	uint32_t	dsize;
	uint32_t	nomsgs;
	uint8_t		nid[16];
	uint8_t		sha1[20];
} hsmp_auth_pkt_hdr_t;

typedef struct hsmp_auth_pkt_s {
	hsmp_auth_pkt_hdr_t	hdr;
	void		*data;      /* Not a real pointer, just an offset reference */
} hsmp_auth_pkt_t;

/* HSMP packet flags: */
#define	HSMP_PKT_FLAG_AUTH	0x00000001	/* Packet is authenticated */

/* HSMP protocol node types ; to be ORed with HSMP_PKT_PROTO_VER : */
#define	HSMP_PKT_NT_SIM	0x01000000 /* A generic simulator */
#define	HSMP_PKT_NT_XPG	0x08000000 /* A generic X-Plane type */
#define	HSMP_PKT_NT_XPM	0x09000000 /* X-Plane on Mac simulator */
#define	HSMP_PKT_NT_XPL	0x0A000000 /* X-Plane on Linux simulator */
#define	HSMP_PKT_NT_XPW	0x0B000000 /* X-Plane on Win simulator */
#define	HSMP_PKT_NT_FS9	0x10000000 /* An FS2002/2009/9 simulator */
#define	HSMP_PKT_NT_FSX	0x11000000 /* An FSX simulator */
#define	HSMP_PKT_NT_P3D	0x12000000 /* A Prepar3D simulator */
#define	HSMP_PKT_NT_ELT	0x30000000 /* An ELITE simulator */

/* App instances must have the most significant BIT set to 1 so 0x80000000
 * to indicate that they are apps and not sims. */
#define	HSMP_PKT_NT_APP        0x80000000 /* A generic app instance */
#define	HSMP_PKT_NT_AIRTRACK   0x81000000	/* An AirTrack app instance */
#define	HSMP_PKT_NT_AIRFMC     0x82000000	/* An AirFMC app instance */
#define	HSMP_PKT_NT_AIREFB     0x84000000	/* An AirEFB app instance */

/* ################### HSMP MESSAGES ############################## */

/* The following contants can be used to identify a class in a MID */
#define HSMP_MSG_CLASS_CTL     0x00000000	/* HSMP control messages */
#define HSMP_MSG_CLASS_AIR     0x20000000	/* Aerospace systems */
#define HSMP_MSG_CLASS_SEA     0x40000000	/* Marine systems */

/* The following define possible message types in a MID */
#define	HSMP_MSG_TYPE_NOP      0x01000000 /* no data */
#define	HSMP_MSG_TYPE_UINT32   0x02000000 /* uint32_t */
#define	HSMP_MSG_TYPE_SINT32   0x03000000 /* int32_t */
#define	HSMP_MSG_TYPE_UINT64   0x04000000 /* uint64_t */
#define	HSMP_MSG_TYPE_SINT64   0x05000000 /* int64_t */
#define	HSMP_MSG_TYPE_FLOAT    0x06000000 /* float, 4 bytes */
#define	HSMP_MSG_TYPE_DOUBLE   0x07000000 /* double, 8 bytes */
#define	HSMP_MSG_TYPE_STR      0x08000000 /* char *, null terminated */
#define HSMP_MSG_TYPE_ROUTEPT  0x09000000 /* route point, hsmp_route_pt_msg_t */
#define	HSMP_MSG_TYPE_RSTACK   0x0A000000 /* radio stack hsmp_radio_stack_t */
#define HSMP_MSG_TYPE_FMCSCRN  0x0B000000 /* screen char, hsmp_fmc_screen_c_t */
#define HSMP_MSG_TYPE_CLISTIDX 0x0C000000 /* checklist index  hsmp_apt_idx_t */
#define HSMP_MSG_TYPE_AT_TCAS  0x0D000000 /* TCAS acf broadcast hsmp_tca_acf_t */
#define HSMP_MSG_TYPE_AT_POS   0x0E000000 /* POS acf broadcast hsmp_pos_acf_t */
#define HSMP_MSG_TYPE_APT_IDX  0x0F000000 /* APT index hsmp_apt_idx_t */
#define HSMP_MSG_TYPE_DREF     0x10000000 /* Dataref specific types, uses VVVVVVVV to ID */

/* #################### HSMP CONTROL  ############################## */

#define	HSMP_MSG_CGRP_FLOW     0x00000100	/* Flow control group */

#define	HSMP_MID_HELLO         0x02010101	/* HELLO message */
#define	HSMP_MID_STREAM_START  0x01000102	/* Request for streamed data */
#define	HSMP_MID_STREAM_STOP   0x01000103	/* Stop sending streamed data */

/* ################### HSMP AEROSPACE ############################# */

/* Aerospace system groups are defined as: */

#define	HSMP_MSG_AGRP_IPOS  	0x00001100	/* ACF instant position */
#define	HSMP_MSG_AGRP_WXR   	0x00001200	/* Instant weather */
#define	HSMP_MSG_AGRP_STATIC	0x00001300	/* ACF static data */
#define	HSMP_MSG_AGRP_VSPDS 	0x00001400	/* ACF Vspeeds */
#define	HSMP_MSG_AGRP_BFLG  	0x00001500	/* Landing gear, flaps, etc */
#define	HSMP_MSG_AGRP_AP    	0x00001600	/* Auto-Pilot systems */
#define	HSMP_MSG_AGRP_MFD   	0x00001700	/* Avionics - MFD */
#define	HSMP_MSG_AGRP_FD    	0x00001800	/* Avionics - Flight Director */
#define	HSMP_MSG_AGRP_RC    	0x00001900	/* Remote Joystick */
#define	HSMP_MSG_AGRP_RAD   	0x00001D00	/* Radio systems */
#define	HSMP_MSG_AGRP_ROUTE 	0x00001E00	/* Route and flight plans */
#define	HSMP_MSG_AGRP_FMC   	0x00001F00	/* FMC */
#define	HSMP_MSG_AGRP_CLIST   0x00002000	/* Checklist sync */
#define	HSMP_MSG_AGRP_AT      0x00002100	/* Air Traffic */
#define HSMP_MSG_AGRP_APT     0x00002200 /* APT scenery */
#define HSMP_MSG_AGRP_DREF    0x00002300 /* Dataref access */

/* And the full list of aerospace MIDs below */

#define	HSMP_MID_IPOS_LAT      0x27021101	/* Latitude, degs, double */
#define	HSMP_MID_IPOS_LON      0x27021102	/* Latitude, degs, double */
#define	HSMP_MID_IPOS_ELEV     0x27021103	/* Elevation, m, double */

/* Theta, the pitch relative to the plane normal to the Y axis */ 
#define	HSMP_MID_IPOS_THETA    0x26011104	/* Theta, degs, float */

#define	HSMP_MID_IPOS_PHI      0x26011105	/* Phy, roll in degs, float */
#define	HSMP_MID_IPOS_PSI      0x26011106	/* PSI, true HDG, degs, float */
#define	HSMP_MID_IPOS_MVAR     0x26011107	/* MVAR, degs, float */

/* Alpha, The pitch relative to the flown path (angle of attack) */
#define	HSMP_MID_IPOS_ALPHA    0x26011108	/* Alpha, degs, float */

/* Beta, The heading relative to the flown path (yaw) */
#define	HSMP_MID_IPOS_BETA     0x26011109	/* Beta, degs, float */

#define	HSMP_MID_IPOS_GS       0x2601110A	/* GS, m/s, float */
#define	HSMP_MID_IPOS_IAS      0x2601110B	/* IAS, knots, float */
#define	HSMP_MID_IPOS_MACH     0x2601110C	/* MACH speed, float */
#define	HSMP_MID_IPOS_TAS      0x2601110D	/* TAS, knots, float */
#define	HSMP_MID_IPOS_VVI      0x2601110E	/* Vertical speed, fpm, float */
#define	HSMP_MID_IPOS_MSL      0x26011110	/* Elevation MSL, m, float */
#define	HSMP_MID_IPOS_AGL      0x26011111	/* Elevation AGL, m, float */

#define	HSMP_MID_WXR_ALTM      0x26011201	/* Altimeter,e.g. 29.92,float */
#define	HSMP_MID_WXR_WSPEED    0x26011202	/* Wind speed, knots, float */
#define	HSMP_MID_WXR_WDIR      0x26011203	/* Wind dir, degs, float */
#define	HSMP_MID_WXR_OTEMP     0x26011204	/* Outside temp, C, float */

#define	HSMP_MID_STAT_TAILNO	0x28021301	/* Tail no, char[8] */
#define	HSMP_MID_STAT_ICAO  	0x28021302	/* ACF ICAO, char[8] */
#define	HSMP_MID_STAT_CALLSGN	0x28021303	/* Callsign, char[8] */

#define	HSMP_MID_VSPD_VS0	0x26011401	/* VS0, float */
#define	HSMP_MID_VSPD_VS	0x26011402	/* VS, float */
#define	HSMP_MID_VSPD_VFE	0x26011403	/* VFE, float */
#define	HSMP_MID_VSPD_VNO	0x26011404	/* VNO, float */
#define	HSMP_MID_VSPD_VNE	0x26011405	/* VNE, float */
#define	HSMP_MID_VSPD_MMO	0x26011406	/* MMO, float */

#define	HSMP_MID_BFLG_LGEAR	0x22011501	/* lgear, 0=up,1=down,uint32 */
#define	HSMP_MID_BFLG_PBRK 	0x26011502	/* pbreak ratio, 1=set, float */
#define	HSMP_MID_BFLG_FLAPS	0x26011503	/* flaps ratio,0=rtct, float */
#define	HSMP_MID_BFLG_SPDBRK	0x26011504	/* spbrk rat,0=retract,float */

#define	HSMP_MID_AP_ALTDIAL	0x26011601	/* Alt dial, feet, float */
#define	HSMP_MID_AP_SPDDIAL	0x26011602	/* SPD dial, KIAS, float */
#define	HSMP_MID_AP_HDGDIAL	0x26011603	/* HDG dial, degs, float */
#define	HSMP_MID_AP_VSDIAL	0x26011604	/* VS dial, fpm, float */

#define	HSMP_MID_AP_SISMACH	0x22011611	/* speed is mach, uint32 */
#define	HSMP_MID_AP_SOURCE 	0x22011612	/* 0=NAV1, 1=NAV2, 2=FMC */

/* Status variables are uint32's with either 0=off, 1=arm or 2=captured/on */
#define	HSMP_MID_AP_SPDSTATUS	0x22011621	/* SPD status, uint32 */
#define	HSMP_MID_AP_LNAVSTATUS	0x22011622	/* LNAV status, uint32 */
#define	HSMP_MID_AP_VNAVSTATUS	0x22011623	/* VNAV status, uint32 */
#define	HSMP_MID_AP_HDGSTATUS	0x22011624	/* HDG HLD status, uint32 */
#define	HSMP_MID_AP_LOCSTATUS	0x22011625	/* VOR/LOC  status, uint32 */
#define	HSMP_MID_AP_ALTSTATUS	0x22011626	/* ALT HLD status, uint32 */
#define	HSMP_MID_AP_VSSSTATUS	0x22011627	/* V/S status, uint32 */
#define	HSMP_MID_AP_GSSTATUS	0x22011628	/* GS/APP status, uint32 */
#define	HSMP_MID_AP_LCSTATUS	0x22011629	/* LCHG status, uint32 */
#define	HSMP_MID_AP_N1STATUS	0x2201162A	/* N1 status, uint32 */

#define	HSMP_MID_AP_OBS1DIAL	0x26011631	/* OBS1 for pilot,degs,float */
#define	HSMP_MID_AP_OBS2DIAL	0x26011632	/* OBS2 for pilot,degs,float */

/* MFD mode is defined as 0=APP, 1=VOR, 2=MAP, 3=NAV and 4=PLN */
#define	HSMP_MID_MFD_MODE	0x22011701	/* MFD mode, uint32 */
#define	HSMP_MFD_MODE_APP	0x00000000
#define	HSMP_MFD_MODE_VOR	0x00000001
#define	HSMP_MFD_MODE_MAP	0x00000002
#define	HSMP_MFD_MODE_NAV	0x00000003
#define	HSMP_MFD_MODE_PLN	0x00000004

/* MFD show is defined accoring to the bitmasks below */
#define	HSMP_MID_MFD_SHOW	0x22011702	/* MFD show mask, uint32 */
#define	HSMP_MFD_SHOW_WXR	0x00000001
#define	HSMP_MFD_SHOW_TCA	0x00000002
#define	HSMP_MFD_SHOW_APT	0x00000004
#define	HSMP_MFD_SHOW_WPT	0x00000008
#define	HSMP_MFD_SHOW_VOR	0x00000010
#define	HSMP_MFD_SHOW_NDB	0x00000020
#define	HSMP_MFD_SHOW_LOC	0x00000040
#define	HSMP_MFD_SHOW_CWP	0x00000080
#define	HSMP_MFD_SHOW_GEO	0x00000100
#define HSMP_MFD_SHOW_EXP   0x00000200

#define	HSMP_MID_MFD_RANGE	0x26011703	/* MFD range, NM, float */

/* N1 and N2 selectors are defined as 0=OFF, 1=NAV1/NAV2 2=ADF1/ADF2 */
#define HSMP_MID_MFD_N1SEL      0x22011711      /* N1 SEL, uint32_t */
#define HSMP_MID_MFD_N2SEL      0x22011712      /* N2 SEL, uint32_t */
#define	HSMP_MFD_NSEL_OFF	0x00000000
#define	HSMP_MFD_NSEL_NAV	0x00000001
#define	HSMP_MFD_NSEL_ADF	0x00000002

#define	HSMP_MID_FD_MODE	0x22011801	/* 0=OFF, 1=ON, 2=AUTO,uint32*/
#define	HSMP_MID_FD_PITCH	0x26011802	/* suggested FD pitch, float */
#define	HSMP_MID_FD_ROLL	0x26011803	/* suggested FD roll, float */

#define HSMP_MID_RC_ROLL    0x26011901  /* Joystick control - roll - float */
#define HSMP_MID_RC_PITCH   0x26011902  /* Joystick control - pitch - float */
#define HSMP_MID_RC_YAW     0x26011903  /* Joystick control - yaw - float */

#define	HSMP_MID_RAD_N1FREQ	0x22011D11	/* NAV1 freq */
#define	HSMP_MID_RAD_N2FREQ	0x22011D12	/* NAV2 freq */
#define	HSMP_MID_RAD_C1FREQ	0x22011D13	/* COM1 freq */
#define	HSMP_MID_RAD_C2FREQ	0x22011D14	/* COM2 freq */
#define	HSMP_MID_RAD_A1FREQ	0x22011D15	/* ADF1 freq */
#define	HSMP_MID_RAD_A2FREQ	0x22011D16	/* ADF2 freq */

#define	HSMP_MID_RAD_N1SFREQ	0x22011D21	/* NAV1 stby freq */
#define	HSMP_MID_RAD_N2SFREQ	0x22011D22	/* NAV2 stby freq */
#define	HSMP_MID_RAD_C1SFREQ	0x22011D23	/* COM1 stby freq */
#define	HSMP_MID_RAD_C2SFREQ	0x22011D24	/* COM2 stby freq */
#define	HSMP_MID_RAD_A1SFREQ	0x22011D25	/* ADF1 stby freq */
#define	HSMP_MID_RAD_A2SFREQ	0x22011D26	/* ADF2 stby freq */

/* The structure hsmp_radio_stack_t encapsulates the entire radio stack in one
 * variable and hence one message. */
typedef struct hsmp_radio_stack_s {
    uint32_t    nav1;
    uint32_t    nav1s;
    uint32_t    nav2;
    uint32_t    nav2s;
    uint32_t    com1;
    uint32_t    com1s;
    uint32_t    com2;
    uint32_t    com2s;
    uint32_t    adf1;
    uint32_t    adf1s;
    uint32_t    adf2;
    uint32_t    adf2s;
} hsmp_radio_stack_t;

/* And the HSMP_MID_RAD_STACK message allows sending all the 12 radio frequencies
 * in one single message which is more efficient. */
#define HSMP_MID_RAD_STACK      0x2A0C1D31  /* hsmp_radio_stack_t */

#define	HSMP_MID_RAD_XPDRCODE	0x22011D31	/* XPDR code */
#define	HSMP_MID_RAD_XPDRMODE	0x22011D32	/* XPDR mode */
#define	HSMP_RAD_XPDRMODE_OFF	0x00000000
#define	HSMP_RAD_XPDRMODE_STBY	0x00000001
#define	HSMP_RAD_XPDRMODE_ON	0x00000002
#define	HSMP_RAD_XPDRMODE_TEST	0x00000003

/******************************************************************/
/* Route and flight plan processing - A route or flight plan is a 
 * sequence of route points together with the notion of active point. 
 */
#define HSMP_ROUTE_MAX_POINTS   2000


/* A route point is defined as */
typedef struct hsmp_route_pt_s {
	uint32_t    ptype;        /* One of NAVDB_* */
	char        pname[8];     /* A string with the point ID name */
	float       lat;          /* Latitude */
	float       lon;          /* Longitude */
	float       elev;         /* Elevation in feet */
	uint32_t    ta;           /* Time of arrival in secs since 1/1/1970 */
} hsmp_route_pt_t;

/* The following structure is defined as being capable of holding
 * a flight plan with up to HSMP_ROUTE_MAX_POINTS points. Some fields
 * were kept as legacy from the previous AirTrack protocol in order to 
 * keep the space since they are used and stored in such a manner elsewhere. */
typedef struct hsmp_route_s {
    uint32_t        msgid;      /* Legacy from ATP <4.0, ignore */
    uint32_t        version;    /* Legacy from ATP <4.0, ignore */
    uint32_t        nopoints;   /* Number of points in plan */
    int32_t         cleg;       /* Current active leg or -1 */
    int32_t         segment;    /* Legacy from ATP <4.0, ignore */
    int32_t         segpoints;  /* Legacy from ATP <4.0, ignore */
    hsmp_route_pt_t pts[HSMP_ROUTE_MAX_POINTS];
} hsmp_route_t;

/* In order to send a flight plan over the network HSMP defines two 
 * MID types; one for sending individual route points and one for sending
 * the active leg index. */

typedef struct hsmp_route_pt_msg_s {
    uint32_t        pindex;     /* The point index from 0 to ... */
    hsmp_route_pt_t pt;         /* The point itself */
} hsmp_route_pt_msg_t;

#define HSMP_MID_ROUTE_CLEG     0x22011E01  /* current leg, 0..., uint32 */
#define HSMP_MID_ROUTE_NOPTS    0x22011E02  /* number of route points */
#define HSMP_MID_ROUTE_POINT    0x29081E03  /* hsmp_route_pt_msg_t */

/* Sending a route plan therefore consists of sending a HSMP_MID_ROUTE_CLEG
 * message plus as many HSMP_MID_ROUTE_POINT messages as there are points in a 
 * route. Caution must be taken in order not to exceed the maximum transmit unit
 * size (MTU) of the media in a packet when transmissing routes with too many 
 * points. The suggested maximum number of points per packet is 40. Since each
 * point of hsmp_route_pt_msg_t is 32 bytes, plus 4 bytes for the MID is 36.
 * 40 points take thus 36*40=1440 bytes.
 * To this we add 8 bytes of HSMP_MID_ROUTE_CLEG plus 8 for HSMP_MID_ROUTE_NOPTS
 * plus 24 bytes of HSMP header plus 20 bytes of UDP header resulting in 1500
 * bytes which is just the typical MTU of 1500.Using a lower number allows route
 * plans to be sent with even lower MTUs so the suggested number is around 30.
 */


#define HSMP_FMC_MAX_LINE_CHARS 26

/* The FMC type */
#define HSMP_MID_FMC_TYPE           0x22011F00
#define HSMP_FMC_TYPE_XPLANE        0x01000000
#define HSMP_FMC_TYPE_XP_X737       0x01000001
#define HSMP_FMC_TYPE_XP_A320N      0x01000002
#define HSMP_FMC_TYPE_XP_PB757      0x01000004
#define HSMP_FMC_TYPE_XP_PB777      0x01000008
#define HSMP_FMC_TYPE_XP_PCRJ200    0x01000010
#define HSMP_FMC_TYPE_XP_XFMC       0x01000020
#define HSMP_FMC_TYPE_XP_UFMC       0x01000040
#define HSMP_FMC_TYPE_XP_X737V5     0x01000080
#define HSMP_FMC_TYPE_XP_A320Q      0x01000100
#define HSMP_FMC_TYPE_XP_IX733      0x01000200
#define HSMP_FMC_TYPE_XP_XP738      0x01000400

/* The hsmp_fmc_screen_c_t structures define a way to
 * represent each individual character on an
 * FMC/MCDU screen */
typedef struct hsmp_fmc_screen_c_s {
    uint8_t row;              /* The row, 0-15 */
    uint8_t col;              /* The column, 0-24 */
    uint8_t fsize;            /* The font size */
    unsigned char  chr;       /* The character */
    uint32_t colour;          /* RGBA */
} hsmp_fmc_screen_c_t;

#define HSMP_FMC_DEF_SCREEN_NOROWS   14
#define HSMP_FMC_DEF_SCREEN_NOCOLS   25

#define HSMP_FMC_MAX_SCREEN_NOROWS   16
#define HSMP_FMC_MAX_SCREEN_NOCOLS   30

typedef struct hsmp_fmc_screen_s {
    hsmp_fmc_screen_c_t matrix[HSMP_FMC_MAX_SCREEN_NOROWS][HSMP_FMC_MAX_SCREEN_NOCOLS];
} hsmp_fmc_screen_t;



/* The FMC left lines from 1 to 14, up to 32 chars each incl \0 */
#define	HSMP_MID_FMC_LINE_L01	0x28081F00
#define	HSMP_MID_FMC_LINE_L02	0x28081F01
#define	HSMP_MID_FMC_LINE_L03	0x28081F02
#define	HSMP_MID_FMC_LINE_L04	0x28081F03
#define	HSMP_MID_FMC_LINE_L05	0x28081F04
#define	HSMP_MID_FMC_LINE_L06	0x28081F05
#define	HSMP_MID_FMC_LINE_L07	0x28081F06
#define	HSMP_MID_FMC_LINE_L08	0x28081F07
#define	HSMP_MID_FMC_LINE_L09	0x28081F08
#define	HSMP_MID_FMC_LINE_L10	0x28081F09
#define	HSMP_MID_FMC_LINE_L11	0x28081F0A
#define	HSMP_MID_FMC_LINE_L12	0x28081F0B
#define	HSMP_MID_FMC_LINE_L13	0x28081F0C
#define	HSMP_MID_FMC_LINE_L14	0x28081F0D
#define	HSMP_MID_FMC_LINE_L15	0x28081F0E
#define	HSMP_MID_FMC_LINE_L16	0x28081F0F

/* The FMC right lines, if any, from 1 to 14, up to 32 chars each incl \0 */
#define	HSMP_MID_FMC_LINE_R01	0x28081F10
#define	HSMP_MID_FMC_LINE_R02	0x28081F11
#define	HSMP_MID_FMC_LINE_R03	0x28081F12
#define	HSMP_MID_FMC_LINE_R04	0x28081F13
#define	HSMP_MID_FMC_LINE_R05	0x28081F14
#define	HSMP_MID_FMC_LINE_R06	0x28081F15
#define	HSMP_MID_FMC_LINE_R07	0x28081F16
#define	HSMP_MID_FMC_LINE_R08	0x28081F17
#define	HSMP_MID_FMC_LINE_R09	0x28081F18
#define	HSMP_MID_FMC_LINE_R10	0x28081F19
#define	HSMP_MID_FMC_LINE_R11	0x28081F1A
#define	HSMP_MID_FMC_LINE_R12	0x28081F1B
#define	HSMP_MID_FMC_LINE_R13	0x28081F1C
#define	HSMP_MID_FMC_LINE_R14	0x28081F1D
#define	HSMP_MID_FMC_LINE_R15	0x28081F1E
#define	HSMP_MID_FMC_LINE_R16	0x28081F1F

/* FMC left and right screen button presses */
#define	HSMP_MID_FMC_BUT_L01    0x21001F21
#define	HSMP_MID_FMC_BUT_L02    0x21001F22
#define	HSMP_MID_FMC_BUT_L03    0x21001F23
#define	HSMP_MID_FMC_BUT_L04    0x21001F24
#define	HSMP_MID_FMC_BUT_L05    0x21001F25
#define	HSMP_MID_FMC_BUT_L06    0x21001F26

#define	HSMP_MID_FMC_BUT_R01    0x21001F31
#define	HSMP_MID_FMC_BUT_R02    0x21001F32
#define	HSMP_MID_FMC_BUT_R03    0x21001F33
#define	HSMP_MID_FMC_BUT_R04    0x21001F34
#define	HSMP_MID_FMC_BUT_R05    0x21001F35
#define	HSMP_MID_FMC_BUT_R06    0x21001F36

#define HSMP_MID_FMC_BUT_D0     0x21001F40
#define HSMP_MID_FMC_BUT_D1     0x21001F41
#define HSMP_MID_FMC_BUT_D2     0x21001F42
#define HSMP_MID_FMC_BUT_D3     0x21001F43
#define HSMP_MID_FMC_BUT_D4     0x21001F44
#define HSMP_MID_FMC_BUT_D5     0x21001F45
#define HSMP_MID_FMC_BUT_D6     0x21001F46
#define HSMP_MID_FMC_BUT_D7     0x21001F47
#define HSMP_MID_FMC_BUT_D8     0x21001F48
#define HSMP_MID_FMC_BUT_D9     0x21001F49
#define HSMP_MID_FMC_BUT_D9     0x21001F49
#define HSMP_MID_FMC_BUT_DOT    0x21001F4A
#define HSMP_MID_FMC_BUT_PLMN   0x21001F4B  /* Plus/minus */

/* Control buttons */
#define HSMP_MID_FMC_BUT_AP      0x21001F50
#define HSMP_MID_FMC_BUT_INIT    0x21001F51
#define HSMP_MID_FMC_BUT_MENU    0x21001F52
#define HSMP_MID_FMC_BUT_N1LIM   0x21001F53
#define HSMP_MID_FMC_BUT_PREVP   0x21001F54
#define HSMP_MID_FMC_BUT_NEXTP   0x21001F55
#define HSMP_MID_FMC_BUT_FIX     0x21001F56
#define HSMP_MID_FMC_BUT_LEGS    0x21001F57
#define HSMP_MID_FMC_BUT_RTE     0x21001F58
#define HSMP_MID_FMC_BUT_CLB     0x21001F59
#define HSMP_MID_FMC_BUT_DEPARR  0x21001F5A
#define HSMP_MID_FMC_BUT_CRZ     0x21001F5B
#define HSMP_MID_FMC_BUT_HOLD    0x21001F5C
#define HSMP_MID_FMC_BUT_DES     0x21001F5D
#define HSMP_MID_FMC_BUT_PROG    0x21001F5E
#define HSMP_MID_FMC_BUT_EXEC    0x21001F5F

/* Alphabet buttons */
#define HSMP_MID_FMC_BUT_A      0x21001F60
#define HSMP_MID_FMC_BUT_B      0x21001F61
#define HSMP_MID_FMC_BUT_C      0x21001F62
#define HSMP_MID_FMC_BUT_D      0x21001F63
#define HSMP_MID_FMC_BUT_E      0x21001F64
#define HSMP_MID_FMC_BUT_F      0x21001F65
#define HSMP_MID_FMC_BUT_G      0x21001F66
#define HSMP_MID_FMC_BUT_H      0x21001F67
#define HSMP_MID_FMC_BUT_I      0x21001F68
#define HSMP_MID_FMC_BUT_J      0x21001F69
#define HSMP_MID_FMC_BUT_K      0x21001F6A
#define HSMP_MID_FMC_BUT_L      0x21001F6B
#define HSMP_MID_FMC_BUT_M      0x21001F6C
#define HSMP_MID_FMC_BUT_N      0x21001F6D
#define HSMP_MID_FMC_BUT_O      0x21001F6E
#define HSMP_MID_FMC_BUT_P      0x21001F6F
#define HSMP_MID_FMC_BUT_Q      0x21001F70
#define HSMP_MID_FMC_BUT_R      0x21001F71
#define HSMP_MID_FMC_BUT_S      0x21001F72
#define HSMP_MID_FMC_BUT_T      0x21001F73
#define HSMP_MID_FMC_BUT_U      0x21001F74
#define HSMP_MID_FMC_BUT_V      0x21001F75
#define HSMP_MID_FMC_BUT_W      0x21001F76
#define HSMP_MID_FMC_BUT_X      0x21001F77
#define HSMP_MID_FMC_BUT_Y      0x21001F78
#define HSMP_MID_FMC_BUT_Z      0x21001F79
#define HSMP_MID_FMC_BUT_SPC    0x21001F7A
#define HSMP_MID_FMC_BUT_DEL    0x21001F7B
#define HSMP_MID_FMC_BUT_SLASH  0x21001F7C
#define HSMP_MID_FMC_BUT_CLR    0x21001F7D

/* X-Plane FMC extra buttons */
#define HSMP_MID_FMC_BUT_APT    0x21001F80
#define HSMP_MID_FMC_BUT_VOR    0x21001F81
#define HSMP_MID_FMC_BUT_NDB    0x21001F82
#define HSMP_MID_FMC_BUT_WPT    0x21001F83
#define HSMP_MID_FMC_BUT_LATLON 0x21001F84
#define HSMP_MID_FMC_BUT_LOAD   0x21001F85
#define HSMP_MID_FMC_BUT_SAVE   0x21001F86

#define HSMP_MID_FMC_BUT_SFPLN  0x21001F89
#define HSMP_MID_FMC_BUT_ALTN   0x21001F8A
#define HSMP_MID_FMC_BUT_ATC    0x21001F8B
#define HSMP_MID_FMC_BUT_FMCCOM 0x21001F8C
#define HSMP_MID_FMC_BUT_MSG    0x21001F8D
#define HSMP_MID_FMC_BUT_LNAV   0x21001F8E
#define HSMP_MID_FMC_BUT_VNAV   0x21001F8F

/* Additional type additional buttons */
#define HSMP_MID_FMC_BUT_INDEX      0x21001F90
#define HSMP_MID_FMC_BUT_AIRPT      0x21001F91
#define HSMP_MID_FMC_BUT_DATA       0x21001F92
#define HSMP_MID_FMC_BUT_FUELP      0x21001F93
#define HSMP_MID_FMC_BUT_PERF       0x21001F94
#define HSMP_MID_FMC_BUT_RADNAV     0x21001F95
#define HSMP_MID_FMC_BUT_FPLN       0x21001F96
#define HSMP_MID_FMC_BUT_DIR        0x21001F97
#define HSMP_MID_FMC_BUT_UP         0x21001F98
#define HSMP_MID_FMC_BUT_DOWN       0x21001F99
#define HSMP_MID_FMC_BUT_RIGHT      0x21001F9A
#define HSMP_MID_FMC_BUT_LEFT       0x21001F9B
#define HSMP_MID_FMC_BUT_BLANK      0x21001F9C
#define HSMP_MID_FMC_BUT_OVFY       0x21001F9D
#define HSMP_MID_FMC_BUT_MFDMENU    0x21001F9E
#define HSMP_MID_FMC_BUT_RADIO      0x21001F9F

#define HSMP_MID_FMC_MCDU_TOGGLE    0x21001FF9  /* Toggle MCDU */

/* Annunciator messages */
#define HSMP_MID_FMC_AN_VSLU_TXT    0x28021FFA  /* LEFT UP MSG */
#define HSMP_MID_FMC_AN_VSLD_TXT    0x28021FFB  /* LEFT DOWN MSG */
#define HSMP_MID_FMC_AN_VSRU_TXT    0x28021FFC  /* RIGHT UP MSG */
#define HSMP_MID_FMC_AN_VSRD_TXT    0x28021FFD  /* RIGHT DOWN MSG */
#define HSMP_MID_FMC_ACTIVE_MCDU    0x22011FFE  /* ACTIVE MCDU , 1 or 2 */

/* Light annunciators, a bitmask of lights on */
#define HSMP_MID_FMC_ANNUNCIATORS   0x2B021FFE  /* Bitmask of the following */
#define HSMP_FMC_ANNUNC_VSLU        0x00000001  /* VERTICAL SIDE MSG LEFT UP */
#define HSMP_FMC_ANNUNC_VSLD        0x00000002  /* VERTICAL SIDE MSG LEFT DOWN */
#define HSMP_FMC_ANNUNC_VSRU        0x00000004  /* VERTICAL SIDE MSG RIGHT UP */
#define HSMP_FMC_ANNUNC_VSRD        0x00000008  /* VERTICAL SIDE MSG RIGHT DOWN */
#define HSMP_FMC_ANNUNC_EXEC        0x00000010  /* EXEC LIGHT */

/* There are two ways of transferring an FMC screen
 * Line by line, with each of the 16 lines or, character by character,
 * with each of the 384 characters being of type hsmp_fmc_screen_c_t and
 * sending each of them on a HSMP_MID_FMC_SCREEN message.
 * Each HSMP_MID_FMC_SCREEN message is succeeded by a hsmp_fmc_screen_c_t
 * 8 byte structure. Because packets are limited in size, it is suggested
 * that an FMC screen is sent in 4 packets, each with 4 lines of screen,
 * producing 4*12*25 = 1200 bytes of message content each. */
#define HSMP_MID_FMC_SCREEN_C   0x2B021FFF

/* ######################## CHECKLIST SYNC ##################################### */

typedef struct hsmp_clist_idx_s {
  char checklist[32];
  uint64_t fsize;
} hsmp_clist_idx_t;

#define  HSMP_MID_CLIST_LIST_REQ    0x21002001  /* A request for the checklist index list */
#define  HSMP_MID_CLIST_IDX         0x2C0A2002  /* A clist.txt index reply */
#define  HSMP_MID_CLIST_REQDL       0x28082003  /* A clist.txt download request */
#define  HSMP_MID_CLIST_REQFAIL     0x28082004  /* A clist.txt request failure to deliver report */

/* ######################## TCAS BCAST ##################################### */

typedef struct hsmp_tca_acf_s {
  uint32_t xprcode;        /* Unique ID / transponder code*/
  float rbearing;          /* Relative bearing in degrees */
  float rdistance;         /* Relative distance in metres */
  float raltitude;         /* Relative altitude in metres */
  char callsign[16];       /* Call sign */
  char icao[8];            /* ICAO if available */
  uint64_t timestamp;      /* In microseconds since 1970 */
} hsmp_tca_acf_t;

#define	HSMP_MID_AT_TCA_ACF      0x2D0C2101	/* An aircraft around us */

#define HSMP_ATC_POS_STAGE_NA         0x00000000
#define HSMP_ATC_POS_STAGE_UNDEF      0x00000001
#define HSMP_ATC_POS_STAGE_PARK       0x00000002
#define HSMP_ATC_POS_STAGE_START      0x00000004
#define HSMP_ATC_POS_STAGE_GATE       0x00000008
#define HSMP_ATC_POS_STAGE_STANDBY    0x00000010
#define HSMP_ATC_POS_STAGE_PUSHBACK   0x00000020
#define HSMP_ATC_POS_STAGE_HOLD       0x00000040
#define HSMP_ATC_POS_STAGE_TAXI       0x00000080
#define HSMP_ATC_POS_STAGE_TAKEOFF    0x00000100
#define HSMP_ATC_POS_STAGE_LANDING    0x00000200
#define HSMP_ATC_POS_STAGE_AIRBORNE   0x00000400
#define HSMP_ATC_POS_STAGE_DESCENDING 0x00000800
#define HSMP_ATC_POS_STAGE_CLIMBING   0x00001000
#define HSMP_ATC_POS_STAGE_CRUISE     0x00002000
#define HSMP_ATC_POS_STAGE_ONRUNWAY   0x00004000
#define HSMP_ATC_POS_STAGE_APPROACH   0x00008000

typedef struct hsmp_atc_pos_s {
  uint32_t xprcode;        /* Unique ID / transponder code*/
  float  heading;
  double latitude;
  double longitude;
  float elevation;         /* MSL */
  float speed;             /* KIAS if available or -1 */
  char callsign[16];       /* Call sign */
  char icao[8];            /* ICAO if available */
  uint32_t stage;          /* The stage of flight */
  uint32_t reserved;       /* For future use */
  uint64_t timestamp;      /* In microseconds since 1970 */
} hsmp_atc_pos_t;

#define	HSMP_MID_AT_POS_ACF      0x2E122102	/* An aircraft around us */

/* ######################### APT LIST ###################################### */

typedef struct hsmp_apt_idx_s {
  char icao[8];
  uint64_t fsize;
} hsmp_apt_idx_t;

#define  HSMP_MID_APT_LIST_REQ      0x21002201  /* A request for the airport index list */
#define  HSMP_MID_APT_IDX           0x2F042202  /* An apt.dat index reply */
#define  HSMP_MID_APT_REQDL         0x28022203  /* An apt.dat download request */
#define  HSMP_MID_APT_REQFAIL       0x28022204  /* An apt.dat request failure to deliver report */

/* ######################### DATAREFS ###################################### */

/* This set of structures and MIDS are used for direct dataref access from an
 * app, such as to retrieve checklist dataref values which are unknown in advance.
 * Although apps tend to use one of the other fixed structures for accessing data,
 * these set of commands allow for an app to have the flexibility of accessing virtually
 * anything and set it, without it having to be defined in the protocol.
 */

/* Used to request a dataref reading */
typedef struct hsmp_dref_read_req_s {
  uint64_t drid;   /* Client dataref ID to include in feedback report */
  char dref[128];  /* The dataref text or dataref value if a reply */
} hsmp_dref_read_req_t;

/* Used to reply to a dataref read request */
typedef struct hsmp_dref_read_rep_s {
  uint64_t drid;   /* Client dataref ID to report back */
  char dval[128];  /* The value of the dataref, may be smaller than 128 depending on type */
} hsmp_dref_read_rep_t;

/* Used to write a specific dataref */
typedef struct hsmp_dref_write_req_s {
  char dref[128];  /* The dataref text or dataref value if a reply */
  char dval[128];  /* May be smaller than 128 depending on type */
} hsmp_dref_write_req_t;

/* Unlike other MIDs, dataref requests use the last 8 VVVVVVVV bits of the MID to
 * specify the type of request, type of dataref and frequency of the report if applicable.
 *
 * VVVVVVVV is defined as OOFFFTTT where:
 *
 *     OO = operation, 00=READ REQUEST, 01 = READ REPLY, 10 = WRITE REQ, 11 = RESERVED
 *     FFF = frequency, 000=STOP SENDING, 001=SEND ONCE, 010=SEND EVERY TICTAC, 011=SEND EVERY SECOND
 *     TTT = type, 000= UNKNOWN, 001=INT32, 010=FLOAT, 011=DOUBLE, 100=STR, 101=[FLOAT], 110=[INT]
 *
 * The SSSSSSSS bits of the dataref MIDs represent the size in 4 byte quantities and
 * naturally depend on the dataref type and request type as defined by VVVVVVVVV.
 *
 * The CCCTTTTT bits of dataref MIDs are set to 0x30 (HSMP_MSG_CLASS_AIR|HSMP_MSG_TYPE_DREF)
 *
 * The GGGGGGGG bits are set to HSMP_MSG_AGRP_DREF (dataref group type)
 *
 */

#define HSMP_MID_DREF_CLASSTYPE 0x30000000

#define HSMP_MID_DREF_GROUP     0x00002300

#define HSMP_MID_DREF_T_UNDEF   0x00000000
#define HSMP_MID_DREF_T_INT32   0x00000001
#define HSMP_MID_DREF_T_FLOAT   0x00000002
#define HSMP_MID_DREF_T_DOUBLE  0x00000003
#define HSMP_MID_DREF_T_STR     0x00000004
#define HSMP_MID_DREF_T_AFLOAT  0x00000005
#define HSMP_MID_DREF_T_AINT32  0x00000006

#define HSMP_MID_DREF_F_DISABLE 0x00000000
#define HSMP_MID_DREF_F_ONCE    0x00000008
#define HSMP_MID_DREF_F_TICTAC  0x00000010
#define HSMP_MID_DREF_F_SECOND  0x00000018

#define HSMP_MID_DREF_O_RD_REQ  0x00000000
#define HSMP_MID_DREF_O_RD_REP  0x00000040
#define HSMP_MID_DREF_O_WR_REQ  0x00000080


#endif /* __HSMPMSG_H__ */
