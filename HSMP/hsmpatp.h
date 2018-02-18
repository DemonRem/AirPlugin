/*-
 * Copyright (c) 2013-2016 Haversine Ltd
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
 *
 * Created on:		2013.11.19
 *
 * This fileprovides compatibility with the old (<4.0) ATP protocol
 * by specifying its format.
 *
 */


#ifndef __HSMP_ATP__
#define __HSMP_ATP__

#include <sys/types.h>

/* ATP protocol message ID, this is the command type */

#define	ATP_POSREPORT	0x0000001   /* Position report */
#define	ATP_RTREPORT	0x0000002   /* Route report */
#define	ATP_POSWRITE	0x0000004   /* Position write */
#define	ATP_RTWRITE     0x0000008   /* Route send */
#define	ATP_UNICASTREQ	0x0000010   /* Switch to unicast if possible */

/* The maximum number of waypoints points in a route packet */
#define	ATP_RTPTSPERPACKET	50      /* 50x28byte points in a 1400 byte pkt */
#define	APT_RT_MAX_POINTS	2000    

/* The AirTrack Protocol version: this is a 4 byte integer where only the
 * lower 3 bytes are used and where each byte represents one part of the
 * version number, so version 2.1.3 becomes 0x00020103. To this the plugin
 * must have its plugin ID which is the 4th byte, a unique number defining the
 * plugin type.
 */

#define	ATP_CUR_VERSION		0x00030500		/* 3.5 */

/* The plugin ID types, to be OR'ed with the ATP version */

#define	PG_TYPE_MATP_XPG	0x08000000	/* Multicast ATP X-Plane Generic */
#define	PG_TYPE_MATP_XPM	0x09000000	/* Multicast ATP X-Plane Mac */
#define	PG_TYPE_MATP_XPL	0x0A000000	/* Multicast ATP X-Plane Linux */
#define	PG_TYPE_MATP_XPW	0x0B000000	/* Multicast ATP X-Plane Windows */

#define	PG_TYPE_MATP_FS9	0x10000000	/* Multicast MS FS2004 */
#define	PG_TYPE_MATP_FSX	0x11000000	/* Multicast MS FS X */
#define	PG_TYPE_MATP_ELITE	0x30000000	/* Multicast MS ELITE */


/* Some type definitions for windows */
#if defined(_WIN32)
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int socklen_t;
#endif

/* The packet header, all packets have it */
typedef struct atp_pkt_hdr_s {
    uint32_t msgid;		/* The message ID / type */
    uint32_t version;		/* The version number */
} atp_pkt_hdr_t;

/* Aircraft general data */
typedef struct atp_acf_s {
    unsigned char	tailno[8];	/* The aircraft tail number,e.g. "N-007"*/
    unsigned char	icao[8];	/* The aircraft icao number,e.g. "B738" */
    unsigned char	callsign[8];    /* The in-use call-sign, e.g. "LH1029" */
    unsigned char	source[16];	/* Source ID, e.g. "X-Plane" or "FSX" */
    uint32_t		lgear;		/* Landing gear, 0 is up, 1 is down */
    float		pbreak;		/* Parking break,1 is set, 0 is released*/
    float		flaps;		/* flaps,0=retracted to 1=fully extended*/
    float		spdbreak;	/* speed breaks,0=retracted,1=extended */
} atp_acf_t;

/* Helper constants for bit-wise operations */
#define	ATP_ACF_TAILNO		0
#define	ATP_ACF_ICAO		1
#define	ATP_ACF_CALLSIGN	2
#define	ATP_ACF_SOURCE		3
#define	ATP_ACF_LGEAR		4
#define	ATP_ACF_PBREAK		5
#define	ATP_ACF_FLAPS		6
#define	ATP_ACF_SPDBREAK	7
#define	ATP_ACF_FIELDS		8	/* Number of fields in ACF */


/* Aircraft speed data */
typedef struct atp_acfspeed_s {
    float			vso;    /* knots */
    float			vs;     /* knots */
    float			vfe;    /* knots */
    float			vno;    /* knots */
    float			vne;    /* knots */
    float			mmo;    /* mach */
} atp_acfspeed_t;

#define	ATP_ACFSPEED_VS0			0
#define	ATP_ACFSPEED_VS       2
#define	ATP_ACFSPEED_VFE			3
#define	ATP_ACFSPEED_VNO			4
#define	ATP_ACFSPEED_VNE			5
#define	ATP_ACFSPEED_MMO			6
#define ATP_ACFSPEED_FIELDS   7


/* Position variables */
typedef struct atp_pos_s {
    double lat;				/* Latitude */
    double lon;				/* Longitude */
    double elev;			/* Elevation in metres */
    float  theta;			/* The pitch relative to the plane
                             normal to the Y axis in degrees */
    float  phi;				/* The roll of the aircraft in degs */
    float  psi;				/* The true heading in degress from Z */
    float  free;			/* Not used at the moment, was mpsi */
    float  mvar;			/* The magnetic variation */
    float  alpha;			/* The pitch relative to the flown path
                             (angle of attack) in degrees */
    float  beta;			/* The heading relative to the flown path
                             (yaw) in degrees */
    float  gs;				/* Ground speed, metres/sec */
    float  ias;				/* Indicated air speed taking into account
                             air density and wind direction, kias */
    float  mach;			/* Mach Speed */
    float  tas;				/* True air speed, not considering wind or
                             air density, in metres/sec */
    float  altimeter;			/* The altimeter setting */
    float  vvifpm;			/* Verfical speed in fpm */
    float  wspeed;			/* Wind speed , knots */
    float  wdir;			/* Wind direction, MAG degrees */
    float  otemp;			/* Outside temperature in degs C */
} atp_pos_t;


/* Helper constants for bit-wise operations */
#define	ATP_POS_LAT         0
#define	ATP_POS_LON         1
#define	ATP_POS_ELEV		2
#define	ATP_POS_THETA		3
#define	ATP_POS_PHI         4
#define	ATP_POS_PSI         5
#define	ATP_POS_FREE		6
#define	ATP_POS_MVAR		7
#define	ATP_POS_ALPHA		8
#define	ATP_POS_BETA		9
#define	ATP_POS_GS          10
#define	ATP_POS_IAS         11
#define	ATP_POS_MACH		12
#define	ATP_POS_TAS         13
#define	ATP_POS_ALTIM		14
#define	ATP_POS_VVIFPM		15
#define	ATP_POS_WSPEED		16
#define	ATP_POS_WDIR		17
#define	ATP_POS_OTEMP		18
#define	ATP_POS_FIELDS		19	/* Number of fields in POS */


/* Auto-Pilot variables */
typedef struct atp_ap_s {
    float	altdial;	/* Altitude dial in feet */
    float	holddial;	/* Altitude hold in feet */
    float	vnavdial;	/* Altitude target in VNAV mode in feet */
    float	speeddial;	/* Air speed dial in knots or mach */
    float	hdgdial;	/* Heading dial in magnetic degrees */
    float	vspeeddial;	/* Vertical speed dial in feet/minute */
    int		fdmode;		/* Flight director; 0=off, 1=on, 2=auto */
    int		speedismach;	/* Air speed is in mach, or not */
    int		speedstatus;	/* SPEED status, 0=off, 1=armed, 2=captured */
    int		lnavstatus;	/* LNAV status, 0=off, 1=armed, 2=captured */
    int		vnavstatus;	/* VNAV status, 0=off, 1=armed, 2=captured */
    int		hdgstatus;	/* HDG status, 0=off, 1=armed, 2=captured */
    int		locstatus;	/* LOC status, 0=off, 1=armed, 2=captured */
    int		altstatus;	/* ALT status, 0=off, 1=armed, 2=captured */
    int		vsstatus;	/* VS status, 0=off, 1=armed, 2=captured */
    int		gsstatus;	/* GS status, 0=off, 1=armed, 2=captured */
    float	obs1p;		/* OBS1 for pilot */
    int		lcstatus;	/* Level-change status */
    float	obs2p;		/* OBS2 for pilot */
    int		n1status;	/* N1 status */
    int		apsource;	/* 0=NAV1, 1=NAV2, 2=FMC/GPS */
    int	  mfdmode;	/* MFD Show Mode, 0=APP,1=VOR,2=MAP,3=NAV,4=PLN */
    int	  mfdshow;	/* Bitmask of MFD show sets */
    float mfdrange;	/* MFD range in nm */
} atp_ap_t;

#define	EFIS_SHOW_WXR	0x00000001
#define	EFIS_SHOW_TCA	0x00000002
#define	EFIS_SHOW_APT	0x00000004
#define	EFIS_SHOW_WPT	0x00000008
#define	EFIS_SHOW_VOR	0x00000010
#define	EFIS_SHOW_NDB	0x00000020

/* Helper constants for bit-wise operations */
#define	ATP_AP_ALTDIAL		0
#define	ATP_AP_HOLDDIAL		1
#define	ATP_AP_VNAVDIAL		2
#define	ATP_AP_SPDDIAL		3
#define	ATP_AP_HDGDIAL		4
#define	ATP_AP_VSPEEDDIAL	5
#define	ATP_AP_FDMODE		6
#define	ATP_AP_SPEEDISMACH	7
#define	ATP_AP_SPEEDSTATUS	8
#define	ATP_AP_LNAVSTATUS	9
#define	ATP_AP_VNAVSTATUS	10
#define	ATP_AP_HDGSTATUS	11
#define	ATP_AP_LOCSTATUS	12
#define	ATP_AP_ALTSTATUS	13
#define	ATP_AP_VSSSTATUS	14
#define	ATP_AP_GSSTATUS		15
#define	ATP_AP_OBS1P		16
#define	ATP_AP_LCSTATUS		17
#define	ATP_AP_OBS2P		18
#define	ATP_AP_N1STATUS		19
#define	ATP_AP_APSOURCE		20
#define	ATP_AP_MFDMODE		21
#define	ATP_AP_MFDRANGE		22
#define	ATP_AP_MFDSHOW		23
#define	ATP_AP_FIELDS		24	/* Number of fields in AP */

typedef struct atp_rad_s {
    
    int	old_nav1pwr;	/* NAV1 power */
    int	old_nav2pwr;	/* NAV2 power */
    int	old_com1pwr;	/* COM1 power */
    int	old_com2pwr;	/* COM1 power */
    int	old_adf1pwr;	/* ADF1 power */
    int	old_adf2pwr;	/* ADF2 power */
    int	old_dmepwr;		/* DME power */
    int	old_gpspwr;		/* GPS power */
    int	nav1freq;	/* NAV1 active frequency */
    int	nav2freq;	/* NAV2 active frequency */
    int	com1freq;	/* COM1 active frequency */
    int	com2freq;	/* COM2 active frequency */
    int	adf1freq;	/* ADF1 active frequency */
    int	adf2freq;	/* ADF2 active frequency */
    int	dmefreq;	/* DME active frequency */
    int	nav1sfreq;	/* NAV1 standby frequency */
    int	nav2sfreq;	/* NAV2 standby frequency */
    int	com1sfreq;	/* COM1 standby frequency */
    int	com2sfreq;	/* COM2 standby frequency */
    int	adf1sfreq;	/* ADF1 standby frequency */
    int	adf2sfreq;	/* ADF2 standby frequency */
    int	dmesfreq;	/* DME standby frequency */
    int	xpderc;		/* Transponder code */
    int	xpdermode;	/* Transponder mode 0=off, 1=stdby, 2=on, 3=test */
    int	xpdersqk;	/* Wether we are squawking */
    int	n1sel;		/* 0=OFF, 1=NAV1, 2=ADF1 */
    int	n2sel;		/* 0=OFF, 1=NAV2, 2=ADF2 */
    
} atp_rad_t;

/* Helper constants for bit-wise operations */
#define	ATP_RAD_NAV1PWR		0
#define	ATP_RAD_NAV2PWR		1
#define	ATP_RAD_COM1PWR		2
#define	ATP_RAD_COM2PWR		3
#define	ATP_RAD_ADF1PWR		4
#define	ATP_RAD_ADF2PWR		5
#define	ATP_RAD_DMEPWR		6
#define	ATP_RAD_GPSPWR		7
#define	ATP_RAD_NAV1FREQ	8
#define	ATP_RAD_NAV2FREQ	9
#define	ATP_RAD_COM1FREQ	10
#define	ATP_RAD_COM2FREQ	11
#define	ATP_RAD_ADF1FREQ	12
#define	ATP_RAD_ADF2FREQ	13
#define	ATP_RAD_DMEFREQ		14
#define	ATP_RAD_NAV1SFREQ	15
#define	ATP_RAD_NAV2SFREQ	16
#define	ATP_RAD_COM1SFREQ	17
#define	ATP_RAD_COM2SFREQ	18
#define	ATP_RAD_ADF1SFREQ	19
#define	ATP_RAD_ADF2SFREQ	20
#define	ATP_RAD_DMESFREQ	21
#define	ATP_RAD_XPDERC		22
#define	ATP_RAD_XPDERMODE	23
#define	ATP_RAD_XPDERSQK	24
#define	ATP_RAD_N1SEL		25
#define	ATP_RAD_N2SEL		26
#define	ATP_RAD_FIELDS		27	/* Number of fields in RAD */

/* Other data */
typedef struct atp_other_s {
    float zulusecs;       /* zulu time secs since midnight */
    float localsecs;      /* local time secs since midnight */
    float	maxfuel;        /* sim/aircraft/weight/acf_m_fuel_tot in lb */
    float totfuel;        /* total fuel weight in kgs */
    float payloadweight;  /* Payload weight in kgs */
    float totalweight;    /* Total weight in kgs */
    float	fdpitch;        /* FD suggested pitch */
    float	fdroll;         /* FD suggested roll */
    float	msl;            /* Elevation MSL in metres*/
    float	agl;            /* Elevation AGL in metres */
    float rho;            /* The density of the air in kg/cubic meters */
    float sigma;          /* the atmospheric density as a ratio compared to sea level */
    float speed_sound_ms; /* This is the speed of sound in meters/second at the plane's location.*/
    float ihg;            /* This is the barometric pressure at the point the current flight is at, 29.92 + ... */
    
} atp_other_t;

/* The position report message */
typedef struct atp_data_s {
    
    atp_pkt_hdr_t hdr;	/* The header */
    
    atp_acf_t acf;		/* Aircraft general data */
    atp_pos_t pos;		/* Position data */
    atp_ap_t ap;		/* Auto Pilot */
    atp_rad_t rad;		/* Radio data */
    
    /* Bitmask set fields, must be after data */
    uint32_t        acf_set;
    uint32_t        pos_set;
    uint32_t        ap_set;
    uint32_t        rad_set;
    uint32_t        acfspeed_set;
    uint32_t        other_set;
    
    atp_acfspeed_t  acfspeed;
    atp_other_t     other;
} atp_data_t;


/* Information used for sending route plans */

#define NAVDB_UNDEF             0x00000000
#define NAVDB_APT               0x00000001
#define NAVDB_NDB               0x00000002
#define NAVDB_VOR               0x00000004
#define NAVDB_ILS               0x00000010
#define NAVDB_FIX               0x00000200
#define NAVDB_LATLON            0x00000800
#define NAVDB_CWP               0x00002000
#define NAVDB_LCT               0x00004000
#define NAVDB_RW                0x00008000
#define NAVDB_PROC              0x01C00000
#define NAVDB_AIRWAY            0x00040000

/* The route point */
typedef struct atp_route_pt_s {
    uint32_t ptype;   /* One of NAVDB_* */
    char	pname[8];		/* A string with the point ID name */
    float	lat;			  /* Latitude */
    float	lon;			  /* Longitude */
    float	elev;			  /* Elevation in feet */
    uint32_t	ta;			/* Time of arrival */
} atp_route_pt_t;

/* The route message with multiple points */
typedef struct atp_route_preamb_s {
    atp_pkt_hdr_t hdr;			/* The packet header */
    int32_t 	nopoints;		  /* Number of waypoints in plan */
    int32_t 	cleg;			    /* The current waypoint */
    int32_t	segment;		    /* The sequence number, 0...+ */
    int32_t	segpoints;
} atp_route_preamb_t;

typedef struct atp_route_s {
    atp_route_preamb_t	preamb;
    atp_route_pt_t	pts[APT_RT_MAX_POINTS]; /* Sequence of waypoints */
} atp_route_t;

typedef struct atp_route_pkt_s {
    atp_route_preamb_t	preamb;
    atp_route_pt_t	pts[ATP_RTPTSPERPACKET]; /* Sequence of waypoints */
} atp_route_pkt_t;


#endif /* __HSMP_ATP__ */
