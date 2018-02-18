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
 * CRJ-200 from J Rollon
 *
 */

#include "hsxplcrj200.h"
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

void hsxpl_set_pcrj200_datarefs(void) {

  char acficao[8]; memset(acficao,0,8);
  if(hsxpl_xplane_datarefs.acficao!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(hsxpl_xplane_datarefs.tailno!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.tailno,actfailno,0,7);

  int isCRJ=0;
  if(XPLMFindDataRef("CRJ/fms/L1_button") != NULL) {
    if(!strncmp(acficao,"CRJ2",7)) {
      if(!strncmp(actfailno,"E-JP32",7)) {
        isCRJ=1;
      }
    }
  }
  if(!isCRJ) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_PCRJ200;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_PCRJ200;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_fmc.key_lk1=XPLMFindDataRef("CRJ/fms/L1_button");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("CRJ/fms/L2_button");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("CRJ/fms/L3_button");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("CRJ/fms/L4_button");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("CRJ/fms/L5_button");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("CRJ/fms/L6_button");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("CRJ/fms/R1_button");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("CRJ/fms/R2_button");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("CRJ/fms/R3_button");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("CRJ/fms/R4_button");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("CRJ/fms/R5_button");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("CRJ/fms/R6_button");

  hsxpl_fmc.key_0=XPLMFindDataRef("CRJ/click/fms_0");
  hsxpl_fmc.key_1=XPLMFindDataRef("CRJ/click/fms_1");
  hsxpl_fmc.key_2=XPLMFindDataRef("CRJ/click/fms_2");
  hsxpl_fmc.key_3=XPLMFindDataRef("CRJ/click/fms_3");
  hsxpl_fmc.key_4=XPLMFindDataRef("CRJ/click/fms_4");
  hsxpl_fmc.key_5=XPLMFindDataRef("CRJ/click/fms_5");
  hsxpl_fmc.key_6=XPLMFindDataRef("CRJ/click/fms_6");
  hsxpl_fmc.key_7=XPLMFindDataRef("CRJ/click/fms_7");
  hsxpl_fmc.key_8=XPLMFindDataRef("CRJ/click/fms_8");
  hsxpl_fmc.key_9=XPLMFindDataRef("CRJ/click/fms_9");

  hsxpl_fmc.key_a=XPLMFindDataRef("CRJ/click/fms_a");
  hsxpl_fmc.key_b=XPLMFindDataRef("CRJ/click/fms_b");
  hsxpl_fmc.key_c=XPLMFindDataRef("CRJ/click/fms_c");
  hsxpl_fmc.key_d=XPLMFindDataRef("CRJ/click/fms_d");
  hsxpl_fmc.key_e=XPLMFindDataRef("CRJ/click/fms_e");
  hsxpl_fmc.key_f=XPLMFindDataRef("CRJ/click/fms_f");
  hsxpl_fmc.key_g=XPLMFindDataRef("CRJ/click/fms_g");
  hsxpl_fmc.key_h=XPLMFindDataRef("CRJ/click/fms_h");
  hsxpl_fmc.key_i=XPLMFindDataRef("CRJ/click/fms_i");
  hsxpl_fmc.key_j=XPLMFindDataRef("CRJ/click/fms_j");
  hsxpl_fmc.key_k=XPLMFindDataRef("CRJ/click/fms_k");
  hsxpl_fmc.key_l=XPLMFindDataRef("CRJ/click/fms_l");
  hsxpl_fmc.key_m=XPLMFindDataRef("CRJ/click/fms_m");
  hsxpl_fmc.key_n=XPLMFindDataRef("CRJ/click/fms_n");
  hsxpl_fmc.key_o=XPLMFindDataRef("CRJ/click/fms_o");
  hsxpl_fmc.key_p=XPLMFindDataRef("CRJ/click/fms_p");
  hsxpl_fmc.key_q=XPLMFindDataRef("CRJ/click/fms_q");
  hsxpl_fmc.key_r=XPLMFindDataRef("CRJ/click/fms_r");
  hsxpl_fmc.key_s=XPLMFindDataRef("CRJ/click/fms_s");
  hsxpl_fmc.key_t=XPLMFindDataRef("CRJ/click/fms_t");
  hsxpl_fmc.key_u=XPLMFindDataRef("CRJ/click/fms_u");
  hsxpl_fmc.key_v=XPLMFindDataRef("CRJ/click/fms_v");
  hsxpl_fmc.key_w=XPLMFindDataRef("CRJ/click/fms_w");
  hsxpl_fmc.key_x=XPLMFindDataRef("CRJ/click/fms_x");
  hsxpl_fmc.key_y=XPLMFindDataRef("CRJ/click/fms_y");
  hsxpl_fmc.key_z=XPLMFindDataRef("CRJ/click/fms_z");

  hsxpl_fmc.key_space=XPLMFindDataRef("CRJ/click/fms_space");
  hsxpl_fmc.key_del=XPLMFindDataRef("CRJ/click/fms_delete");
  hsxpl_fmc.key_slash=XPLMFindDataRef("CRJ/click/fms_bar");
  hsxpl_fmc.key_clr=XPLMFindDataRef("CRJ/click/fms_clr");
  hsxpl_fmc.key_dot=XPLMFindDataRef("CRJ/click/fms_point");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("CRJ/click/fms_plus_minus");

  hsxpl_fmc.key_menu=XPLMFindDataRef("CRJ/fms/mcdu_menu_button");

  hsxpl_fmc.key_index=XPLMFindDataRef("CRJ/fms/index_button");
  hsxpl_fmc.key_fpln=XPLMFindDataRef("CRJ/fms/fpln_button");

  hsxpl_fmc.key_dir=XPLMFindDataRef("CRJ/fms/dir_intc_button");

  hsxpl_fmc.key_nextpage=XPLMFindDataRef("CRJ/fms/next_page_button");
  hsxpl_fmc.key_prevpage=XPLMFindDataRef("CRJ/fms/prev_page_button");

  hsxpl_fmc.key_deparr=XPLMFindDataRef("CRJ/fms/dep_arr_button");
  hsxpl_fmc.key_legs=XPLMFindDataRef("CRJ/fms/legs_button");
  hsxpl_fmc.key_hold=XPLMFindDataRef("CRJ/fms/hold_button");
  hsxpl_fmc.key_radio=XPLMFindDataRef("CRJ/fms/radio_button");
  hsxpl_fmc.key_perf=XPLMFindDataRef("CRJ/fms/perf_button");
  hsxpl_fmc.key_exec=XPLMFindDataRef("CRJ/fms/exec_button");
  hsxpl_fmc.key_mfdmenu=XPLMFindDataRef("CRJ/fms/mfd_menu_button");

}

/* We only send the FMC type as the screen is retrieved throught TCP port 18387 */
void hsxpl_send_pcrj200_data(void) {

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t n=HSMP_FMC_TYPE_XP_PCRJ200;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }
}
