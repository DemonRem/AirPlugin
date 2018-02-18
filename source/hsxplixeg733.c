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
 * IXEG 737 Classic specifics
 *
 */

#include "hsxplixeg733.h"

#include "hsxpl.h"
#include "hsxplmisc.h"
#include "hsxpldatarefs.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

hsxpl_ix733_datarefs_t hsxpl_ix733_datarefs;
extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;
extern uint32_t hsxpl_active_mcdu;
extern hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;

/* Local prototypes */
void hsxpl_build_ix737_screen_right_line(hsmp_fmc_screen_c_t *matrix,char *line,uint32_t colour);
void hsxpl_build_ix737_screen_left_line(hsmp_fmc_screen_c_t *matrix,char *line,uint32_t colour);


void hsxpl_set_ix733_datarefs(void) {

  if (XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1L")==NULL)
    return;

  char acficao[8]; memset(acficao,0,8);
  if(hsxpl_xplane_datarefs.acficao!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(hsxpl_xplane_datarefs.tailno!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.tailno,actfailno,0,7);

  int isIX733=0;
  if(XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1L") != NULL) {
    if(!strncmp(acficao,"B733",7)) {
      if(!strncmp(actfailno,"737300",7)) {
        isIX733=1;
      } else if(!strncmp(actfailno,"D-IXEG",7)) {
        isIX733=1;
      }
    }
  }
  if(!isIX733) {
    return;
  }

  /* x737 datarefs */
  memset(&hsxpl_ix733_datarefs,0,sizeof(struct hsxpl_ix733_datarefs_s));

  hsxpl_plane_type = HSXPL_PLANE_IX733;

  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_IX733;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  if(hsxpl_active_mcdu==1) {

    hsxpl_fmc.key_lk1=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1L");
    hsxpl_fmc.key_lk2=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_2L");
    hsxpl_fmc.key_lk3=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_3L");
    hsxpl_fmc.key_lk4=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_4L");
    hsxpl_fmc.key_lk5=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_5L");
    hsxpl_fmc.key_lk6=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_6L");

    hsxpl_fmc.key_rk1=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1R");
    hsxpl_fmc.key_rk2=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_2R");
    hsxpl_fmc.key_rk3=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_3R");
    hsxpl_fmc.key_rk4=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_4R");
    hsxpl_fmc.key_rk5=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_5R");
    hsxpl_fmc.key_rk6=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_6R");

    hsxpl_fmc.key_1=XPLMFindDataRef("ixeg/733/FMC/cdu1_1");
    hsxpl_fmc.key_2=XPLMFindDataRef("ixeg/733/FMC/cdu1_2");
    hsxpl_fmc.key_3=XPLMFindDataRef("ixeg/733/FMC/cdu1_3");
    hsxpl_fmc.key_4=XPLMFindDataRef("ixeg/733/FMC/cdu1_4");
    hsxpl_fmc.key_5=XPLMFindDataRef("ixeg/733/FMC/cdu1_5");
    hsxpl_fmc.key_6=XPLMFindDataRef("ixeg/733/FMC/cdu1_6");
    hsxpl_fmc.key_7=XPLMFindDataRef("ixeg/733/FMC/cdu1_7");
    hsxpl_fmc.key_8=XPLMFindDataRef("ixeg/733/FMC/cdu1_8");
    hsxpl_fmc.key_9=XPLMFindDataRef("ixeg/733/FMC/cdu1_9");
    hsxpl_fmc.key_0=XPLMFindDataRef("ixeg/733/FMC/cdu1_0");

    hsxpl_fmc.key_a=XPLMFindDataRef("ixeg/733/FMC/cdu1_A");
    hsxpl_fmc.key_b=XPLMFindDataRef("ixeg/733/FMC/cdu1_B");
    hsxpl_fmc.key_c=XPLMFindDataRef("ixeg/733/FMC/cdu1_C");
    hsxpl_fmc.key_d=XPLMFindDataRef("ixeg/733/FMC/cdu1_D");
    hsxpl_fmc.key_e=XPLMFindDataRef("ixeg/733/FMC/cdu1_E");
    hsxpl_fmc.key_f=XPLMFindDataRef("ixeg/733/FMC/cdu1_F");
    hsxpl_fmc.key_g=XPLMFindDataRef("ixeg/733/FMC/cdu1_G");
    hsxpl_fmc.key_h=XPLMFindDataRef("ixeg/733/FMC/cdu1_H");
    hsxpl_fmc.key_i=XPLMFindDataRef("ixeg/733/FMC/cdu1_I");
    hsxpl_fmc.key_j=XPLMFindDataRef("ixeg/733/FMC/cdu1_J");
    hsxpl_fmc.key_k=XPLMFindDataRef("ixeg/733/FMC/cdu1_K");
    hsxpl_fmc.key_l=XPLMFindDataRef("ixeg/733/FMC/cdu1_L");
    hsxpl_fmc.key_m=XPLMFindDataRef("ixeg/733/FMC/cdu1_M");
    hsxpl_fmc.key_n=XPLMFindDataRef("ixeg/733/FMC/cdu1_N");
    hsxpl_fmc.key_o=XPLMFindDataRef("ixeg/733/FMC/cdu1_O");
    hsxpl_fmc.key_p=XPLMFindDataRef("ixeg/733/FMC/cdu1_P");
    hsxpl_fmc.key_q=XPLMFindDataRef("ixeg/733/FMC/cdu1_Q");
    hsxpl_fmc.key_r=XPLMFindDataRef("ixeg/733/FMC/cdu1_R");
    hsxpl_fmc.key_s=XPLMFindDataRef("ixeg/733/FMC/cdu1_S");
    hsxpl_fmc.key_t=XPLMFindDataRef("ixeg/733/FMC/cdu1_T");
    hsxpl_fmc.key_u=XPLMFindDataRef("ixeg/733/FMC/cdu1_U");
    hsxpl_fmc.key_v=XPLMFindDataRef("ixeg/733/FMC/cdu1_V");
    hsxpl_fmc.key_w=XPLMFindDataRef("ixeg/733/FMC/cdu1_W");
    hsxpl_fmc.key_x=XPLMFindDataRef("ixeg/733/FMC/cdu1_X");
    hsxpl_fmc.key_y=XPLMFindDataRef("ixeg/733/FMC/cdu1_Y");
    hsxpl_fmc.key_z=XPLMFindDataRef("ixeg/733/FMC/cdu1_Z");

    hsxpl_fmc.key_slash=XPLMFindDataRef("ixeg/733/FMC/cdu1_slash");
    hsxpl_fmc.key_dot=XPLMFindDataRef("ixeg/733/FMC/cdu1_dot");
    hsxpl_fmc.key_plusminus=XPLMFindDataRef("ixeg/733/FMC/cdu1_plus");

    hsxpl_fmc.key_clr=XPLMFindCommand("ixeg/733/FMC/cdu1_clr");
    hsxpl_fmc.key_del=XPLMFindDataRef("ixeg/733/FMC/cdu1_del");

    hsxpl_fmc.key_init=XPLMFindDataRef("ixeg/733/FMC/cdu1_initref");
    hsxpl_fmc.key_rte=XPLMFindDataRef("ixeg/733/FMC/cdu1_rte");
    hsxpl_fmc.key_clb=XPLMFindDataRef("ixeg/733/FMC/cdu1_clb");
    hsxpl_fmc.key_crz=XPLMFindDataRef("ixeg/733/FMC/cdu1_crz");
    hsxpl_fmc.key_des=XPLMFindDataRef("ixeg/733/FMC/cdu1_des");
    hsxpl_fmc.key_menu=XPLMFindDataRef("ixeg/733/FMC/cdu1_menu");
    hsxpl_fmc.key_legs=XPLMFindDataRef("ixeg/733/FMC/cdu1_legs");
    hsxpl_fmc.key_deparr=XPLMFindDataRef("ixeg/733/FMC/cdu1_deparr");
    hsxpl_fmc.key_hold=XPLMFindDataRef("ixeg/733/FMC/cdu1_hold");
    hsxpl_fmc.key_prog=XPLMFindDataRef("ixeg/733/FMC/cdu1_prog");
    hsxpl_fmc.key_n1limit=XPLMFindDataRef("ixeg/733/FMC/cdu1_n1limit");
    hsxpl_fmc.key_fix=XPLMFindDataRef("ixeg/733/FMC/cdu1_fix");
    hsxpl_fmc.key_prevpage=XPLMFindDataRef("ixeg/733/FMC/cdu1_prev");
    hsxpl_fmc.key_nextpage=XPLMFindDataRef("ixeg/733/FMC/cdu1_next");
    hsxpl_fmc.key_exec=XPLMFindDataRef("ixeg/733/FMC/cdu1_exec");

    hsxpl_fmc.exec_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_exec_ann");
    hsxpl_fmc.ofst_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_ofst_ann");
    hsxpl_fmc.msg_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_msg_ann");
    hsxpl_fmc.fail_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_fail_ann");

    hsxpl_ix733_datarefs.title=XPLMFindDataRef("ixeg/733/FMC/cdu1D_title");
    hsxpl_ix733_datarefs.scratchPad=XPLMFindDataRef("ixeg/733/FMC/cdu1D_scrpad");
    hsxpl_ix733_datarefs.pageNo=XPLMFindDataRef("ixeg/733/FMC/cdu1D_pg_number");

    hsxpl_ix733_datarefs.llines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1L_t");
    hsxpl_ix733_datarefs.llines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1L_d");
    hsxpl_ix733_datarefs.llines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2L_t");
    hsxpl_ix733_datarefs.llines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2L_d");
    hsxpl_ix733_datarefs.llines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3L_t");
    hsxpl_ix733_datarefs.llines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3L_d");
    hsxpl_ix733_datarefs.llines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4L_t");
    hsxpl_ix733_datarefs.llines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4L_d");
    hsxpl_ix733_datarefs.llines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5L_t");
    hsxpl_ix733_datarefs.llines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5L_d");
    hsxpl_ix733_datarefs.llines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6L_t");
    hsxpl_ix733_datarefs.llines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6L_d");

    hsxpl_ix733_datarefs.rlines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1R_t");
    hsxpl_ix733_datarefs.rlines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1R_d");
    hsxpl_ix733_datarefs.rlines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2R_t");
    hsxpl_ix733_datarefs.rlines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2R_d");
    hsxpl_ix733_datarefs.rlines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3R_t");
    hsxpl_ix733_datarefs.rlines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3R_d");
    hsxpl_ix733_datarefs.rlines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4R_t");
    hsxpl_ix733_datarefs.rlines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4R_d");
    hsxpl_ix733_datarefs.rlines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5R_t");
    hsxpl_ix733_datarefs.rlines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5R_d");
    hsxpl_ix733_datarefs.rlines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6R_t");
    hsxpl_ix733_datarefs.rlines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6R_d");

  } else if (hsxpl_active_mcdu == 2) {

    hsxpl_fmc.key_lk1=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_1L");
    hsxpl_fmc.key_lk2=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_2L");
    hsxpl_fmc.key_lk3=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_3L");
    hsxpl_fmc.key_lk4=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_4L");
    hsxpl_fmc.key_lk5=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_5L");
    hsxpl_fmc.key_lk6=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_6L");

    hsxpl_fmc.key_rk1=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_1R");
    hsxpl_fmc.key_rk2=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_2R");
    hsxpl_fmc.key_rk3=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_3R");
    hsxpl_fmc.key_rk4=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_4R");
    hsxpl_fmc.key_rk5=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_5R");
    hsxpl_fmc.key_rk6=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_6R");

    hsxpl_fmc.key_1=XPLMFindDataRef("ixeg/733/FMC/cdu2_1");
    hsxpl_fmc.key_2=XPLMFindDataRef("ixeg/733/FMC/cdu2_2");
    hsxpl_fmc.key_3=XPLMFindDataRef("ixeg/733/FMC/cdu2_3");
    hsxpl_fmc.key_4=XPLMFindDataRef("ixeg/733/FMC/cdu2_4");
    hsxpl_fmc.key_5=XPLMFindDataRef("ixeg/733/FMC/cdu2_5");
    hsxpl_fmc.key_6=XPLMFindDataRef("ixeg/733/FMC/cdu2_6");
    hsxpl_fmc.key_7=XPLMFindDataRef("ixeg/733/FMC/cdu2_7");
    hsxpl_fmc.key_8=XPLMFindDataRef("ixeg/733/FMC/cdu2_8");
    hsxpl_fmc.key_9=XPLMFindDataRef("ixeg/733/FMC/cdu2_9");
    hsxpl_fmc.key_0=XPLMFindDataRef("ixeg/733/FMC/cdu2_0");

    hsxpl_fmc.key_a=XPLMFindDataRef("ixeg/733/FMC/cdu2_A");
    hsxpl_fmc.key_b=XPLMFindDataRef("ixeg/733/FMC/cdu2_B");
    hsxpl_fmc.key_c=XPLMFindDataRef("ixeg/733/FMC/cdu2_C");
    hsxpl_fmc.key_d=XPLMFindDataRef("ixeg/733/FMC/cdu2_D");
    hsxpl_fmc.key_e=XPLMFindDataRef("ixeg/733/FMC/cdu2_E");
    hsxpl_fmc.key_f=XPLMFindDataRef("ixeg/733/FMC/cdu2_F");
    hsxpl_fmc.key_g=XPLMFindDataRef("ixeg/733/FMC/cdu2_G");
    hsxpl_fmc.key_h=XPLMFindDataRef("ixeg/733/FMC/cdu2_H");
    hsxpl_fmc.key_i=XPLMFindDataRef("ixeg/733/FMC/cdu2_I");
    hsxpl_fmc.key_j=XPLMFindDataRef("ixeg/733/FMC/cdu2_J");
    hsxpl_fmc.key_k=XPLMFindDataRef("ixeg/733/FMC/cdu2_K");
    hsxpl_fmc.key_l=XPLMFindDataRef("ixeg/733/FMC/cdu2_L");
    hsxpl_fmc.key_m=XPLMFindDataRef("ixeg/733/FMC/cdu2_M");
    hsxpl_fmc.key_n=XPLMFindDataRef("ixeg/733/FMC/cdu2_N");
    hsxpl_fmc.key_o=XPLMFindDataRef("ixeg/733/FMC/cdu2_O");
    hsxpl_fmc.key_p=XPLMFindDataRef("ixeg/733/FMC/cdu2_P");
    hsxpl_fmc.key_q=XPLMFindDataRef("ixeg/733/FMC/cdu2_Q");
    hsxpl_fmc.key_r=XPLMFindDataRef("ixeg/733/FMC/cdu2_R");
    hsxpl_fmc.key_s=XPLMFindDataRef("ixeg/733/FMC/cdu2_S");
    hsxpl_fmc.key_t=XPLMFindDataRef("ixeg/733/FMC/cdu2_T");
    hsxpl_fmc.key_u=XPLMFindDataRef("ixeg/733/FMC/cdu2_U");
    hsxpl_fmc.key_v=XPLMFindDataRef("ixeg/733/FMC/cdu2_V");
    hsxpl_fmc.key_w=XPLMFindDataRef("ixeg/733/FMC/cdu2_W");
    hsxpl_fmc.key_x=XPLMFindDataRef("ixeg/733/FMC/cdu2_X");
    hsxpl_fmc.key_y=XPLMFindDataRef("ixeg/733/FMC/cdu2_Y");
    hsxpl_fmc.key_z=XPLMFindDataRef("ixeg/733/FMC/cdu2_Z");

    hsxpl_fmc.key_slash=XPLMFindDataRef("ixeg/733/FMC/cdu2_slash");
    hsxpl_fmc.key_dot=XPLMFindDataRef("ixeg/733/FMC/cdu2_dot");
    hsxpl_fmc.key_plusminus=XPLMFindDataRef("ixeg/733/FMC/cdu2_plus");
    hsxpl_fmc.key_clr=XPLMFindCommand("ixeg/733/FMC/cdu2_clr");
    hsxpl_fmc.key_del=XPLMFindDataRef("ixeg/733/FMC/cdu2_del");

    hsxpl_fmc.key_init=XPLMFindDataRef("ixeg/733/FMC/cdu2_initref");
    hsxpl_fmc.key_rte=XPLMFindDataRef("ixeg/733/FMC/cdu2_rte");
    hsxpl_fmc.key_clb=XPLMFindDataRef("ixeg/733/FMC/cdu2_clb");
    hsxpl_fmc.key_crz=XPLMFindDataRef("ixeg/733/FMC/cdu2_crz");
    hsxpl_fmc.key_des=XPLMFindDataRef("ixeg/733/FMC/cdu2_des");
    hsxpl_fmc.key_menu=XPLMFindDataRef("ixeg/733/FMC/cdu2_menu");
    hsxpl_fmc.key_legs=XPLMFindDataRef("ixeg/733/FMC/cdu2_legs");
    hsxpl_fmc.key_deparr=XPLMFindDataRef("ixeg/733/FMC/cdu2_deparr");
    hsxpl_fmc.key_hold=XPLMFindDataRef("ixeg/733/FMC/cdu2_hold");
    hsxpl_fmc.key_prog=XPLMFindDataRef("ixeg/733/FMC/cdu2_prog");
    hsxpl_fmc.key_n1limit=XPLMFindDataRef("ixeg/733/FMC/cdu2_n1limit");
    hsxpl_fmc.key_fix=XPLMFindDataRef("ixeg/733/FMC/cdu2_fix");
    hsxpl_fmc.key_prevpage=XPLMFindDataRef("ixeg/733/FMC/cdu2_prev");
    hsxpl_fmc.key_nextpage=XPLMFindDataRef("ixeg/733/FMC/cdu2_next");
    hsxpl_fmc.key_exec=XPLMFindDataRef("ixeg/733/FMC/cdu2_exec");

    hsxpl_fmc.exec_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_exec_ann");
    hsxpl_fmc.ofst_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_ofst_ann");
    hsxpl_fmc.msg_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_msg_ann");
    hsxpl_fmc.fail_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_fail_ann");


    hsxpl_ix733_datarefs.title=XPLMFindDataRef("ixeg/733/FMC/cdu2D_title");
    hsxpl_ix733_datarefs.scratchPad=XPLMFindDataRef("ixeg/733/FMC/cdu2D_scrpad");
    hsxpl_ix733_datarefs.pageNo=XPLMFindDataRef("ixeg/733/FMC/cdu2D_pg_number");

    hsxpl_ix733_datarefs.llines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1L_t");
    hsxpl_ix733_datarefs.llines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1L_d");
    hsxpl_ix733_datarefs.llines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2L_t");
    hsxpl_ix733_datarefs.llines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2L_d");
    hsxpl_ix733_datarefs.llines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3L_t");
    hsxpl_ix733_datarefs.llines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3L_d");
    hsxpl_ix733_datarefs.llines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4L_t");
    hsxpl_ix733_datarefs.llines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4L_d");
    hsxpl_ix733_datarefs.llines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5L_t");
    hsxpl_ix733_datarefs.llines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5L_d");
    hsxpl_ix733_datarefs.llines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6L_t");
    hsxpl_ix733_datarefs.llines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6L_d");

    hsxpl_ix733_datarefs.rlines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1R_t");
    hsxpl_ix733_datarefs.rlines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1R_d");
    hsxpl_ix733_datarefs.rlines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2R_t");
    hsxpl_ix733_datarefs.rlines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2R_d");
    hsxpl_ix733_datarefs.rlines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3R_t");
    hsxpl_ix733_datarefs.rlines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3R_d");
    hsxpl_ix733_datarefs.rlines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4R_t");
    hsxpl_ix733_datarefs.rlines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4R_d");
    hsxpl_ix733_datarefs.rlines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5R_t");
    hsxpl_ix733_datarefs.rlines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5R_d");
    hsxpl_ix733_datarefs.rlines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6R_t");
    hsxpl_ix733_datarefs.rlines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6R_d");

  }
}

void hsxpl_send_ix733_data(void) {

  uint8_t i;
  uint8_t j;

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
    for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=0x00FF00FF;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }

  char line[32];

  memset(line,0,32);
  XPLMGetDatab(hsxpl_ix733_datarefs.title,line,0,31);
  hsxpl_build_ix737_screen_left_line(screen.matrix[0],line,0x00FF00FF);

  memset(line,0,32);
  XPLMGetDatab(hsxpl_ix733_datarefs.pageNo,line,0,31);
  hsxpl_build_ix737_screen_right_line(screen.matrix[0],line,0x00FF00FF);

  for(i=0;i<12;i++) {
    if (hsxpl_ix733_datarefs.llines[i]!=NULL) {
      memset(line,0,32);
      XPLMGetDatab(hsxpl_ix733_datarefs.llines[i],line,0,31);
      hsxpl_build_ix737_screen_left_line(screen.matrix[i+1],line,0x00FF00FF);
      memset(line,0,32);
      XPLMGetDatab(hsxpl_ix733_datarefs.rlines[i],line,0,31);
      hsxpl_build_ix737_screen_right_line(screen.matrix[i+1],line,0x00FF00FF);

    }
  }

  memset(line,0,32);
  XPLMGetDatab(hsxpl_ix733_datarefs.scratchPad,line,0,31);
  hsxpl_build_ix737_screen_left_line(screen.matrix[13],line,0x00FF00FF);

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    for(i=0;i<4;i++) {
      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }
  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    for(i=4;i<8;i++) {
      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }
  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    for(i=8;i<12;i++) {
      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }
  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    for(i=12;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    uint32_t i=0;
    if(hsxpl_fmc.exec_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.exec_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_EXEC;
    }
    if(hsxpl_fmc.fail_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.fail_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_VSLD;
    }
    if(hsxpl_fmc.msg_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.msg_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_VSRU;
    }
    if(hsxpl_fmc.ofst_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.ofst_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_VSRD;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&i);

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ACTIVE_MCDU,&hsxpl_active_mcdu);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }
}


void hsxpl_build_ix737_screen_left_line(hsmp_fmc_screen_c_t *matrix,char *line,uint32_t colour) {

  unsigned char *cp=(unsigned char *)line;
  uint8_t i;
  uint8_t fSize=26;
  uint32_t fColour=colour;

  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOCOLS;i++) {

    if(*cp=='\0') break;
    if(*cp==' ') { cp++; continue; }

    if(*cp == 164) { fSize=26; cp++; i--; continue; }
    else if(*cp == 163) { fSize=22; cp++; i--; continue; }
    else if(*cp == '&') {
      if(fColour==colour) {
        fColour=0xFFFFFFFF;
      } else {
        fColour=colour;
      }
      cp++; i--;
      continue;

    };

    matrix[i].fsize=fSize;
    matrix[i].chr= toupper(*cp);
    matrix[i].colour=fColour;
    cp++;
  }
}
void hsxpl_build_ix737_screen_right_line(hsmp_fmc_screen_c_t *matrix,char *line,uint32_t colour) {

  unsigned char *bp=(unsigned char *)line;
  unsigned char *cp=bp;
  uint8_t i;
  uint8_t fSize=26;
  uint32_t fColour=colour;

  while(*cp!='\0' && (cp-bp) < 32) cp++;
  cp--;

  for(i=23;i>=0;i--) {

    if(cp<bp) break;
    if(*cp==' ') { cp--; continue; }

    if(*cp == 164) { cp--; i++; continue; }
    else if(*cp == 163) { cp--; i++; continue;  }
    else if(*cp == '&') { cp--; i++; continue;  }

    matrix[i].chr= toupper(*cp);

    cp--;
  }
  cp++; i++;
  for(;i<24;i++) {
    if(*cp == 164) { fSize=26; cp++; i--; continue; }
    else if(*cp == 163) { fSize=22; cp++; i--; continue; }
    else if(*cp == '&') {
      if(fColour==colour) {
        fColour=0xFFFFFFFF;
      } else {
        fColour=colour;
      }
      cp++; i--;
      continue;
    };
    matrix[i].colour=fColour;
    matrix[i].fsize=fSize;
  }

}


