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
 * JA Design A320 (and A330) support: http://jardesign.org/
 *
 */

#include "hsxpljara320.h"
#include "hsxpl.h"
#include "hsxpldatarefs.h"
#include "hsxplmisc.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

hsxpl_a320n_datarefs_t hsxpl_a320n_datarefs;
extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;


/* Looks for datarefs in either sim/custom/xap/mcdu/ or jd/mcdu/ */
XPLMDataRef hsxpl_a320n_lookup_dataref(char *dref) {

  char str[512];
  XPLMDataRef r;

  sprintf(str,"sim/custom/xap/mcdu/%s",dref);
  r = XPLMFindDataRef(str);

  if(r==NULL) {
    sprintf(str,"jd/mcdu/%s",dref);
    r = XPLMFindDataRef(str);
  }
  return r;
}



/* Overrides FMC default datarefs with A320/A330 specific ones */
void hsxpl_set_a320n_datarefs(void) {

  if(strcmp(hsxpl_acf_icao(),"A320") && strcmp(hsxpl_acf_icao(),"A330")) return;
  if(hsxpl_a320n_lookup_dataref("click_0") == NULL) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_A320N;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_A320N;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));
  memset(&hsxpl_a320n_datarefs,0,sizeof(struct hsxpl_a320n_datarefs_s));

  hsxpl_fmc.key_0=hsxpl_a320n_lookup_dataref("click_0");
  hsxpl_fmc.key_1=hsxpl_a320n_lookup_dataref("click_1");
  hsxpl_fmc.key_2=hsxpl_a320n_lookup_dataref("click_2");
  hsxpl_fmc.key_3=hsxpl_a320n_lookup_dataref("click_3");
  hsxpl_fmc.key_4=hsxpl_a320n_lookup_dataref("click_4");
  hsxpl_fmc.key_5=hsxpl_a320n_lookup_dataref("click_5");
  hsxpl_fmc.key_6=hsxpl_a320n_lookup_dataref("click_6");
  hsxpl_fmc.key_7=hsxpl_a320n_lookup_dataref("click_7");
  hsxpl_fmc.key_8=hsxpl_a320n_lookup_dataref("click_8");
  hsxpl_fmc.key_9=hsxpl_a320n_lookup_dataref("click_9");
  hsxpl_fmc.key_dot=hsxpl_a320n_lookup_dataref("click_dot");
  hsxpl_fmc.key_plusminus=hsxpl_a320n_lookup_dataref("plusmin");

  hsxpl_fmc.key_a=hsxpl_a320n_lookup_dataref("click_a");
  hsxpl_fmc.key_b=hsxpl_a320n_lookup_dataref("click_b");
  hsxpl_fmc.key_c=hsxpl_a320n_lookup_dataref("click_c");
  hsxpl_fmc.key_d=hsxpl_a320n_lookup_dataref("click_d");
  hsxpl_fmc.key_e=hsxpl_a320n_lookup_dataref("click_e");
  hsxpl_fmc.key_f=hsxpl_a320n_lookup_dataref("click_f");
  hsxpl_fmc.key_g=hsxpl_a320n_lookup_dataref("click_g");
  hsxpl_fmc.key_h=hsxpl_a320n_lookup_dataref("click_h");
  hsxpl_fmc.key_i=hsxpl_a320n_lookup_dataref("click_i");
  hsxpl_fmc.key_j=hsxpl_a320n_lookup_dataref("click_j");
  hsxpl_fmc.key_k=hsxpl_a320n_lookup_dataref("click_k");
  hsxpl_fmc.key_l=hsxpl_a320n_lookup_dataref("click_l");
  hsxpl_fmc.key_m=hsxpl_a320n_lookup_dataref("click_m");
  hsxpl_fmc.key_n=hsxpl_a320n_lookup_dataref("click_n");
  hsxpl_fmc.key_o=hsxpl_a320n_lookup_dataref("click_o");
  hsxpl_fmc.key_p=hsxpl_a320n_lookup_dataref("click_p");
  hsxpl_fmc.key_q=hsxpl_a320n_lookup_dataref("click_q");
  hsxpl_fmc.key_r=hsxpl_a320n_lookup_dataref("click_r");
  hsxpl_fmc.key_s=hsxpl_a320n_lookup_dataref("click_s");
  hsxpl_fmc.key_t=hsxpl_a320n_lookup_dataref("click_t");
  hsxpl_fmc.key_u=hsxpl_a320n_lookup_dataref("click_u");
  hsxpl_fmc.key_v=hsxpl_a320n_lookup_dataref("click_v");
  hsxpl_fmc.key_w=hsxpl_a320n_lookup_dataref("click_w");
  hsxpl_fmc.key_x=hsxpl_a320n_lookup_dataref("click_x");
  hsxpl_fmc.key_y=hsxpl_a320n_lookup_dataref("click_y");
  hsxpl_fmc.key_z=hsxpl_a320n_lookup_dataref("click_z");

  hsxpl_fmc.key_clr=hsxpl_a320n_lookup_dataref("click_clr");
  hsxpl_fmc.key_space=hsxpl_a320n_lookup_dataref("click_sp");
  hsxpl_fmc.key_slash=hsxpl_a320n_lookup_dataref("click_slash");
  hsxpl_fmc.key_ovfy=hsxpl_a320n_lookup_dataref("click_ovfy");

  hsxpl_fmc.key_airp=hsxpl_a320n_lookup_dataref("click_airp");
  hsxpl_fmc.key_data=hsxpl_a320n_lookup_dataref("click_data");
  hsxpl_fmc.key_dir=hsxpl_a320n_lookup_dataref("click_dir");
  hsxpl_fmc.key_fpln=hsxpl_a320n_lookup_dataref("click_fpln");
  hsxpl_fmc.key_init=hsxpl_a320n_lookup_dataref("click_int");
  hsxpl_fmc.key_fuel=hsxpl_a320n_lookup_dataref("click_fuel");
  hsxpl_fmc.key_menu=hsxpl_a320n_lookup_dataref("click_mcdumenu");
  hsxpl_fmc.key_perf=hsxpl_a320n_lookup_dataref("click_perf");
  hsxpl_fmc.key_radnav=hsxpl_a320n_lookup_dataref("click_radnav");
  hsxpl_fmc.key_prog=hsxpl_a320n_lookup_dataref("click_prog");
  hsxpl_fmc.key_blank=hsxpl_a320n_lookup_dataref("click_blank");

  hsxpl_fmc.key_down=hsxpl_a320n_lookup_dataref("click_up");
  hsxpl_fmc.key_up=hsxpl_a320n_lookup_dataref("click_down");

  hsxpl_fmc.key_left=hsxpl_a320n_lookup_dataref("click_left");
  hsxpl_fmc.key_right=hsxpl_a320n_lookup_dataref("click_right");

  hsxpl_fmc.key_lk1=hsxpl_a320n_lookup_dataref("click_l1");
  hsxpl_fmc.key_lk2=hsxpl_a320n_lookup_dataref("click_l2");
  hsxpl_fmc.key_lk3=hsxpl_a320n_lookup_dataref("click_l3");
  hsxpl_fmc.key_lk4=hsxpl_a320n_lookup_dataref("click_l4");
  hsxpl_fmc.key_lk5=hsxpl_a320n_lookup_dataref("click_l5");
  hsxpl_fmc.key_lk6=hsxpl_a320n_lookup_dataref("click_l6");

  hsxpl_fmc.key_rk1=hsxpl_a320n_lookup_dataref("click_r1");
  hsxpl_fmc.key_rk2=hsxpl_a320n_lookup_dataref("click_r2");
  hsxpl_fmc.key_rk3=hsxpl_a320n_lookup_dataref("click_r3");
  hsxpl_fmc.key_rk4=hsxpl_a320n_lookup_dataref("click_r4");
  hsxpl_fmc.key_rk5=hsxpl_a320n_lookup_dataref("click_r5");
  hsxpl_fmc.key_rk6=hsxpl_a320n_lookup_dataref("click_r6");

  hsxpl_a320n_datarefs.label1=hsxpl_a320n_lookup_dataref("label_1");
  hsxpl_a320n_datarefs.label2=hsxpl_a320n_lookup_dataref("label_2");
  hsxpl_a320n_datarefs.label3=hsxpl_a320n_lookup_dataref("label_3");
  hsxpl_a320n_datarefs.label4=hsxpl_a320n_lookup_dataref("label_4");
  hsxpl_a320n_datarefs.label5=hsxpl_a320n_lookup_dataref("label_5");
  hsxpl_a320n_datarefs.label6=hsxpl_a320n_lookup_dataref("label_6");

  hsxpl_a320n_datarefs.line1a=hsxpl_a320n_lookup_dataref("line_1a");
  hsxpl_a320n_datarefs.line1b=hsxpl_a320n_lookup_dataref("line_1b");
  hsxpl_a320n_datarefs.line1g=hsxpl_a320n_lookup_dataref("line_1g");
  hsxpl_a320n_datarefs.line1m=hsxpl_a320n_lookup_dataref("line_1m");
  hsxpl_a320n_datarefs.line1w=hsxpl_a320n_lookup_dataref("line_1w");
  hsxpl_a320n_datarefs.line1y=hsxpl_a320n_lookup_dataref("line_1y");

  hsxpl_a320n_datarefs.line2a=hsxpl_a320n_lookup_dataref("line_2a");
  hsxpl_a320n_datarefs.line2b=hsxpl_a320n_lookup_dataref("line_2b");
  hsxpl_a320n_datarefs.line2g=hsxpl_a320n_lookup_dataref("line_2g");
  hsxpl_a320n_datarefs.line2m=hsxpl_a320n_lookup_dataref("line_2m");
  hsxpl_a320n_datarefs.line2w=hsxpl_a320n_lookup_dataref("line_2w");
  hsxpl_a320n_datarefs.line2y=hsxpl_a320n_lookup_dataref("line_2y");

  hsxpl_a320n_datarefs.line3a=hsxpl_a320n_lookup_dataref("line_3a");
  hsxpl_a320n_datarefs.line3b=hsxpl_a320n_lookup_dataref("line_3b");
  hsxpl_a320n_datarefs.line3g=hsxpl_a320n_lookup_dataref("line_3g");
  hsxpl_a320n_datarefs.line3m=hsxpl_a320n_lookup_dataref("line_3m");
  hsxpl_a320n_datarefs.line3w=hsxpl_a320n_lookup_dataref("line_3w");
  hsxpl_a320n_datarefs.line3y=hsxpl_a320n_lookup_dataref("line_3y");

  hsxpl_a320n_datarefs.line4a=hsxpl_a320n_lookup_dataref("line_4a");
  hsxpl_a320n_datarefs.line4b=hsxpl_a320n_lookup_dataref("line_4b");
  hsxpl_a320n_datarefs.line4g=hsxpl_a320n_lookup_dataref("line_4g");
  hsxpl_a320n_datarefs.line4m=hsxpl_a320n_lookup_dataref("line_4m");
  hsxpl_a320n_datarefs.line4w=hsxpl_a320n_lookup_dataref("line_4w");
  hsxpl_a320n_datarefs.line4y=hsxpl_a320n_lookup_dataref("line_4y");

  hsxpl_a320n_datarefs.line5a=hsxpl_a320n_lookup_dataref("line_5a");
  hsxpl_a320n_datarefs.line5b=hsxpl_a320n_lookup_dataref("line_5b");
  hsxpl_a320n_datarefs.line5g=hsxpl_a320n_lookup_dataref("line_5g");
  hsxpl_a320n_datarefs.line5m=hsxpl_a320n_lookup_dataref("line_5m");
  hsxpl_a320n_datarefs.line5w=hsxpl_a320n_lookup_dataref("line_5w");
  hsxpl_a320n_datarefs.line5y=hsxpl_a320n_lookup_dataref("line_5y");

  hsxpl_a320n_datarefs.line6a=hsxpl_a320n_lookup_dataref("line_6a");
  hsxpl_a320n_datarefs.line6b=hsxpl_a320n_lookup_dataref("line_6b");
  hsxpl_a320n_datarefs.line6g=hsxpl_a320n_lookup_dataref("line_6g");
  hsxpl_a320n_datarefs.line6m=hsxpl_a320n_lookup_dataref("line_6m");
  hsxpl_a320n_datarefs.line6w=hsxpl_a320n_lookup_dataref("line_6w");
  hsxpl_a320n_datarefs.line6y=hsxpl_a320n_lookup_dataref("line_6y");

  hsxpl_a320n_datarefs.scratchpad=hsxpl_a320n_lookup_dataref("scratchpad");
  hsxpl_a320n_datarefs.scratchpad_a=hsxpl_a320n_lookup_dataref("scratchpad_a");

  hsxpl_a320n_datarefs.title_g=hsxpl_a320n_lookup_dataref("title_g");
  hsxpl_a320n_datarefs.title_w=hsxpl_a320n_lookup_dataref("title_w");
  hsxpl_a320n_datarefs.title_y=hsxpl_a320n_lookup_dataref("title_y");

}

void hsxpl_send_a320n_fmc_data(void) {


  uint8_t i;
  uint8_t j;

  /* Build screen, prepare matrix  */

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
    for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=HSXPL_A320N_FMC_COL_WHITE;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }

  char line[26];line[25]='\0';

  /* Line 0 is title, in green,  white and yellow */
  if(hsxpl_a320n_datarefs.title_g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.title_g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.title_w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.title_w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.title_y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.title_y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }


  /* Scratchpad is line 13 */
  if(hsxpl_a320n_datarefs.scratchpad!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.scratchpad,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.scratchpad_a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.scratchpad_a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }

  /* Line1 = row 2 */
  if(hsxpl_a320n_datarefs.line1w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line1w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.line1g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line1g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.line1b!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line1b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(hsxpl_a320n_datarefs.line1a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line1a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(hsxpl_a320n_datarefs.line1m!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line1m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320n_datarefs.line1y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line1y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }


  /* Line2 = row 4*/
  if(hsxpl_a320n_datarefs.line2w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line2w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.line2g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line2g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.line2b!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line2b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(hsxpl_a320n_datarefs.line2a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line2a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(hsxpl_a320n_datarefs.line2m!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line2m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320n_datarefs.line2y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line2y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Line3 = row 6 */
  if(hsxpl_a320n_datarefs.line3w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line3w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.line3g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line3g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.line3b!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line3b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(hsxpl_a320n_datarefs.line3a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line3a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(hsxpl_a320n_datarefs.line3m!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line3m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320n_datarefs.line3y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line3y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }


  /* Line4 = row 8 */
  if(hsxpl_a320n_datarefs.line4w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line4w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.line4g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line4g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.line4b!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line4b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(hsxpl_a320n_datarefs.line4a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line4a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(hsxpl_a320n_datarefs.line4m!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line4m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320n_datarefs.line4y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line4y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Line5 = row 10 */
  if(hsxpl_a320n_datarefs.line5w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line5w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.line5g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line5g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.line5b!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line5b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(hsxpl_a320n_datarefs.line5a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line5a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(hsxpl_a320n_datarefs.line5m!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line5m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320n_datarefs.line5y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line5y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Line6 = row 12 */
  if(hsxpl_a320n_datarefs.line6w!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line6w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.line6g!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line6g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(hsxpl_a320n_datarefs.line6b!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line6b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(hsxpl_a320n_datarefs.line6a!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line6a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(hsxpl_a320n_datarefs.line6m!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line6m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320n_datarefs.line6y!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.line6y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Labels are line 1,3,5,7,9,11 */
  if(hsxpl_a320n_datarefs.label1!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.label1,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.label2!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.label2,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[3],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.label3!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.label3,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[5],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.label4!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.label4,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[7],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.label5!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.label5,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[9],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(hsxpl_a320n_datarefs.label6!=NULL) {
    XPLMGetDatab(hsxpl_a320n_datarefs.label6,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[11],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }

  /* Now that we have a screen matrix, send it in 4 packets */

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    uint32_t n=HSMP_FMC_TYPE_XP_A320N;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

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

    uint32_t n=HSMP_FMC_TYPE_XP_A320N;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

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

    uint32_t n=HSMP_FMC_TYPE_XP_A320N;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

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
    uint32_t n=HSMP_FMC_TYPE_XP_A320N;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    for(i=12;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }


}



