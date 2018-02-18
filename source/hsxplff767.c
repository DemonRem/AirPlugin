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
 * Flight factor 767
 *
 */

#include "hsxplff767.h"
#include "hsxpl.h"
#include "hsxpldatarefs.h"
#include "hsxplmisc.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;
extern hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;

void hsxpl_set_p767_datarefs(void) {

  if(XPLMFindDataRef("757Avionics/CDU/exec") == NULL) return;
  if(strncmp(hsxpl_acf_icao(),"B763",7)) return;

  hsxpl_plane_type = HSXPL_PLANE_FF767;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_PB757;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_fmc.exec_light_on=XPLMFindDataRef("war/main/58");
  hsxpl_fmc.dspy_light_on=XPLMFindDataRef("war/main/51");
  hsxpl_fmc.fail_light_on=XPLMFindDataRef("war/main/52");
  hsxpl_fmc.msg_light_on=XPLMFindDataRef("war/main/53");
  hsxpl_fmc.ofst_light_on=XPLMFindDataRef("war/main/54");

  hsxpl_fmc.key_0=XPLMFindDataRef("757Avionics/CDU/0");
  hsxpl_fmc.key_1=XPLMFindDataRef("757Avionics/CDU/1");
  hsxpl_fmc.key_2=XPLMFindDataRef("757Avionics/CDU/2");
  hsxpl_fmc.key_3=XPLMFindDataRef("757Avionics/CDU/3");
  hsxpl_fmc.key_4=XPLMFindDataRef("757Avionics/CDU/4");
  hsxpl_fmc.key_5=XPLMFindDataRef("757Avionics/CDU/5");
  hsxpl_fmc.key_6=XPLMFindDataRef("757Avionics/CDU/6");
  hsxpl_fmc.key_7=XPLMFindDataRef("757Avionics/CDU/7");
  hsxpl_fmc.key_8=XPLMFindDataRef("757Avionics/CDU/8");
  hsxpl_fmc.key_9=XPLMFindDataRef("757Avionics/CDU/9");
  hsxpl_fmc.key_dot=XPLMFindDataRef("757Avionics/CDU/point");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("757Avionics/CDU/plusminus");

  hsxpl_fmc.key_a=XPLMFindDataRef("757Avionics/CDU/A");
  hsxpl_fmc.key_b=XPLMFindDataRef("757Avionics/CDU/B");
  hsxpl_fmc.key_c=XPLMFindDataRef("757Avionics/CDU/C");
  hsxpl_fmc.key_d=XPLMFindDataRef("757Avionics/CDU/D");
  hsxpl_fmc.key_e=XPLMFindDataRef("757Avionics/CDU/E");
  hsxpl_fmc.key_f=XPLMFindDataRef("757Avionics/CDU/F");
  hsxpl_fmc.key_g=XPLMFindDataRef("757Avionics/CDU/G");
  hsxpl_fmc.key_h=XPLMFindDataRef("757Avionics/CDU/H");
  hsxpl_fmc.key_i=XPLMFindDataRef("757Avionics/CDU/I");
  hsxpl_fmc.key_j=XPLMFindDataRef("757Avionics/CDU/J");
  hsxpl_fmc.key_k=XPLMFindDataRef("757Avionics/CDU/K");
  hsxpl_fmc.key_l=XPLMFindDataRef("757Avionics/CDU/L");
  hsxpl_fmc.key_m=XPLMFindDataRef("757Avionics/CDU/M");
  hsxpl_fmc.key_n=XPLMFindDataRef("757Avionics/CDU/N");
  hsxpl_fmc.key_o=XPLMFindDataRef("757Avionics/CDU/O");
  hsxpl_fmc.key_p=XPLMFindDataRef("757Avionics/CDU/P");
  hsxpl_fmc.key_q=XPLMFindDataRef("757Avionics/CDU/Q");
  hsxpl_fmc.key_r=XPLMFindDataRef("757Avionics/CDU/R");
  hsxpl_fmc.key_s=XPLMFindDataRef("757Avionics/CDU/S");
  hsxpl_fmc.key_t=XPLMFindDataRef("757Avionics/CDU/T");
  hsxpl_fmc.key_u=XPLMFindDataRef("757Avionics/CDU/U");
  hsxpl_fmc.key_v=XPLMFindDataRef("757Avionics/CDU/V");
  hsxpl_fmc.key_w=XPLMFindDataRef("757Avionics/CDU/W");
  hsxpl_fmc.key_x=XPLMFindDataRef("757Avionics/CDU/X");
  hsxpl_fmc.key_y=XPLMFindDataRef("757Avionics/CDU/Y");
  hsxpl_fmc.key_z=XPLMFindDataRef("757Avionics/CDU/Z");
  hsxpl_fmc.key_space=XPLMFindDataRef("757Avionics/CDU/space");
  hsxpl_fmc.key_slash=XPLMFindDataRef("757Avionics/CDU/slash");
  hsxpl_fmc.key_clr=XPLMFindDataRef("757Avionics/CDU/clear");
  hsxpl_fmc.key_del=XPLMFindDataRef("757Avionics/CDU/delete");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("757Avionics/CDU/LLSK1");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("757Avionics/CDU/LLSK2");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("757Avionics/CDU/LLSK3");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("757Avionics/CDU/LLSK4");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("757Avionics/CDU/LLSK5");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("757Avionics/CDU/LLSK6");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("757Avionics/CDU/RLSK1");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("757Avionics/CDU/RLSK2");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("757Avionics/CDU/RLSK3");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("757Avionics/CDU/RLSK4");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("757Avionics/CDU/RLSK5");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("757Avionics/CDU/RLSK6");

  hsxpl_fmc.key_init=XPLMFindDataRef("757Avionics/CDU/init_ref");
  hsxpl_fmc.key_rte=XPLMFindDataRef("757Avionics/CDU/rte");
  hsxpl_fmc.key_clb=XPLMFindDataRef("757Avionics/CDU/clb");
  hsxpl_fmc.key_crz=XPLMFindDataRef("757Avionics/CDU/crz");
  hsxpl_fmc.key_des=XPLMFindDataRef("757Avionics/CDU/des");

  hsxpl_fmc.key_dir=XPLMFindDataRef("757Avionics/CDU/dir");
  hsxpl_fmc.key_legs=XPLMFindDataRef("757Avionics/CDU/legs");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("757Avionics/CDU/dep_arr");
  hsxpl_fmc.key_hold=XPLMFindDataRef("757Avionics/CDU/hold");
  hsxpl_fmc.key_prog=XPLMFindDataRef("757Avionics/CDU/prog");
  hsxpl_fmc.key_exec=XPLMFindDataRef("757Avionics/CDU/exec");

  hsxpl_fmc.key_menu=XPLMFindDataRef("757Avionics/CDU/mcdu_menu");
  hsxpl_fmc.key_fix=XPLMFindDataRef("757Avionics/CDU/fix");
  hsxpl_fmc.key_prevpage=XPLMFindDataRef("757Avionics/CDU/prev_page");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("757Avionics/CDU/next_page");

}
