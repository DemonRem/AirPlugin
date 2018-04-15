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
 *
 * Description:     The X-Plane plugin for the Haversine Message Protocol
 *
 */

#ifndef __HS_HSAIRXPL_H__
#define __HS_HSAIRXPL_H__

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <io.h>
#else
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#endif

#if IBM
#include <windows.h>
#else
#include <termios.h>
#include <syslog.h>
#include <sys/select.h>
#endif

/* Some type definitions for windows */
#if defined(_WIN32)
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int socklen_t;
#endif

/* The X-Plane SDK include files */

#include <XPLMProcessing.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMNavigation.h>
#include <XPLMMenus.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>

#include <XPWidgets.h>
#include <XPStandardWidgets.h>
#include <XPUIGraphics.h>
#include <XPLMPlugin.h>

#define HSAIRXPL_VERSION    "4.7.7"

/* Plane type, set in the hsxpl_plane_type global var */
#define HSXPL_PLANE_XPLANE  0x00000000  /* X-Plane default plane */
#define HSXPL_PLANE_X737    0x00000001  /* The x737 from EADT */
#define HSXPL_PLANE_A320N   0x00000002  /* A320 from JAR design */
#define HSXPL_PLANE_FF757   0x00000004  /* 757 from FlightFactor */
#define HSXPL_PLANE_FF777   0x00000008  /* 777 from FlightFactor */
#define HSXPL_PLANE_PCRJ200 0x00000010  /* CRJ 200 from Javier Rollon */
#define HSXPL_PLANE_A320Q   0x00000100  /* A320 from QPAC */
#define HSXPL_PLANE_A330J   0x00000200  /* A330 from JAR design, not yet used */
#define HSXPL_PLANE_FF767   0x00000400  /* 767 from FlightFactor */
#define HSXPL_PLANE_IX733   0x00000800  /* IXEG 737 Classic */
#define HSXPL_PLANE_Z738    0x00001000  /* 737-800 modified by zibo */
#define HSXPL_PLANE_B738    0x00002000  /* X-Plane 11 default 737-800 */

float hsxpl_runtime(float                inElapsedSinceLastCall,
                    float                inElapsedTimeSinceLastFlightLoop,
                    int                  inCounter,
                    void *               inRefcon);

void hsxpl_hsmp_message_callback(uint32_t mid,void *data,struct sockaddr_in *from);

void hsxpl_send_subsecond_data(void);
void hsxpl_send_second_data(void);

void hsxpl_set_datarefs(void);

#endif  /* __HS_HSAIRXPL_H__ */
