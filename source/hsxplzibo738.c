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
 * Specifics for the Zibo 737-800
 * https://forums.x-plane.org/index.php?/forums/topic/111305-b738-800-modified/
 *
 */

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

#include "hsxplzibo738.h"
#include "hsxpl.h"
#include "hsxplmisc.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"

#include "hsmpnet.h"

/* FMC types of screen lines */
#define HSAIRXPL_Z783_L_L  0
#define HSAIRXPL_Z783_L_S  1
#define HSAIRXPL_Z783_L_I  2
#define HSAIRXPL_Z783_L_M  3
#define HSAIRXPL_Z783_L_G  4

/* Different FMC line colours in use */
#define HSXPL_Z738_FMC_COL_WHITE         0xFFFFFFFF
#define HSXPL_Z738_FMC_COL_GREEN         0x00FF00FF
#define HSXPL_Z738_FMC_COL_MAGENTA       0xFF00FFFF

/* The zibo 738 specific datarefs */
struct z738_datarefs_s {
  XPLMDataRef lines[14][HSAIRXPL_Z783_L_G+1];
  XPLMDataRef execLight;
  XPLMDataRef mcpAltDialUp;
  XPLMDataRef mcpAltDialDown;
  XPLMDataRef mcpHdgDialUp;
  XPLMDataRef mcpHdgDialDown;
  XPLMDataRef mcpN1led;
  XPLMDataRef mcpSPEEDled;
  XPLMDataRef mcpLVLCHGled;
  XPLMDataRef mcpVNAVled;
  XPLMDataRef mcpLNAVled;
  XPLMDataRef mcpVORLOCled;
  XPLMDataRef mcpAPPled;
  XPLMDataRef mcpHDGSELled;
  XPLMDataRef mcpALTHLDled;
  XPLMDataRef mcpVSled;
  XPLMDataRef mcpCmdAled;
  XPLMDataRef mcpCmdBled;
} z738data;

/* Shared global var to hold FMC specific datarefs */
extern hsxpl_fmc_t hsxpl_fmc;

/* The FMC type as defined from hsmpnet.h */
extern uint32_t hsxpl_fmc_type;

/* The plane type from hsxpl.h */
extern uint32_t hsxpl_plane_type;

#pragma mark Datarefs

/* Sets the datarefs required specific to zibo's 737-800 */
void hsxpl_set_z738_datarefs(void) {

  int i;
  char dref[128];

  /* The Zibo 737-800 is identified by finding the B738 ICAO and ZB738 tail no in
   * its b738.acf file */

  /* Must match ICAO */
  if(strncmp(hsxpl_acf_icao(),"B738",7))
    return;

  /* Must match tailno */
  if(strncmp(hsxpl_acf_tailno(),"ZB738",7))
    return;

  hsxpl_plane_type = HSXPL_PLANE_Z738;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_XP738;

  memset(&z738data,0,sizeof(struct z738_datarefs_s));

  hsxpl_fmc.exec_light_on=XPLMFindDataRef("laminar/B738/indicators/fms_exec_light_pilot");

  for(i=0;i<14;i+=2) {

    sprintf(dref,"laminar/B738/fmc1/Line%02d_L",i/2);
    z738data.lines[i][HSAIRXPL_Z783_L_L]=XPLMFindDataRef(dref);

    sprintf(dref,"laminar/B738/fmc1/Line%02d_S",i/2);
    z738data.lines[i][HSAIRXPL_Z783_L_S]=XPLMFindDataRef(dref);

    sprintf(dref,"laminar/B738/fmc1/Line%02d_I",i/2);
    z738data.lines[i][HSAIRXPL_Z783_L_I]=XPLMFindDataRef(dref);

    sprintf(dref,"laminar/B738/fmc1/Line%02d_M",i/2);
    z738data.lines[i][HSAIRXPL_Z783_L_M]=XPLMFindDataRef(dref);

    sprintf(dref,"laminar/B738/fmc1/Line%02d_G",i/2);
    z738data.lines[i][HSAIRXPL_Z783_L_G]=XPLMFindDataRef(dref);

    if(i>0) {
      sprintf(dref,"laminar/B738/fmc1/Line%02d_X",i/2);
      z738data.lines[i-1][HSAIRXPL_Z783_L_L]=XPLMFindDataRef(dref);
    }
  }

  z738data.lines[13][HSAIRXPL_Z783_L_L]=XPLMFindDataRef("laminar/B738/fmc1/Line_entry");
  z738data.lines[13][HSAIRXPL_Z783_L_I]=XPLMFindDataRef("laminar/B738/fmc1/Line_entry_I");

  /* Set FMC keys, defined below */
  hsxpl_set_z738_fmc_keys();

  /* MCP commands and datarefs, zibo specific */
  z738data.mcpAltDialUp=XPLMFindCommand("laminar/B738/autopilot/altitude_up");
  z738data.mcpAltDialDown=XPLMFindCommand("laminar/B738/autopilot/altitude_dn");
  z738data.mcpHdgDialUp=XPLMFindCommand("laminar/B738/autopilot/heading_up");
  z738data.mcpHdgDialDown=XPLMFindCommand("laminar/B738/autopilot/heading_dn");

  /* Autopilot Lights */
  z738data.mcpN1led	= XPLMFindDataRef("laminar/B738/autopilot/n1_status");
  z738data.mcpSPEEDled	= XPLMFindDataRef("laminar/B738/autopilot/speed_status1");
  z738data.mcpLVLCHGled	= XPLMFindDataRef("laminar/B738/autopilot/lvl_chg_status");
  z738data.mcpVNAVled	= XPLMFindDataRef("laminar/B738/autopilot/vnav_status1");
  z738data.mcpLNAVled	= XPLMFindDataRef("laminar/B738/autopilot/lnav_status");
  z738data.mcpVORLOCled	= XPLMFindDataRef("laminar/B738/autopilot/vorloc_status");
  z738data.mcpAPPled	= XPLMFindDataRef("laminar/B738/autopilot/app_status");
  z738data.mcpHDGSELled	= XPLMFindDataRef("laminar/B738/autopilot/hdg_sel_status");
  z738data.mcpALTHLDled	= XPLMFindDataRef("laminar/B738/autopilot/alt_hld_status");
  z738data.mcpVSled	= XPLMFindDataRef("laminar/B738/autopilot/vs_status");
  z738data.mcpCmdAled	= XPLMFindDataRef("laminar/B738/autopilot/cmd_a_status");
  z738data.mcpCmdBled	= XPLMFindDataRef("laminar/B738/autopilot/cmd_b_status");
}

#pragma mark MCP stuff

/* The Zibo alt and hdg dials seem to be a bit different in that instead of writing to
 * their value, they need to be issued with up and down commands. The default 737-800
 * however seems to accept writing the values and doesn't have these specific commands.
 * Hence the following setters; the mcp getter functions work fine so don't need to
 * be specified separately and the plugin will use the default ones.
 *
 * In order to achieve the desired values, the corresponding UP and DOWN buttons
 * need to be pressed exactly the number of increments or decrements and because
 * the values aren't immediately changed we can't just do a cyle and wait for them
 * to be correct here but we have to rely on the fact that they will be right given
 * the right number of presses. */
void hsairpl_mcp_z738_set_alt_dial(uint32_t v) {

  /* Make sure the new value is a multiple of 100 as the zibo increments in 100's */
  v/=100; v*=100;

  /* Get current MCP value */
  int32_t mcpv=(int32_t)hsairpl_mcp_get_alt_dial();
  mcpv/=100; mcpv*=100;

  /* Determine number of increments or decrements */
  int32_t numcrements = ((int32_t)v-mcpv)/100;

  /* And issue the number of commands accordingly */
  if(numcrements>0) {
    int j;
    for(j=0;j<numcrements;j++) {
      XPLMCommandOnce(z738data.mcpAltDialUp);
    }
  } else if(numcrements<0) {
    int j;
    for(j=0;j>numcrements;j--) {
      XPLMCommandOnce(z738data.mcpAltDialDown);
    }
  }
}

void hsairpl_mcp_z738_set_hdg_dial(uint32_t v) {

  /* Get current MCP value */
  int32_t mcpv=(int32_t)hsairpl_mcp_get_hdg_dial();

  /* Determine number of increments or decrements */
  int32_t numcrements = ((int32_t)v-mcpv);

  /* And issue the number of commands accordingly */
  if(numcrements>0) {
    int j;
    for(j=0;j<numcrements;j++) {
      XPLMCommandOnce(z738data.mcpHdgDialUp);
    }
  } else if(numcrements<0) {
    int j;
    for(j=0;j>numcrements;j--) {
      XPLMCommandOnce(z738data.mcpHdgDialDown);
    }
  }
}

uint32_t hsairpl_mcp_z738_get_n1_led() {
  if ( z738data.mcpN1led != NULL ) {
    return XPLMGetDatai(z738data.mcpN1led);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_speed_led() {
  if ( z738data.mcpSPEEDled != NULL ) {
    return XPLMGetDatai(z738data.mcpSPEEDled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_lvlchg_led() {
  if ( z738data.mcpLVLCHGled != NULL ) {
    return XPLMGetDatai(z738data.mcpLVLCHGled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_vnav_led() {
  if ( z738data.mcpVNAVled != NULL ) {
    return XPLMGetDatai(z738data.mcpVNAVled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_lnav_led() {
  if ( z738data.mcpLNAVled != NULL ) {
    return XPLMGetDatai(z738data.mcpLNAVled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_vorloc_led() {
  if ( z738data.mcpVORLOCled != NULL ) {
    return XPLMGetDatai(z738data.mcpVORLOCled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_app_led() {
  if ( z738data.mcpAPPled != NULL ) {
    return XPLMGetDatai(z738data.mcpAPPled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_hdgsel_led() {
  if ( z738data.mcpHDGSELled != NULL ) {
    return XPLMGetDatai(z738data.mcpHDGSELled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_althld_led() {
  if ( z738data.mcpALTHLDled != NULL ) {
    return XPLMGetDatai(z738data.mcpALTHLDled);
  }
  return 0;
}
uint32_t hsairpl_mcp_z738_get_vs_led() {
  if ( z738data.mcpVSled != NULL ) {
    return XPLMGetDatai(z738data.mcpVSled);
  }
  return 0;
}


#pragma mark FMC stuff

/* FMC datarefs */
extern hsxpl_fmc_t hsxpl_fmc;

/* Sets B737FMC keys to the corresponding command refs */
/* These seem to be zibo specific and the laminar version itself
 * doesn't seem to support them yet */
void hsxpl_set_z738_fmc_keys(void) {

  hsxpl_fmc.key_0=XPLMFindCommand("laminar/B738/button/fmc1_0");
  hsxpl_fmc.key_1=XPLMFindCommand("laminar/B738/button/fmc1_1");
  hsxpl_fmc.key_2=XPLMFindCommand("laminar/B738/button/fmc1_2");
  hsxpl_fmc.key_3=XPLMFindCommand("laminar/B738/button/fmc1_3");
  hsxpl_fmc.key_4=XPLMFindCommand("laminar/B738/button/fmc1_4");
  hsxpl_fmc.key_5=XPLMFindCommand("laminar/B738/button/fmc1_5");
  hsxpl_fmc.key_6=XPLMFindCommand("laminar/B738/button/fmc1_6");
  hsxpl_fmc.key_7=XPLMFindCommand("laminar/B738/button/fmc1_7");
  hsxpl_fmc.key_8=XPLMFindCommand("laminar/B738/button/fmc1_8");
  hsxpl_fmc.key_9=XPLMFindCommand("laminar/B738/button/fmc1_9");

  hsxpl_fmc.key_lk1=XPLMFindCommand("laminar/B738/button/fmc1_1L");
  hsxpl_fmc.key_lk2=XPLMFindCommand("laminar/B738/button/fmc1_2L");
  hsxpl_fmc.key_lk3=XPLMFindCommand("laminar/B738/button/fmc1_3L");
  hsxpl_fmc.key_lk4=XPLMFindCommand("laminar/B738/button/fmc1_4L");
  hsxpl_fmc.key_lk5=XPLMFindCommand("laminar/B738/button/fmc1_5L");
  hsxpl_fmc.key_lk6=XPLMFindCommand("laminar/B738/button/fmc1_6L");

  hsxpl_fmc.key_rk1=XPLMFindCommand("laminar/B738/button/fmc1_1R");
  hsxpl_fmc.key_rk2=XPLMFindCommand("laminar/B738/button/fmc1_2R");
  hsxpl_fmc.key_rk3=XPLMFindCommand("laminar/B738/button/fmc1_3R");
  hsxpl_fmc.key_rk4=XPLMFindCommand("laminar/B738/button/fmc1_4R");
  hsxpl_fmc.key_rk5=XPLMFindCommand("laminar/B738/button/fmc1_5R");
  hsxpl_fmc.key_rk6=XPLMFindCommand("laminar/B738/button/fmc1_6R");

  hsxpl_fmc.key_a=XPLMFindCommand("laminar/B738/button/fmc1_A");
  hsxpl_fmc.key_b=XPLMFindCommand("laminar/B738/button/fmc1_B");
  hsxpl_fmc.key_c=XPLMFindCommand("laminar/B738/button/fmc1_C");
  hsxpl_fmc.key_d=XPLMFindCommand("laminar/B738/button/fmc1_D");
  hsxpl_fmc.key_e=XPLMFindCommand("laminar/B738/button/fmc1_E");
  hsxpl_fmc.key_f=XPLMFindCommand("laminar/B738/button/fmc1_F");
  hsxpl_fmc.key_g=XPLMFindCommand("laminar/B738/button/fmc1_G");
  hsxpl_fmc.key_h=XPLMFindCommand("laminar/B738/button/fmc1_H");
  hsxpl_fmc.key_i=XPLMFindCommand("laminar/B738/button/fmc1_I");
  hsxpl_fmc.key_j=XPLMFindCommand("laminar/B738/button/fmc1_J");
  hsxpl_fmc.key_k=XPLMFindCommand("laminar/B738/button/fmc1_K");
  hsxpl_fmc.key_l=XPLMFindCommand("laminar/B738/button/fmc1_L");
  hsxpl_fmc.key_m=XPLMFindCommand("laminar/B738/button/fmc1_M");
  hsxpl_fmc.key_n=XPLMFindCommand("laminar/B738/button/fmc1_N");
  hsxpl_fmc.key_o=XPLMFindCommand("laminar/B738/button/fmc1_O");
  hsxpl_fmc.key_p=XPLMFindCommand("laminar/B738/button/fmc1_P");
  hsxpl_fmc.key_q=XPLMFindCommand("laminar/B738/button/fmc1_Q");
  hsxpl_fmc.key_r=XPLMFindCommand("laminar/B738/button/fmc1_R");
  hsxpl_fmc.key_s=XPLMFindCommand("laminar/B738/button/fmc1_S");
  hsxpl_fmc.key_t=XPLMFindCommand("laminar/B738/button/fmc1_T");
  hsxpl_fmc.key_u=XPLMFindCommand("laminar/B738/button/fmc1_U");
  hsxpl_fmc.key_v=XPLMFindCommand("laminar/B738/button/fmc1_V");
  hsxpl_fmc.key_w=XPLMFindCommand("laminar/B738/button/fmc1_W");
  hsxpl_fmc.key_x=XPLMFindCommand("laminar/B738/button/fmc1_X");
  hsxpl_fmc.key_y=XPLMFindCommand("laminar/B738/button/fmc1_Y");
  hsxpl_fmc.key_z=XPLMFindCommand("laminar/B738/button/fmc1_Z");

  hsxpl_fmc.key_plusminus=XPLMFindCommand("laminar/B738/button/fmc1_minus");
  hsxpl_fmc.key_dot=XPLMFindCommand("laminar/B738/button/fmc1_period");

  hsxpl_fmc.key_del=XPLMFindCommand("laminar/B738/button/fmc1_del");
  hsxpl_fmc.key_space=XPLMFindCommand("laminar/B738/button/fmc1_SP");
  hsxpl_fmc.key_slash=XPLMFindCommand("laminar/B738/button/fmc1_slash");
  hsxpl_fmc.key_clr=XPLMFindCommand("laminar/B738/button/fmc1_clr");

  hsxpl_fmc.key_init=XPLMFindCommand("laminar/B738/button/fmc1_init_ref");
  hsxpl_fmc.key_rte=XPLMFindCommand("laminar/B738/button/fmc1_rte");
  hsxpl_fmc.key_clb=XPLMFindCommand("laminar/B738/button/fmc1_clb");
  hsxpl_fmc.key_crz=XPLMFindCommand("laminar/B738/button/fmc1_crz");
  hsxpl_fmc.key_des=XPLMFindCommand("laminar/B738/button/fmc1_des");

  hsxpl_fmc.key_menu=XPLMFindCommand("laminar/B738/button/fmc1_menu");
  hsxpl_fmc.key_legs=XPLMFindCommand("laminar/B738/button/fmc1_legs");
  hsxpl_fmc.key_deparr=XPLMFindCommand("laminar/B738/button/fmc1_dep_app");
  hsxpl_fmc.key_hold=XPLMFindCommand("laminar/B738/button/fmc1_hold");
  hsxpl_fmc.key_prog=XPLMFindCommand("laminar/B738/button/fmc1_prog");
  hsxpl_fmc.key_exec=XPLMFindCommand("laminar/B738/button/fmc1_exec");

  hsxpl_fmc.key_n1limit=XPLMFindCommand("laminar/B738/button/fmc1_n1_lim");
  hsxpl_fmc.key_fix=XPLMFindCommand("laminar/B738/button/fmc1_fix");

  hsxpl_fmc.key_prevpage=XPLMFindCommand("laminar/B738/button/fmc1_prev_page");
  hsxpl_fmc.key_nextpage=XPLMFindCommand("laminar/B738/button/fmc1_next_page");

}

/* Sends the z738 specific data to AirEFB i.e. its MCDU screen lines and states */
void hsxpl_send_z738_data(void) {

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
      screen.matrix[i][j].colour=HSXPL_Z738_FMC_COL_WHITE;
      screen.matrix[i][j].fsize=fSize;
      screen.matrix[i][j].chr=' ';
    }
  }

  uint8_t k;
  char line[32];
  /* char str[512]; */
  char *cp=NULL;
  hsmp_pkt_t *pkt=NULL;
  uint32_t fmcType=HSMP_FMC_TYPE_XP_XP738;

  for(k=0;k<4;k++) {
    pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {
      uint8_t i,j;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&fmcType);

      for(i=0;i<4;i++) {
        uint8_t l=(k*4)+i;
        if(l>=14) break;

        fSize=22;

        if(z738data.lines[l][HSAIRXPL_Z783_L_L]!=NULL) {
          memset(line,0,32);
          XPLMGetDatab(z738data.lines[l][HSAIRXPL_Z783_L_L],line,0,24);
          cp=line; j=0;
          while(*cp!='\0') {
            if(*cp!=' ') {
              screen.matrix[l][j].fsize=22;
              screen.matrix[l][j].chr= *cp;
              screen.matrix[l][j].colour=HSXPL_Z738_FMC_COL_WHITE;
            }
            cp++; j++;
            if(j>=24) break;
          }
        }

        if(z738data.lines[l][HSAIRXPL_Z783_L_G]!=NULL) {
          memset(line,0,32);
          XPLMGetDatab(z738data.lines[l][HSAIRXPL_Z783_L_G],line,0,24);
          cp=line; j=0;
          while(*cp!='\0') {
            if(*cp!=' ') {
              screen.matrix[l][j].fsize=22;
              screen.matrix[l][j].chr= *cp;
              screen.matrix[l][j].colour=HSXPL_Z738_FMC_COL_GREEN;
            }
            cp++; j++;
            if(j>=24) break;
          }
        }

        if(z738data.lines[l][HSAIRXPL_Z783_L_M]!=NULL) {
          memset(line,0,32);
          XPLMGetDatab(z738data.lines[l][HSAIRXPL_Z783_L_M],line,0,24);
          cp=line; j=0;
          while(*cp!='\0') {
            if(*cp!=' ') {
              screen.matrix[l][j].fsize=22;
              screen.matrix[l][j].chr= *cp;
              screen.matrix[l][j].colour=HSXPL_Z738_FMC_COL_MAGENTA;
            }
            cp++; j++;
            if(j>=24) break;
          }
        }

        if(z738data.lines[l][HSAIRXPL_Z783_L_S]!=NULL) {
          memset(line,0,32);
          XPLMGetDatab(z738data.lines[l][HSAIRXPL_Z783_L_S],line,0,24);
          cp=line; j=0;
          while(*cp!='\0') {
            if(*cp!=' ') {
              screen.matrix[l][j].fsize=18;
              screen.matrix[l][j].chr= *cp;
              screen.matrix[l][j].colour=HSXPL_Z738_FMC_COL_WHITE;
            }
            cp++; j++;
            if(j>=24) break;
          }
        }

        if(z738data.lines[l][HSAIRXPL_Z783_L_I]!=NULL) {
          memset(line,0,32);
          XPLMGetDatab(z738data.lines[l][HSAIRXPL_Z783_L_I],line,0,24);
          cp=line; j=0;
          while(*cp!='\0') {
            if(*cp!=' ') {
              screen.matrix[l][j].fsize=22;
              screen.matrix[l][j].chr= *cp;
              screen.matrix[l][j].colour=HSXPL_Z738_FMC_COL_WHITE;
            }
            cp++; j++;
            if(j>=24) break;
          }
        }


        for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
          hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[l][j]));
        }
      }

      if(k==3) {

        uint32_t an=0;
        if(hsxpl_fmc.exec_light_on!=NULL) {
          if(XPLMGetDatai(hsxpl_fmc.exec_light_on)!=0) an|=HSMP_FMC_ANNUNC_EXEC;
        }
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&an);
      }

      hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
      free(pkt);
    }
  }
}
