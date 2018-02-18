/* Copyright (c) 2013-2017 Haversine Ltd
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
* UFMC/X737FMC from Javier Cortes : http://ufmc.eadt.eu
*
*/

#include "hsxplufmc.h"
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

hsxpl_ufmc_datarefs_t hsxpl_ufmc_datarefs;

void hsxpl_set_ufmc_datarefs(void) {


  if(XPLMFindDataRef("SSG/UFMC/PRESENT") != NULL) {
    if(XPLMGetDataf(XPLMFindDataRef("SSG/UFMC/PRESENT"))!=0) {

      hsxpl_fmc_type = HSMP_FMC_TYPE_XP_UFMC;

      memset(&hsxpl_ufmc_datarefs,0,sizeof(struct hsxpl_ufmc_datarefs_s));
      memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

      hsxpl_fmc.key_n1limit=XPLMFindDataRef("SSG/UFMC/N1_LIMIT");
      hsxpl_fmc.key_lk1=XPLMFindDataRef("SSG/UFMC/LK1");
      hsxpl_fmc.key_lk2=XPLMFindDataRef("SSG/UFMC/LK2");
      hsxpl_fmc.key_lk3=XPLMFindDataRef("SSG/UFMC/LK3");
      hsxpl_fmc.key_lk4=XPLMFindDataRef("SSG/UFMC/LK4");
      hsxpl_fmc.key_lk5=XPLMFindDataRef("SSG/UFMC/LK5");
      hsxpl_fmc.key_lk6=XPLMFindDataRef("SSG/UFMC/LK6");

      hsxpl_fmc.key_rk1=XPLMFindDataRef("SSG/UFMC/RK1");
      hsxpl_fmc.key_rk2=XPLMFindDataRef("SSG/UFMC/RK2");
      hsxpl_fmc.key_rk3=XPLMFindDataRef("SSG/UFMC/RK3");
      hsxpl_fmc.key_rk4=XPLMFindDataRef("SSG/UFMC/RK4");
      hsxpl_fmc.key_rk5=XPLMFindDataRef("SSG/UFMC/RK5");
      hsxpl_fmc.key_rk6=XPLMFindDataRef("SSG/UFMC/RK6");

      hsxpl_fmc.key_1=XPLMFindDataRef("SSG/UFMC/1");
      hsxpl_fmc.key_2=XPLMFindDataRef("SSG/UFMC/2");
      hsxpl_fmc.key_3=XPLMFindDataRef("SSG/UFMC/3");
      hsxpl_fmc.key_4=XPLMFindDataRef("SSG/UFMC/4");
      hsxpl_fmc.key_5=XPLMFindDataRef("SSG/UFMC/5");
      hsxpl_fmc.key_6=XPLMFindDataRef("SSG/UFMC/6");
      hsxpl_fmc.key_7=XPLMFindDataRef("SSG/UFMC/7");
      hsxpl_fmc.key_8=XPLMFindDataRef("SSG/UFMC/8");
      hsxpl_fmc.key_9=XPLMFindDataRef("SSG/UFMC/9");
      hsxpl_fmc.key_0=XPLMFindDataRef("SSG/UFMC/0");

      hsxpl_fmc.key_a=XPLMFindDataRef("SSG/UFMC/A");
      hsxpl_fmc.key_b=XPLMFindDataRef("SSG/UFMC/B");
      hsxpl_fmc.key_c=XPLMFindDataRef("SSG/UFMC/C");
      hsxpl_fmc.key_d=XPLMFindDataRef("SSG/UFMC/D");
      hsxpl_fmc.key_e=XPLMFindDataRef("SSG/UFMC/E");
      hsxpl_fmc.key_f=XPLMFindDataRef("SSG/UFMC/F");
      hsxpl_fmc.key_g=XPLMFindDataRef("SSG/UFMC/G");
      hsxpl_fmc.key_h=XPLMFindDataRef("SSG/UFMC/H");
      hsxpl_fmc.key_i=XPLMFindDataRef("SSG/UFMC/I");
      hsxpl_fmc.key_j=XPLMFindDataRef("SSG/UFMC/J");
      hsxpl_fmc.key_k=XPLMFindDataRef("SSG/UFMC/K");
      hsxpl_fmc.key_l=XPLMFindDataRef("SSG/UFMC/L");
      hsxpl_fmc.key_m=XPLMFindDataRef("SSG/UFMC/M");
      hsxpl_fmc.key_n=XPLMFindDataRef("SSG/UFMC/N");
      hsxpl_fmc.key_o=XPLMFindDataRef("SSG/UFMC/O");
      hsxpl_fmc.key_p=XPLMFindDataRef("SSG/UFMC/P");
      hsxpl_fmc.key_q=XPLMFindDataRef("SSG/UFMC/Q");
      hsxpl_fmc.key_r=XPLMFindDataRef("SSG/UFMC/R");
      hsxpl_fmc.key_s=XPLMFindDataRef("SSG/UFMC/S");
      hsxpl_fmc.key_t=XPLMFindDataRef("SSG/UFMC/T");
      hsxpl_fmc.key_u=XPLMFindDataRef("SSG/UFMC/U");
      hsxpl_fmc.key_v=XPLMFindDataRef("SSG/UFMC/V");
      hsxpl_fmc.key_w=XPLMFindDataRef("SSG/UFMC/W");
      hsxpl_fmc.key_x=XPLMFindDataRef("SSG/UFMC/X");
      hsxpl_fmc.key_y=XPLMFindDataRef("SSG/UFMC/Y");
      hsxpl_fmc.key_z=XPLMFindDataRef("SSG/UFMC/Z");

      hsxpl_fmc.key_slash=XPLMFindDataRef("SSG/UFMC/barra");
      hsxpl_fmc.key_dot=XPLMFindDataRef("SSG/UFMC/punto");
      hsxpl_fmc.key_space=XPLMFindDataRef("SSG/UFMC/espacio");
      hsxpl_fmc.key_plusminus=XPLMFindDataRef("SSG/UFMC/menos");

      hsxpl_fmc.key_init=XPLMFindDataRef("SSG/UFMC/INITREF");
      hsxpl_fmc.key_rte=XPLMFindDataRef("SSG/UFMC/RTE");
      hsxpl_fmc.key_deparr=XPLMFindDataRef("SSG/UFMC/DEPARR");

      hsxpl_fmc.key_fix=XPLMFindDataRef("SSG/UFMC/FIX");
      hsxpl_fmc.key_legs=XPLMFindDataRef("SSG/UFMC/LEGS");
      hsxpl_fmc.key_hold=XPLMFindDataRef("SSG/UFMC/HOLD");
      hsxpl_fmc.key_prog=XPLMFindDataRef("SSG/UFMC/PROG");
      hsxpl_fmc.key_exec=XPLMFindDataRef("SSG/UFMC/EXEC");
      hsxpl_fmc.key_menu=XPLMFindDataRef("SSG/UFMC/MENU");

      hsxpl_fmc.key_prevpage=XPLMFindDataRef("SSG/UFMC/PREVPAGE");
      hsxpl_fmc.key_nextpage=XPLMFindDataRef("SSG/UFMC/NEXTPAGE");
      hsxpl_fmc.key_clr=XPLMFindDataRef("SSG/UFMC/CLR");
      hsxpl_fmc.key_crz=XPLMFindDataRef("SSG/UFMC/CRZ_x737");
      hsxpl_fmc.key_clb=XPLMFindDataRef("SSG/UFMC/CLB_x737");
      hsxpl_fmc.key_des=XPLMFindDataRef("SSG/UFMC/DES_x737");
      hsxpl_fmc.key_del=XPLMFindDataRef("SSG/UFMC/DEL");
      hsxpl_fmc.key_fmccom=XPLMFindDataRef("SSG/UFMC/FMCCOM");
      hsxpl_fmc.key_radnav=XPLMFindDataRef("SSG/UFMC/NAVRAD");
      hsxpl_fmc.key_atc=XPLMFindDataRef("SSG/UFMC/ATC");
      hsxpl_fmc.key_vnav=XPLMFindDataRef("SSG/UFMC/VNAV");
      hsxpl_fmc.exec_light_on=XPLMFindDataRef("SSG/UFMC/Exec_Light_on");
      hsxpl_fmc.ofst_light_on=XPLMFindDataRef("SSG/UFMC/Offset_on");
      hsxpl_fmc.msg_light_on=XPLMFindDataRef("SSG/UFMC/Msg_on");
      if(hsxpl_fmc.msg_light_on==NULL)
        hsxpl_fmc.msg_light_on=XPLMFindDataRef("SSG/UFMC/MSG_on");

      hsxpl_ufmc_datarefs.line[0]=XPLMFindDataRef("SSG/UFMC/LINE_1");
      hsxpl_ufmc_datarefs.line[1]=XPLMFindDataRef("SSG/UFMC/LINE_2");
      hsxpl_ufmc_datarefs.line[2]=XPLMFindDataRef("SSG/UFMC/LINE_3");
      hsxpl_ufmc_datarefs.line[3]=XPLMFindDataRef("SSG/UFMC/LINE_4");
      hsxpl_ufmc_datarefs.line[4]=XPLMFindDataRef("SSG/UFMC/LINE_5");
      hsxpl_ufmc_datarefs.line[5]=XPLMFindDataRef("SSG/UFMC/LINE_6");
      hsxpl_ufmc_datarefs.line[6]=XPLMFindDataRef("SSG/UFMC/LINE_7");
      hsxpl_ufmc_datarefs.line[7]=XPLMFindDataRef("SSG/UFMC/LINE_8");
      hsxpl_ufmc_datarefs.line[8]=XPLMFindDataRef("SSG/UFMC/LINE_9");
      hsxpl_ufmc_datarefs.line[9]=XPLMFindDataRef("SSG/UFMC/LINE_10");
      hsxpl_ufmc_datarefs.line[10]=XPLMFindDataRef("SSG/UFMC/LINE_11");
      hsxpl_ufmc_datarefs.line[11]=XPLMFindDataRef("SSG/UFMC/LINE_12");
      hsxpl_ufmc_datarefs.line[12]=XPLMFindDataRef("SSG/UFMC/LINE_13");
      hsxpl_ufmc_datarefs.line[13]=XPLMFindDataRef("SSG/UFMC/LINE_14");

      return;
    }
  }

  int ufmc_dataref_type=0;

  if(XPLMFindDataRef("FJCC/UFMC/PRESENT") != NULL) {
    if(XPLMGetDataf(XPLMFindDataRef("FJCC/UFMC/PRESENT"))!=0) {
      ufmc_dataref_type=1;
    }
  }
  if(!ufmc_dataref_type) {
    if(XPLMFindDataRef("FJCC/UFMC_1/PRESENT") != NULL) {
      if(XPLMGetDataf(XPLMFindDataRef("FJCC/UFMC_1/PRESENT"))!=0) {
        ufmc_dataref_type=2;
      }
    }
  }

  if(!ufmc_dataref_type)
    return;

  if(hsxpl_plane_type!=HSXPL_PLANE_X737)
    hsxpl_fmc_type = HSMP_FMC_TYPE_XP_UFMC;
  else if(XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK1")!=NULL) {
    hsxpl_fmc_type = HSMP_FMC_TYPE_XP_X737V5;
  } else
    hsxpl_fmc_type = HSMP_FMC_TYPE_XP_X737;

  memset(&hsxpl_ufmc_datarefs,0,sizeof(struct hsxpl_ufmc_datarefs_s));
  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  if(ufmc_dataref_type==1) {

    hsxpl_fmc.key_n1limit=XPLMFindDataRef("FJCC/UFMC/N1_LIMIT");
    hsxpl_fmc.key_lk1=XPLMFindDataRef("FJCC/UFMC/LK1");
    hsxpl_fmc.key_lk2=XPLMFindDataRef("FJCC/UFMC/LK2");
    hsxpl_fmc.key_lk3=XPLMFindDataRef("FJCC/UFMC/LK3");
    hsxpl_fmc.key_lk4=XPLMFindDataRef("FJCC/UFMC/LK4");
    hsxpl_fmc.key_lk5=XPLMFindDataRef("FJCC/UFMC/LK5");
    hsxpl_fmc.key_lk6=XPLMFindDataRef("FJCC/UFMC/LK6");

    hsxpl_fmc.key_rk1=XPLMFindDataRef("FJCC/UFMC/RK1");
    hsxpl_fmc.key_rk2=XPLMFindDataRef("FJCC/UFMC/RK2");
    hsxpl_fmc.key_rk3=XPLMFindDataRef("FJCC/UFMC/RK3");
    hsxpl_fmc.key_rk4=XPLMFindDataRef("FJCC/UFMC/RK4");
    hsxpl_fmc.key_rk5=XPLMFindDataRef("FJCC/UFMC/RK5");
    hsxpl_fmc.key_rk6=XPLMFindDataRef("FJCC/UFMC/RK6");

    hsxpl_fmc.key_1=XPLMFindDataRef("FJCC/UFMC/1");
    hsxpl_fmc.key_2=XPLMFindDataRef("FJCC/UFMC/2");
    hsxpl_fmc.key_3=XPLMFindDataRef("FJCC/UFMC/3");
    hsxpl_fmc.key_4=XPLMFindDataRef("FJCC/UFMC/4");
    hsxpl_fmc.key_5=XPLMFindDataRef("FJCC/UFMC/5");
    hsxpl_fmc.key_6=XPLMFindDataRef("FJCC/UFMC/6");
    hsxpl_fmc.key_7=XPLMFindDataRef("FJCC/UFMC/7");
    hsxpl_fmc.key_8=XPLMFindDataRef("FJCC/UFMC/8");
    hsxpl_fmc.key_9=XPLMFindDataRef("FJCC/UFMC/9");
    hsxpl_fmc.key_0=XPLMFindDataRef("FJCC/UFMC/0");

    hsxpl_fmc.key_a=XPLMFindDataRef("FJCC/UFMC/A");
    hsxpl_fmc.key_b=XPLMFindDataRef("FJCC/UFMC/B");
    hsxpl_fmc.key_c=XPLMFindDataRef("FJCC/UFMC/C");
    hsxpl_fmc.key_d=XPLMFindDataRef("FJCC/UFMC/D");
    hsxpl_fmc.key_e=XPLMFindDataRef("FJCC/UFMC/E");
    hsxpl_fmc.key_f=XPLMFindDataRef("FJCC/UFMC/F");
    hsxpl_fmc.key_g=XPLMFindDataRef("FJCC/UFMC/G");
    hsxpl_fmc.key_h=XPLMFindDataRef("FJCC/UFMC/H");
    hsxpl_fmc.key_i=XPLMFindDataRef("FJCC/UFMC/I");
    hsxpl_fmc.key_j=XPLMFindDataRef("FJCC/UFMC/J");
    hsxpl_fmc.key_k=XPLMFindDataRef("FJCC/UFMC/K");
    hsxpl_fmc.key_l=XPLMFindDataRef("FJCC/UFMC/L");
    hsxpl_fmc.key_m=XPLMFindDataRef("FJCC/UFMC/M");
    hsxpl_fmc.key_n=XPLMFindDataRef("FJCC/UFMC/N");
    hsxpl_fmc.key_o=XPLMFindDataRef("FJCC/UFMC/O");
    hsxpl_fmc.key_p=XPLMFindDataRef("FJCC/UFMC/P");
    hsxpl_fmc.key_q=XPLMFindDataRef("FJCC/UFMC/Q");
    hsxpl_fmc.key_r=XPLMFindDataRef("FJCC/UFMC/R");
    hsxpl_fmc.key_s=XPLMFindDataRef("FJCC/UFMC/S");
    hsxpl_fmc.key_t=XPLMFindDataRef("FJCC/UFMC/T");
    hsxpl_fmc.key_u=XPLMFindDataRef("FJCC/UFMC/U");
    hsxpl_fmc.key_v=XPLMFindDataRef("FJCC/UFMC/V");
    hsxpl_fmc.key_w=XPLMFindDataRef("FJCC/UFMC/W");
    hsxpl_fmc.key_x=XPLMFindDataRef("FJCC/UFMC/X");
    hsxpl_fmc.key_y=XPLMFindDataRef("FJCC/UFMC/Y");
    hsxpl_fmc.key_z=XPLMFindDataRef("FJCC/UFMC/Z");

    hsxpl_fmc.key_slash=XPLMFindDataRef("FJCC/UFMC/barra");
    hsxpl_fmc.key_dot=XPLMFindDataRef("FJCC/UFMC/punto");
    hsxpl_fmc.key_space=XPLMFindDataRef("FJCC/UFMC/espacio");
    hsxpl_fmc.key_plusminus=XPLMFindDataRef("FJCC/UFMC/menos");

    hsxpl_fmc.key_init=XPLMFindDataRef("FJCC/UFMC/INITREF");
    hsxpl_fmc.key_rte=XPLMFindDataRef("FJCC/UFMC/RTE");
    hsxpl_fmc.key_deparr=XPLMFindDataRef("FJCC/UFMC/DEPARR");

    hsxpl_fmc.key_fix=XPLMFindDataRef("FJCC/UFMC/FIX");
    hsxpl_fmc.key_legs=XPLMFindDataRef("FJCC/UFMC/LEGS");
    hsxpl_fmc.key_hold=XPLMFindDataRef("FJCC/UFMC/HOLD");
    hsxpl_fmc.key_prog=XPLMFindDataRef("FJCC/UFMC/PROG");
    hsxpl_fmc.key_exec=XPLMFindDataRef("FJCC/UFMC/EXEC");
    hsxpl_fmc.key_menu=XPLMFindDataRef("FJCC/UFMC/MENU");

    hsxpl_fmc.key_prevpage=XPLMFindDataRef("FJCC/UFMC/PREVPAGE");
    hsxpl_fmc.key_nextpage=XPLMFindDataRef("FJCC/UFMC/NEXTPAGE");
    hsxpl_fmc.key_clr=XPLMFindDataRef("FJCC/UFMC/CLR");
    hsxpl_fmc.key_crz=XPLMFindDataRef("FJCC/UFMC/CRZ_x737");
    hsxpl_fmc.key_clb=XPLMFindDataRef("FJCC/UFMC/CLB_x737");
    hsxpl_fmc.key_des=XPLMFindDataRef("FJCC/UFMC/DES_x737");
    hsxpl_fmc.key_del=XPLMFindDataRef("FJCC/UFMC/DEL");
    hsxpl_fmc.key_fmccom=XPLMFindDataRef("FJCC/UFMC/FMCCOM");
    hsxpl_fmc.key_radnav=XPLMFindDataRef("FJCC/UFMC/NAVRAD");
    hsxpl_fmc.key_atc=XPLMFindDataRef("FJCC/UFMC/ATC");
    hsxpl_fmc.key_vnav=XPLMFindDataRef("FJCC/UFMC/VNAV");
    hsxpl_fmc.exec_light_on=XPLMFindDataRef("FJCC/UFMC/Exec_Light_on");
    hsxpl_fmc.ofst_light_on=XPLMFindDataRef("FJCC/UFMC/Offset_on");
    hsxpl_fmc.msg_light_on=XPLMFindDataRef("FJCC/UFMC/Msg_on");
    if(hsxpl_fmc.msg_light_on==NULL)
      hsxpl_fmc.msg_light_on=XPLMFindDataRef("FJCC/UFMC/MSG_on");



    hsxpl_ufmc_datarefs.line[0]=XPLMFindDataRef("FJCC/UFMC/LINE_1");
    hsxpl_ufmc_datarefs.line[1]=XPLMFindDataRef("FJCC/UFMC/LINE_2");
    hsxpl_ufmc_datarefs.line[2]=XPLMFindDataRef("FJCC/UFMC/LINE_3");
    hsxpl_ufmc_datarefs.line[3]=XPLMFindDataRef("FJCC/UFMC/LINE_4");
    hsxpl_ufmc_datarefs.line[4]=XPLMFindDataRef("FJCC/UFMC/LINE_5");
    hsxpl_ufmc_datarefs.line[5]=XPLMFindDataRef("FJCC/UFMC/LINE_6");
    hsxpl_ufmc_datarefs.line[6]=XPLMFindDataRef("FJCC/UFMC/LINE_7");
    hsxpl_ufmc_datarefs.line[7]=XPLMFindDataRef("FJCC/UFMC/LINE_8");
    hsxpl_ufmc_datarefs.line[8]=XPLMFindDataRef("FJCC/UFMC/LINE_9");
    hsxpl_ufmc_datarefs.line[9]=XPLMFindDataRef("FJCC/UFMC/LINE_10");
    hsxpl_ufmc_datarefs.line[10]=XPLMFindDataRef("FJCC/UFMC/LINE_11");
    hsxpl_ufmc_datarefs.line[11]=XPLMFindDataRef("FJCC/UFMC/LINE_12");
    hsxpl_ufmc_datarefs.line[12]=XPLMFindDataRef("FJCC/UFMC/LINE_13");
    hsxpl_ufmc_datarefs.line[13]=XPLMFindDataRef("FJCC/UFMC/LINE_14");
  }

  else if(ufmc_dataref_type==2) {

    hsxpl_fmc.key_n1limit=XPLMFindDataRef("FJCC/UFMC_1/N1_LIMIT");
    hsxpl_fmc.key_lk1=XPLMFindDataRef("FJCC/UFMC_1/LK1");
    hsxpl_fmc.key_lk2=XPLMFindDataRef("FJCC/UFMC_1/LK2");
    hsxpl_fmc.key_lk3=XPLMFindDataRef("FJCC/UFMC_1/LK3");
    hsxpl_fmc.key_lk4=XPLMFindDataRef("FJCC/UFMC_1/LK4");
    hsxpl_fmc.key_lk5=XPLMFindDataRef("FJCC/UFMC_1/LK5");
    hsxpl_fmc.key_lk6=XPLMFindDataRef("FJCC/UFMC_1/LK6");

    hsxpl_fmc.key_rk1=XPLMFindDataRef("FJCC/UFMC_1/RK1");
    hsxpl_fmc.key_rk2=XPLMFindDataRef("FJCC/UFMC_1/RK2");
    hsxpl_fmc.key_rk3=XPLMFindDataRef("FJCC/UFMC_1/RK3");
    hsxpl_fmc.key_rk4=XPLMFindDataRef("FJCC/UFMC_1/RK4");
    hsxpl_fmc.key_rk5=XPLMFindDataRef("FJCC/UFMC_1/RK5");
    hsxpl_fmc.key_rk6=XPLMFindDataRef("FJCC/UFMC_1/RK6");

    hsxpl_fmc.key_1=XPLMFindDataRef("FJCC/UFMC_1/1");
    hsxpl_fmc.key_2=XPLMFindDataRef("FJCC/UFMC_1/2");
    hsxpl_fmc.key_3=XPLMFindDataRef("FJCC/UFMC_1/3");
    hsxpl_fmc.key_4=XPLMFindDataRef("FJCC/UFMC_1/4");
    hsxpl_fmc.key_5=XPLMFindDataRef("FJCC/UFMC_1/5");
    hsxpl_fmc.key_6=XPLMFindDataRef("FJCC/UFMC_1/6");
    hsxpl_fmc.key_7=XPLMFindDataRef("FJCC/UFMC_1/7");
    hsxpl_fmc.key_8=XPLMFindDataRef("FJCC/UFMC_1/8");
    hsxpl_fmc.key_9=XPLMFindDataRef("FJCC/UFMC_1/9");
    hsxpl_fmc.key_0=XPLMFindDataRef("FJCC/UFMC_1/0");

    hsxpl_fmc.key_a=XPLMFindDataRef("FJCC/UFMC_1/A");
    hsxpl_fmc.key_b=XPLMFindDataRef("FJCC/UFMC_1/B");
    hsxpl_fmc.key_c=XPLMFindDataRef("FJCC/UFMC_1/C");
    hsxpl_fmc.key_d=XPLMFindDataRef("FJCC/UFMC_1/D");
    hsxpl_fmc.key_e=XPLMFindDataRef("FJCC/UFMC_1/E");
    hsxpl_fmc.key_f=XPLMFindDataRef("FJCC/UFMC_1/F");
    hsxpl_fmc.key_g=XPLMFindDataRef("FJCC/UFMC_1/G");
    hsxpl_fmc.key_h=XPLMFindDataRef("FJCC/UFMC_1/H");
    hsxpl_fmc.key_i=XPLMFindDataRef("FJCC/UFMC_1/I");
    hsxpl_fmc.key_j=XPLMFindDataRef("FJCC/UFMC_1/J");
    hsxpl_fmc.key_k=XPLMFindDataRef("FJCC/UFMC_1/K");
    hsxpl_fmc.key_l=XPLMFindDataRef("FJCC/UFMC_1/L");
    hsxpl_fmc.key_m=XPLMFindDataRef("FJCC/UFMC_1/M");
    hsxpl_fmc.key_n=XPLMFindDataRef("FJCC/UFMC_1/N");
    hsxpl_fmc.key_o=XPLMFindDataRef("FJCC/UFMC_1/O");
    hsxpl_fmc.key_p=XPLMFindDataRef("FJCC/UFMC_1/P");
    hsxpl_fmc.key_q=XPLMFindDataRef("FJCC/UFMC_1/Q");
    hsxpl_fmc.key_r=XPLMFindDataRef("FJCC/UFMC_1/R");
    hsxpl_fmc.key_s=XPLMFindDataRef("FJCC/UFMC_1/S");
    hsxpl_fmc.key_t=XPLMFindDataRef("FJCC/UFMC_1/T");
    hsxpl_fmc.key_u=XPLMFindDataRef("FJCC/UFMC_1/U");
    hsxpl_fmc.key_v=XPLMFindDataRef("FJCC/UFMC_1/V");
    hsxpl_fmc.key_w=XPLMFindDataRef("FJCC/UFMC_1/W");
    hsxpl_fmc.key_x=XPLMFindDataRef("FJCC/UFMC_1/X");
    hsxpl_fmc.key_y=XPLMFindDataRef("FJCC/UFMC_1/Y");
    hsxpl_fmc.key_z=XPLMFindDataRef("FJCC/UFMC_1/Z");

    hsxpl_fmc.key_slash=XPLMFindDataRef("FJCC/UFMC_1/barra");
    hsxpl_fmc.key_dot=XPLMFindDataRef("FJCC/UFMC_1/punto");
    hsxpl_fmc.key_space=XPLMFindDataRef("FJCC/UFMC_1/espacio");
    hsxpl_fmc.key_plusminus=XPLMFindDataRef("FJCC/UFMC_1/menos");

    hsxpl_fmc.key_init=XPLMFindDataRef("FJCC/UFMC_1/INITREF");
    hsxpl_fmc.key_rte=XPLMFindDataRef("FJCC/UFMC_1/RTE");
    hsxpl_fmc.key_deparr=XPLMFindDataRef("FJCC/UFMC_1/DEPARR");

    hsxpl_fmc.key_fix=XPLMFindDataRef("FJCC/UFMC_1/FIX");
    hsxpl_fmc.key_legs=XPLMFindDataRef("FJCC/UFMC_1/LEGS");
    hsxpl_fmc.key_hold=XPLMFindDataRef("FJCC/UFMC_1/HOLD");
    hsxpl_fmc.key_prog=XPLMFindDataRef("FJCC/UFMC_1/PROG");
    hsxpl_fmc.key_exec=XPLMFindDataRef("FJCC/UFMC_1/EXEC");
    hsxpl_fmc.key_menu=XPLMFindDataRef("FJCC/UFMC_1/MENU");

    hsxpl_fmc.key_prevpage=XPLMFindDataRef("FJCC/UFMC_1/PREVPAGE");
    hsxpl_fmc.key_nextpage=XPLMFindDataRef("FJCC/UFMC_1/NEXTPAGE");
    hsxpl_fmc.key_clr=XPLMFindDataRef("FJCC/UFMC_1/CLR");
    hsxpl_fmc.key_crz=XPLMFindDataRef("FJCC/UFMC_1/CRZ_x737");
    hsxpl_fmc.key_clb=XPLMFindDataRef("FJCC/UFMC_1/CLB_x737");
    hsxpl_fmc.key_des=XPLMFindDataRef("FJCC/UFMC_1/DES_x737");
    hsxpl_fmc.key_del=XPLMFindDataRef("FJCC/UFMC_1/DEL");
    hsxpl_fmc.key_fmccom=XPLMFindDataRef("FJCC/UFMC_1/FMCCOM");
    hsxpl_fmc.key_radnav=XPLMFindDataRef("FJCC/UFMC_1/NAVRAD");
    hsxpl_fmc.key_atc=XPLMFindDataRef("FJCC/UFMC_1/ATC");
    hsxpl_fmc.key_vnav=XPLMFindDataRef("FJCC/UFMC_1/VNAV");
    hsxpl_fmc.exec_light_on=XPLMFindDataRef("FJCC/UFMC_1/Exec_Light_on");
    hsxpl_fmc.ofst_light_on=XPLMFindDataRef("FJCC/UFMC_1/Offset_on");
    hsxpl_fmc.msg_light_on=XPLMFindDataRef("FJCC/UFMC_1/Msg_on");
    if(hsxpl_fmc.msg_light_on==NULL)
      hsxpl_fmc.msg_light_on=XPLMFindDataRef("FJCC/UFMC_1/MSG_on");


    hsxpl_ufmc_datarefs.line[0]=XPLMFindDataRef("FJCC/UFMC_1/LINE_1");
    hsxpl_ufmc_datarefs.line[1]=XPLMFindDataRef("FJCC/UFMC_1/LINE_2");
    hsxpl_ufmc_datarefs.line[2]=XPLMFindDataRef("FJCC/UFMC_1/LINE_3");
    hsxpl_ufmc_datarefs.line[3]=XPLMFindDataRef("FJCC/UFMC_1/LINE_4");
    hsxpl_ufmc_datarefs.line[4]=XPLMFindDataRef("FJCC/UFMC_1/LINE_5");
    hsxpl_ufmc_datarefs.line[5]=XPLMFindDataRef("FJCC/UFMC_1/LINE_6");
    hsxpl_ufmc_datarefs.line[6]=XPLMFindDataRef("FJCC/UFMC_1/LINE_7");
    hsxpl_ufmc_datarefs.line[7]=XPLMFindDataRef("FJCC/UFMC_1/LINE_8");
    hsxpl_ufmc_datarefs.line[8]=XPLMFindDataRef("FJCC/UFMC_1/LINE_9");
    hsxpl_ufmc_datarefs.line[9]=XPLMFindDataRef("FJCC/UFMC_1/LINE_10");
    hsxpl_ufmc_datarefs.line[10]=XPLMFindDataRef("FJCC/UFMC_1/LINE_11");
    hsxpl_ufmc_datarefs.line[11]=XPLMFindDataRef("FJCC/UFMC_1/LINE_12");
    hsxpl_ufmc_datarefs.line[12]=XPLMFindDataRef("FJCC/UFMC_1/LINE_13");
    hsxpl_ufmc_datarefs.line[13]=XPLMFindDataRef("FJCC/UFMC_1/LINE_14");
  }

  /* Overwrite datarefs for new 3D version of x737 if the datarefs are available */
  if(hsxpl_plane_type == HSXPL_PLANE_X737) {

    XPLMDataRef dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK1");
    if(dr!=NULL)
      hsxpl_fmc.key_lk1=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK2");
    if(dr!=NULL)
      hsxpl_fmc.key_lk2=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK3");
    if(dr!=NULL)
      hsxpl_fmc.key_lk3=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK4");
    if(dr!=NULL)
      hsxpl_fmc.key_lk4=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK5");
    if(dr!=NULL)
      hsxpl_fmc.key_lk5=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LK6");
    if(dr!=NULL)
      hsxpl_fmc.key_lk6=dr;

    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RK1");
    if(dr!=NULL)
      hsxpl_fmc.key_rk1=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RK2");
    if(dr!=NULL)
      hsxpl_fmc.key_rk2=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RK3");
    if(dr!=NULL)
      hsxpl_fmc.key_rk3=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RK4");
    if(dr!=NULL)
      hsxpl_fmc.key_rk4=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RK5");
    if(dr!=NULL)
      hsxpl_fmc.key_rk5=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RK6");
    if(dr!=NULL)
      hsxpl_fmc.key_rk6=dr;

    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM1");
    if(dr!=NULL)
      hsxpl_fmc.key_1=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM2");
    if(dr!=NULL)
      hsxpl_fmc.key_2=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM3");
    if(dr!=NULL)
      hsxpl_fmc.key_3=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM4");
    if(dr!=NULL)
      hsxpl_fmc.key_4=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM5");
    if(dr!=NULL)
      hsxpl_fmc.key_5=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM6");
    if(dr!=NULL)
      hsxpl_fmc.key_6=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM7");
    if(dr!=NULL)
      hsxpl_fmc.key_7=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM8");
    if(dr!=NULL)
      hsxpl_fmc.key_8=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM9");
    if(dr!=NULL)
      hsxpl_fmc.key_9=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUM0");
    if(dr!=NULL)
      hsxpl_fmc.key_0=dr;

    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_A");
    if(dr!=NULL)
      hsxpl_fmc.key_a=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_B");
    if(dr!=NULL)
      hsxpl_fmc.key_b=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_C");
    if(dr!=NULL)
      hsxpl_fmc.key_c=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_D");
    if(dr!=NULL)
      hsxpl_fmc.key_d=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_E");
    if(dr!=NULL)
      hsxpl_fmc.key_e=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_F");
    if(dr!=NULL)
      hsxpl_fmc.key_f=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_G");
    if(dr!=NULL)
      hsxpl_fmc.key_g=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_H");
    if(dr!=NULL)
      hsxpl_fmc.key_h=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_I");
    if(dr!=NULL)
      hsxpl_fmc.key_i=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_J");
    if(dr!=NULL)
      hsxpl_fmc.key_j=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_K");
    if(dr!=NULL)
      hsxpl_fmc.key_k=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_L");
    if(dr!=NULL)
      hsxpl_fmc.key_l=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_M");
    if(dr!=NULL)
      hsxpl_fmc.key_m=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_N");
    if(dr!=NULL)
      hsxpl_fmc.key_n=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_O");
    if(dr!=NULL)
      hsxpl_fmc.key_o=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_P");
    if(dr!=NULL)
      hsxpl_fmc.key_p=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_Q");
    if(dr!=NULL)
      hsxpl_fmc.key_q=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_R");
    if(dr!=NULL)
      hsxpl_fmc.key_r=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_S");
    if(dr!=NULL)
      hsxpl_fmc.key_s=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_T");
    if(dr!=NULL)
      hsxpl_fmc.key_t=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_U");
    if(dr!=NULL)
      hsxpl_fmc.key_u=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_V");
    if(dr!=NULL)
      hsxpl_fmc.key_v=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_W");
    if(dr!=NULL)
      hsxpl_fmc.key_w=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_X");
    if(dr!=NULL)
      hsxpl_fmc.key_x=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_Y");
    if(dr!=NULL)
      hsxpl_fmc.key_y=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_Z");
    if(dr!=NULL)
      hsxpl_fmc.key_z=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUMPLUSMINUS");
    if(dr!=NULL)
      hsxpl_fmc.key_plusminus=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NUMDOT");
    if(dr!=NULL)
      hsxpl_fmc.key_dot=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_BAR");
    if(dr!=NULL)
      hsxpl_fmc.key_slash=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_SPACE");
    if(dr!=NULL)
      hsxpl_fmc.key_space=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_DEL");
    if(dr!=NULL)
      hsxpl_fmc.key_del=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_CLR");
    if(dr!=NULL)
      hsxpl_fmc.key_clr=dr;

    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_INITREF");
    if(dr!=NULL)
      hsxpl_fmc.key_init=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_RTE");
    if(dr!=NULL)
      hsxpl_fmc.key_rte=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_CLB");
    if(dr!=NULL)
      hsxpl_fmc.key_clb=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_CRZ");
    if(dr!=NULL)
      hsxpl_fmc.key_crz=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_DES");
    if(dr!=NULL)
      hsxpl_fmc.key_des=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_MENU");
    if(dr!=NULL)
      hsxpl_fmc.key_menu=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_LEGS");
    if(dr!=NULL)
      hsxpl_fmc.key_legs=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_DEPARR");
    if(dr!=NULL)
      hsxpl_fmc.key_deparr=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_HOLD");
    if(dr!=NULL)
      hsxpl_fmc.key_hold=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_PROG");
    if(dr!=NULL)
      hsxpl_fmc.key_prog=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_EXEC");
    if(dr!=NULL)
      hsxpl_fmc.key_exec=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_FIX");
    if(dr!=NULL)
      hsxpl_fmc.key_fix=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_PREVPAGE");
    if(dr!=NULL)
      hsxpl_fmc.key_prevpage=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NEXTPAGE");
    if(dr!=NULL)
      hsxpl_fmc.key_nextpage=dr;

    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_FMCCOM");
    if(dr!=NULL)
      hsxpl_fmc.key_fmccom=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_ATC");
    if(dr!=NULL)
      hsxpl_fmc.key_atc=dr;
    dr=XPLMFindDataRef("x737/systems/FMC/capt/KEY_NAVRAD");
    if(dr!=NULL)
      hsxpl_fmc.key_radnav=dr;
  }
  return;
}

/* If UFMC is available, send its data */
void hsxpl_send_ufmc_data(void) {

  uint8_t i;
  uint8_t j;
  uint8_t fSize=22;

  /* Build screen, prepare matrix  */

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
    for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=0xFFFFFFFF;
      screen.matrix[i][j].fsize=fSize;
      screen.matrix[i][j].chr=' ';
    }
  }

  /* Now that we have a screen matrix, send it in 4 packets */
  uint32_t fmcType;
  if(hsxpl_plane_type==HSXPL_PLANE_X737 || hsxpl_plane_type==HSXPL_PLANE_IX733)
    fmcType=HSMP_FMC_TYPE_XP_X737;
  else
    fmcType=HSMP_FMC_TYPE_XP_UFMC;


  char line[64];char *cp;

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&fmcType);



    for(i=0;i<4;i++) {
      fSize=22;
      memset(line,0,64);
      XPLMGetDatab(hsxpl_ufmc_datarefs.line[i],line,0,63);

      cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }

      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        screen.matrix[i][j].chr=' ';
      }

      j=0;
      cp=line;
      while(*cp!='\0') {
        if(*cp=='$') {
          if(fSize==22) fSize=26; else fSize=22;
          cp++;
          continue;
        }
        screen.matrix[i][j].fsize=fSize;
        screen.matrix[i][j].chr= *cp;
        cp++; j++; if(j>24) break;
      }

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

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&fmcType);



    for(i=4;i<8;i++) {
      fSize=22;
      memset(line,0,64);
      XPLMGetDatab(hsxpl_ufmc_datarefs.line[i],line,0,63);

      cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }

      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        screen.matrix[i][j].chr=' ';
      }

      j=0;
      cp=line;

      while(*cp!='\0') {
        if(*cp=='$') {
          if(fSize==22) fSize=26; else fSize=22;
          cp++;
          continue;
        }
        screen.matrix[i][j].fsize=fSize;
        screen.matrix[i][j].chr= *cp;
        cp++; j++; if(j>24) break;
      }


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

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&fmcType);



    for(i=8;i<12;i++) {
      fSize=22;
      memset(line,0,64);
      XPLMGetDatab(hsxpl_ufmc_datarefs.line[i],line,0,63);

      cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }

      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        screen.matrix[i][j].chr=' ';
      }

      j=0;
      cp=line;

      while(*cp!='\0') {
        if(*cp=='$') {
          if(fSize==22) fSize=26; else fSize=22;
          cp++;
          continue;
        }
        screen.matrix[i][j].fsize=fSize;
        screen.matrix[i][j].chr= *cp;
        cp++; j++; if(j>24) break;
      }


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

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&fmcType);

    for(i=12;i<14;i++) {
      fSize=22;
      memset(line,0,64);
      XPLMGetDatab(hsxpl_ufmc_datarefs.line[i],line,0,63);

      cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }

      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        screen.matrix[i][j].chr=' ';
      }

      j=0;
      cp=line;

      while(*cp!='\0') {
        if(*cp=='$') {
          if(fSize==22) fSize=26; else fSize=22;
          cp++;
          continue;
        }
        screen.matrix[i][j].fsize=fSize;
        screen.matrix[i][j].chr= *cp;
        cp++; j++; if(j>24) break;
      }

      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }

    }

    uint32_t an=0;
    if(hsxpl_fmc.exec_light_on!=NULL) {
      if(XPLMGetDataf(hsxpl_fmc.exec_light_on)!=0.0) an|=HSMP_FMC_ANNUNC_EXEC;
    }
    if(hsxpl_fmc.msg_light_on!=NULL) {
      if(XPLMGetDataf(hsxpl_fmc.msg_light_on)!=0.0) an|=HSMP_FMC_ANNUNC_VSRU;
    }
    if(hsxpl_fmc.ofst_light_on!=NULL) {
      if(XPLMGetDataf(hsxpl_fmc.ofst_light_on)!=0.0) an|=HSMP_FMC_ANNUNC_VSRD;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&an);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  /*
   hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
   if(pkt!=NULL) {

   uint32_t n;
   if(hsxpl_plane_type==HSXPL_PLANE_X737)
   n=HSMP_FMC_TYPE_XP_X737;
   else
   n=HSMP_FMC_TYPE_XP_UFMC;
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

   #define HSXPL_UFMC_LINE_SIZE  32
   char line[HSXPL_UFMC_LINE_SIZE];
   char *cp;

   if(hsxpl_ufmc_datarefs.line1!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line1,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L01,line);
   }
   if(hsxpl_ufmc_datarefs.line2!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line2,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L02,line);
   }
   if(hsxpl_ufmc_datarefs.line3!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line3,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L03,line);
   }
   if(hsxpl_ufmc_datarefs.line4!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line4,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L04,line);
   }
   if(hsxpl_ufmc_datarefs.line5!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line5,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L05,line);
   }
   if(hsxpl_ufmc_datarefs.line6!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line6,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,line);
   }
   if(hsxpl_ufmc_datarefs.line7!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line7,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L07,line);
   }
   if(hsxpl_ufmc_datarefs.line8!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line8,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L08,line);
   }
   if(hsxpl_ufmc_datarefs.line9!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line9,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L09,line);
   }
   if(hsxpl_ufmc_datarefs.line10!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line10,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L10,line);
   }
   if(hsxpl_ufmc_datarefs.line11!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line11,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L11,line);
   }
   if(hsxpl_ufmc_datarefs.line12!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line12,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L12,line);
   }
   if(hsxpl_ufmc_datarefs.line13!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line13,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L13,line);
   }
   if(hsxpl_ufmc_datarefs.line14!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(hsxpl_ufmc_datarefs.line14,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L14,line);
   }

   uint32_t an=0;
   if(hsxpl_fmc.exec_light_on!=NULL) {
   if(XPLMGetDataf(hsxpl_fmc.exec_light_on)!=0.0) an|=HSMP_FMC_ANNUNC_EXEC;
   }
   if(hsxpl_fmc.msg_light_on!=NULL) {
   if(XPLMGetDataf(hsxpl_fmc.msg_light_on)!=0.0) an|=HSMP_FMC_ANNUNC_VSRU;
   }
   if(hsxpl_fmc.ofst_light_on!=NULL) {
   if(XPLMGetDataf(hsxpl_fmc.ofst_light_on)!=0.0) an|=HSMP_FMC_ANNUNC_VSRD;
   }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&an);

   hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
   free(pkt);
   }

   */
}

