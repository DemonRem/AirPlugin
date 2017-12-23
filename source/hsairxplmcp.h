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

#ifndef __HS__HSAIRXPLMCP__
#define __HS__HSAIRXPLMCP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "hsairxpl.h"

void hsairpl_mcp_update_datarefs(void);           /* Updates - looks up the datarefs */

void hsairpl_mcp_press_cmda(void);                /* Toggle */
void hsairpl_mcp_press_cmdb(void);                /* Toggle */

void hsairpl_mcp_press_cswa(void);                /* Toggle */
void hsairpl_mcp_press_cswb(void);                /* Toggle */

void hsairpl_mcp_press_co(void);                  /* Toggle */
void hsairpl_mcp_press_spdintv(void);             /* Toggle */
void hsairpl_mcp_press_altintv(void);             /* Toggle */

void hsairpl_mcp_press_n1(void);                  /* Toggle */
void hsairpl_mcp_press_spd(void);                 /* Toggle */
void hsairpl_mcp_press_lchg(void);                /* Toggle */
void hsairpl_mcp_press_vnav(void);                /* Toggle */

void hsairpl_mcp_press_hdg(void);                 /* Toggle */
void hsairpl_mcp_press_lnav(void);                /* Toggle */
void hsairpl_mcp_press_loc(void);                 /* Toggle */
void hsairpl_mcp_press_app(void);                 /* Toggle */

void hsairpl_mcp_press_alt(void);                 /* Toggle */
void hsairpl_mcp_press_vs(void);                  /* Toggle */

void  hsairpl_mcp_set_fd1(uint32_t v);            /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_fd1_mode(void);
void  hsairpl_mcp_set_fd2(uint32_t v);            /* 0 or !=0 , 0 is off */
float hsairpl_ap_get_fdroll(void);
float hsairpl_ap_get_fdpitch(void);


void hsairpl_mcp_set_atarm(uint32_t v);            /* 0 or !=0 , 0 is off */

void hsairpl_mcp_set_apdisc(uint32_t v);           /* 0 or !=0 , 0 is disconnect */

void hsairpl_mcp_set_bank_angle(uint32_t v);

uint32_t hsairpl_mcp_get_overspeed(void);

uint32_t hsairpl_mcp_get_cmda_led(void);          /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_cmdb_led(void);          /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_cwsa_led(void);          /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_cwsb_led(void);          /* 0 or !=0 , 0 is off */

uint32_t hsairpl_mcp_get_n1_led(void);            /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_spd_led(void);           /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_lchg_led(void);          /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_vnav_led(void);          /* 0 or !=0 , 0 is off */

uint32_t hsairpl_mcp_get_hdg_led(void);           /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_lnav_led(void);          /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_loc_led(void);           /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_app_led(void);           /* 0 or !=0 , 0 is off */

uint32_t hsairpl_mcp_get_alt_led(void);           /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_vs_led(void);            /* 0 or !=0 , 0 is off */

uint32_t hsairpl_mcp_get_ma1_led(void);           /* 0 or !=0 , 0 is off */
uint32_t hsairpl_mcp_get_ma2_led(void);            /* 0 or !=0 , 0 is off */

void hsairpl_mcp_set_crs1_dial(uint32_t v);  /* 000 to 359 */
uint32_t hsairpl_mcp_get_crs1_dial(void);

void hsairpl_mcp_set_crs2_dial(uint32_t v);  /* 000 to 359 */
uint32_t hsairpl_mcp_get_crs2_dial(void);

void hsairpl_mcp_set_speed_dial(float v);    /* 000 to 999 or mach */
float hsairpl_mcp_get_speed_dial(void);

double hsairpl_mcp_mach_from_tas(double tas,double temp);
double hsairpl_mcp_tas_from_mach(double mach,double temp);
void hsairpl_mcp_set_speed_is_mach(uint32_t v);
uint32_t hsairpl_mcp_get_speed_is_mach(void);

void hsairpl_mcp_set_hdg_dial(uint32_t v);   /* 000 to 359 */
uint32_t hsairpl_mcp_get_hdg_dial(void);

void hsairpl_mcp_set_alt_dial(uint32_t v);   /* 0 to 99999 */
uint32_t hsairpl_mcp_get_alt_dial(void);

void hsairpl_mcp_set_vvm_dial(int32_t v);   /* -9999 to 9999 */
int32_t hsairpl_mcp_get_vvm_dial(void);

/* 0=NAV1, 1=NAV2, 2=FMC */
void hsairpl_ap_set_source(uint32_t v);
void hsairpl_ap_toggle_source(void);
uint32_t hsairpl_ap_get_source(void);

/* STATUS are generally 0=off, 1=arm, 2=on */
uint32_t hsairpl_ap_speed_status(void);
uint32_t hsairpl_ap_vnav_status(void);
uint32_t hsairpl_ap_hdg_status(void);
uint32_t hsairpl_ap_vs_status(void);
uint32_t hsairpl_ap_alt_status(void);
uint32_t hsairpl_ap_lnav_status(void);
uint32_t hsairpl_ap_loc_status(void);
uint32_t hsairpl_ap_gs_status(void);
uint32_t hsairpl_ap_lc_status(void);
uint32_t hsairpl_ap_n1_status(void);

/* EFIS */
void hsairpl_efis1_press_wxr(void);
void hsairpl_efis1_press_sta(void);
void hsairpl_efis1_press_wpt(void);
void hsairpl_efis1_press_arpt(void);
void hsairpl_efis1_press_data(void);
void hsairpl_efis1_press_pos(void);
void hsairpl_efis1_press_terr(void);
void hsairpl_efis1_set_show(uint32_t v);
uint32_t hsairpl_efis1_get_show(void);

void hsairpl_efis1_press_ctr(void);
void hsairpl_efis1_press_tfc(void);

void hsairpl_efis1_press_fpv(void);
void hsairpl_efis1_press_mtrs(void);

void hsairpl_efis1_mins_press_rst(void);
void hsairpl_efis1_mins_select_radio(void);
void hsairpl_efis1_mins_select_baro(void);

void hsairpl_efis1_select_hpa(void);
void hsairpl_efis1_select_inhg(void);
void hsairpl_efis1_press_std(void);
void hsairpl_efis1_inc_baro(float v);
void hsairpl_efis1_dec_baro(float v);
void hsairpl_efis1_set_baro(float v);
float hsairpl_efis1_get_baro(void);

void hsairpl_efis1_inc_mins(float v);
void hsairpl_efis1_dec_mins(float v);

void hsairpl_efis1_set_vas1(uint32_t v);
uint32_t hsairpl_efis1_get_vas1(void);
void hsairpl_efis1_set_vas2(uint32_t v);
uint32_t hsairpl_efis1_get_vas2(void);

uint32_t hsairpl_efis1_get_mode(void);
void hsairpl_efis1_set_mode(uint32_t v);
void hsairpl_efis1_inc_mode(void);
void hsairpl_efis1_dec_mode(void);

void hsairpl_efis1_set_range(float v);
float hsairpl_efis1_get_range(void);

/* hsairpl_second_timer() is executed once per second from the main loop */
void hsairpl_mcp_second_timer(void);

#endif /* defined(__HS__HSAIRXPLMCP__) */
