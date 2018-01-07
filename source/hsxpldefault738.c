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
#include "hsxplfmc.h"
#include "hsxplmcp.h"

/* The default Laminar Boeing 737-800 specific datarefs */
struct b738_datarefs_s {
  XPLMDataRef mcpAtArmToggle;
  XPLMDataRef mcpFdCaToggle;
  XPLMDataRef mcpFdCaPos;
  XPLMDataRef mcpFdFoToggle;
  XPLMDataRef mcpFdFoPos;
  XPLMDataRef mcpN1press;
  XPLMDataRef mcpSPEEDpress;
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
  b738data.mcpFdCaPos		= XPLMFindDataRef("laminar/B738/autopilot/flight_director_pos");
  b738data.mcpFdFoPos		= XPLMFindDataRef("laminar/B738/autopilot/flight_director_fo_pos");

  /* push buttons */
  b738data.mcpN1press		= XPLMFindCommand("laminar/B738/autopilot/n1_press");
  b738data.mcpSPEEDpress	= XPLMFindCommand("laminar/B738/autopilot/speed_press");
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
}

/* FD captain side */
void hsairpl_mcp_b738_fd_ca_toggle(void) {
  if ( b738data.mcpFdCaToggle != NULL ) {
     XPLMCommandOnce(b738data.mcpFdCaToggle);
  }
}

uint32_t hsairpl_mcp_b738_get_fd_ca(void) {
  if ( b738data.mcpFdCaPos != NULL ) {
     if (XPLMGetDatai(b738data.mcpFdCaPos) > 0)
     	return(1);
  }
  return(0);
}

/* FD first officer side */
void hsairpl_mcp_b738_fd_fo_toggle(void) {
  if ( b738data.mcpFdFoToggle != NULL ) {
     XPLMCommandOnce(b738data.mcpFdFoToggle);
  }
}

uint32_t hsairpl_mcp_b738_get_fd_fo(void) {
  if ( b738data.mcpFdFoPos != NULL ) {
     if (XPLMGetDatai(b738data.mcpFdFoPos) > 0)
     	return(1);
  }
  return(0);
}

/* A/T Arm */
void hsairpl_mcp_b738_at_arm_toggle(void) {
  if ( b738data.mcpAtArmToggle != NULL ) {
     XPLMCommandOnce(b738data.mcpAtArmToggle);
  }
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
