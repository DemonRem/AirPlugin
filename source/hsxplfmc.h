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
 * FMC specifics.
 *
 */

#ifndef __HSAIRXPLFMC__
#define __HSAIRXPLFMC__

#include <stdio.h>
#include <XPLMDataAccess.h>

#include "hsmpnet.h"

/* A typedef to hold all standard FMC datarefs (for AirFMC) */
typedef struct hsxpl_fmc_s {

  XPLMDataRef key_load;;
  XPLMDataRef key_save;

  XPLMDataRef key_apt;
  XPLMDataRef key_wpt;
  XPLMDataRef key_vor;
  XPLMDataRef key_ndb;
  XPLMDataRef key_latlon;


  XPLMDataRef key_lk1;
  XPLMDataRef key_lk2;
  XPLMDataRef key_lk3;
  XPLMDataRef key_lk4;
  XPLMDataRef key_lk5;
  XPLMDataRef key_lk6;

  XPLMDataRef key_rk1;
  XPLMDataRef key_rk2;
  XPLMDataRef key_rk3;
  XPLMDataRef key_rk4;
  XPLMDataRef key_rk5;
  XPLMDataRef key_rk6;

  XPLMDataRef key_1;
  XPLMDataRef key_2;
  XPLMDataRef key_3;
  XPLMDataRef key_4;
  XPLMDataRef key_5;
  XPLMDataRef key_6;
  XPLMDataRef key_7;
  XPLMDataRef key_8;
  XPLMDataRef key_9;
  XPLMDataRef key_0;
  XPLMDataRef key_a;
  XPLMDataRef key_b;
  XPLMDataRef key_c;
  XPLMDataRef key_d;
  XPLMDataRef key_e;
  XPLMDataRef key_f;
  XPLMDataRef key_g;
  XPLMDataRef key_h;
  XPLMDataRef key_i;
  XPLMDataRef key_j;
  XPLMDataRef key_k;
  XPLMDataRef key_l;
  XPLMDataRef key_m;
  XPLMDataRef key_n;
  XPLMDataRef key_o;
  XPLMDataRef key_p;
  XPLMDataRef key_q;
  XPLMDataRef key_r;
  XPLMDataRef key_s;
  XPLMDataRef key_t;
  XPLMDataRef key_u;
  XPLMDataRef key_v;
  XPLMDataRef key_w;
  XPLMDataRef key_x;
  XPLMDataRef key_y;
  XPLMDataRef key_z;
  XPLMDataRef key_slash;
  XPLMDataRef key_dot;
  XPLMDataRef key_space;
  XPLMDataRef key_plusminus;
  XPLMDataRef key_clr;
  XPLMDataRef key_del;

  XPLMDataRef key_init;
  XPLMDataRef key_rte;
  XPLMDataRef key_deparr;
  XPLMDataRef key_clb;
  XPLMDataRef key_crz;
  XPLMDataRef key_des;
  XPLMDataRef key_dir;
  XPLMDataRef key_legs;
  XPLMDataRef key_hold;
  XPLMDataRef key_prog;
  XPLMDataRef key_altn;
  XPLMDataRef key_exec;
  XPLMDataRef key_menu;
  XPLMDataRef key_fix;
  XPLMDataRef key_prevpage;
  XPLMDataRef key_nextpage;

  XPLMDataRef key_ovfy;
  XPLMDataRef key_perf;
  XPLMDataRef key_airp;
  XPLMDataRef key_radnav;
  XPLMDataRef key_fpln;
  XPLMDataRef key_sfpln;
  XPLMDataRef key_fuel;
  XPLMDataRef key_data;
  XPLMDataRef key_blank;

  XPLMDataRef key_left;
  XPLMDataRef key_right;
  XPLMDataRef key_up;
  XPLMDataRef key_down;

  XPLMDataRef key_fmccom;
  XPLMDataRef key_n1limit;
  XPLMDataRef key_atc;

  XPLMDataRef key_index;
  XPLMDataRef key_radio;
  XPLMDataRef key_mfdmenu;

  XPLMDataRef key_lnav;
  XPLMDataRef key_vnav;
  XPLMDataRef key_msg;
  XPLMDataRef key_ap;

  XPLMDataRef exec_light_on;
  XPLMDataRef dspy_light_on;
  XPLMDataRef fail_light_on;
  XPLMDataRef msg_light_on;
  XPLMDataRef ofst_light_on;

} hsxpl_fmc_t;

void hsxpl_build_fmc_screen_line(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size,uint32_t colour);

void hsxpl_send_fmc_data(void);
void hsxpl_fmc_press_key(uint32_t mid,XPLMDataRef k);
void hsxpl_mcdu_toggle(void);

#endif /* __HSAIRXPLFMC__ */
