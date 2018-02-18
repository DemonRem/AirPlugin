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
 * Flight factor 777
 *
 */

#include "hsxplff777.h"
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

void hsxpl_set_p777_datarefs(void) {

  char acficao[8]; memset(acficao,0,8);
  if(hsxpl_xplane_datarefs.acficao!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(hsxpl_xplane_datarefs.tailno!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.tailno,actfailno,0,7);

  int isT7=0;
  if(XPLMFindDataRef("T7Avionics/CDU/exec") != NULL) {
    if(!strncmp(acficao,"B77L",7) || !strncmp(acficao,"B77W",7) || !strncmp(acficao,"B772",7)) {
      /*if(!strncmp(actfailno,"YNKV41",7)) { */
      isT7=1;
      /* } */
    }
  }
  if(!isT7) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_FF777;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_PB777;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_fmc.exec_light_on=XPLMFindDataRef("lamps/208");
  hsxpl_fmc.dspy_light_on=XPLMFindDataRef("lamps/205");
  hsxpl_fmc.msg_light_on =XPLMFindDataRef("lamps/206");
  hsxpl_fmc.ofst_light_on=XPLMFindDataRef("lamps/207");

  hsxpl_fmc.key_0=XPLMFindDataRef("T7Avionics/CDU/0");
  hsxpl_fmc.key_1=XPLMFindDataRef("T7Avionics/CDU/1");
  hsxpl_fmc.key_2=XPLMFindDataRef("T7Avionics/CDU/2");
  hsxpl_fmc.key_3=XPLMFindDataRef("T7Avionics/CDU/3");
  hsxpl_fmc.key_4=XPLMFindDataRef("T7Avionics/CDU/4");
  hsxpl_fmc.key_5=XPLMFindDataRef("T7Avionics/CDU/5");
  hsxpl_fmc.key_6=XPLMFindDataRef("T7Avionics/CDU/6");
  hsxpl_fmc.key_7=XPLMFindDataRef("T7Avionics/CDU/7");
  hsxpl_fmc.key_8=XPLMFindDataRef("T7Avionics/CDU/8");
  hsxpl_fmc.key_9=XPLMFindDataRef("T7Avionics/CDU/9");
  hsxpl_fmc.key_dot=XPLMFindDataRef("T7Avionics/CDU/point");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("T7Avionics/CDU/plusminus");

  hsxpl_fmc.key_a=XPLMFindDataRef("T7Avionics/CDU/A");
  hsxpl_fmc.key_b=XPLMFindDataRef("T7Avionics/CDU/B");
  hsxpl_fmc.key_c=XPLMFindDataRef("T7Avionics/CDU/C");
  hsxpl_fmc.key_d=XPLMFindDataRef("T7Avionics/CDU/D");
  hsxpl_fmc.key_e=XPLMFindDataRef("T7Avionics/CDU/E");
  hsxpl_fmc.key_f=XPLMFindDataRef("T7Avionics/CDU/F");
  hsxpl_fmc.key_g=XPLMFindDataRef("T7Avionics/CDU/G");
  hsxpl_fmc.key_h=XPLMFindDataRef("T7Avionics/CDU/H");
  hsxpl_fmc.key_i=XPLMFindDataRef("T7Avionics/CDU/I");
  hsxpl_fmc.key_j=XPLMFindDataRef("T7Avionics/CDU/J");
  hsxpl_fmc.key_k=XPLMFindDataRef("T7Avionics/CDU/K");
  hsxpl_fmc.key_l=XPLMFindDataRef("T7Avionics/CDU/L");
  hsxpl_fmc.key_m=XPLMFindDataRef("T7Avionics/CDU/M");
  hsxpl_fmc.key_n=XPLMFindDataRef("T7Avionics/CDU/N");
  hsxpl_fmc.key_o=XPLMFindDataRef("T7Avionics/CDU/O");
  hsxpl_fmc.key_p=XPLMFindDataRef("T7Avionics/CDU/P");
  hsxpl_fmc.key_q=XPLMFindDataRef("T7Avionics/CDU/Q");
  hsxpl_fmc.key_r=XPLMFindDataRef("T7Avionics/CDU/R");
  hsxpl_fmc.key_s=XPLMFindDataRef("T7Avionics/CDU/S");
  hsxpl_fmc.key_t=XPLMFindDataRef("T7Avionics/CDU/T");
  hsxpl_fmc.key_u=XPLMFindDataRef("T7Avionics/CDU/U");
  hsxpl_fmc.key_v=XPLMFindDataRef("T7Avionics/CDU/V");
  hsxpl_fmc.key_w=XPLMFindDataRef("T7Avionics/CDU/W");
  hsxpl_fmc.key_x=XPLMFindDataRef("T7Avionics/CDU/X");
  hsxpl_fmc.key_y=XPLMFindDataRef("T7Avionics/CDU/Y");
  hsxpl_fmc.key_z=XPLMFindDataRef("T7Avionics/CDU/Z");
  hsxpl_fmc.key_space=XPLMFindDataRef("T7Avionics/CDU/space");
  hsxpl_fmc.key_slash=XPLMFindDataRef("T7Avionics/CDU/slash");
  hsxpl_fmc.key_clr=XPLMFindDataRef("T7Avionics/CDU/clear");
  hsxpl_fmc.key_del=XPLMFindDataRef("T7Avionics/CDU/delete");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("T7Avionics/CDU/LLSK1");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("T7Avionics/CDU/LLSK2");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("T7Avionics/CDU/LLSK3");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("T7Avionics/CDU/LLSK4");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("T7Avionics/CDU/LLSK5");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("T7Avionics/CDU/LLSK6");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("T7Avionics/CDU/RLSK1");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("T7Avionics/CDU/RLSK2");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("T7Avionics/CDU/RLSK3");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("T7Avionics/CDU/RLSK4");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("T7Avionics/CDU/RLSK5");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("T7Avionics/CDU/RLSK6");

  hsxpl_fmc.key_init=XPLMFindDataRef("T7Avionics/CDU/init_ref");
  hsxpl_fmc.key_rte=XPLMFindDataRef("T7Avionics/CDU/rte");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("T7Avionics/CDU/dep_arr");
  hsxpl_fmc.key_altn=XPLMFindDataRef("T7Avionics/CDU/altn");
  hsxpl_fmc.key_vnav=XPLMFindDataRef("T7Avionics/CDU/vnav");

  hsxpl_fmc.key_fix=XPLMFindDataRef("T7Avionics/CDU/fix");
  hsxpl_fmc.key_legs=XPLMFindDataRef("T7Avionics/CDU/legs");
  hsxpl_fmc.key_hold=XPLMFindDataRef("T7Avionics/CDU/hold");
  hsxpl_fmc.key_fmccom=XPLMFindDataRef("T7Avionics/CDU/fmc_comm");
  hsxpl_fmc.key_prog=XPLMFindDataRef("T7Avionics/CDU/prog");
  hsxpl_fmc.key_exec=XPLMFindDataRef("T7Avionics/CDU/exec");

  hsxpl_fmc.key_menu=XPLMFindDataRef("T7Avionics/CDU/mcdu_menu");
  hsxpl_fmc.key_radnav=XPLMFindDataRef("T7Avionics/CDU/nav_rad");

  hsxpl_fmc.key_prevpage=XPLMFindDataRef("T7Avionics/CDU/prev_page");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("T7Avionics/CDU/next_page");

}

/* We only send the FMC type and some annunciators as the screen
 * is retrieved throught TCP port 18387 */
void hsxpl_send_p777_data(void) {

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t n=HSMP_FMC_TYPE_XP_PB777;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    uint32_t i=0;
    if(hsxpl_fmc.exec_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.exec_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_EXEC;
    }
    if(hsxpl_fmc.dspy_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.dspy_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSLU;
    }
    if(hsxpl_fmc.fail_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.fail_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSLD;
    }
    if(hsxpl_fmc.msg_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.msg_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSRU;
    }
    if(hsxpl_fmc.ofst_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.ofst_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSRD;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&i);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

}
