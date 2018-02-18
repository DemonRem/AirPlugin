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
 * Implementation of the X-Plane default Boeing 737-800 datarefs and
 * specific functions.
 *
 */

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

#include "hsxpldefault738.h"
#include "hsxpl.h"
#include "hsxpldatarefs.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"

/* The default Laminar Boeing 737-800 specific datarefs */
struct b738_datarefs_s {
  XPLMDataRef mcpAtArmToggle;
  XPLMDataRef mcpAtArmPos;
  XPLMDataRef mcpFdCaToggle;
  XPLMDataRef mcpFdCaPos;
  XPLMDataRef mcpFdFoToggle;
  XPLMDataRef mcpFdFoPos;
  XPLMDataRef mcpN1press;
  XPLMDataRef mcpSPEEDpress;
  XPLMDataRef mcpCOpress;
  XPLMDataRef mcpLVLCHGpress;
  XPLMDataRef mcpVNAVpress;
  XPLMDataRef mcpLNAVpress;
  XPLMDataRef mcpVORLOCpress;
  XPLMDataRef mcpAPPpress;
  XPLMDataRef mcpHDGSELpress;
  XPLMDataRef mcpALTHLDpress;
  XPLMDataRef mcpVSpress;
  XPLMDataRef mcpCmdApress;
  XPLMDataRef mcpCmdBpress;
  XPLMDataRef mcpCwsApress;
  XPLMDataRef mcpCwsBpress;
  XPLMDataRef mcpMaCaled;
  XPLMDataRef mcpMaFoled;
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
  XPLMDataRef mcpCwsAled;
  XPLMDataRef mcpCwsBled;
  XPLMDataRef efisCptWXRpress;
  XPLMDataRef efisCptSTApress;
  XPLMDataRef efisCptWPTpress;
  XPLMDataRef efisCptARPTpress;
  XPLMDataRef efisCptDATApress;
  XPLMDataRef efisCptPOSpress;
  XPLMDataRef efisCptTERRpress;
  XPLMDataRef efisCptFPVpress;
  XPLMDataRef efisCptMTRSpress;
  XPLMDataRef efisCptBaroSTDpress;
} b738data;

/* The plane type from hsxpl.h */
extern uint32_t hsxpl_plane_type;

void hsxpl_set_default_738_datarefs(void) {

  /* Must match ICAO */
  if(strncmp(hsxpl_acf_icao(),"B738",7))
    return;

  /* The default Laminar 737-800 shares a lot with the Zibo mod 737-800, the latter, in fact,
   * builds upon the the default 737-800. Therefore, in this file, we code what is specific
   * for the default Laminar 737-800 and what is equal on the Zibo mod 737-800. Code that is
   * only for the Zibo mod 737-800 is in hsxplzibo738.c
   */

  /* Must match tailno */
  if ( strncmp(hsxpl_acf_tailno(),"N816NN",7) == 0 )
     hsxpl_plane_type = HSXPL_PLANE_B738;
  else if ( strncmp(hsxpl_acf_tailno(),"ZB738",7) == 0 )
	  hsxpl_plane_type = HSXPL_PLANE_Z738;
       else
	  return;

  memset(&b738data,0,sizeof(struct b738_datarefs_s));

  /* MCP commands and datarefs, default 737-800 specific */
  /* switches */
  b738data.mcpAtArmToggle	= XPLMFindCommand("laminar/B738/autopilot/autothrottle_arm_toggle");
  b738data.mcpFdCaToggle	= XPLMFindCommand("laminar/B738/autopilot/flight_director_toggle");
  b738data.mcpFdFoToggle	= XPLMFindCommand("laminar/B738/autopilot/flight_director_fo_toggle");

  /* switch positions */
  b738data.mcpAtArmPos		= XPLMFindDataRef("laminar/B738/autopilot/autothrottle_arm_pos");
  b738data.mcpFdCaPos		= XPLMFindDataRef("laminar/B738/autopilot/flight_director_pos");
  b738data.mcpFdFoPos		= XPLMFindDataRef("laminar/B738/autopilot/flight_director_fo_pos");

  /* push buttons */
  b738data.mcpN1press		= XPLMFindCommand("laminar/B738/autopilot/n1_press");
  b738data.mcpSPEEDpress	= XPLMFindCommand("laminar/B738/autopilot/speed_press");
  b738data.mcpCOpress		= XPLMFindCommand("sim/autopilot/knots_mach_toggle");
  b738data.mcpLVLCHGpress	= XPLMFindCommand("laminar/B738/autopilot/lvl_chg_press");
  b738data.mcpVNAVpress		= XPLMFindCommand("laminar/B738/autopilot/vnav_press");
  b738data.mcpLNAVpress		= XPLMFindCommand("laminar/B738/autopilot/lnav_press");
  b738data.mcpVORLOCpress	= XPLMFindCommand("laminar/B738/autopilot/vorloc_press");
  b738data.mcpAPPpress		= XPLMFindCommand("laminar/B738/autopilot/app_press");
  b738data.mcpHDGSELpress	= XPLMFindCommand("laminar/B738/autopilot/hdg_sel_press");
  b738data.mcpALTHLDpress	= XPLMFindCommand("laminar/B738/autopilot/alt_hld_press");
  b738data.mcpVSpress		= XPLMFindCommand("laminar/B738/autopilot/vs_press");
  b738data.mcpCmdApress		= XPLMFindCommand("laminar/B738/autopilot/cmd_a_press");
  b738data.mcpCmdBpress		= XPLMFindCommand("laminar/B738/autopilot/cmd_b_press");
  b738data.mcpCwsApress		= XPLMFindCommand("laminar/B738/autopilot/cws_a_press");
  b738data.mcpCwsBpress		= XPLMFindCommand("laminar/B738/autopilot/cws_b_press");

  /* Autopilot Lights */
  b738data.mcpMaCaled	= XPLMFindDataRef("laminar/B738/autopilot/master_capt_status");
  b738data.mcpMaFoled	= XPLMFindDataRef("laminar/B738/autopilot/master_fo_status");
  b738data.mcpN1led	= XPLMFindDataRef("laminar/B738/autopilot/n1_status");
  b738data.mcpSPEEDled	= XPLMFindDataRef("laminar/B738/autopilot/speed_status1");
  b738data.mcpLVLCHGled	= XPLMFindDataRef("laminar/B738/autopilot/lvl_chg_status");
  b738data.mcpVNAVled	= XPLMFindDataRef("laminar/B738/autopilot/vnav_status1");
  b738data.mcpLNAVled	= XPLMFindDataRef("laminar/B738/autopilot/lnav_status");
  b738data.mcpVORLOCled	= XPLMFindDataRef("laminar/B738/autopilot/vorloc_status");
  b738data.mcpAPPled	= XPLMFindDataRef("laminar/B738/autopilot/app_status");
  b738data.mcpHDGSELled	= XPLMFindDataRef("laminar/B738/autopilot/hdg_sel_status");
  b738data.mcpALTHLDled	= XPLMFindDataRef("laminar/B738/autopilot/alt_hld_status");
  b738data.mcpVSled	= XPLMFindDataRef("laminar/B738/autopilot/vs_status");
  b738data.mcpCmdAled	= XPLMFindDataRef("laminar/B738/autopilot/cmd_a_status");
  b738data.mcpCmdBled	= XPLMFindDataRef("laminar/B738/autopilot/cmd_b_status");
  b738data.mcpCwsAled	= XPLMFindDataRef("laminar/B738/autopilot/cws_a_status");
  b738data.mcpCwsBled	= XPLMFindDataRef("laminar/B738/autopilot/cws_b_status");

  /* EFIS commands and datarefs, default 737-800 specific */
  b738data.efisCptWXRpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/wxr_press");
  b738data.efisCptSTApress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/sta_press");
  b738data.efisCptWPTpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/wpt_press");
  b738data.efisCptARPTpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/arpt_press");
  b738data.efisCptDATApress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/data_press");
  b738data.efisCptPOSpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/pos_press");
  b738data.efisCptTERRpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/terr_press");
  b738data.efisCptFPVpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/fpv_press");
  b738data.efisCptMTRSpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/mtrs_press");
  b738data.efisCptBaroSTDpress	= XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/std_press");
}

#pragma mark MCP stuff

/* FD captain side */
void hsairpl_mcp_b738_fd_ca_toggle(uint32_t onoff) {
  if ( b738data.mcpFdCaToggle != NULL ) {
     if ( hsairpl_mcp_b738_get_fd_ca() != onoff )
        XPLMCommandOnce(b738data.mcpFdCaToggle);
  }
}

uint32_t hsairpl_mcp_b738_get_fd_ca() {
  if ( b738data.mcpFdCaPos != NULL ) {
     if (XPLMGetDatai(b738data.mcpFdCaPos) > 0)
     	return(1);
  }
  return(0);
}

/* FD first officer side */
void hsairpl_mcp_b738_fd_fo_toggle(uint32_t onoff) {
  if ( b738data.mcpFdFoToggle != NULL ) {
     if ( hsairpl_mcp_b738_get_fd_fo() != onoff )
        XPLMCommandOnce(b738data.mcpFdFoToggle);
  }
}

uint32_t hsairpl_mcp_b738_get_fd_fo() {
  if ( b738data.mcpFdFoPos != NULL ) {
     if (XPLMGetDatai(b738data.mcpFdFoPos) > 0)
     	return(1);
  }
  return(0);
}

/* A/T Arm */
void hsairpl_mcp_b738_at_arm_toggle(uint32_t armoff) {
  if ( b738data.mcpAtArmToggle != NULL ) {
     if ( hsairpl_mcp_b738_get_at_arm() != armoff )
        XPLMCommandOnce(b738data.mcpAtArmToggle);
  }
}

uint32_t hsairpl_mcp_b738_get_at_arm(void) {
  if ( b738data.mcpAtArmPos != NULL ) {
     if (XPLMGetDatai(b738data.mcpAtArmPos) > 0)
     	return(1);
  }
  return(0);
}

/* N1 */
void hsairpl_mcp_b738_n1_press(void) {
  if ( b738data.mcpN1press != NULL ) {
     XPLMCommandOnce(b738data.mcpN1press);
  }
}
/* SPEED */
void hsairpl_mcp_b738_speed_press(void) {
  if ( b738data.mcpSPEEDpress != NULL ) {
     XPLMCommandOnce(b738data.mcpSPEEDpress);
  }
}
/* C/O IAS/MACH */
void hsairpl_mcp_b738_co_press(void) {
  if (b738data.mcpCOpress != NULL ) {
     XPLMCommandOnce(b738data.mcpCOpress);
  }
}
/* LVL CHG */
void hsairpl_mcp_b738_lvlchg_press(void) {
  if ( b738data.mcpLVLCHGpress != NULL ) {
     XPLMCommandOnce(b738data.mcpLVLCHGpress);
  }
}
/* VNAV */
void hsairpl_mcp_b738_vnav_press(void) {
  if ( b738data.mcpVNAVpress != NULL ) {
     XPLMCommandOnce(b738data.mcpVNAVpress);
  }
}
/* LNAV */
void hsairpl_mcp_b738_lnav_press(void) {
  if ( b738data.mcpLNAVpress != NULL ) {
     XPLMCommandOnce(b738data.mcpLNAVpress);
  }
}
/* VOR LOC */
void hsairpl_mcp_b738_vorloc_press(void) {
  if ( b738data.mcpVORLOCpress != NULL ) {
     XPLMCommandOnce(b738data.mcpVORLOCpress);
  }
}
/* APP */
void hsairpl_mcp_b738_app_press(void) {
  if ( b738data.mcpAPPpress != NULL ) {
     XPLMCommandOnce(b738data.mcpAPPpress);
  }
}
/* HDG SEL */
void hsairpl_mcp_b738_hdgsel_press(void) {
  if ( b738data.mcpHDGSELpress != NULL ) {
     XPLMCommandOnce(b738data.mcpHDGSELpress);
  }
}
/* ALT HLD */
void hsairpl_mcp_b738_althld_press(void) {
  if ( b738data.mcpALTHLDpress != NULL ) {
     XPLMCommandOnce(b738data.mcpALTHLDpress);
  }
}
/* V/S */
void hsairpl_mcp_b738_vs_press(void) {
  if ( b738data.mcpVSpress != NULL ) {
     XPLMCommandOnce(b738data.mcpVSpress);
  }
}
/* CMD A */
void hsairpl_mcp_b738_cmd_a_press(void) {
  if ( b738data.mcpCmdApress != NULL ) {
     XPLMCommandOnce(b738data.mcpCmdApress);
  }
}

/* CMD B */
void hsairpl_mcp_b738_cmd_b_press(void) {
  if ( b738data.mcpCmdBpress != NULL ) {
     XPLMCommandOnce(b738data.mcpCmdBpress);
  }
}

/* CWS A */
void hsairpl_mcp_b738_cws_a_press(void) {
  if ( b738data.mcpCwsApress != NULL ) {
     XPLMCommandOnce(b738data.mcpCwsApress);
  }
}

/* CWS B */
void hsairpl_mcp_b738_cws_b_press(void) {
  if ( b738data.mcpCwsBpress != NULL ) {
     XPLMCommandOnce(b738data.mcpCwsBpress);
  }
}

uint32_t hsairpl_mcp_b738_get_ma_ca_led(void) {
  if ( b738data.mcpMaCaled != NULL ) {
    return XPLMGetDatai(b738data.mcpMaCaled);
  }
  return 0;
}

uint32_t hsairpl_mcp_b738_get_ma_fo_led(void) {
  if ( b738data.mcpMaFoled != NULL ) {
    return XPLMGetDatai(b738data.mcpMaFoled);
  }
  return 0;
}

uint32_t hsairpl_mcp_b738_get_n1_led(void) {
  if ( b738data.mcpN1led != NULL ) {
    return XPLMGetDatai(b738data.mcpN1led);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_speed_led(void) {
  if ( b738data.mcpSPEEDled != NULL ) {
    return XPLMGetDatai(b738data.mcpSPEEDled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_lvlchg_led(void) {
  if ( b738data.mcpLVLCHGled != NULL ) {
    return XPLMGetDatai(b738data.mcpLVLCHGled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_vnav_led(void) {
  if ( b738data.mcpVNAVled != NULL ) {
    return XPLMGetDatai(b738data.mcpVNAVled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_lnav_led(void) {
  if ( b738data.mcpLNAVled != NULL ) {
    return XPLMGetDatai(b738data.mcpLNAVled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_vorloc_led(void) {
  if ( b738data.mcpVORLOCled != NULL ) {
    return XPLMGetDatai(b738data.mcpVORLOCled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_app_led(void) {
  if ( b738data.mcpAPPled != NULL ) {
    return XPLMGetDatai(b738data.mcpAPPled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_hdgsel_led(void) {
  if ( b738data.mcpHDGSELled != NULL ) {
    return XPLMGetDatai(b738data.mcpHDGSELled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_althld_led(void) {
  if ( b738data.mcpALTHLDled != NULL ) {
    return XPLMGetDatai(b738data.mcpALTHLDled);
  }
  return 0;
}
uint32_t hsairpl_mcp_b738_get_vs_led(void) {
  if ( b738data.mcpVSled != NULL ) {
    return XPLMGetDatai(b738data.mcpVSled);
  }
  return 0;
}

uint32_t hsairpl_mcp_b738_get_cmda_led(void) {
  if ( b738data.mcpCmdAled != NULL ) {
    return XPLMGetDatai(b738data.mcpCmdAled);
  }
  return 0;
}

uint32_t hsairpl_mcp_b738_get_cmdb_led(void) {
  if ( b738data.mcpCmdBled != NULL ) {
    return XPLMGetDatai(b738data.mcpCmdBled);
  }
  return 0;
}

uint32_t hsairpl_mcp_b738_get_cwsa_led(void) {
  if ( b738data.mcpCwsAled != NULL ) {
    return XPLMGetDatai(b738data.mcpCwsAled);
  }
  return 0;
}

uint32_t hsairpl_mcp_b738_get_cwsb_led(void) {
  if ( b738data.mcpCwsBled != NULL ) {
    return XPLMGetDatai(b738data.mcpCwsBled);
  }
  return 0;
}


#pragma mark EFIS Captain side stuff

/* WXR */
void hsairpl_efis1_b738_wxr_press(void) {
  if ( b738data.efisCptWXRpress != NULL )
    XPLMCommandOnce(b738data.efisCptWXRpress);
}

/* STA */
void hsairpl_efis1_b738_sta_press(void) {
  if ( b738data.efisCptSTApress != NULL )
    XPLMCommandOnce(b738data.efisCptSTApress);
}

/* WPT */
void hsairpl_efis1_b738_wpt_press(void) {
  if ( b738data.efisCptWPTpress != NULL )
    XPLMCommandOnce(b738data.efisCptWPTpress);
}

/* ARPT */
void hsairpl_efis1_b738_arpt_press(void) {
  if ( b738data.efisCptARPTpress != NULL )
    XPLMCommandOnce(b738data.efisCptARPTpress);
}

/* DATA */
void hsairpl_efis1_b738_data_press(void) {
  if ( b738data.efisCptDATApress != NULL )
    XPLMCommandOnce(b738data.efisCptDATApress);
}
/* POS */
void hsairpl_efis1_b738_pos_press(void) {
  if ( b738data.efisCptPOSpress != NULL )
    XPLMCommandOnce(b738data.efisCptPOSpress);
}

/* TERR */
void hsairpl_efis1_b738_terr_press(void) {
  if ( b738data.efisCptTERRpress != NULL )
    XPLMCommandOnce(b738data.efisCptTERRpress);
}

/* FPV */
void hsairpl_efis1_b738_fpv_press(void) {
  if ( b738data.efisCptFPVpress != NULL )
    XPLMCommandOnce(b738data.efisCptFPVpress);
}

/* MTRS */
void hsairpl_efis1_b738_mtrs_press(void) {
  if ( b738data.efisCptMTRSpress != NULL )
    XPLMCommandOnce(b738data.efisCptMTRSpress);
}

void hsairpl_efis1_b738_baro_std_press(void) {
  if ( b738data.efisCptBaroSTDpress != NULL )
    XPLMCommandOnce(b738data.efisCptBaroSTDpress);
}
