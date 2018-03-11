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
 * Old QPAC A320 / new ToLis A319 : toliss.com
 *
 */

#include "hsxplqa320.h"
#include "hsxpljara320.h"
#include "hsxpl.h"
#include "hsxpldatarefs.h"
#include "hsxplmisc.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

hsxpl_a320q_datarefs_t hsxpl_a320q_datarefs;

extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;
extern uint32_t hsxpl_active_mcdu;
extern hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;

/* Local prototypes */
void hsxpl_build_a320q_screen_line(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size,uint32_t colour);
void hsxpl_build_a320q_screen_sline(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size);


void hsxpl_set_a320q_datarefs(void) {

  int i;
  char dref[64];

  if(XPLMFindDataRef("AirbusFBW/MCDU1label1m") == NULL) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_A320Q;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_A320Q;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));
  memset(&hsxpl_a320q_datarefs,0,sizeof(struct hsxpl_a320q_datarefs_s));

  if(hsxpl_active_mcdu==2) {

    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU2titlew");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU2titleg");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU2titleb");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU2titlea");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU2titley");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU2titlem");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef("AirbusFBW/MCDU2titles");

    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU2stitlew");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU2stitleg");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU2stitleb");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU2stitlea");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU2stitley");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU2stitlem");

    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU2spw");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU2spg");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU2spb");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU2spa");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU2spy");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU2spm");

  } else {

    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU1titlew");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU1titleg");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU1titleb");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU1titlea");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU1titley");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU1titlem");
    hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef("AirbusFBW/MCDU1titles");

    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU1stitlew");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU1stitleg");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU1stitleb");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU1stitlea");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU1stitley");
    hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU1stitlem");

    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU1spw");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU1spg");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU1spb");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU1spa");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU1spy");
    hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU1spm");
  }

  for(i=0;i<6;i++) {

    sprintf(dref,"AirbusFBW/MCDU%dscont%dw",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%dg",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%db",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%da",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%dy",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%dm",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%ds",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef(dref);

    sprintf(dref,"AirbusFBW/MCDU%dcont%dw",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%dg",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%db",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%da",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%dy",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%dm",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%ds",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef(dref);


    sprintf(dref,"AirbusFBW/MCDU%dlabel%dw",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%dg",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%db",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%da",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%dy",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%dm",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%ds",hsxpl_active_mcdu,i+1);
    hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef(dref);

  }

  if(hsxpl_active_mcdu==2) {

    hsxpl_fmc.key_lk1 = XPLMCreateCommand("AirbusFBW/MCDU2LSK1L", "LSK 1L on MCDU2 pressed");
    hsxpl_fmc.key_lk2 = XPLMCreateCommand("AirbusFBW/MCDU2LSK2L", "LSK 2L on MCDU2 pressed");
    hsxpl_fmc.key_lk3 = XPLMCreateCommand("AirbusFBW/MCDU2LSK3L", "LSK 3L on MCDU2 pressed");
    hsxpl_fmc.key_lk4 = XPLMCreateCommand("AirbusFBW/MCDU2LSK4L", "LSK 4L on MCDU2 pressed");
    hsxpl_fmc.key_lk5 = XPLMCreateCommand("AirbusFBW/MCDU2LSK5L", "LSK 5L on MCDU2 pressed");
    hsxpl_fmc.key_lk6 = XPLMCreateCommand("AirbusFBW/MCDU2LSK6L", "LSK 6L on MCDU2 pressed");
    hsxpl_fmc.key_rk1 = XPLMCreateCommand("AirbusFBW/MCDU2LSK1R", "LSK 1R on MCDU2 pressed");
    hsxpl_fmc.key_rk2 = XPLMCreateCommand("AirbusFBW/MCDU2LSK2R", "LSK 2R on MCDU2 pressed");
    hsxpl_fmc.key_rk3 = XPLMCreateCommand("AirbusFBW/MCDU2LSK3R", "LSK 3R on MCDU2 pressed");
    hsxpl_fmc.key_rk4 = XPLMCreateCommand("AirbusFBW/MCDU2LSK4R", "LSK 4R on MCDU2 pressed");
    hsxpl_fmc.key_rk5 = XPLMCreateCommand("AirbusFBW/MCDU2LSK5R", "LSK 5R on MCDU2 pressed");
    hsxpl_fmc.key_rk6 = XPLMCreateCommand("AirbusFBW/MCDU2LSK6R", "LSK 6R on MCDU2 pressed");

    hsxpl_fmc.key_a = XPLMCreateCommand("AirbusFBW/MCDU2KeyA", "A Key on MCDU2 pressed");
    hsxpl_fmc.key_b = XPLMCreateCommand("AirbusFBW/MCDU2KeyB", "B Key on MCDU2 pressed");
    hsxpl_fmc.key_c = XPLMCreateCommand("AirbusFBW/MCDU2KeyC", "C Key on MCDU2 pressed");
    hsxpl_fmc.key_d = XPLMCreateCommand("AirbusFBW/MCDU2KeyD", "D Key on MCDU2 pressed");
    hsxpl_fmc.key_e = XPLMCreateCommand("AirbusFBW/MCDU2KeyE", "E Key on MCDU2 pressed");
    hsxpl_fmc.key_f = XPLMCreateCommand("AirbusFBW/MCDU2KeyF", "F Key on MCDU2 pressed");
    hsxpl_fmc.key_g = XPLMCreateCommand("AirbusFBW/MCDU2KeyG", "G Key on MCDU2 pressed");
    hsxpl_fmc.key_h = XPLMCreateCommand("AirbusFBW/MCDU2KeyH", "H Key on MCDU2 pressed");
    hsxpl_fmc.key_i = XPLMCreateCommand("AirbusFBW/MCDU2KeyI", "I Key on MCDU2 pressed");
    hsxpl_fmc.key_j = XPLMCreateCommand("AirbusFBW/MCDU2KeyJ", "J Key on MCDU2 pressed");
    hsxpl_fmc.key_k = XPLMCreateCommand("AirbusFBW/MCDU2KeyK", "K Key on MCDU2 pressed");
    hsxpl_fmc.key_l = XPLMCreateCommand("AirbusFBW/MCDU2KeyL", "L Key on MCDU2 pressed");
    hsxpl_fmc.key_m = XPLMCreateCommand("AirbusFBW/MCDU2KeyM", "M Key on MCDU2 pressed");
    hsxpl_fmc.key_n = XPLMCreateCommand("AirbusFBW/MCDU2KeyN", "N Key on MCDU2 pressed");
    hsxpl_fmc.key_o = XPLMCreateCommand("AirbusFBW/MCDU2KeyO", "O Key on MCDU2 pressed");
    hsxpl_fmc.key_p = XPLMCreateCommand("AirbusFBW/MCDU2KeyP", "P Key on MCDU2 pressed");
    hsxpl_fmc.key_q = XPLMCreateCommand("AirbusFBW/MCDU2KeyQ", "Q Key on MCDU2 pressed");
    hsxpl_fmc.key_r = XPLMCreateCommand("AirbusFBW/MCDU2KeyR", "R Key on MCDU2 pressed");
    hsxpl_fmc.key_s = XPLMCreateCommand("AirbusFBW/MCDU2KeyS", "S Key on MCDU2 pressed");
    hsxpl_fmc.key_t = XPLMCreateCommand("AirbusFBW/MCDU2KeyT", "T Key on MCDU2 pressed");
    hsxpl_fmc.key_u = XPLMCreateCommand("AirbusFBW/MCDU2KeyU", "U Key on MCDU2 pressed");
    hsxpl_fmc.key_v = XPLMCreateCommand("AirbusFBW/MCDU2KeyV", "V Key on MCDU2 pressed");
    hsxpl_fmc.key_w = XPLMCreateCommand("AirbusFBW/MCDU2KeyW", "W Key on MCDU2 pressed");
    hsxpl_fmc.key_x = XPLMCreateCommand("AirbusFBW/MCDU2KeyX", "X Key on MCDU2 pressed");
    hsxpl_fmc.key_y = XPLMCreateCommand("AirbusFBW/MCDU2KeyY", "Y Key on MCDU2 pressed");
    hsxpl_fmc.key_z = XPLMCreateCommand("AirbusFBW/MCDU2KeyZ", "Z Key on MCDU2 pressed");

    hsxpl_fmc.key_slash = XPLMCreateCommand("AirbusFBW/MCDU2KeySlash", "Slash Key on MCDU2 pressed");
    hsxpl_fmc.key_space = XPLMCreateCommand("AirbusFBW/MCDU2KeySpace", "Space Key on MCDU2 pressed");
    hsxpl_fmc.key_ovfy = XPLMCreateCommand("AirbusFBW/MCDU2KeyOverfly", "Overfly Key on MCDU2 pressed");
    hsxpl_fmc.key_clr = XPLMCreateCommand("AirbusFBW/MCDU2KeyClear", "Clear Key on MCDU2 pressed");
    hsxpl_fmc.key_1 = XPLMCreateCommand("AirbusFBW/MCDU2Key1", "R Key on MCDU2 pressed");
    hsxpl_fmc.key_2 = XPLMCreateCommand("AirbusFBW/MCDU2Key2", "S Key on MCDU2 pressed");
    hsxpl_fmc.key_3 = XPLMCreateCommand("AirbusFBW/MCDU2Key3", "T Key on MCDU2 pressed");
    hsxpl_fmc.key_4 = XPLMCreateCommand("AirbusFBW/MCDU2Key4", "U Key on MCDU2 pressed");
    hsxpl_fmc.key_5 = XPLMCreateCommand("AirbusFBW/MCDU2Key5", "V Key on MCDU2 pressed");
    hsxpl_fmc.key_6 = XPLMCreateCommand("AirbusFBW/MCDU2Key6", "W Key on MCDU2 pressed");
    hsxpl_fmc.key_7 = XPLMCreateCommand("AirbusFBW/MCDU2Key7", "X Key on MCDU2 pressed");
    hsxpl_fmc.key_8 = XPLMCreateCommand("AirbusFBW/MCDU2Key8", "Y Key on MCDU2 pressed");
    hsxpl_fmc.key_9 = XPLMCreateCommand("AirbusFBW/MCDU2Key9", "Z Key on MCDU2 pressed");
    hsxpl_fmc.key_0 = XPLMCreateCommand("AirbusFBW/MCDU2Key0", "Y Key on MCDU2 pressed");
    hsxpl_fmc.key_dot = XPLMCreateCommand("AirbusFBW/MCDU2KeyDecimal", "Decimal Key on MCDU2 pressed");
    hsxpl_fmc.key_plusminus = XPLMCreateCommand("AirbusFBW/MCDU2KeyPM", "PlusMinus Key on MCDU2 pressed");

    hsxpl_fmc.key_init = XPLMCreateCommand("AirbusFBW/MCDU2Init", "Init button on MCDU2 pressed");
    hsxpl_fmc.key_fpln = XPLMCreateCommand("AirbusFBW/MCDU2Fpln", "Flight plan button on MCDU2 pressed");
    hsxpl_fmc.key_perf = XPLMCreateCommand("AirbusFBW/MCDU2Perf", "Performance button on MCDU2 pressed");
    hsxpl_fmc.key_radnav = XPLMCreateCommand("AirbusFBW/MCDU2RadNav", "Radio Nav button on MCDU2 pressed");
    hsxpl_fmc.key_dir = XPLMCreateCommand("AirbusFBW/MCDU2DirTo", "Dir To button on MCDU2 pressed");
    hsxpl_fmc.key_prog = XPLMCreateCommand("AirbusFBW/MCDU2Prog", "Prog button on MCDU2 pressed");
    hsxpl_fmc.key_data = XPLMCreateCommand("AirbusFBW/MCDU2Data", "Data button on MCDU2 pressed");
    hsxpl_fmc.key_fuel = XPLMCreateCommand("AirbusFBW/MCDU2FuelPred", "Fuel Prediction button on MCDU2 pressed");
    hsxpl_fmc.key_sfpln = XPLMCreateCommand("AirbusFBW/MCDU2SecFpln", "Secondary Fpln button on MCDU2 pressed");
    hsxpl_fmc.key_atc = XPLMCreateCommand("AirbusFBW/MCDU2ATC", "ATC Comm button on MCDU2 pressed");
    hsxpl_fmc.key_menu = XPLMCreateCommand("AirbusFBW/MCDU2Menu", "MCDU Menu button on MCDU2 pressed");
    hsxpl_fmc.key_airp = XPLMCreateCommand("AirbusFBW/MCDU2Airport", "Airport button on MCDU2 pressed");
    hsxpl_fmc.key_down = XPLMCreateCommand("AirbusFBW/MCDU2SlewDown", "Slew Down button on MCDU2 pressed");
    hsxpl_fmc.key_up = XPLMCreateCommand("AirbusFBW/MCDU2SlewUp", "Slew Up button on MCDU2 pressed");
    hsxpl_fmc.key_left = XPLMCreateCommand("AirbusFBW/MCDU2SlewLeft", "Slew Left button on MCDU2 pressed");
    hsxpl_fmc.key_right = XPLMCreateCommand("AirbusFBW/MCDU2SlewRight", "Slew Right button on MCDU2 pressed");

  } else {

    hsxpl_fmc.key_lk1 = XPLMCreateCommand("AirbusFBW/MCDU1LSK1L", "LSK 1L on MCDU1 pressed");
    hsxpl_fmc.key_lk2 = XPLMCreateCommand("AirbusFBW/MCDU1LSK2L", "LSK 2L on MCDU1 pressed");
    hsxpl_fmc.key_lk3 = XPLMCreateCommand("AirbusFBW/MCDU1LSK3L", "LSK 3L on MCDU1 pressed");
    hsxpl_fmc.key_lk4 = XPLMCreateCommand("AirbusFBW/MCDU1LSK4L", "LSK 4L on MCDU1 pressed");
    hsxpl_fmc.key_lk5 = XPLMCreateCommand("AirbusFBW/MCDU1LSK5L", "LSK 5L on MCDU1 pressed");
    hsxpl_fmc.key_lk6 = XPLMCreateCommand("AirbusFBW/MCDU1LSK6L", "LSK 6L on MCDU1 pressed");
    hsxpl_fmc.key_rk1 = XPLMCreateCommand("AirbusFBW/MCDU1LSK1R", "LSK 1R on MCDU1 pressed");
    hsxpl_fmc.key_rk2 = XPLMCreateCommand("AirbusFBW/MCDU1LSK2R", "LSK 2R on MCDU1 pressed");
    hsxpl_fmc.key_rk3 = XPLMCreateCommand("AirbusFBW/MCDU1LSK3R", "LSK 3R on MCDU1 pressed");
    hsxpl_fmc.key_rk4 = XPLMCreateCommand("AirbusFBW/MCDU1LSK4R", "LSK 4R on MCDU1 pressed");
    hsxpl_fmc.key_rk5 = XPLMCreateCommand("AirbusFBW/MCDU1LSK5R", "LSK 5R on MCDU1 pressed");
    hsxpl_fmc.key_rk6 = XPLMCreateCommand("AirbusFBW/MCDU1LSK6R", "LSK 6R on MCDU1 pressed");

    hsxpl_fmc.key_a = XPLMCreateCommand("AirbusFBW/MCDU1KeyA", "A Key on MCDU1 pressed");
    hsxpl_fmc.key_b = XPLMCreateCommand("AirbusFBW/MCDU1KeyB", "B Key on MCDU1 pressed");
    hsxpl_fmc.key_c = XPLMCreateCommand("AirbusFBW/MCDU1KeyC", "C Key on MCDU1 pressed");
    hsxpl_fmc.key_d = XPLMCreateCommand("AirbusFBW/MCDU1KeyD", "D Key on MCDU1 pressed");
    hsxpl_fmc.key_e = XPLMCreateCommand("AirbusFBW/MCDU1KeyE", "E Key on MCDU1 pressed");
    hsxpl_fmc.key_f = XPLMCreateCommand("AirbusFBW/MCDU1KeyF", "F Key on MCDU1 pressed");
    hsxpl_fmc.key_g = XPLMCreateCommand("AirbusFBW/MCDU1KeyG", "G Key on MCDU1 pressed");
    hsxpl_fmc.key_h = XPLMCreateCommand("AirbusFBW/MCDU1KeyH", "H Key on MCDU1 pressed");
    hsxpl_fmc.key_i = XPLMCreateCommand("AirbusFBW/MCDU1KeyI", "I Key on MCDU1 pressed");
    hsxpl_fmc.key_j = XPLMCreateCommand("AirbusFBW/MCDU1KeyJ", "J Key on MCDU1 pressed");
    hsxpl_fmc.key_k = XPLMCreateCommand("AirbusFBW/MCDU1KeyK", "K Key on MCDU1 pressed");
    hsxpl_fmc.key_l = XPLMCreateCommand("AirbusFBW/MCDU1KeyL", "L Key on MCDU1 pressed");
    hsxpl_fmc.key_m = XPLMCreateCommand("AirbusFBW/MCDU1KeyM", "M Key on MCDU1 pressed");
    hsxpl_fmc.key_n = XPLMCreateCommand("AirbusFBW/MCDU1KeyN", "N Key on MCDU1 pressed");
    hsxpl_fmc.key_o = XPLMCreateCommand("AirbusFBW/MCDU1KeyO", "O Key on MCDU1 pressed");
    hsxpl_fmc.key_p = XPLMCreateCommand("AirbusFBW/MCDU1KeyP", "P Key on MCDU1 pressed");
    hsxpl_fmc.key_q = XPLMCreateCommand("AirbusFBW/MCDU1KeyQ", "Q Key on MCDU1 pressed");
    hsxpl_fmc.key_r = XPLMCreateCommand("AirbusFBW/MCDU1KeyR", "R Key on MCDU1 pressed");
    hsxpl_fmc.key_s = XPLMCreateCommand("AirbusFBW/MCDU1KeyS", "S Key on MCDU1 pressed");
    hsxpl_fmc.key_t = XPLMCreateCommand("AirbusFBW/MCDU1KeyT", "T Key on MCDU1 pressed");
    hsxpl_fmc.key_u = XPLMCreateCommand("AirbusFBW/MCDU1KeyU", "U Key on MCDU1 pressed");
    hsxpl_fmc.key_v = XPLMCreateCommand("AirbusFBW/MCDU1KeyV", "V Key on MCDU1 pressed");
    hsxpl_fmc.key_w = XPLMCreateCommand("AirbusFBW/MCDU1KeyW", "W Key on MCDU1 pressed");
    hsxpl_fmc.key_x = XPLMCreateCommand("AirbusFBW/MCDU1KeyX", "X Key on MCDU1 pressed");
    hsxpl_fmc.key_y = XPLMCreateCommand("AirbusFBW/MCDU1KeyY", "Y Key on MCDU1 pressed");
    hsxpl_fmc.key_z = XPLMCreateCommand("AirbusFBW/MCDU1KeyZ", "Z Key on MCDU1 pressed");
    hsxpl_fmc.key_slash = XPLMCreateCommand("AirbusFBW/MCDU1KeySlash", "Slash Key on MCDU1 pressed");
    hsxpl_fmc.key_space = XPLMCreateCommand("AirbusFBW/MCDU1KeySpace", "Space Key on MCDU1 pressed");
    hsxpl_fmc.key_ovfy = XPLMCreateCommand("AirbusFBW/MCDU1KeyOverfly", "Overfly Key on MCDU1 pressed");
    hsxpl_fmc.key_clr = XPLMCreateCommand("AirbusFBW/MCDU1KeyClear", "Clear Key on MCDU1 pressed");
    hsxpl_fmc.key_1 = XPLMCreateCommand("AirbusFBW/MCDU1Key1", "R Key on MCDU1 pressed");
    hsxpl_fmc.key_2 = XPLMCreateCommand("AirbusFBW/MCDU1Key2", "S Key on MCDU1 pressed");
    hsxpl_fmc.key_3 = XPLMCreateCommand("AirbusFBW/MCDU1Key3", "T Key on MCDU1 pressed");
    hsxpl_fmc.key_4 = XPLMCreateCommand("AirbusFBW/MCDU1Key4", "U Key on MCDU1 pressed");
    hsxpl_fmc.key_5 = XPLMCreateCommand("AirbusFBW/MCDU1Key5", "V Key on MCDU1 pressed");
    hsxpl_fmc.key_6 = XPLMCreateCommand("AirbusFBW/MCDU1Key6", "W Key on MCDU1 pressed");
    hsxpl_fmc.key_7 = XPLMCreateCommand("AirbusFBW/MCDU1Key7", "X Key on MCDU1 pressed");
    hsxpl_fmc.key_8 = XPLMCreateCommand("AirbusFBW/MCDU1Key8", "Y Key on MCDU1 pressed");
    hsxpl_fmc.key_9 = XPLMCreateCommand("AirbusFBW/MCDU1Key9", "Z Key on MCDU1 pressed");
    hsxpl_fmc.key_0 = XPLMCreateCommand("AirbusFBW/MCDU1Key0", "Y Key on MCDU1 pressed");
    hsxpl_fmc.key_dot = XPLMCreateCommand("AirbusFBW/MCDU1KeyDecimal", "Decimal Key on MCDU1 pressed");
    hsxpl_fmc.key_plusminus = XPLMCreateCommand("AirbusFBW/MCDU1KeyPM", "PlusMinus Key on MCDU1 pressed");


    hsxpl_fmc.key_init = XPLMCreateCommand("AirbusFBW/MCDU1Init", "Init button on MCDU1 pressed");
    hsxpl_fmc.key_fpln = XPLMCreateCommand("AirbusFBW/MCDU1Fpln", "Flight plan button on MCDU1 pressed");
    hsxpl_fmc.key_perf = XPLMCreateCommand("AirbusFBW/MCDU1Perf", "Performance button on MCDU1 pressed");
    hsxpl_fmc.key_radnav = XPLMCreateCommand("AirbusFBW/MCDU1RadNav", "Radio Nav button on MCDU1 pressed");
    hsxpl_fmc.key_dir = XPLMCreateCommand("AirbusFBW/MCDU1DirTo", "Dir To button on MCDU1 pressed");
    hsxpl_fmc.key_prog = XPLMCreateCommand("AirbusFBW/MCDU1Prog", "Prog button on MCDU1 pressed");
    hsxpl_fmc.key_data = XPLMCreateCommand("AirbusFBW/MCDU1Data", "Data button on MCDU1 pressed");
    hsxpl_fmc.key_fuel = XPLMCreateCommand("AirbusFBW/MCDU1FuelPred", "Fuel Prediction button on MCDU1 pressed");
    hsxpl_fmc.key_sfpln = XPLMCreateCommand("AirbusFBW/MCDU1SecFpln", "Secondary Fpln button on MCDU1 pressed");
    hsxpl_fmc.key_atc = XPLMCreateCommand("AirbusFBW/MCDU1ATC", "ATC Comm button on MCDU1 pressed");
    hsxpl_fmc.key_menu = XPLMCreateCommand("AirbusFBW/MCDU1Menu", "MCDU Menu button on MCDU1 pressed");
    hsxpl_fmc.key_airp = XPLMCreateCommand("AirbusFBW/MCDU1Airport", "Airport button on MCDU1 pressed");
    hsxpl_fmc.key_down = XPLMCreateCommand("AirbusFBW/MCDU1SlewDown", "Slew Down button on MCDU1 pressed");
    hsxpl_fmc.key_up = XPLMCreateCommand("AirbusFBW/MCDU1SlewUp", "Slew Up button on MCDU1 pressed");
    hsxpl_fmc.key_left = XPLMCreateCommand("AirbusFBW/MCDU1SlewLeft", "Slew Left button on MCDU1 pressed");
    hsxpl_fmc.key_right = XPLMCreateCommand("AirbusFBW/MCDU1SlewRight", "Slew Right button on MCDU1 pressed");
  }

}

void hsxpl_send_a320q_fmc_data(void) {


  uint8_t i;
  uint8_t j;

  /* Datarefs in this plane may change? so we try to lazily re-load them here */
/*  hsxpl_set_a320q_datarefs(); */

  /* Build screen, prepare matrix  */

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSXPL_A320Q_NO_COLS;i++) {
    for(j=0;j<HSXPL_A320Q_NO_COLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=HSXPL_A320Q_FMC_COL_WHITE;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }

  char line[HSXPL_A320Q_NO_COLS+1];line[HSXPL_A320Q_NO_COLS]='\0';

  /* Line 0 is title */

  if(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_WHITE);
  }
  if(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_GREEN);
  }
  if(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_CYAN);
  }
  if(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_AMBER);
  }
  if(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_YELLOW);
  }
  if(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_MAGENTA);
  }

  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_WHITE],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_WHITE);
  }
  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_GREEN],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_GREEN);
  }
  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_BLUE],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_CYAN);
  }
  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_AMBER],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_AMBER);
  }
  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_YELLOW],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_YELLOW);
  }
  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_MAGENTA);
  }
  if(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_S]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.title[HSXPL_A320Q_COL_IDX_S],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT);
  }

  /* Scratchpad is line 13 */
  if(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_WHITE);
  }
  if(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_GREEN);
  }
  if(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_CYAN);
  }
  if(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_AMBER);
  }
  if(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_YELLOW);
  }
  if(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
    XPLMGetDatab(hsxpl_a320q_datarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA],line,0,HSXPL_A320Q_NO_COLS);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_MAGENTA);
  }

  /* Now for the rest */
  for(i=0;i<6;i++) {


    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_WHITE],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_WHITE);
    }
    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_GREEN],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_GREEN);
    }
    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_BLUE],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_CYAN);
    }
    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_AMBER],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_AMBER);
    }
    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_YELLOW],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_YELLOW);
    }
    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_MAGENTA],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_MAGENTA);
    }
    if(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_S]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.label[i][HSXPL_A320Q_COL_IDX_S],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL);
    }

    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_WHITE],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_WHITE);
    }
    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_GREEN],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_GREEN);
    }
    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_BLUE],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_CYAN);
    }
    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_AMBER],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_AMBER);
    }
    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_S]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_S],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL);
    }
    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_YELLOW],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_YELLOW);
    }
    if(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_small[i][HSXPL_A320Q_COL_IDX_MAGENTA],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_MAGENTA);
    }

    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_WHITE],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_WHITE);
    }
    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_GREEN],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_GREEN);
    }
    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_BLUE],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_CYAN);
    }
    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_AMBER],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_AMBER);
    }
    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_S]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_S],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT);
    }
    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_YELLOW],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_YELLOW);
    }
    if(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
      XPLMGetDatab(hsxpl_a320q_datarefs.content_large[i][HSXPL_A320Q_COL_IDX_MAGENTA],line,0,HSXPL_A320Q_NO_COLS);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_MAGENTA);
    }
  }

  /* Send active MCDU info */
  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {
    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ACTIVE_MCDU,&hsxpl_active_mcdu);
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  /* Now that we have a screen matrix, send it in 4 packets */
  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);
    for(i=0;i<4;i++) {
      for(j=0;j<HSXPL_A320Q_NO_COLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    for(i=4;i<8;i++) {
      for(j=0;j<HSXPL_A320Q_NO_COLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    for(i=8;i<12;i++) {
      for(j=0;j<HSXPL_A320Q_NO_COLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;
    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    for(i=12;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
      for(j=0;j<HSXPL_A320Q_NO_COLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }
}


void hsxpl_build_a320q_screen_line(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size,uint32_t colour) {

  char *cp=line;
  uint8_t i;

  for(i=0;i<HSXPL_A320Q_NO_COLS;i++) {

    if(*cp=='\0') break;
    if(*cp==' ') { cp++; continue; }

    matrix[i].fsize=font_size;
    if(*cp == '`') matrix[i].chr=(char)161;
    else if(*cp == '|') matrix[i].chr='^';
    else matrix[i].chr= toupper(*cp);
    matrix[i].colour=colour;
    cp++;
  }
}

void hsxpl_build_a320q_screen_sline(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size) {

  char *cp=line;
  uint8_t i;

  for(i=0;i<HSXPL_A320Q_NO_COLS;i++) {

    if(*cp=='\0') break;
    if(*cp==' ') { cp++; continue; }

    matrix[i].fsize=font_size;

    switch(*cp) {

      case('0'):matrix[i].chr='<';matrix[i].colour=HSXPL_A320Q_FMC_COL_CYAN; break;
      case('1'):matrix[i].chr='>';matrix[i].colour=HSXPL_A320Q_FMC_COL_CYAN; break;
      case('2'):matrix[i].chr='<';matrix[i].colour=HSXPL_A320Q_FMC_COL_WHITE; break;
      case('3'):matrix[i].chr='>';matrix[i].colour=HSXPL_A320Q_FMC_COL_WHITE; break;
      case('4'):matrix[i].chr='<';matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;
      case('5'):matrix[i].chr='>';matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;
      case('6'):matrix[i].chr='<';matrix[i].colour=HSXPL_A320Q_FMC_COL_GREEN; break;
      case('7'):matrix[i].chr='>';matrix[i].colour=HSXPL_A320Q_FMC_COL_GREEN; break;
      case('8'):matrix[i].chr='<';matrix[i].colour=HSXPL_A320Q_FMC_COL_YELLOW; break;
      case('9'):matrix[i].chr='>';matrix[i].colour=HSXPL_A320Q_FMC_COL_YELLOW; break;
      case('A'):matrix[i].chr='[';matrix[i].colour=HSXPL_A320Q_FMC_COL_CYAN; break;
      case('B'):matrix[i].chr=']';matrix[i].colour=HSXPL_A320Q_FMC_COL_CYAN; break;
      case('C'):matrix[i].chr='^';matrix[i].colour=HSXPL_A320Q_FMC_COL_WHITE; break;
      case('D'):matrix[i].chr='v';matrix[i].colour=HSXPL_A320Q_FMC_COL_WHITE; break;
      case('E'):matrix[i].chr='*';matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;

      case('F'):matrix[i].chr=' ';matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;
      case('G'):matrix[i].chr=' ';matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;

      case('`'):matrix[i].chr=(char)161;matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;
      case('|'):matrix[i].chr='^';matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;

      default:matrix[i].chr=*cp;matrix[i].colour=HSXPL_A320Q_FMC_COL_AMBER; break;
    }

    cp++;
  }
}

