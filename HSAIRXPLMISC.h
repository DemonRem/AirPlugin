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
 */

#ifndef HSAIRXPLMISC_h
#define HSAIRXPLMISC_h

#if defined(_WIN32)
#include <Winsock2.h>
#include <Ws2tcpip.h>
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
#define IPONAV_LATLON_FMT_DMM       1
#define IPONAV_LATLON_FMT_DMS       2
#define IPONAV_LATLON_FMT_DDD       3

void IPONAVLatitudeCStringFor(double lat,char *latStr,int fmt);
void IPONAVLongitudeCStringFor(double lon,char *lonStr,int fmt);

char *hsxpl_strqentry(int32_t element,char *str,char delim,char *entry,char quotechar);
char *hsxpl_strentry(int32_t element,char *str,char delim,char *entry);

/* hsxpl_point_name_for_position() creates and returns a name for a
 * point based on its latitude and longitude. */
void hsxpl_point_name_for_position(double lat,double lon,char *pName);

#define HSXPLDEBUG_ERROR    1
#define HSXPLDEBUG_ACTION   2
#define HSXPLDEBUG_INFO     3
#define HSXPLDEBUG_LOOP     4
#define HSXPLDEBUG_DETAIL   5

void hsxpl_log(int level,char *logstr);
void hsxpl_log_str(char *logstr);

#endif /* HSAIRXPLMISC_h */
