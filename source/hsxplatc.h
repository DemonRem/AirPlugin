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
 */


#ifndef __HS_AIRXPLATC_H__
#define __HS_AIRXPLATC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "hsxpl.h"

/* #define HSAIRPL_ATC_MAX_ACF    25  */ /* To fit 25 hsmp_atc_pos_t in 1440 bytes */
#define HSAIRPL_ATC_MAX_ACF    20     /* X-Plane limit */

/* The available sources */
#define HSAIRPL_ATC_SOURCE_UNDEFINED      0
#define HSAIRPL_ATC_SOURCE_XPLANE_AI      1
#define HSAIRPL_ATC_SOURCE_WORLDTRAFFIC   2
#define HSAIRPL_ATC_SOURCE_XLIFE          3
#define HSAIRPL_ATC_SOURCE_VATSIM         4

#define HSAIRPL_ATC_PG_SIGNATURE_VATSIM          "vatsim.protodev.clients.xsquawkbox"
#define HSAIRPL_ATC_PG_SIGNATURE_XLIFE           "jardesign.x-life"
#define HSAIRPL_ATC_PG_SIGNATURE_WORLDTRAFFIC    "cjs.util.worldtraffic"

int hsairpl_atc_total_aircrafts(void);
int hsairpl_atc_active_aircrafts(void);
int hsairpl_atc_wt_total_aircrafts(void);
int hsairpl_atc_wt_active_aircrafts(void);
int hsairpl_atc_xpai_total_aircrafts(void);
int hsairpl_atc_xpai_active_aircrafts(void);
int hsairpl_atc_plugin_id(void);
void hsairpl_atc_update_datarefs(void);
void hsairpl_atc_send_traffic(void);
void hsairpl_atc_xpai_send_traffic(void);
void hsairpl_atc_wt_send_traffic(void);
void hsairpl_atc_xl_send_traffic(void);
int hsairpl_atc_xl_get_tfc(void *inRefcon,void *outValue,int inOffset,int inMaxLength);
void	hsairpl_atc_xl_set_tfc(void *inRefcon,void *inValue,int inOffset,int inLength);

#endif /* __HS_AIRXPLATC_H__ */
