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
 */

#include "hsxpl.h"
#include "hsxplmcp.h"
#include "hsxplzibo738.h"
#include "hsxpldefault738.h"
#include "hsxplmisc.h"
#include "hsmpmsg.h"
#include "hsmpnet.h"

/* The X-Plane SDK include files */

#include <XPLMProcessing.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMNavigation.h>
#include <XPLMMenus.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>

#include <XPWidgets.h>
#include <XPStandardWidgets.h>
#include <XPUIGraphics.h>

extern uint32_t hsxpl_plane_type;

/* A structure to hold all the datarefs in memory */
struct hsairxpl_mcp_datarefs_s {
  
  /* Autopilot references state */
  
  XPLMDataRef	pos_otemp;
  XPLMDataRef pos_theta;
  
  XPLMDataRef	ap_altdial;
  XPLMDataRef	ap_hdgdial;
  XPLMDataRef	ap_speeddial;
  XPLMDataRef	ap_vspeeddial;
  XPLMDataRef	ap_althold;
  XPLMDataRef	ap_altvnav;
  XPLMDataRef	ap_apon;
  XPLMDataRef	ap_fdmode;
  XPLMDataRef	ap_at;
  XPLMDataRef	ap_speedismac;
  XPLMDataRef	ap_speed_status;
  XPLMDataRef	ap_lnav_status;
  XPLMDataRef	ap_vnav_status;
  XPLMDataRef	ap_hdg_status;
  XPLMDataRef	ap_loc_status;
  XPLMDataRef	ap_alt_status;
  XPLMDataRef	ap_vs_status;
  XPLMDataRef	ap_gs_status;
  XPLMDataRef	ap_n1status;
  XPLMDataRef	ap_obs1p;
  XPLMDataRef	ap_obs2p;
  XPLMDataRef	ap_state;
  XPLMDataRef	ap_hsisource;
  XPLMDataRef	ap_rmisource;
  XPLMDataRef ap_navhdgctl;
  XPLMDataRef ap_mfdrange;
  XPLMDataRef ap_mfdmode;
  XPLMDataRef ap_fdpitch;
  XPLMDataRef ap_fdroll;
  
  XPLMDataRef efis_swxr;
  XPLMDataRef efis_stca;
  XPLMDataRef efis_sapt;
  XPLMDataRef efis_sfix;
  XPLMDataRef efis_svor;
  XPLMDataRef efis_sndb;
  XPLMDataRef efis_n1sel;
  XPLMDataRef efis_n2sel;
  XPLMDataRef efis_altimeter;
  XPLMDataRef efis_hsimode;
  float efis1_local_mfdrange;
  uint32_t efis1_local_mfdshow;
  
  XPLMDataRef rad_nav1sfreq;
  XPLMDataRef rad_nav2sfreq;
  XPLMDataRef rad_adf1freq;
  XPLMDataRef rad_adf2freq;
  XPLMDataRef rad_adf1sfreq;
  XPLMDataRef rad_adf2sfreq;
  XPLMDataRef	rad_com1sfreq;
  XPLMDataRef	rad_com2sfreq;
  XPLMDataRef rad_nav1freq;
  XPLMDataRef rad_nav2freq;
  XPLMDataRef	rad_com1freq;
  XPLMDataRef	rad_com2freq;
  
  /* The x737 set of functions are meant to overcome some dataref
   * differences between the normal aircraft and the EADT x737 project
   * Boeing 737-NG.
   */
  XPLMDataRef	x737_ap_cmda;
  XPLMDataRef	x737_ap_cmdb;
  XPLMDataRef x737_ap_disengage;
  XPLMDataRef	x737_ap_atarm_status;
  XPLMDataRef	x737_ap_speed_status;
  XPLMDataRef	x737_ap_lnav_arm;
  XPLMDataRef	x737_ap_lnav_on;
  XPLMDataRef	x737_ap_vnav_status;
  XPLMDataRef	x737_ap_hdg_status;
  XPLMDataRef	x737_ap_loc_status;
  XPLMDataRef	x737_ap_alt_status;
  XPLMDataRef	x737_ap_vs_on;
  XPLMDataRef	x737_ap_vs_armed;
  XPLMDataRef	x737_ap_vs_armed_or_eng;
  XPLMDataRef	x737_ap_plugin_status;
  XPLMDataRef	x737_ap_n1status;
  XPLMDataRef	x737_ap_n1mode;
  XPLMDataRef	x737_ap_lcstatus;
  XPLMDataRef	x737_ap_fd_on;
  XPLMDataRef	x737_ap_fd2_on;
  XPLMDataRef x737_ap_fdpitch;
  XPLMDataRef x737_ap_fdroll;
  
  XPLMDataRef	x737_ap_alt_dial;
  XPLMDataRef	x737_ap_vs_dial;
  
  XPLMDataRef	x737_ap_spd_mach_dial;
  XPLMDataRef	x737_ap_spd_ias_dial;
  XPLMDataRef	x737_ap_spd_is_mach;
  
  XPLMDataRef x737_ap_app_status;
  XPLMDataRef x737_ap_app_toggle;
  XPLMDataRef x737_ap_vnav_spd;
  
  XPLMDataRef x737_ap_loc_armed;
  XPLMDataRef x737_ap_loc_captured;
  XPLMDataRef x737_ap_gs_armed;
  XPLMDataRef x737_ap_gs_captured;
  
  XPLMDataRef x737_ap_lnavled;
  XPLMDataRef x737_ap_vnavled;
  XPLMDataRef x737_ap_vorlocled;
  XPLMDataRef x737_ap_appled;
  XPLMDataRef x737_ap_ma1led;
  XPLMDataRef x737_ap_ma2led;
  XPLMDataRef x737_ap_overspeed;
  XPLMDataRef x737_ap_spd_intv;
  XPLMDataRef x737_ap_alt_intv;
  XPLMDataRef x737_ap_co;
  
  XPLMDataRef x737_efis_barounits;
  uint32_t x737_local_ap_source;
  
  /* Likewise for the 777 */
  
  XPLMDataRef t7_ap_spd_text;
  XPLMDataRef t7_ap_spd_dial;
  XPLMDataRef t7_ap_hdg_text;
  XPLMDataRef t7_ap_hdg_dial;
  XPLMDataRef t7_ap_alt_text;
  XPLMDataRef t7_ap_alt_dial;
  XPLMDataRef t7_ap_vs_text;
  XPLMDataRef t7_ap_vs_dial;
  
  
  XPLMDataRef t7_ap_co;
  XPLMDataRef t7_ap_cmda;
  XPLMDataRef t7_ap_cmdb;
  XPLMDataRef t7_ap_cmda_led;
  XPLMDataRef t7_ap_cmdb_led;
  
  XPLMDataRef t7_ap_hdg_led;
  XPLMDataRef t7_ap_vs_led;
  XPLMDataRef t7_ap_alt_led;
  XPLMDataRef t7_ap_fchg_led;
  XPLMDataRef t7_ap_lnav_led;
  XPLMDataRef t7_ap_vnav_led;
  XPLMDataRef t7_ap_app_led;
  XPLMDataRef t7_ap_loc_led;
  XPLMDataRef t7_ap_spd_led;
  
  XPLMDataRef t7_ap_at_but;
  XPLMDataRef t7_ap_disc_but;
  XPLMDataRef t7_ap_hdg_sel_but;
  XPLMDataRef t7_ap_hdg_hld_but;
  XPLMDataRef t7_ap_vs_hld_but;
  XPLMDataRef t7_ap_alt_hld_but;
  XPLMDataRef t7_ap_fchg_but;
  XPLMDataRef t7_ap_clbcon_but;
  XPLMDataRef t7_ap_toga_but;
  XPLMDataRef t7_ap_lnav_but;
  XPLMDataRef t7_ap_vnav_but;
  XPLMDataRef t7_ap_loc_but;
  XPLMDataRef t7_ap_app_but;
  XPLMDataRef t7_ap_altintv;
  XPLMDataRef t7_ap_spdintv;
  XPLMDataRef t7_ap_fd1;
  XPLMDataRef t7_ap_fd2;
  XPLMDataRef t7_ap_atarm;
  XPLMDataRef t7_ap_roll_mode_on;
  XPLMDataRef t7_ap_roll_mode_arm;
  XPLMDataRef t7_ap_iasmach;
  XPLMDataRef t7_ap_pitch_mode_on;
  XPLMDataRef t7_ap_pitch_mode_arm;
  
  XPLMDataRef t7_efis_wxr;
  XPLMDataRef t7_efis_sta;
  XPLMDataRef t7_efis_wpt;
  XPLMDataRef t7_efis_arpt;
  XPLMDataRef t7_efis_data;
  XPLMDataRef t7_efis_pos;
  
  XPLMDataRef t7_efis_nd_mode;
  XPLMDataRef t7_efis_nd_range;
  XPLMDataRef t7_efis_nd_ctr;
  XPLMDataRef t7_efis_nd_tfc;
  
  XPLMDataRef t7_efis_fpv;
  XPLMDataRef t7_efis_mtrs;
  
  XPLMDataRef t7_efis_baro_setting;
  XPLMDataRef t7_efis_baro_std;
  XPLMDataRef t7_efis_baro_type;
  
  XPLMDataRef t7_efis_mins_setting;
  XPLMDataRef t7_efis_mins_rst;
  XPLMDataRef t7_efis_mins_type;
  
  XPLMDataRef t7_efis_vas1;
  XPLMDataRef t7_efis_vas2;
  
  /* 757 Flight Factor */
  
  XPLMDataRef b757_ap_spd_text;
  XPLMDataRef b757_ap_spd_dial;
  XPLMDataRef b757_ap_spd_setting;
  
  XPLMDataRef b757_ap_hdg_text;
  XPLMDataRef b757_ap_hdg_dial;
  XPLMDataRef b757_ap_alt_text;
  XPLMDataRef b757_ap_alt_dial;
  XPLMDataRef b757_ap_vs_text;
  XPLMDataRef b757_ap_vs_dial;
  
  XPLMDataRef b757_ap_fd1;
  XPLMDataRef b757_ap_fd2;
  XPLMDataRef b757_ap_atarm;
  
  XPLMDataRef b757_ap_cmda_led;
  XPLMDataRef b757_ap_cmdb_led;
  XPLMDataRef b757_ap_hdg_led;
  XPLMDataRef b757_ap_vs_led;
  XPLMDataRef b757_ap_alt_led;
  XPLMDataRef b757_ap_fchg_led;
  XPLMDataRef b757_ap_lnav_led;
  XPLMDataRef b757_ap_vnav_led;
  XPLMDataRef b757_ap_app_led;
  XPLMDataRef b757_ap_loc_led;
  XPLMDataRef b757_ap_spd_led;
  XPLMDataRef b757_ap_epr_led;
  
  XPLMDataRef b757_ap_spd_but;
  XPLMDataRef b757_ap_disc_but;
  XPLMDataRef b757_ap_hdg_sel_but;
  XPLMDataRef b757_ap_hdg_hld_but;
  XPLMDataRef b757_ap_vs_hld_but;
  XPLMDataRef b757_ap_alt_hld_but;
  XPLMDataRef b757_ap_fchg_but;
  XPLMDataRef b757_ap_epr_but;
  XPLMDataRef b757_ap_lnav_but;
  XPLMDataRef b757_ap_vnav_but;
  XPLMDataRef b757_ap_loc_but;
  XPLMDataRef b757_ap_app_but;
  XPLMDataRef b757_ap_cmda;
  XPLMDataRef b757_ap_cmdb;
  XPLMDataRef b757_ap_iasmach;
  XPLMDataRef b757_ap_roll_mode_on;
  XPLMDataRef b757_ap_roll_mode_arm;
  XPLMDataRef b757_ap_pitch_mode_on;
  XPLMDataRef b757_ap_pitch_mode_arm;
  
  XPLMDataRef b757_efis_terr;
  XPLMDataRef b757_efis_wxr;
  XPLMDataRef b757_efis_sta;
  XPLMDataRef b757_efis_arpt;
  XPLMDataRef b757_efis_wpt;
  XPLMDataRef b757_efis_data;
  XPLMDataRef b757_efis_pos;
  
  uint32_t b757_local_nd_ctr_mode;
  XPLMDataRef b757_efis_nd_mode;
  XPLMDataRef b757_efis_nd_range;
  
  XPLMDataRef b757_efis_mins_setting;
  XPLMDataRef b757_efis_mins_rst;
  
  XPLMDataRef b757_efis_baro_setting1;
  XPLMDataRef b757_efis_baro_setting2;
  XPLMDataRef b757_efis_baro_setting3;
  
  XPLMDataRef b757_efis_vas1_sel;
  XPLMDataRef b757_efis_vas2_sel;
  
  /* 767 Flight Factor */
  
  XPLMDataRef b767_ap_spd_text;
  XPLMDataRef b767_ap_spd_dial;
  
  XPLMDataRef b767_ap_hdg_text;
  XPLMDataRef b767_ap_hdg_dial;
  XPLMDataRef b767_ap_alt_text;
  XPLMDataRef b767_ap_alt_dial;
  XPLMDataRef b767_ap_vs_text;
  XPLMDataRef b767_ap_vs_dial;
  
  XPLMDataRef b767_ap_fd1;
  XPLMDataRef b767_ap_fd2;
  XPLMDataRef b767_ap_atarm;
  
  XPLMDataRef b767_ap_cmda_led;
  XPLMDataRef b767_ap_cmdb_led;
  XPLMDataRef b767_ap_hdg_led;
  XPLMDataRef b767_ap_vs_led;
  XPLMDataRef b767_ap_alt_led;
  XPLMDataRef b767_ap_fchg_led;
  XPLMDataRef b767_ap_lnav_led;
  XPLMDataRef b767_ap_vnav_led;
  XPLMDataRef b767_ap_app_led;
  XPLMDataRef b767_ap_loc_led;
  XPLMDataRef b767_ap_spd_led;
  XPLMDataRef b767_ap_epr_led;
  
  XPLMDataRef b767_ap_spd_but;
  XPLMDataRef b767_ap_disc_but;
  XPLMDataRef b767_ap_hdg_sel_but;
  XPLMDataRef b767_ap_hdg_hld_but;
  XPLMDataRef b767_ap_vs_hld_but;
  XPLMDataRef b767_ap_alt_hld_but;
  XPLMDataRef b767_ap_fchg_but;
  XPLMDataRef b767_ap_epr_but;
  XPLMDataRef b767_ap_lnav_but;
  XPLMDataRef b767_ap_vnav_but;
  XPLMDataRef b767_ap_loc_but;
  XPLMDataRef b767_ap_app_but;
  XPLMDataRef b767_ap_cmda;
  XPLMDataRef b767_ap_cmdb;
  XPLMDataRef b767_ap_iasmach;
  XPLMDataRef b767_ap_roll_mode_on;
  XPLMDataRef b767_ap_roll_mode_arm;
  XPLMDataRef b767_ap_pitch_mode_on;
  XPLMDataRef b767_ap_pitch_mode_arm;
  
  XPLMDataRef b767_efis_terr;
  XPLMDataRef b767_efis_wxr;
  XPLMDataRef b767_efis_sta;
  XPLMDataRef b767_efis_arpt;
  XPLMDataRef b767_efis_wpt;
  XPLMDataRef b767_efis_data;
  XPLMDataRef b767_efis_pos;
  
  XPLMDataRef b767_efis_nd_mode;
  XPLMDataRef b767_efis_nd_range;
  
  uint32_t b767_local_tfc_mode;
  
  XPLMDataRef b767_efis_mins_setting;
  XPLMDataRef b767_efis_mins_rst;
  
  XPLMDataRef b767_efis_baro_setting1;
  XPLMDataRef b767_efis_baro_setting2;
  XPLMDataRef b767_efis_baro_setting3;
  
  XPLMDataRef b767_efis_vas1_sel;
  XPLMDataRef b767_efis_vas2_sel;
  
  
  /* A320 from JAR design */
  
  XPLMDataRef a320n_fcu_fd;
  XPLMDataRef a320n_fcu_ap1;
  XPLMDataRef a320n_fcu_ap2;
  XPLMDataRef a320n_fcu_nd_mode;
  XPLMDataRef a320n_fcu_alt_dial;
  
  /* CRJ200 */
  
  XPLMDataRef crj_ap_fd_btn;
  XPLMDataRef crj_ap_ap_btn;
  XPLMDataRef crj_ap_apdisc_btn;
  XPLMDataRef crj_ap_spd_btn;
  XPLMDataRef crj_ap_hdg_btn;
  XPLMDataRef crj_ap_alt_btn;
  XPLMDataRef crj_ap_vs_btn;
  XPLMDataRef crj_ap_nav_btn;
  XPLMDataRef crj_ap_12_btn;
  XPLMDataRef crj_ap_bcrs_btn;
  XPLMDataRef crj_ap_toga_btn;
  XPLMDataRef crj_ap_xpr_btn;
  XPLMDataRef crj_ap_turb_btn;
  XPLMDataRef crj_ap_appr_btn;
  XPLMDataRef crj_ap_alt_sync;
  XPLMDataRef crj_ap_hdg_sync;
  XPLMDataRef crj_ap_crs1_sync;
  XPLMDataRef crj_ap_spd_sync;
  XPLMDataRef crj_ap_spd_dial;
  XPLMDataRef crj_ap_hdg_dial;
  XPLMDataRef crj_ap_alt_dial;
  XPLMDataRef crj_ap_vs_dial;
  XPLMDataRef crj_ap_crs_dial;
  XPLMDataRef crj_ap_fd_lit;
  XPLMDataRef crj_ap_eng_lit;
  XPLMDataRef crj_ap_hdg_lit;
  XPLMDataRef crj_ap_nav_lit;
  XPLMDataRef crj_ap_alt_lit;
  XPLMDataRef crj_ap_spd_lit;
  XPLMDataRef crj_ap_vs_lit;
  XPLMDataRef crj_ap_xpr_lit;
  XPLMDataRef crj_ap_turb_lit;
  XPLMDataRef crj_ap_bank_lit;
  XPLMDataRef crj_ap_bcrs_lit;
  XPLMDataRef crj_ap_appr_lit;
  
  XPLMDataRef crj_efis_baro_dial;
  XPLMDataRef crj_efis_baro_std_but;
  XPLMDataRef crj_efis_baro_hpa_but;
  
  XPLMDataRef crj_efis_brg1_but;
  XPLMDataRef crj_efis_brg2_but;
  XPLMDataRef crj_efis_nav_source;
  XPLMDataRef crj_efis_mfd_format;
  XPLMDataRef crj_efis_mfd_range;
  
  XPLMDataRef x733_fd1_ann;
  XPLMDataRef x733_fd2_ann;
  XPLMDataRef x733_fd1_act;
  XPLMDataRef x733_fd2_act;
  XPLMDataRef x733_cmda_act;
  XPLMDataRef x733_cmdb_act;
  XPLMDataRef x733_cwsa_act;
  XPLMDataRef x733_cwsb_act;
  XPLMDataRef x733_cmda_ann;
  XPLMDataRef x733_cmdb_ann;
  XPLMDataRef x733_cwsa_ann;
  XPLMDataRef x733_cwsb_ann;
  XPLMDataRef x733_apdisc;
  XPLMDataRef x733_atarm_act;
  XPLMDataRef x733_atarm_ann;
  XPLMDataRef x733_vs_dial;
  XPLMDataRef x733_n1_ann;
  XPLMDataRef x733_n1_act;
  XPLMDataRef x733_spd_ann;
  XPLMDataRef x733_spd_act;
  XPLMDataRef x733_lc_ann;
  XPLMDataRef x733_lc_act;
  XPLMDataRef x733_vnav_ann;
  XPLMDataRef x733_vnav_act;
  XPLMDataRef x733_hdg_ann;
  XPLMDataRef x733_hdg_act;
  XPLMDataRef x733_lnav_ann;
  XPLMDataRef x733_lnav_act;
  XPLMDataRef x733_vorloc_ann;
  XPLMDataRef x733_vorloc_act;
  XPLMDataRef x733_app_ann;
  XPLMDataRef x733_app_act;
  XPLMDataRef x733_alt_ann;
  XPLMDataRef x733_alt_act;
  XPLMDataRef x733_vs_ann;
  XPLMDataRef x733_vs_act;
  XPLMDataRef x733_hsi_range;
  XPLMDataRef x733_hsi_wxr_act;
  XPLMDataRef x733_hsi_navaid_act;
  XPLMDataRef x733_hsi_arpt_act;
  XPLMDataRef x733_hsi_wpt_act;
  XPLMDataRef x733_hsi_mode;
  
  
} hsairxpl_mcp_datarefs;

void hsairpl_mcp_update_datarefs(void) {
  
  memset(&hsairxpl_mcp_datarefs,0,sizeof(hsairxpl_mcp_datarefs));
  
  hsairxpl_mcp_datarefs.pos_theta=XPLMFindDataRef("sim/flightmodel/position/theta");
  hsairxpl_mcp_datarefs.pos_otemp=XPLMFindDataRef("sim/weather/temperature_ambient_c");
  hsairxpl_mcp_datarefs.ap_altdial=XPLMFindDataRef("sim/cockpit2/autopilot/altitude_dial_ft");
  hsairxpl_mcp_datarefs.ap_hdgdial=XPLMFindDataRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");
  hsairxpl_mcp_datarefs.ap_speeddial=XPLMFindDataRef("sim/cockpit2/autopilot/airspeed_dial_kts_mach");
  hsairxpl_mcp_datarefs.ap_vspeeddial=XPLMFindDataRef("sim/cockpit2/autopilot/vvi_dial_fpm");
  hsairxpl_mcp_datarefs.ap_obs1p=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot");
  hsairxpl_mcp_datarefs.ap_obs2p=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_obs_deg_mag_pilot");
  hsairxpl_mcp_datarefs.ap_speedismac=XPLMFindDataRef("sim/cockpit2/autopilot/airspeed_is_mach");
  hsairxpl_mcp_datarefs.ap_vs_status=XPLMFindDataRef("sim/cockpit2/autopilot/vvi_status");
  hsairxpl_mcp_datarefs.ap_state=XPLMFindDataRef("sim/cockpit/autopilot/autopilot_state");
  hsairxpl_mcp_datarefs.ap_mfdrange=XPLMFindDataRef("sim/cockpit2/EFIS/map_range");
  hsairxpl_mcp_datarefs.ap_mfdmode=XPLMFindDataRef("sim/cockpit2/EFIS/map_mode");
  hsairxpl_mcp_datarefs.ap_fdmode=XPLMFindDataRef("sim/cockpit2/autopilot/flight_director_mode");
  hsairxpl_mcp_datarefs.ap_fdpitch=XPLMFindDataRef("sim/cockpit/autopilot/flight_director_pitch");
  hsairxpl_mcp_datarefs.ap_fdroll=XPLMFindDataRef("sim/cockpit/autopilot/flight_director_roll");
  
  hsairxpl_mcp_datarefs.ap_althold=XPLMFindDataRef("sim/cockpit2/autopilot/altitude_hold_ft");
  hsairxpl_mcp_datarefs.ap_altvnav=XPLMFindDataRef("sim/cockpit2/autopilot/altitude_vnav_ft");
  hsairxpl_mcp_datarefs.ap_apon=XPLMFindDataRef("sim/cockpit2/autopilot/autopilot_on");
  hsairxpl_mcp_datarefs.ap_at=XPLMFindDataRef("sim/cockpit2/autopilot/autothrottle_enabled");
  hsairxpl_mcp_datarefs.ap_speed_status=XPLMFindDataRef("sim/cockpit2/autopilot/speed_status");
  hsairxpl_mcp_datarefs.ap_lnav_status=XPLMFindDataRef("sim/cockpit2/autopilot/nav_status");
  hsairxpl_mcp_datarefs.ap_vnav_status=XPLMFindDataRef("sim/cockpit2/autopilot/vnav_status");
  hsairxpl_mcp_datarefs.ap_hdg_status=XPLMFindDataRef("sim/cockpit2/autopilot/heading_status");
  hsairxpl_mcp_datarefs.ap_loc_status=XPLMFindDataRef("sim/cockpit2/autopilot/approach_status");
  hsairxpl_mcp_datarefs.ap_alt_status=XPLMFindDataRef("sim/cockpit2/autopilot/altitude_hold_status");
  hsairxpl_mcp_datarefs.ap_gs_status=XPLMFindDataRef("sim/cockpit2/autopilot/glideslope_status");
  hsairxpl_mcp_datarefs.ap_navhdgctl=XPLMFindDataRef("sim/operation/override/override_nav_heading");
  hsairxpl_mcp_datarefs.ap_hsisource=XPLMFindDataRef("sim/cockpit2/radios/actuators/HSI_source_select_pilot");
  hsairxpl_mcp_datarefs.ap_rmisource=XPLMFindDataRef("sim/cockpit2/radios/actuators/RMI_source_select_pilot");
  hsairxpl_mcp_datarefs.ap_n1status=NULL;
  
  hsairxpl_mcp_datarefs.efis_swxr=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_weather_on");
  hsairxpl_mcp_datarefs.efis_stca=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_tcas_on");
  hsairxpl_mcp_datarefs.efis_sapt=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_airport_on");
  hsairxpl_mcp_datarefs.efis_sfix=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_fix_on");
  hsairxpl_mcp_datarefs.efis_svor=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_vor_on");
  hsairxpl_mcp_datarefs.efis_sndb=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_ndb_on");
  /*	hsairxpl_mcp_datarefs.efis_n1sel=XPLMFindDataRef("sim/cockpit/switches/EFIS_dme_1_selector"); */
  /*	hsairxpl_mcp_datarefs.efis_n2sel=XPLMFindDataRef("sim/cockpit/switches/EFIS_dme_2_selector"); */
  hsairxpl_mcp_datarefs.efis_n1sel=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_1_selection_pilot");
  hsairxpl_mcp_datarefs.efis_n2sel=XPLMFindDataRef("sim/cockpit2/EFIS/EFIS_2_selection_pilot");
  hsairxpl_mcp_datarefs.efis_altimeter=XPLMFindDataRef("sim/cockpit/misc/barometer_setting");
  hsairxpl_mcp_datarefs.efis_hsimode=XPLMFindDataRef("sim/cockpit2/EFIS/map_mode_is_HSI");
  
  
  hsairxpl_mcp_datarefs.rad_nav1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_frequency_hz");
  hsairxpl_mcp_datarefs.rad_nav2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_frequency_hz");
  hsairxpl_mcp_datarefs.rad_nav1sfreq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_hz");
  hsairxpl_mcp_datarefs.rad_nav2sfreq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_standby_frequency_hz");
  hsairxpl_mcp_datarefs.rad_adf1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_frequency_hz");
  hsairxpl_mcp_datarefs.rad_adf2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_frequency_hz");
  hsairxpl_mcp_datarefs.rad_adf1sfreq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_standby_frequency_hz");
  hsairxpl_mcp_datarefs.rad_adf2sfreq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");
  hsairxpl_mcp_datarefs.rad_com1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_frequency_hz");
  hsairxpl_mcp_datarefs.rad_com2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_frequency_hz");
  hsairxpl_mcp_datarefs.rad_com1sfreq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_hz");
  hsairxpl_mcp_datarefs.rad_com2sfreq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_hz");
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    
    hsairxpl_mcp_datarefs.x733_fd1_ann=XPLMFindDataRef("ixeg/733/mcp/fd_master_pt");
    hsairxpl_mcp_datarefs.x733_fd2_ann=XPLMFindDataRef("ixeg/733/mcp/fd_master_ct");
    hsairxpl_mcp_datarefs.x733_fd1_act=XPLMFindDataRef("ixeg/733/MCP/mcp_plt_fd_act");
    hsairxpl_mcp_datarefs.x733_fd2_act=XPLMFindDataRef("ixeg/733/MCP/mcp_cplt_fd_act");
    hsairxpl_mcp_datarefs.x733_cmda_act=XPLMFindDataRef("ixeg/733/MCP/mcp_a_cmd_act");
    hsairxpl_mcp_datarefs.x733_cmdb_act=XPLMFindDataRef("ixeg/733/MCP/mcp_b_cmd_act");
    hsairxpl_mcp_datarefs.x733_cmda_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_a_comm_ann");
    hsairxpl_mcp_datarefs.x733_cmdb_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_b_comm_ann");
    hsairxpl_mcp_datarefs.x733_cwsa_act=XPLMFindDataRef("ixeg/733/MCP/mcp_a_cws_act");
    hsairxpl_mcp_datarefs.x733_cwsb_act=XPLMFindDataRef("ixeg/733/MCP/mcp_b_cws_act");
    hsairxpl_mcp_datarefs.x733_cwsa_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_a_cws_ann");
    hsairxpl_mcp_datarefs.x733_cwsb_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_b_cws_ann");
    hsairxpl_mcp_datarefs.x733_apdisc=XPLMFindDataRef("ixeg/733/MCP/mcp_ap_disengage_act");
    hsairxpl_mcp_datarefs.x733_atarm_act=XPLMFindDataRef("ixeg/733/MCP/mcp_at_arm_act");
    hsairxpl_mcp_datarefs.x733_atarm_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_at_arm_ann");
    hsairxpl_mcp_datarefs.x733_vs_dial=XPLMFindDataRef("ixeg/733/MCP/mcp_vs_dial_ann");
    hsairxpl_mcp_datarefs.x733_n1_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_n1_ann");
    hsairxpl_mcp_datarefs.x733_n1_act=XPLMFindDataRef("ixeg/733/MCP/mcp_n1_act");
    hsairxpl_mcp_datarefs.x733_spd_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_speed_ann");
    hsairxpl_mcp_datarefs.x733_spd_act=XPLMFindDataRef("ixeg/733/MCP/mcp_speed_act");
    hsairxpl_mcp_datarefs.x733_lc_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_level_change_ann");
    hsairxpl_mcp_datarefs.x733_lc_act=XPLMFindDataRef("ixeg/733/MCP/mcp_level_change_act");
    hsairxpl_mcp_datarefs.x733_vnav_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_vnav_ann");
    hsairxpl_mcp_datarefs.x733_vnav_act=XPLMFindDataRef("ixeg/733/MCP/mcp_vnav_act");
    hsairxpl_mcp_datarefs.x733_hdg_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_hdg_ann");
    hsairxpl_mcp_datarefs.x733_hdg_act=XPLMFindDataRef("ixeg/733/MCP/mcp_hdg_act");
    hsairxpl_mcp_datarefs.x733_lnav_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_lnav_ann");
    hsairxpl_mcp_datarefs.x733_lnav_act=XPLMFindDataRef("ixeg/733/MCP/mcp_lnav_act");
    hsairxpl_mcp_datarefs.x733_vorloc_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_vor_loc_ann");
    hsairxpl_mcp_datarefs.x733_vorloc_act=XPLMFindDataRef("ixeg/733/MCP/mcp_vor_loc_act");
    hsairxpl_mcp_datarefs.x733_app_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_app_ann");
    hsairxpl_mcp_datarefs.x733_app_act=XPLMFindDataRef("ixeg/733/MCP/mcp_app_act");
    hsairxpl_mcp_datarefs.x733_alt_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_alt_hold_ann");
    hsairxpl_mcp_datarefs.x733_alt_act=XPLMFindDataRef("ixeg/733/MCP/mcp_alt_hold_act");
    hsairxpl_mcp_datarefs.x733_vs_ann=XPLMFindDataRef("ixeg/733/MCP/mcp_vs_ann");
    hsairxpl_mcp_datarefs.x733_vs_act=XPLMFindDataRef("ixeg/733/MCP/mcp_vert_speed_act");
    hsairxpl_mcp_datarefs.x733_hsi_range=XPLMFindDataRef("ixeg/733/ehsi/ehsi_range_pt_act");
    hsairxpl_mcp_datarefs.x733_hsi_wxr_act=XPLMFindDataRef("ixeg/733/ehsi/ehsi_wxr_pt_act");
    hsairxpl_mcp_datarefs.x733_hsi_arpt_act=XPLMFindDataRef("ixeg/733/ehsi/ehsi_arpt_pt_act");
    hsairxpl_mcp_datarefs.x733_hsi_navaid_act=XPLMFindDataRef("ixeg/733/ehsi/ehsi_navaid_pt_act");
    hsairxpl_mcp_datarefs.x733_hsi_wpt_act=XPLMFindDataRef("ixeg/733/ehsi/ehsi_wpt_pt_act");
    hsairxpl_mcp_datarefs.x733_hsi_mode=XPLMFindDataRef("ixeg/733/ehsi/ehsi_mode_pt_act");
  }
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    
    hsairxpl_mcp_datarefs.x737_ap_cmda=XPLMFindDataRef("x737/systems/afds/CMD_A");
    hsairxpl_mcp_datarefs.x737_ap_cmdb=XPLMFindDataRef("x737/systems/afds/CMD_B");
    hsairxpl_mcp_datarefs.x737_ap_disengage=XPLMFindDataRef("x737/systems/afds/APengaged");
    
    hsairxpl_mcp_datarefs.x737_ap_atarm_status=XPLMFindDataRef("x737/systems/athr/athr_armed");
    hsairxpl_mcp_datarefs.x737_ap_lnav_arm=XPLMFindDataRef("x737/systems/afds/LNAV_arm");
    hsairxpl_mcp_datarefs.x737_ap_lnav_on=XPLMFindDataRef("x737/systems/afds/LNAV");
    hsairxpl_mcp_datarefs.x737_ap_vs_armed_or_eng=XPLMFindDataRef("x737/systems/afds/VS_armed_or_eng");
    
    
    hsairxpl_mcp_datarefs.x737_ap_vs_on=XPLMFindDataRef("x737/systems/PFD/PFD_VS_mode_on");
    if(hsairxpl_mcp_datarefs.x737_ap_vs_on==NULL)
      hsairxpl_mcp_datarefs.x737_ap_vs_on=XPLMFindDataRef("x737/systems/afds/VS");
    
    hsairxpl_mcp_datarefs.x737_ap_vs_armed=XPLMFindDataRef("x737/systems/PFD/PFD_VSARMED_mode_on");
    if(hsairxpl_mcp_datarefs.x737_ap_vs_armed==NULL)
      hsairxpl_mcp_datarefs.x737_ap_vs_armed=XPLMFindDataRef("x737/systems/afds/VS_arm");
    
    hsairxpl_mcp_datarefs.x737_ap_vnav_status=XPLMFindDataRef("x737/systems/afds/VNAV");
    hsairxpl_mcp_datarefs.x737_ap_vnav_spd=XPLMFindDataRef("x737/systems/afds/VNAV_SPD");
    hsairxpl_mcp_datarefs.x737_ap_hdg_status=XPLMFindDataRef("x737/systems/afds/HDG");
    hsairxpl_mcp_datarefs.x737_ap_app_status=XPLMFindDataRef("x737/systems/afds/APP");
    hsairxpl_mcp_datarefs.x737_ap_app_toggle=XPLMFindDataRef("x737/systems/afds/APP_toggle");
    hsairxpl_mcp_datarefs.x737_ap_loc_status=XPLMFindDataRef("x737/systems/afds/VORLOC");
    hsairxpl_mcp_datarefs.x737_ap_loc_armed=XPLMFindDataRef("x737/systems/PFD/PFD_VORLOCARMED_mode_on");
    hsairxpl_mcp_datarefs.x737_ap_loc_captured=XPLMFindDataRef("x737/systems/PFD/PFD_VORLOC_mode_on");
    
    hsairxpl_mcp_datarefs.x737_ap_alt_status=XPLMFindDataRef("x737/systems/afds/ALTHLD");
    hsairxpl_mcp_datarefs.x737_ap_speed_status=XPLMFindDataRef("x737/systems/athr/MCPSPD_mode");
    hsairxpl_mcp_datarefs.x737_ap_n1status=XPLMFindDataRef("x737/systems/athr/N1_mode");
    hsairxpl_mcp_datarefs.x737_ap_n1mode=XPLMFindDataRef("x737/systems/athr/N1_mode");
    hsairxpl_mcp_datarefs.x737_ap_lcstatus=XPLMFindDataRef("x737/systems/afds/LVLCHG");
    hsairxpl_mcp_datarefs.x737_ap_fd_on=XPLMFindDataRef("x737/systems/afds/fdA_status");
    hsairxpl_mcp_datarefs.x737_ap_fd2_on=XPLMFindDataRef("x737/systems/afds/fdB_status");
    hsairxpl_mcp_datarefs.x737_ap_alt_dial=XPLMFindDataRef("x737/systems/afds/ALTHLD_baroalt");
    hsairxpl_mcp_datarefs.x737_ap_vs_dial=XPLMFindDataRef("x737/systems/afds/VS_vvi");
    
    /* === Documented and present but not yet implemented ===
     hsairxpl_mcp_datarefs.x737_ap_spd_mach_dial=XPLMFindDataRef("x737/systems/athr/FMCSPD_spdmach");
     hsairxpl_mcp_datarefs.x737_ap_spd_ias_dial=XPLMFindDataRef("x737/systems/athr/FMCSPD_spdkias");
     hsairxpl_mcp_datarefs.x737_ap_spd_is_mach=XPLMFindDataRef("x737/systems/athr/FMCSPD_ismach");
     */
    
    hsairxpl_mcp_datarefs.x737_ap_fdroll=XPLMFindDataRef("x737/systems/afds/AP_A_roll");
    /* x7datarefs.fdpitch=XPLMFindDataRef("x737/systems/afds/AP_A_pitch_abs"); */
    hsairxpl_mcp_datarefs.x737_ap_fdpitch=XPLMFindDataRef("x737/systems/afds/AP_A_pitch");
    
    
    hsairxpl_mcp_datarefs.x737_ap_gs_armed=XPLMFindDataRef("x737/systems/PFD/PFD_GSARMED_mode_on");
    hsairxpl_mcp_datarefs.x737_ap_gs_captured=XPLMFindDataRef("x737/systems/PFD/PFD_GS_mode_on");
    
    hsairxpl_mcp_datarefs.x737_efis_barounits=XPLMFindDataRef("x737/systems/units/baroPressUnit");
    hsairxpl_mcp_datarefs.x737_ap_overspeed=XPLMFindDataRef("x737/systems/athr/MCPSPD_overspeed");
    hsairxpl_mcp_datarefs.x737_ap_spd_intv=XPLMFindDataRef("x737/systems/afds/SPDINTV");
    hsairxpl_mcp_datarefs.x737_ap_alt_intv=XPLMFindDataRef("x737/systems/afds/ALTINTV");
    hsairxpl_mcp_datarefs.x737_ap_co=XPLMFindDataRef("x737/systems/afds/CHANGEOVER_toggle");
    
    hsairxpl_mcp_datarefs.x737_ap_lnavled=XPLMFindDataRef("x737/systems/MCP/LED_LNAV_on");
    hsairxpl_mcp_datarefs.x737_ap_vnavled=XPLMFindDataRef("x737/systems/MCP/LED_VNAV_on");
    hsairxpl_mcp_datarefs.x737_ap_vorlocled=XPLMFindDataRef("x737/systems/MCP/LED_VORLOC_on");
    hsairxpl_mcp_datarefs.x737_ap_appled=XPLMFindDataRef("x737/systems/MCP/LED_APP_on");
    hsairxpl_mcp_datarefs.x737_ap_ma1led=XPLMFindDataRef("x737/systems/MCP/LED_FDA_MA_on");
    hsairxpl_mcp_datarefs.x737_ap_ma2led=XPLMFindDataRef("x737/systems/MCP/LED_FDB_MA_on");
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    hsairxpl_mcp_datarefs.ap_hdgdial=XPLMFindDataRef("laminar/B738/autopilot/mcp_hdg_dial");
    hsairxpl_mcp_datarefs.ap_obs2p=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_copilot");
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairxpl_mcp_datarefs.ap_obs2p=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_obs_deg_mag_copilot");
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairxpl_mcp_datarefs.ap_speed_status=XPLMFindDataRef("laminar/B738/autopilot/speed_status1");
  }

  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    
    /*
     anim/21/button	int	y	bool	HDG/TRK changeover button
     anim/23/button	int	y	bool	VS/FPA changeover button
     anim/69/switch	int	y	bool	ALT AUTO/1000 selector
     
     anim/68/switch	int	y	0-6	MCP bank limit selector
     screen/choise int y enum -1=left MFD, 0=center lower MFD, 1=right MFD (inop)
     screen/MFD int y enum 4..15 MFD screens
     T7Avionics/ap/at_mode	int	n	enum	A/T mode anunciator -1=NONE 0=THR, 1=THR_REF, 2=HOLD, 3=IDLE, 4=SPD, 5=MACH
     T7Avionics/ap/ap_mode	int	n	enum	A/P mode anunciator -1=NONE 0=FLT_DIR, 1=A_P, 2=LAND_2, 3=LAND_3
     T7Avionics/ap/roll_mode_armed	int	n	enum	roll mode arm anunicator
     
     
     anim/61/button int y bool captain's radio OFF button
     radio/left/mode int y enum captain's radio VHF mode button 1 - VHF L, 2 - VHF C, 3 - VHF R
     anim/57/button int y bool captain's radio swap button
     */
    
    hsairxpl_mcp_datarefs.t7_efis_vas1=XPLMFindDataRef("anim/31/switch");
    hsairxpl_mcp_datarefs.t7_efis_vas2=XPLMFindDataRef("anim/32/switch");
    
    hsairxpl_mcp_datarefs.t7_efis_mins_setting=XPLMFindDataRef("anim/24/rotery");
    hsairxpl_mcp_datarefs.t7_efis_mins_rst=XPLMFindDataRef("anim/174/button");
    hsairxpl_mcp_datarefs.t7_efis_mins_type=XPLMFindDataRef("anim/66/switch");
    
    
    hsairxpl_mcp_datarefs.t7_efis_baro_setting=XPLMFindDataRef("anim/25/rotery");
    hsairxpl_mcp_datarefs.t7_efis_baro_std=XPLMFindDataRef("anim/175/button");
    hsairxpl_mcp_datarefs.t7_efis_baro_type=XPLMFindDataRef("anim/67/switch");
    
    hsairxpl_mcp_datarefs.t7_efis_fpv=XPLMFindDataRef("anim/11/button");
    hsairxpl_mcp_datarefs.t7_efis_mtrs=XPLMFindDataRef("anim/12/button");
    
    hsairxpl_mcp_datarefs.t7_efis_wxr=XPLMFindDataRef("anim/5/button");
    hsairxpl_mcp_datarefs.t7_efis_sta=XPLMFindDataRef("anim/6/button");
    hsairxpl_mcp_datarefs.t7_efis_wpt=XPLMFindDataRef("anim/7/button");
    hsairxpl_mcp_datarefs.t7_efis_arpt=XPLMFindDataRef("anim/8/button");
    hsairxpl_mcp_datarefs.t7_efis_data=XPLMFindDataRef("anim/9/button");
    hsairxpl_mcp_datarefs.t7_efis_pos=XPLMFindDataRef("anim/10/button");
    
    hsairxpl_mcp_datarefs.t7_ap_spd_text=XPLMFindDataRef("T7Avionics/mcp/speed/text");
    hsairxpl_mcp_datarefs.t7_ap_spd_dial=XPLMFindDataRef("T7Avionics/ap/spd_act");
    hsairxpl_mcp_datarefs.t7_ap_hdg_text=XPLMFindDataRef("T7Avionics/mcp/hdg/text");
    hsairxpl_mcp_datarefs.t7_ap_hdg_dial=XPLMFindDataRef("T7Avionics/ap/hdg_act");
    hsairxpl_mcp_datarefs.t7_ap_alt_text=XPLMFindDataRef("T7Avionics/mcp/alt/text");
    hsairxpl_mcp_datarefs.t7_ap_alt_dial=XPLMFindDataRef("T7Avionics/ap/alt_act");
    hsairxpl_mcp_datarefs.t7_ap_vs_text=XPLMFindDataRef("T7Avionics/mcp/vs/text");
    hsairxpl_mcp_datarefs.t7_ap_vs_dial=XPLMFindDataRef("T7Avionics/ap/vs_act");
    
    /* PFD pitch mode annunciator */
    /* -1=NONE 0=PTO_GA, 1=ALT, 2=VS, 3=VNAV_PTH, 4=VNAV_SPD, 5=VNAV_ALT, 6=G_S, 7=FLARE, 8=FLCH, 9=FPA */
    hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm=XPLMFindDataRef("T7Avionics/ap/pitch_mode_armed");
    hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on=XPLMFindDataRef("T7Avionics/ap/pitch_mode_engaged");
    
    hsairxpl_mcp_datarefs.t7_ap_co=XPLMFindDataRef("anim/19/button");
    
    hsairxpl_mcp_datarefs.t7_ap_cmda_led=XPLMFindDataRef("lamps/251");
    hsairxpl_mcp_datarefs.t7_ap_cmdb_led=XPLMFindDataRef("lamps/262");
    hsairxpl_mcp_datarefs.t7_ap_cmda=XPLMFindDataRef("anim/13/button");
    hsairxpl_mcp_datarefs.t7_ap_cmdb=XPLMFindDataRef("anim/28/button");
    
    hsairxpl_mcp_datarefs.t7_ap_hdg_led=XPLMFindDataRef("lamps/257");
    hsairxpl_mcp_datarefs.t7_ap_vs_led=XPLMFindDataRef("lamps/258");
    hsairxpl_mcp_datarefs.t7_ap_alt_led=XPLMFindDataRef("lamps/259");
    hsairxpl_mcp_datarefs.t7_ap_fchg_led=XPLMFindDataRef("lamps/255");
    hsairxpl_mcp_datarefs.t7_ap_lnav_led=XPLMFindDataRef("lamps/253");
    hsairxpl_mcp_datarefs.t7_ap_vnav_led=XPLMFindDataRef("lamps/254");
    hsairxpl_mcp_datarefs.t7_ap_app_led=XPLMFindDataRef("lamps/261");
    hsairxpl_mcp_datarefs.t7_ap_loc_led=XPLMFindDataRef("lamps/260");
    hsairxpl_mcp_datarefs.t7_ap_spd_led=XPLMFindDataRef("lamps/252");
    
    hsairxpl_mcp_datarefs.t7_ap_at_but=XPLMFindDataRef("anim/15/button");
    hsairxpl_mcp_datarefs.t7_ap_disc_but=XPLMFindDataRef("anim/176/button");
    hsairxpl_mcp_datarefs.t7_ap_hdg_sel_but=XPLMFindDataRef("anim/173/button");
    hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but=XPLMFindDataRef("anim/22/button");
    hsairxpl_mcp_datarefs.t7_ap_vs_hld_but=XPLMFindDataRef("anim/24/button");
    hsairxpl_mcp_datarefs.t7_ap_alt_hld_but=XPLMFindDataRef("anim/25/button");
    hsairxpl_mcp_datarefs.t7_ap_fchg_but=XPLMFindDataRef("anim/18/button");
    hsairxpl_mcp_datarefs.t7_ap_clbcon_but=XPLMFindDataRef("anim/14/button");
    hsairxpl_mcp_datarefs.t7_ap_lnav_but=XPLMFindDataRef("anim/16/button");
    hsairxpl_mcp_datarefs.t7_ap_vnav_but=XPLMFindDataRef("anim/17/button");
    hsairxpl_mcp_datarefs.t7_ap_loc_but=XPLMFindDataRef("anim/26/button");
    hsairxpl_mcp_datarefs.t7_ap_app_but=XPLMFindDataRef("anim/27/button");
    hsairxpl_mcp_datarefs.t7_ap_toga_but=XPLMFindDataRef("anim/149/button");
    
    hsairxpl_mcp_datarefs.t7_ap_altintv=XPLMFindDataRef("anim/184/button");
    hsairxpl_mcp_datarefs.t7_ap_spdintv=XPLMFindDataRef("anim/186/button");
    
    hsairxpl_mcp_datarefs.t7_ap_fd1=XPLMFindDataRef("anim/34/switch");
    hsairxpl_mcp_datarefs.t7_ap_fd2=XPLMFindDataRef("anim/36/switch");
    hsairxpl_mcp_datarefs.t7_ap_atarm=XPLMFindDataRef("anim/33/switch");
    
    hsairxpl_mcp_datarefs.t7_ap_iasmach=XPLMFindDataRef("T7Avionics/mcp/speed/label");
    
    
    /* -1=NONE 0=HDG_HOLD, 1=HDG_SEL, 2=LNAV, 3=LOC, 4=ROLLOUT, 5=RTO_GA, 6=TRK_SEL, 7=TRK_HOLD, 8=ATT */
    hsairxpl_mcp_datarefs.t7_ap_roll_mode_on=XPLMFindDataRef("T7Avionics/ap/roll_mode_engaged");
    hsairxpl_mcp_datarefs.t7_ap_roll_mode_arm=XPLMFindDataRef("T7Avionics/ap/roll_mode_armed");
    
    hsairxpl_mcp_datarefs.t7_efis_nd_mode=XPLMFindDataRef("anim/64/switch");
    hsairxpl_mcp_datarefs.t7_efis_nd_range=XPLMFindDataRef("anim/65/switch");
    
    hsairxpl_mcp_datarefs.t7_efis_nd_ctr=XPLMFindDataRef("anim/171/button");
    hsairxpl_mcp_datarefs.t7_efis_nd_tfc=XPLMFindDataRef("anim/172/button");
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    hsairxpl_mcp_datarefs.b757_ap_spd_setting=XPLMFindDataRef("1-sim/AP/spdSetting");
    hsairxpl_mcp_datarefs.b757_ap_spd_text=XPLMFindDataRef("757Avionics/mcp/speed/text");
    hsairxpl_mcp_datarefs.b757_ap_spd_dial=XPLMFindDataRef("757Avionics/ap/spd_act");
    hsairxpl_mcp_datarefs.b757_ap_hdg_text=XPLMFindDataRef("757Avionics/mcp/hdg/text");
    hsairxpl_mcp_datarefs.b757_ap_hdg_dial=XPLMFindDataRef("757Avionics/ap/hdg_act");
    hsairxpl_mcp_datarefs.b757_ap_alt_text=XPLMFindDataRef("757Avionics/mcp/alt/text");
    hsairxpl_mcp_datarefs.b757_ap_alt_dial=XPLMFindDataRef("757Avionics/ap/alt_act");
    hsairxpl_mcp_datarefs.b757_ap_vs_text=XPLMFindDataRef("757Avionics/mcp/vs/text");
    hsairxpl_mcp_datarefs.b757_ap_vs_dial=XPLMFindDataRef("757Avionics/ap/vs_act");
    
    hsairxpl_mcp_datarefs.b757_ap_fd1=XPLMFindDataRef("1-sim/AP/fd1Switcher");
    hsairxpl_mcp_datarefs.b757_ap_fd2=XPLMFindDataRef("1-sim/AP/fd2Switcher");
    hsairxpl_mcp_datarefs.b757_ap_atarm=XPLMFindDataRef("1-sim/AP/atSwitcher");
    
    hsairxpl_mcp_datarefs.b757_ap_cmda_led=XPLMFindDataRef("1-sim/AP/lamp/12");
    hsairxpl_mcp_datarefs.b757_ap_cmdb_led=XPLMFindDataRef("1-sim/AP/lamp/14");
    hsairxpl_mcp_datarefs.b757_ap_hdg_led=XPLMFindDataRef("1-sim/AP/lamp/6");
    hsairxpl_mcp_datarefs.b757_ap_vs_led=XPLMFindDataRef("1-sim/AP/lamp/7");
    hsairxpl_mcp_datarefs.b757_ap_alt_led=XPLMFindDataRef("1-sim/AP/lamp/8");
    hsairxpl_mcp_datarefs.b757_ap_fchg_led=XPLMFindDataRef("1-sim/AP/lamp/5");
    hsairxpl_mcp_datarefs.b757_ap_lnav_led=XPLMFindDataRef("1-sim/AP/lamp/3");
    hsairxpl_mcp_datarefs.b757_ap_vnav_led=XPLMFindDataRef("1-sim/AP/lamp/4");
    hsairxpl_mcp_datarefs.b757_ap_app_led=XPLMFindDataRef("1-sim/AP/lamp/11");
    hsairxpl_mcp_datarefs.b757_ap_loc_led=XPLMFindDataRef("1-sim/AP/lamp/10");
    hsairxpl_mcp_datarefs.b757_ap_spd_led=XPLMFindDataRef("1-sim/AP/lamp/2");
    hsairxpl_mcp_datarefs.b757_ap_epr_led=XPLMFindDataRef("1-sim/AP/lamp/1");
    
    hsairxpl_mcp_datarefs.b757_ap_spd_but=XPLMFindDataRef("1-sim/AP/spdButton");
    hsairxpl_mcp_datarefs.b757_ap_hdg_sel_but=XPLMFindDataRef("1-sim/AP/hdgConfButton");
    hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but=XPLMFindDataRef("1-sim/AP/hdgHoldButton");
    hsairxpl_mcp_datarefs.b757_ap_vs_hld_but=XPLMFindDataRef("1-sim/AP/vviButton");
    hsairxpl_mcp_datarefs.b757_ap_alt_hld_but=XPLMFindDataRef("1-sim/AP/altHoldButton");
    hsairxpl_mcp_datarefs.b757_ap_fchg_but=XPLMFindDataRef("1-sim/AP/flchButton");
    hsairxpl_mcp_datarefs.b757_ap_epr_but=XPLMFindDataRef("1-sim/AP/eprButton");
    hsairxpl_mcp_datarefs.b757_ap_lnav_but=XPLMFindDataRef("1-sim/AP/lnavButton");
    hsairxpl_mcp_datarefs.b757_ap_vnav_but=XPLMFindDataRef("1-sim/AP/vnavButton");
    hsairxpl_mcp_datarefs.b757_ap_loc_but=XPLMFindDataRef("1-sim/AP/locButton");
    hsairxpl_mcp_datarefs.b757_ap_app_but=XPLMFindDataRef("1-sim/AP/appButton");
    
    hsairxpl_mcp_datarefs.b757_ap_cmda=XPLMFindDataRef("1-sim/AP/cmd_L_Button");
    hsairxpl_mcp_datarefs.b757_ap_cmdb=XPLMFindDataRef("1-sim/AP/cmd_R_Button");
    hsairxpl_mcp_datarefs.b757_ap_disc_but=XPLMFindDataRef("1-sim/AP/desengageLever");
    
    hsairxpl_mcp_datarefs.b757_ap_iasmach=XPLMFindDataRef("1-sim/AP/iasmach");
    
    hsairxpl_mcp_datarefs.b757_ap_roll_mode_on=XPLMFindDataRef("757Avionics/ap/roll_mode_engaged");
    hsairxpl_mcp_datarefs.b757_ap_roll_mode_arm=XPLMFindDataRef("757Avionics/ap/roll_mode_armed");
    
    /* -1=NONE 0=PTO_GA, 1=ALT, 2=VS, 3=VNAV_PTH, 4=VNAV_SPD, 5=VNAV_ALT, 6=G_S, 7=FLARE, 8=FLCH, 9=FPA */
    hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on=XPLMFindDataRef("757Avionics/ap/pitch_mode_engaged");
    hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm=XPLMFindDataRef("757Avionics/ap/pitch_mode_armed");
    
    hsairxpl_mcp_datarefs.b757_efis_terr=XPLMFindDataRef("1-sim/ndpanel/1/hsiTerr");
    hsairxpl_mcp_datarefs.b757_efis_wxr=XPLMFindDataRef("1-sim/ndpanel/1/hsiWxr");
    hsairxpl_mcp_datarefs.b757_efis_sta=XPLMFindDataRef("1-sim/ndpanel/1/map2");
    hsairxpl_mcp_datarefs.b757_efis_arpt=XPLMFindDataRef("1-sim/ndpanel/1/map3");
    hsairxpl_mcp_datarefs.b757_efis_data=XPLMFindDataRef("1-sim/ndpanel/1/map4");
    hsairxpl_mcp_datarefs.b757_efis_wpt=XPLMFindDataRef("1-sim/ndpanel/1/map5");
    hsairxpl_mcp_datarefs.b757_efis_pos=XPLMFindDataRef("1-sim/ndpanel/1/map1");
    
    hsairxpl_mcp_datarefs.b757_efis_nd_mode=XPLMFindDataRef("1-sim/ndpanel/1/hsiModeRotary");
    hsairxpl_mcp_datarefs.b757_efis_nd_range=XPLMFindDataRef("1-sim/ndpanel/1/hsiRangeRotary");
    
    hsairxpl_mcp_datarefs.b757_efis_mins_setting=XPLMFindDataRef("1-sim/ndpanel/1/dhRotary");
    hsairxpl_mcp_datarefs.b757_efis_mins_rst=XPLMFindDataRef("1-sim/ndpanel/1/dhResetButton");
    
    hsairxpl_mcp_datarefs.b757_efis_baro_setting1=XPLMFindDataRef("1-sim/gauges/baroRotary_left");
    hsairxpl_mcp_datarefs.b757_efis_baro_setting2=XPLMFindDataRef("1-sim/gauges/baroRotary_stby");
    hsairxpl_mcp_datarefs.b757_efis_baro_setting3=XPLMFindDataRef("1-sim/gauges/baroRotary_right");
    
    hsairxpl_mcp_datarefs.b757_efis_vas1_sel=XPLMFindDataRef("1-sim/gauges/adf1sourceSwitcher_left");
    hsairxpl_mcp_datarefs.b757_efis_vas2_sel=XPLMFindDataRef("1-sim/gauges/adf2sourceSwitcher_left");
    
    
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_mode)<2)
      hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode=1;
    else
      hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode=0;
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    hsairxpl_mcp_datarefs.b767_ap_spd_text=XPLMFindDataRef("757Avionics/mcp/speed/text");
    hsairxpl_mcp_datarefs.b767_ap_spd_dial=XPLMFindDataRef("757Avionics/ap/spd_act");
    hsairxpl_mcp_datarefs.b767_ap_hdg_text=XPLMFindDataRef("757Avionics/mcp/hdg/text");
    hsairxpl_mcp_datarefs.b767_ap_hdg_dial=XPLMFindDataRef("757Avionics/ap/hdg_act");
    hsairxpl_mcp_datarefs.b767_ap_alt_text=XPLMFindDataRef("757Avionics/mcp/alt/text");
    hsairxpl_mcp_datarefs.b767_ap_alt_dial=XPLMFindDataRef("757Avionics/ap/alt_act");
    hsairxpl_mcp_datarefs.b767_ap_vs_text=XPLMFindDataRef("757Avionics/mcp/vs/text");
    hsairxpl_mcp_datarefs.b767_ap_vs_dial=XPLMFindDataRef("757Avionics/ap/vs_act");
    
    hsairxpl_mcp_datarefs.b767_ap_fd1=XPLMFindDataRef("1-sim/AP/fd1Switcher");
    hsairxpl_mcp_datarefs.b767_ap_fd2=XPLMFindDataRef("1-sim/AP/fd2Switcher");
    hsairxpl_mcp_datarefs.b767_ap_atarm=XPLMFindDataRef("1-sim/AP/atSwitcher");
    
    hsairxpl_mcp_datarefs.b767_ap_cmda_led=XPLMFindDataRef("1-sim/AP/lamp/12");
    hsairxpl_mcp_datarefs.b767_ap_cmdb_led=XPLMFindDataRef("1-sim/AP/lamp/14");
    hsairxpl_mcp_datarefs.b767_ap_hdg_led=XPLMFindDataRef("1-sim/AP/lamp/6");
    hsairxpl_mcp_datarefs.b767_ap_vs_led=XPLMFindDataRef("1-sim/AP/lamp/7");
    hsairxpl_mcp_datarefs.b767_ap_alt_led=XPLMFindDataRef("1-sim/AP/lamp/8");
    hsairxpl_mcp_datarefs.b767_ap_fchg_led=XPLMFindDataRef("1-sim/AP/lamp/5");
    hsairxpl_mcp_datarefs.b767_ap_lnav_led=XPLMFindDataRef("1-sim/AP/lamp/3");
    hsairxpl_mcp_datarefs.b767_ap_vnav_led=XPLMFindDataRef("1-sim/AP/lamp/4");
    hsairxpl_mcp_datarefs.b767_ap_app_led=XPLMFindDataRef("1-sim/AP/lamp/11");
    hsairxpl_mcp_datarefs.b767_ap_loc_led=XPLMFindDataRef("1-sim/AP/lamp/10");
    hsairxpl_mcp_datarefs.b767_ap_spd_led=XPLMFindDataRef("1-sim/AP/lamp/2");
    hsairxpl_mcp_datarefs.b767_ap_epr_led=XPLMFindDataRef("1-sim/AP/lamp/1");
    
    hsairxpl_mcp_datarefs.b767_ap_spd_but=XPLMFindDataRef("1-sim/AP/spdButton");
    hsairxpl_mcp_datarefs.b767_ap_hdg_sel_but=XPLMFindDataRef("1-sim/AP/hdgConfButton");
    hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but=XPLMFindDataRef("1-sim/AP/hdgHoldButton");
    hsairxpl_mcp_datarefs.b767_ap_vs_hld_but=XPLMFindDataRef("1-sim/AP/vviButton");
    hsairxpl_mcp_datarefs.b767_ap_alt_hld_but=XPLMFindDataRef("1-sim/AP/altHoldButton");
    hsairxpl_mcp_datarefs.b767_ap_fchg_but=XPLMFindDataRef("1-sim/AP/flchButton");
    hsairxpl_mcp_datarefs.b767_ap_epr_but=XPLMFindDataRef("1-sim/AP/eprButton");
    hsairxpl_mcp_datarefs.b767_ap_lnav_but=XPLMFindDataRef("1-sim/AP/lnavButton");
    hsairxpl_mcp_datarefs.b767_ap_vnav_but=XPLMFindDataRef("1-sim/AP/vnavButton");
    hsairxpl_mcp_datarefs.b767_ap_loc_but=XPLMFindDataRef("1-sim/AP/locButton");
    hsairxpl_mcp_datarefs.b767_ap_app_but=XPLMFindDataRef("1-sim/AP/appButton");
    
    hsairxpl_mcp_datarefs.b767_ap_cmda=XPLMFindDataRef("1-sim/AP/cmd_L_Button");
    hsairxpl_mcp_datarefs.b767_ap_cmdb=XPLMFindDataRef("1-sim/AP/cmd_R_Button");
    hsairxpl_mcp_datarefs.b767_ap_disc_but=XPLMFindDataRef("1-sim/AP/desengageLever");
    
    hsairxpl_mcp_datarefs.b767_ap_iasmach=XPLMFindDataRef("1-sim/AP/iasmach");
    
    hsairxpl_mcp_datarefs.b767_ap_roll_mode_on=XPLMFindDataRef("757Avionics/ap/roll_mode_engaged");
    hsairxpl_mcp_datarefs.b767_ap_roll_mode_arm=XPLMFindDataRef("757Avionics/ap/roll_mode_armed");
    
    /* -1=NONE 0=PTO_GA, 1=ALT, 2=VS, 3=VNAV_PTH, 4=VNAV_SPD, 5=VNAV_ALT, 6=G_S, 7=FLARE, 8=FLCH, 9=FPA */
    hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on=XPLMFindDataRef("757Avionics/ap/pitch_mode_engaged");
    hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm=XPLMFindDataRef("757Avionics/ap/pitch_mode_armed");
    
    hsairxpl_mcp_datarefs.b767_efis_terr=XPLMFindDataRef("1-sim/ndpanel/1/hsiTerr");
    hsairxpl_mcp_datarefs.b767_efis_wxr=XPLMFindDataRef("1-sim/ndpanel/1/hsiWxr");
    hsairxpl_mcp_datarefs.b767_efis_sta=XPLMFindDataRef("1-sim/ndpanel/1/map2");
    hsairxpl_mcp_datarefs.b767_efis_arpt=XPLMFindDataRef("1-sim/ndpanel/1/map3");
    hsairxpl_mcp_datarefs.b767_efis_data=XPLMFindDataRef("1-sim/ndpanel/1/map4");
    hsairxpl_mcp_datarefs.b767_efis_wpt=XPLMFindDataRef("1-sim/ndpanel/1/map5");
    hsairxpl_mcp_datarefs.b767_efis_pos=XPLMFindDataRef("1-sim/ndpanel/1/map1");
    
    hsairxpl_mcp_datarefs.b767_efis_nd_mode=XPLMFindDataRef("1-sim/ndpanel/1/hsiModeRotary");
    hsairxpl_mcp_datarefs.b767_efis_nd_range=XPLMFindDataRef("1-sim/ndpanel/1/hsiRangeRotary");
    
    hsairxpl_mcp_datarefs.b767_efis_mins_setting=XPLMFindDataRef("1-sim/ndpanel/1/dhRotary");
    hsairxpl_mcp_datarefs.b767_efis_mins_rst=XPLMFindDataRef("1-sim/ndpanel/1/dhResetButton");
    
    hsairxpl_mcp_datarefs.b767_efis_baro_setting1=XPLMFindDataRef("1-sim/gauges/baroRotary_left");
    hsairxpl_mcp_datarefs.b767_efis_baro_setting2=XPLMFindDataRef("1-sim/gauges/baroRotary_stby");
    hsairxpl_mcp_datarefs.b767_efis_baro_setting3=XPLMFindDataRef("1-sim/gauges/baroRotary_right");
    
    hsairxpl_mcp_datarefs.b767_efis_vas1_sel=XPLMFindDataRef("1-sim/gauges/adf1sourceSwitcher_left");
    hsairxpl_mcp_datarefs.b767_efis_vas2_sel=XPLMFindDataRef("1-sim/gauges/adf2sourceSwitcher_left");
    
    
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    hsairxpl_mcp_datarefs.a320n_fcu_fd=XPLMFindDataRef("sim/custom/xap/fcu/fd");
    hsairxpl_mcp_datarefs.a320n_fcu_ap1=XPLMFindDataRef("sim/custom/xap/fcu/ap1");
    hsairxpl_mcp_datarefs.a320n_fcu_ap2=XPLMFindDataRef("sim/custom/xap/fcu/ap2");
    hsairxpl_mcp_datarefs.a320n_fcu_nd_mode=XPLMFindDataRef("sim/custom/xap/fcu/nd_mode");
    hsairxpl_mcp_datarefs.a320n_fcu_alt_dial=XPLMFindDataRef("sim/custom/xap/fcu/alt100x");
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_PCRJ200) {
    
    hsairxpl_mcp_datarefs.crj_ap_fd_btn=XPLMFindDataRef("CRJ/autopilot/fd_button");
    hsairxpl_mcp_datarefs.crj_ap_ap_btn=XPLMFindDataRef("CRJ/autopilot/ap_eng_button");
    hsairxpl_mcp_datarefs.crj_ap_apdisc_btn=XPLMFindDataRef("CRJ/autopilot/ap_disc_button");
    hsairxpl_mcp_datarefs.crj_ap_spd_btn=XPLMFindDataRef("CRJ/autopilot/speed_button");
    hsairxpl_mcp_datarefs.crj_ap_hdg_btn=XPLMFindDataRef("CRJ/autopilot/hdg_button");
    hsairxpl_mcp_datarefs.crj_ap_alt_btn=XPLMFindDataRef("CRJ/autopilot/alt_button");
    hsairxpl_mcp_datarefs.crj_ap_vs_btn=XPLMFindDataRef("CRJ/autopilot/v_button");
    hsairxpl_mcp_datarefs.crj_ap_nav_btn=XPLMFindDataRef("CRJ/autopilot/nav_button");
    hsairxpl_mcp_datarefs.crj_ap_12_btn=XPLMFindDataRef("CRJ/autopilot/12_bank_button");
    hsairxpl_mcp_datarefs.crj_ap_bcrs_btn=XPLMFindDataRef("CRJ/autopilot/backcrs_button");
    hsairxpl_mcp_datarefs.crj_ap_toga_btn=XPLMFindDataRef("CRJ/autopilot/toga_button");
    hsairxpl_mcp_datarefs.crj_ap_xpr_btn=XPLMFindDataRef("CRJ/autopilot/xpr_button");
    hsairxpl_mcp_datarefs.crj_ap_turb_btn=XPLMFindDataRef("CRJ/autopilot/turbo_button");
    hsairxpl_mcp_datarefs.crj_ap_appr_btn=XPLMFindDataRef("CRJ/autopilot/appr_button");
    hsairxpl_mcp_datarefs.crj_ap_alt_sync=XPLMFindDataRef("CRJ/autopilot/alt_sync");
    hsairxpl_mcp_datarefs.crj_ap_hdg_sync=XPLMFindDataRef("CRJ/autopilot/hdg_sync");
    hsairxpl_mcp_datarefs.crj_ap_crs1_sync=XPLMFindDataRef("CRJ/autopilot/crs1_sync");
    hsairxpl_mcp_datarefs.crj_ap_spd_sync=XPLMFindDataRef("CRJ/autopilot/knots_mach");
    hsairxpl_mcp_datarefs.crj_ap_spd_dial=XPLMFindDataRef("CRJ/autopilot/speed");
    hsairxpl_mcp_datarefs.crj_ap_hdg_dial=XPLMFindDataRef("CRJ/autopilot/hdg");
    hsairxpl_mcp_datarefs.crj_ap_alt_dial=XPLMFindDataRef("CRJ/autopilot/alt");
    hsairxpl_mcp_datarefs.crj_ap_vs_dial=XPLMFindDataRef("CRJ/autopilot/verticalspeed");
    hsairxpl_mcp_datarefs.crj_ap_crs_dial=XPLMFindDataRef("CRJ/autopilot/crs1");
    hsairxpl_mcp_datarefs.crj_ap_fd_lit=XPLMFindDataRef("CRJ/autopilot/fd_light");
    hsairxpl_mcp_datarefs.crj_ap_eng_lit=XPLMFindDataRef("CRJ/autopilot/ap_eng_light");
    hsairxpl_mcp_datarefs.crj_ap_hdg_lit=XPLMFindDataRef("CRJ/autopilot/hdg_light");
    hsairxpl_mcp_datarefs.crj_ap_nav_lit=XPLMFindDataRef("CRJ/autopilot/nav_light");
    hsairxpl_mcp_datarefs.crj_ap_alt_lit=XPLMFindDataRef("CRJ/autopilot/alt_light");
    hsairxpl_mcp_datarefs.crj_ap_spd_lit=XPLMFindDataRef("CRJ/autopilot/spd_light");
    hsairxpl_mcp_datarefs.crj_ap_vs_lit=XPLMFindDataRef("CRJ/autopilot/vs_light");
    hsairxpl_mcp_datarefs.crj_ap_xpr_lit=XPLMFindDataRef("CRJ/autopilot/xpr_light");
    hsairxpl_mcp_datarefs.crj_ap_turb_lit=XPLMFindDataRef("CRJ/autopilot/turb_light");
    hsairxpl_mcp_datarefs.crj_ap_bank_lit=XPLMFindDataRef("CRJ/autopilot/bank_light");
    hsairxpl_mcp_datarefs.crj_ap_bcrs_lit=XPLMFindDataRef("CRJ/autopilot/bcrs_light");
    hsairxpl_mcp_datarefs.crj_ap_appr_lit=XPLMFindDataRef("CRJ/autopilot/appr_light");
    
    hsairxpl_mcp_datarefs.crj_efis_baro_dial=XPLMFindDataRef("CRJ/baro/pressure");
    hsairxpl_mcp_datarefs.crj_efis_baro_std_but=XPLMFindDataRef("CRJ/baro/pressure_std");
    hsairxpl_mcp_datarefs.crj_efis_baro_hpa_but=XPLMFindDataRef("CRJ/baro/in_hpa");
    
    hsairxpl_mcp_datarefs.crj_efis_brg1_but=XPLMFindDataRef("CRJ/mfd/brg1_button");
    hsairxpl_mcp_datarefs.crj_efis_brg2_but=XPLMFindDataRef("CRJ/mfd/brg2_button");
    
    /* 0 =  FMS1 | 1 = VOR/LOC1 | 2 = VOR/LOC2 | 3 = OFF */
    hsairxpl_mcp_datarefs.crj_efis_nav_source=XPLMFindDataRef("CRJ/mfd/nav_source");
    
    /* 0 = HSI Compass | 1 = Map Sector(default) | 3 = plan map */
    hsairxpl_mcp_datarefs.crj_efis_mfd_format=XPLMFindDataRef("CRJ/mfd/mode");
    
    /*  0 = 5nm | 1 = 10nm | 2 = 20nm | 3 = 40nm | 4 = 80nm | 5 = 160nm | 6 = 320nm */
    hsairxpl_mcp_datarefs.crj_efis_mfd_range=XPLMFindDataRef("CRJ/mfd/range");
    
  }
  
  
  
}
/* ######## PRESS BUTTONS ######## */

void hsairpl_mcp_press_cmda(void) {
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cmda_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_cmda_act,1.0);
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_cmda!=NULL) {
      
      if(hsairpl_mcp_get_cmda_led()) {
        
        hsairpl_mcp_set_apdisc(0);
        
      } else {
        
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_cmda))
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_cmda,0);
        else
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_cmda,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_cmda!=NULL) {
      
      if(hsairpl_mcp_get_cmda_led()) {
        hsairpl_mcp_set_apdisc(0);
      } else {
        hsairpl_mcp_set_apdisc(1);
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_cmda))
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_cmda,0);
        else
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_cmda,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_cmda!=NULL) {
      
      if(hsairpl_mcp_get_cmda_led()) {
        hsairpl_mcp_set_apdisc(0);
      } else {
        hsairpl_mcp_set_apdisc(1);
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_cmda))
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_cmda,0);
        else
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_cmda,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_cmda!=NULL) {
      if(hsairpl_mcp_get_cmda_led())
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_cmda,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_cmda,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_ap1!=NULL) {
      if(hsairpl_mcp_get_cmda_led())
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_ap1,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_ap1,1);
      return;
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_cmd_a_press();
    return;
  }
  
  if(hsairxpl_mcp_datarefs.ap_fdmode!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_fdmode)==2) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_fdmode,1);
    } else {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_fdmode,2);
    }
  }
  
}
void hsairpl_mcp_press_cmdb(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cmdb_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_cmdb_act,1.0);
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_cmdb!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_cmdb,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_cmdb!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_cmdb))
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_cmdb,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_cmdb,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_cmdb!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_cmdb))
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_cmdb,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_cmdb,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_cmdb!=NULL) {
      if(hsairpl_mcp_get_cmdb_led())
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_cmdb,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_cmdb,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_ap2!=NULL) {
      if(hsairpl_mcp_get_cmdb_led())
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_ap2,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_ap2,1);
      return;
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_cmd_b_press();
    return;
  }
}

void hsairpl_mcp_press_cwsa(void) {
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cwsa_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_cwsa_act,1.0);
    }
    return;
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_cws_a_press();
    return;
  }
}

void hsairpl_mcp_press_cwsb(void) {
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cwsb_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_cwsb_act,1.0);
    }
    return;
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_cws_b_press();
    return;
  }
}

void hsairpl_mcp_press_co(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_co!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_co))
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_co,0);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_co,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_iasmach!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_iasmach)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_iasmach,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_iasmach,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_iasmach!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_iasmach)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_iasmach,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_iasmach,1);
      }
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_co!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_co,1);
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_co_press();
    return;
  }

  /* Default */
  if(hsairpl_mcp_get_speed_is_mach()) {
    hsairpl_mcp_set_speed_is_mach(0);
  } else{
    hsairpl_mcp_set_speed_is_mach(1);
  }
}

void hsairpl_mcp_press_spdintv(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_spdintv!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_spdintv)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_spdintv,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_spdintv,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_spd_intv!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_spd_intv)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_spd_intv,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_spd_intv,1);
      }
      return;
      
    }
  }
}

void hsairpl_mcp_press_altintv(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_altintv!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_altintv)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_altintv,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_altintv,1);
      }
    }
  }
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_alt_intv!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_intv)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_intv,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_intv,1);
      }
      return;
      
    }
  }
}


void hsairpl_mcp_press_n1(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_n1_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_n1_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_n1_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_toga_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_toga_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_toga_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_toga_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_epr_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_epr_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_epr_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_epr_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_epr_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_epr_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_epr_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_epr_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_n1mode!=NULL) {
      if(hsairpl_mcp_get_n1_led()) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_n1mode,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_n1mode,1);
      }
      return;
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_n1_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    /* if(hsairpl_mcp_get_lchg_led()) */
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,64);
  }
  
}


void hsairpl_mcp_press_spd(void) {

  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_spd_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_spd_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_spd_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_at_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_at_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_at_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_at_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_spd_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_spd_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_spd_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_spd_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_spd_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_spd_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_spd_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_spd_but,1);
      }
    }
  }
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_speed_status!=NULL) {
      if(hsairpl_mcp_get_spd_led()) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_speed_status,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_speed_status,2);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_speed_press();
    return;
  }

  /*
   if(hsairxpl_mcp_datarefs.ap_at!=NULL){
   if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_at)==0){
   XPLMSetDatai(hsairxpl_mcp_datarefs.ap_at, 1);
   } else {
   XPLMSetDatai(hsairxpl_mcp_datarefs.ap_at, 0);
   }
   }
   */
  
  
  if(hsairxpl_mcp_datarefs.ap_speed_status!=NULL && hsairxpl_mcp_datarefs.ap_at!=NULL) {
    if(hsairpl_mcp_get_spd_led()) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_at,0);
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_speed_status,1);
    } else {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_at,1);
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_speed_status,2);
    }
  }
  
  
}


void hsairpl_mcp_press_lchg(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_lc_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_lc_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_lc_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_fchg_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_fchg_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fchg_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fchg_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_fchg_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_fchg_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_fchg_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_fchg_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_fchg_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_fchg_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_fchg_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_fchg_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_lcstatus!=NULL) {
      if(hsairpl_mcp_get_lchg_led()) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_lcstatus,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_lcstatus,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_lvlchg_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,64);
  }
}

void hsairpl_mcp_press_vnav(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vnav_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vnav_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vnav_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_vnav_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_vnav_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_vnav_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_vnav_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_vnav_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_vnav_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_vnav_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_vnav_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_vnav_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_vnav_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_vnav_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_vnav_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vnav_status!=NULL) {
      if(hsairpl_mcp_get_vnav_led()) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_vnav_status,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_vnav_status,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_vnav_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,4096);
  }
  
  
}

void hsairpl_mcp_press_hdg(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hdg_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hdg_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hdg_act,0.0);
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    /* Our implementation of this button serves two purposes; if we are in
     * HDG HOLD mode it activates HDG SEL mode, if we are in HDG SEL mode
     * it activates HDG HOLD mode */
    if(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but!=NULL &&
       hsairxpl_mcp_datarefs.t7_ap_hdg_sel_but!=NULL&&
       hsairxpl_mcp_datarefs.t7_ap_roll_mode_on!=NULL) {
      
      int roll_mode=XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on);
      if(roll_mode!=1 && roll_mode!=6) {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_sel_but)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_sel_but,0);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_sel_but,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but,0);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but,1);
        }
      }
    }
    
    /*
     if(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but!=NULL) {
     if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but)) {
     XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but,0);
     } else {
     XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_hdg_hld_but,1);
     }
     }
     */
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    /* Our implementation of this button serves two purposes; if we are in 
     * HDG HOLD mode it activates HDG SEL mode, if we are in HDG SEL mode
     * it activates HDG HOLD mode */
    if(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but!=NULL &&
       hsairxpl_mcp_datarefs.b757_ap_hdg_sel_but!=NULL&&
       hsairxpl_mcp_datarefs.b757_ap_roll_mode_on!=NULL) {
      
      int roll_mode=XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on);
      if(roll_mode==0) {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_sel_but)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_sel_but,0);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_sel_but,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but,0);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but,1);
        }
      }
    }
    
    
    /*
     if(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but!=NULL) {
     if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but)) {
     XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but,0);
     } else {
     XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_hdg_hld_but,1);
     }
     }
     */
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    /* Our implementation of this button serves two purposes; if we are in
     * HDG HOLD mode it activates HDG SEL mode, if we are in HDG SEL mode
     * it activates HDG HOLD mode */
    if(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but!=NULL &&
       hsairxpl_mcp_datarefs.b767_ap_hdg_sel_but!=NULL&&
       hsairxpl_mcp_datarefs.b767_ap_roll_mode_on!=NULL) {
      
      int roll_mode=XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on);
      if(roll_mode==0) {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_sel_but)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_sel_but,0);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_sel_but,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but,0);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but,1);
        }
      }
    }
    
    
    /*
     if(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but!=NULL) {
     if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but)) {
     XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but,0);
     } else {
     XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_hdg_hld_but,1);
     }
     }
     */
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_hdg_status!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_hdg_status)){
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_hdg_status,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_hdg_status,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_hdgsel_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,00002);
  }
  
}


void hsairpl_mcp_press_lnav(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_lnav_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_lnav_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_lnav_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_lnav_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_lnav_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_lnav_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_lnav_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_lnav_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_lnav_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_lnav_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_lnav_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_lnav_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_lnav_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_lnav_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_lnav_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_lnav_arm!=NULL && hsairxpl_mcp_datarefs.x737_ap_lnav_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_lnav_on)){
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_lnav_on,0);
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_lnav_arm,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_lnav_arm,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_lnav_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,256);
  }
}

void hsairpl_mcp_press_loc(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vorloc_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vorloc_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vorloc_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_loc_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_loc_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_loc_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_loc_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_loc_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_loc_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_loc_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_loc_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_loc_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_loc_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_loc_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_loc_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_loc_status!=NULL && hsairxpl_mcp_datarefs.x737_ap_loc_armed!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_status)||XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_armed)){
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_status,0);
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_armed,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_status,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_vorloc_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL && hsairxpl_mcp_datarefs.ap_hsisource!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_hsisource) < 2) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,256);
    } else {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state) & (256|512) && hsairpl_mcp_get_loc_led())
        XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,256);
    }
  }
  
}


void hsairpl_mcp_press_app(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_app_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_app_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_app_act,0.0);
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_app_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_app_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_app_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_app_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_app_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_app_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_app_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_app_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_app_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_app_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_app_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_app_but,1);
      }
    }
  }
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_app_toggle!=NULL){
      XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_app_toggle,1);
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_app_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL && hsairxpl_mcp_datarefs.ap_hsisource!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_hsisource) < 2) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,1024);
    } else {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state) & (1024|2048) && hsairpl_mcp_get_loc_led())
        XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,1024);
    }
  }
}

void hsairpl_mcp_press_alt(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_alt_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_alt_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_alt_act,0.0);
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_alt_hld_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_alt_hld_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_alt_hld_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_alt_hld_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_alt_hld_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_alt_hld_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_alt_hld_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_alt_hld_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_alt_hld_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_alt_hld_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_alt_hld_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_alt_hld_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_alt_status!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_status)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_status,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_status,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_althld_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,16384);
  }
  
}

void hsairpl_mcp_press_vs(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vs_act!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vs_act,1.0);
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vs_act,0.0);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_vs_hld_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_vs_hld_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_vs_hld_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_vs_hld_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_vs_hld_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_vs_hld_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_vs_hld_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_vs_hld_but,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_vs_hld_but!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_vs_hld_but)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_vs_hld_but,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_vs_hld_but,1);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vs_armed!=NULL){
      if(hsairpl_mcp_get_vs_led()) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_vs_armed,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_vs_armed,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_vs_press();
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_state,16);
  }
  
}

/* ######## SWITCHES ######## */

void hsairpl_mcp_set_fd1(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_fd1_act!=NULL) {
      if(v){
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_fd1_act,1.0);
      } else {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_fd1_act,0.0);
        
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_fd1!=NULL) {
      /*
       if(v){
       XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd1,1);
       } else {
       XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd1,0);
       }
       */
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd1,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd1,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_fd1!=NULL) {
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_fd1,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_fd1,1);
        
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_fd1!=NULL) {
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_fd1,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_fd1,1);
        
      }
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_fd_ca_toggle(v);
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_fd_on!=NULL) {
      if(v) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_fd_on, 1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_fd_on, 0);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_fd!=NULL) {
      if(v)
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_fd,1);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_fd,0);
      return;
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_fdmode!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_fdmode)==0 && v){
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_fdmode, 1);
    } else if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_fdmode) && !v) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_fdmode, 0);
    }
  }
}
uint32_t hsairpl_mcp_get_fd1_mode(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_fd1_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_fd1_ann)>0.2) {
        return 1;
      }
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_fd!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.a320n_fcu_fd))
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    return( hsairpl_mcp_b738_get_fd_ca() );
  }

  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_fd_on!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_fd_on))
        return 1;
      else
        return 0;
    }
  }
  
  
  
  
  if(hsairxpl_mcp_datarefs.ap_fdmode!=NULL) {
    return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.ap_fdmode);
  }
  
  return 0;
  
}

void hsairpl_mcp_set_fd2(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_fd2_act!=NULL) {
      if(v){
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_fd2_act,1.0);
      } else {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_fd2_act,0.0);
        
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_fd2!=NULL) {
      /*
       if(v){
       XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd2,1);
       } else {
       XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd2,0);
       }
       */
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd2,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_fd2,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_fd2!=NULL) {
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_fd2,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_fd2,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_fd2!=NULL) {
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_fd2,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_fd2,1);
      }
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_fd_fo_toggle(v);
    return;
  }

  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_fd2_on!=NULL) {
      if(v) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_fd2_on, 1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_fd2_on, 0);
      }
      return;
    }
  }
  
  
}

float hsairpl_ap_get_fdroll(void){
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_fdroll!=NULL){
      return XPLMGetDataf(hsairxpl_mcp_datarefs.x737_ap_fdroll);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_fdroll!=NULL){
    return XPLMGetDataf(hsairxpl_mcp_datarefs.ap_fdroll);
  }
  
  return 0.0;
  
}
float hsairpl_ap_get_fdpitch(void){
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_fdpitch!=NULL){
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.x737_ap_fdpitch);
      
      if(hsairxpl_mcp_datarefs.pos_theta!=NULL)
        f += XPLMGetDataf(hsairxpl_mcp_datarefs.pos_theta);
      
      return f;
    }
  }
  
  
  if(hsairxpl_mcp_datarefs.ap_fdpitch!=NULL){
    return XPLMGetDataf(hsairxpl_mcp_datarefs.ap_fdpitch);
  }
  return 0.0;
}

void hsairpl_mcp_set_atarm(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_atarm_act!=NULL) {
      if(v)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_atarm_act,1.0);
      else
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_atarm_act,0.0);
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_atarm!=NULL) {
      /*
       if(v){
       XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_atarm,1);
       } else {
       XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_atarm,0);
       }
       */
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_atarm,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_atarm,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_atarm!=NULL) {
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_atarm,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_atarm,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_atarm!=NULL) {
      if(v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_atarm,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_atarm,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_atarm_status!=NULL) {
      if(v) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_atarm_status, 1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_atarm_status, 0);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_mcp_b738_at_arm_toggle(v);
    return;
  }

  if(hsairxpl_mcp_datarefs.ap_at!=NULL && hsairxpl_mcp_datarefs.ap_speed_status!=NULL){
    if(v) {
      if(!XPLMGetDatai(hsairxpl_mcp_datarefs.ap_speed_status)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.ap_speed_status,1);
      }
    } else {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_at,0);
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_speed_status,0);
      
    }
  }
  
  
  
  
}

void hsairpl_mcp_set_apdisc(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_apdisc!=NULL) {
      if(v){
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_apdisc,0.0);
      } else {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_apdisc,1.0);
        
      }
    }
  }
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_disc_but!=NULL) {
      if(!v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_disc_but,1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_disc_but,0);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_disc_but!=NULL) {
      if(!v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_disc_but,1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_disc_but,0);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_disc_but!=NULL) {
      if(!v){
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_disc_but,1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_disc_but,0);
      }
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_disengage!=NULL) {
      if(v) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_disengage,1);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_disengage,0);
      }
      return;
    }
  }
  
  if(!v) {
    if(hsairxpl_mcp_datarefs.ap_fdmode!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_fdmode,0);
    }
  }
  
}

uint32_t hsairpl_mcp_get_overspeed(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_overspeed!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_overspeed);
    }
  }
  
  return 0;
}


void hsairpl_mcp_set_bank_angle(uint32_t v) {
  
}

/* ######## LEDS ######## */


uint32_t hsairpl_mcp_get_cmda_led(void) {
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cmda_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_cmda_ann)>0.2)
        return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_cmda_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_cmda_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_cmda_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_cmda_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_cmda_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_cmda_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_cmda!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_cmda);
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    return( hsairpl_mcp_b738_get_cmda_led() );
  }

  if(hsairxpl_mcp_datarefs.ap_fdmode!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_fdmode)==2){
      return 1;
    }
  }
  return 0;
}

uint32_t hsairpl_mcp_get_cmdb_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cmdb_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_cmdb_ann)>0.2)
        return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_cmdb_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_cmda_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_cmdb_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_cmdb_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_cmdb_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_cmdb_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_cmdb!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_cmdb);
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    return( hsairpl_mcp_b738_get_cmdb_led() );
  }

  return 0;
}
uint32_t hsairpl_mcp_get_cwsa_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cwsa_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_cwsa_ann)>0.2)
        return 1;
    }
    return 0;
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    return( hsairpl_mcp_b738_get_cwsa_led() );
  }

  return 0;
}
uint32_t hsairpl_mcp_get_cwsb_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_cwsb_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_cwsb_ann)>0.2)
        return 1;
    }
    return 0;
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    return( hsairpl_mcp_b738_get_cwsb_led() );
  }

  return 0;
}

uint32_t hsairpl_mcp_get_n1_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_n1_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_n1_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_epr_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_epr_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_epr_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_epr_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_n1_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_n1mode!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_n1mode);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&64) {
      return 1;
    }
  }
  
  return 0;
}


uint32_t hsairpl_mcp_get_lchg_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_lc_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_lc_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_fchg_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_fchg_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_fchg_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_fchg_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_fchg_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_fchg_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_lvlchg_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_lcstatus!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_lcstatus);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&64) {
      return 1;
    }
  }
  
  return 0;
}

uint32_t hsairpl_mcp_get_spd_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_spd_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_spd_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_spd_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_spd_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_spd_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_spd_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_spd_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_spd_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_speed_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_speed_status!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_speed_status);
    }
  }
  if(hsairxpl_mcp_datarefs.ap_at!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_at)) {
      return 1;
    }
  }
  return 0;
}


uint32_t hsairpl_mcp_get_vnav_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vnav_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_vnav_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_vnav_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_vnav_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_vnav_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_vnav_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_vnav_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_vnav_led)>0.2)
        return 1;
      else
        return 0;
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_vnav_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vnavled!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_vnavled);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&4096) { /* 8xxx for arm */
      return 1;
    }
  }
  
  return 0;
}


uint32_t hsairpl_mcp_get_hdg_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hdg_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hdg_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    
    /* Instead of actually showing the HDG HOLD LED here as in the real 777,
     * we show the state of HDG SEL mode because what we want is to know if
     * the plane is following the dialed heading or not */
    if(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on)==1 ||
         XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on)==6) {
        return 1;
      } else {
        return 0;
      }
    }
    
    /*
     if(hsairxpl_mcp_datarefs.t7_ap_hdg_led!=NULL) {
     if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_hdg_led)>0.2)
     return 1;
     else
     return 0;
     }
     */
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    /* Instead of actually showing the HDG HOLD LED here as in the real 757,
     * we show the state of HDG SEL mode because what we want is to know if 
     * the plane is following the dialed heading or not */
    if(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on)==1) {
        return 1;
      } else if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on)==6) {
        return 1;
      } else {
        return 0;
      }
    }
    
    /*
     if(hsairxpl_mcp_datarefs.b757_ap_hdg_led!=NULL) {
     if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_hdg_led)>0.2)
     return 1;
     else
     return 0;
     }
     */
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    /* Instead of actually showing the HDG HOLD LED here as in the real 757,
     * we show the state of HDG SEL mode because what we want is to know if
     * the plane is following the dialed heading or not */
    if(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on)==1) {
        return 1;
      } else if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on)==6) {
        return 1;
      } else {
        return 0;
      }
    }
    
    /*
     if(hsairxpl_mcp_datarefs.b767_ap_hdg_led!=NULL) {
     if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_hdg_led)>0.2)
     return 1;
     else
     return 0;
     }
     */
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_hdgsel_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_hdg_status!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_hdg_status);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&2) {
      return 1;
    }
  }
  
  return 0;
}

uint32_t hsairpl_mcp_get_lnav_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_lnav_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_lnav_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_lnav_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_lnav_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_lnav_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_lnav_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_lnav_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_lnav_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_lnav_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_lnavled!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_lnavled);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&512) { /* 256 for arm */
      return 1;
    }
  }
  return 0;
}

uint32_t hsairpl_mcp_get_loc_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vorloc_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_vorloc_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_loc_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_loc_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_loc_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_loc_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_loc_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_loc_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_vorloc_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_loc_status!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_status);
    }
  }
  if(hsairxpl_mcp_datarefs.ap_state!=NULL && hsairxpl_mcp_datarefs.ap_hsisource!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_hsisource)<2) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&512) { /* 256 for arm */
        return 1;
      }
    }
  }
  return 0;
}


uint32_t hsairpl_mcp_get_app_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_app_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_app_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_app_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_app_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_app_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_app_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_app_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_app_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_app_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_app_status!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_app_status);
    }
  }
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&2048) { /* 1024 for arm */
      return 1;
    }
  }
  
  return 0;
}


uint32_t hsairpl_mcp_get_alt_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_alt_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_alt_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_alt_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_alt_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_alt_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_alt_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_alt_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_alt_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_althld_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_alt_status!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_status);
    }
  }
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&16384) { /* 32 for arm */
      return 1;
    }
  }
  
  return 0;
  
}

uint32_t hsairpl_mcp_get_vs_led(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vs_ann!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_vs_ann)>0.2)
        return 1;
      else
        return 0;
    }
  }
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_vs_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.t7_ap_vs_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_vs_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_vs_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_vs_led!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.b767_ap_vs_led)>0.2)
        return 1;
      else
        return 0;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_vs_led() );
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vs_on!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_vs_on);
    }
  }
  if(hsairxpl_mcp_datarefs.ap_state!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state)&16) { /* 32 for arm */
      return 1;
    }
  }
  
  return 0;
}

uint32_t hsairpl_mcp_get_ma1_led(void) {
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_ma1led!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_ma1led);
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_ma_ca_led() );
  }
  return 0;
}

uint32_t hsairpl_mcp_get_ma2_led(void) {
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_ma2led!=NULL) {
      return XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_ma2led);
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
     return( hsairpl_mcp_b738_get_ma_fo_led() );
  }
  return 0;
}

/* ######## DIALS ######## */

void hsairpl_mcp_set_crs1_dial(uint32_t v) {
  v = v % 360;

  if(hsairxpl_mcp_datarefs.ap_obs1p!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.ap_obs1p,(float)v);
  }
}

uint32_t hsairpl_mcp_get_crs1_dial(void) {
  
  if(hsairxpl_mcp_datarefs.ap_obs1p!=NULL) {
    return (uint32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.ap_obs1p);
  }
  return 0;
}

void hsairpl_mcp_set_crs2_dial(uint32_t v) {
  v = v % 360;

  if(hsairxpl_mcp_datarefs.ap_obs2p!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.ap_obs2p,(float)v);
  }
}

uint32_t hsairpl_mcp_get_crs2_dial(void) {
  
  if(hsairxpl_mcp_datarefs.ap_obs2p!=NULL) {
    return (uint32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.ap_obs2p);
  }
  return 0;
}

void hsairpl_mcp_set_hdg_dial(uint32_t v) {
  
  v = v % 360;
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_hdg_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_ap_hdg_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_hdg_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_ap_hdg_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_hdg_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_ap_hdg_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    hsairpl_mcp_z738_set_hdg_dial(v);
    return;
  }
  
  if(hsairxpl_mcp_datarefs.ap_hdgdial!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.ap_hdgdial,(float)v);
  }
  
}

uint32_t hsairpl_mcp_get_hdg_dial(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_hdg_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.t7_ap_hdg_text,str,0,8); str[7]='\0';
      int i=atoi(str);
      if(i>=0 && i<360) return i;
      return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    if(hsairxpl_mcp_datarefs.b757_ap_hdg_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b757_ap_hdg_text,str,0,8); str[7]='\0';
      int i=atoi(str);
      if(i>=0 && i<360) return i;
      return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    if(hsairxpl_mcp_datarefs.b767_ap_hdg_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b767_ap_hdg_text,str,0,8); str[7]='\0';
      int i=atoi(str);
      if(i>=0 && i<360) return i;
      return 0;
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_hdgdial!=NULL) {
    return (uint32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.ap_hdgdial);
  }
  return 0;
}

void hsairpl_mcp_set_alt_dial(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_alt_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_ap_alt_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_alt_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_ap_alt_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_alt_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_ap_alt_dial,(float)v);
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_alt_dial!=NULL){
      XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_alt_dial,v/100);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    hsairpl_mcp_z738_set_alt_dial(v);
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_alt_dial!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x737_ap_alt_dial,(float)v);
      return;
    }
    
  }
  
  if(hsairxpl_mcp_datarefs.ap_altdial!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.ap_altdial,(float)v);
  }
}

uint32_t hsairpl_mcp_get_alt_dial(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_alt_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.t7_ap_alt_text,str,0,8); str[7]='\0';
      return atoi(str);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_alt_dial!=NULL){
      int i=XPLMGetDatai(hsairxpl_mcp_datarefs.a320n_fcu_alt_dial);
      return i*100;
    }
  }
  
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_alt_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b757_ap_alt_text,str,0,8); str[7]='\0';
      return atoi(str);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_alt_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b767_ap_alt_text,str,0,8); str[7]='\0';
      return atoi(str);
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_alt_dial!=NULL) {
      return (uint32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.x737_ap_alt_dial);
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_altdial!=NULL) {
    return (uint32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.ap_altdial);
  }
  return 0;
}

void hsairpl_mcp_set_speed_dial(float v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_spd_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_ap_spd_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    if(hsairxpl_mcp_datarefs.b757_ap_spd_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_ap_spd_dial,(float)v);
      return;
    }
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    if(hsairxpl_mcp_datarefs.b767_ap_spd_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_ap_spd_dial,(float)v);
      return;
    }
    
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(!hsairpl_mcp_get_speed_is_mach()){
      if(hsairxpl_mcp_datarefs.x737_ap_spd_ias_dial!=NULL) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x737_ap_spd_ias_dial,(float)v);
        return;
      }
    } else {
      if(hsairxpl_mcp_datarefs.x737_ap_spd_mach_dial!=NULL) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x737_ap_spd_mach_dial,(float)v);
        return;
      }
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_speeddial!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.ap_speeddial,(float)v);
  }
}

float hsairpl_mcp_get_speed_dial(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_spd_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.t7_ap_spd_text,str,0,8); str[7]='\0';
      float f=atof(str);
      return f;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    if(hsairxpl_mcp_datarefs.b757_ap_spd_setting!=NULL){
      float v = XPLMGetDataf(hsairxpl_mcp_datarefs.b757_ap_spd_setting);
      if(hsairpl_mcp_get_speed_is_mach())
        v /= 1000.0;
      return v;
    }
    
    /* Otherwise */
    if(hsairxpl_mcp_datarefs.b757_ap_spd_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b757_ap_spd_text,str,0,8); str[7]='\0';
      int i=atoi(str);
      float v= (float)i;
      if(hsairpl_mcp_get_speed_is_mach())
        v /= 1000.0;
      return v;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    
    /* Otherwise */
    if(hsairxpl_mcp_datarefs.b767_ap_spd_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b767_ap_spd_text,str,0,8); str[7]='\0';
      int i=atoi(str);
      float v= (float)i;
      if(hsairpl_mcp_get_speed_is_mach())
        v /= 1000.0;
      return v;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(!hsairpl_mcp_get_speed_is_mach()){
      if(hsairxpl_mcp_datarefs.x737_ap_spd_ias_dial!=NULL) {
        return (float)XPLMGetDataf(hsairxpl_mcp_datarefs.x737_ap_spd_ias_dial);
      }
    } else{
      if(hsairxpl_mcp_datarefs.x737_ap_spd_mach_dial!=NULL) {
        return (float)XPLMGetDataf(hsairxpl_mcp_datarefs.x737_ap_spd_mach_dial);
      }
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_speeddial!=NULL) {
    return (float)XPLMGetDataf(hsairxpl_mcp_datarefs.ap_speeddial);
  }
  return 0;
}


double hsairpl_mcp_mach_from_tas(double tas,double temp) {
  double m=tas/(39*sqrt(temp+273.15));
  return m;
}

double hsairpl_mcp_tas_from_mach(double mach,double temp) {
  double t=mach*(39*sqrt(temp+273.15));
  return t;
}

void hsairpl_mcp_set_speed_is_mach(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_spd_is_mach!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.x737_ap_spd_is_mach,v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_iasmach!=NULL) {
      if(v)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_iasmach,2);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_ap_iasmach,1);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_iasmach!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.b757_ap_iasmach,v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_iasmach!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.b767_ap_iasmach,v);
      return;
    }
  }
  
  
  if(hsairxpl_mcp_datarefs.ap_speedismac!=NULL && hsairxpl_mcp_datarefs.pos_otemp!=NULL) {
    if(hsairpl_mcp_get_speed_is_mach()!=v){
      float old_speed=hsairpl_mcp_get_speed_dial();
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_speedismac,v);
      if(hsxpl_plane_type==HSXPL_PLANE_XPLANE) {
        if(v) {
          hsairpl_mcp_set_speed_dial(hsairpl_mcp_mach_from_tas(old_speed,XPLMGetDataf(hsairxpl_mcp_datarefs.pos_otemp)));
        } else {
          hsairpl_mcp_set_speed_dial(hsairpl_mcp_tas_from_mach(old_speed,XPLMGetDataf(hsairxpl_mcp_datarefs.pos_otemp)));
        }
      }
    }
  }
}

uint32_t hsairpl_mcp_get_speed_is_mach(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_spd_is_mach!=NULL) {
      return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_spd_is_mach);
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_iasmach!=NULL){
      int i=(uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_iasmach);
      if(i==2) return 1;
      return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_iasmach!=NULL){
      return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_iasmach);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_iasmach!=NULL){
      return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_iasmach);
    }
  }
  
  
  
  if(hsairxpl_mcp_datarefs.ap_speedismac!=NULL) {
    return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.ap_speedismac);
  }
  return 0;
}

void hsairpl_mcp_set_vvm_dial(int32_t v) {
  
  v = v / 100; v = v * 100;   /* Round to 100 precision */
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vs_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_vs_dial,(float)v);
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_vs_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_ap_vs_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_vs_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_ap_vs_dial,(float)v);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_vs_dial!=NULL){
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_ap_vs_dial,(float)v);
      return;
    }
  }
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vs_dial!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x737_ap_vs_dial,(float)v);
      return;
    }
  }
  
  if(hsairxpl_mcp_datarefs.ap_vspeeddial!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.ap_vspeeddial,(float)v);
  }
}

int32_t hsairpl_mcp_get_vvm_dial(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_vs_dial!=NULL){
      return (int32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.x733_vs_dial);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_vs_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.t7_ap_vs_text,str,0,8); str[7]='\0';
      return atoi(str);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_vs_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b757_ap_vs_text,str,0,8); str[7]='\0';
      return atoi(str);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_vs_text!=NULL){
      char str[8];
      XPLMGetDatab(hsairxpl_mcp_datarefs.b767_ap_vs_text,str,0,8); str[7]='\0';
      return atoi(str);
    }
  }
  
  
  
  if(hsairxpl_mcp_datarefs.ap_vspeeddial!=NULL) {
    return (int32_t)XPLMGetDataf(hsairxpl_mcp_datarefs.ap_vspeeddial);
  }
  return 0;
}

/* AP SOURCE 0=NAV1, 1=NAV2, 2=FMC */
uint32_t hsairpl_ap_get_source(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    return hsairxpl_mcp_datarefs.x737_local_ap_source;
  }
  if(hsairxpl_mcp_datarefs.ap_hsisource!=NULL) {
    return(uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.ap_hsisource);
  }
  return 0;
  
}
void hsairpl_ap_set_source(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    hsairxpl_mcp_datarefs.x737_local_ap_source=v;
  }
  
  if(hsairxpl_mcp_datarefs.ap_hsisource!=NULL)
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_hsisource,v);
  
  if(hsairxpl_mcp_datarefs.ap_rmisource!=NULL)
    XPLMSetDatai(hsairxpl_mcp_datarefs.ap_rmisource,v);
}




void hsairpl_ap_toggle_source(void) {
  
  int apsrc=hsairpl_ap_get_source();
  apsrc++;
  if(apsrc>2) apsrc=0;
  hsairpl_ap_set_source(apsrc);
}

uint32_t hsairpl_ap_speed_status(void){
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    uint32_t i=0;
    if(hsairxpl_mcp_datarefs.x737_ap_speed_status) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_speed_status))  i=2;
    }
    
    if(hsairxpl_mcp_datarefs.x737_ap_atarm_status!=NULL) {
      if(!i && XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_atarm_status))  i=1;
    }
    return i;
  }
  
  if(hsairpl_mcp_get_spd_led())
    return 2;
  
  if(hsairxpl_mcp_datarefs.ap_at!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_at))
      return 2;
  }
  
  return 0;
}
uint32_t hsairpl_ap_vnav_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==3) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==4) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==5) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==3) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==4) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==5) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==3) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==4) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==5) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==3) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==4) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==5) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==3) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==4) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==5) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==3) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==4) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==5) return 1;
    }
    return 0;
  }
  
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vnav_status) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_vnav_status))  return 2;
      else return 0;
    }
  }
  
  if(hsairpl_mcp_get_vnav_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  if(x&4096) return 2;
  else if(x&8192) return 1;
  
  return 0;
}
uint32_t hsairpl_ap_hdg_status(void) {
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_hdg_status) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_hdg_status))  return 2;
      else return 0;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on)==1) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on)==6) return 2;
      
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_arm)==1) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_arm)==6) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on)==1) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on)==6) return 2;
      
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_arm)==1) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_arm)==6) return 1;
    }
    return 0;
  }
  
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on)==1) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on)==6) return 2;
      
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_arm)==1) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_arm)==6) return 1;
    }
    return 0;
  }
  
  
  
  if(hsairpl_mcp_get_hdg_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  
  if(x&2) return 2;
  
  return 0;
}
uint32_t hsairpl_ap_vs_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==2) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==2) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==2) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==2) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==2) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==2) return 1;
    }
    return 0;
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_vs_armed!=NULL && hsairxpl_mcp_datarefs.x737_ap_vs_on!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_vs_on))  return 2;
      else if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_vs_armed)) return 1;
      else return 0;
      
    }
  }
  
  if(hsairpl_mcp_get_vs_led())
    return 2;
  
  
  if(hsairxpl_mcp_datarefs.ap_vs_status)
    return XPLMGetDatai(hsairxpl_mcp_datarefs.ap_vs_status);
  
  return 0;
}

uint32_t hsairpl_ap_alt_status(void) {
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==1) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==1) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==1) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==1) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==1) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==1) return 1;
    }
    return 0;
  }
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_alt_status) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_alt_status))  return 2;
      else return 0;
    }
  }
  
  if(hsairpl_mcp_get_alt_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  
  if(x&16384) return 2;
  else if(x&32) return 1;
  
  return 0;
}
uint32_t hsairpl_ap_lnav_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_lnav_arm!=NULL && hsairxpl_mcp_datarefs.x737_ap_lnav_on!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_lnav_on))  return 2;
      else if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_lnav_arm)) return 1;
      else return 0;
      
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on)==2) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_arm)==2) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on)==2) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_arm)==2) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on)==2) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_arm)==2) return 1;
    }
    return 0;
  }
  
  
  
  
  if(hsairpl_mcp_get_lnav_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  
  
  if(hsairxpl_mcp_datarefs.ap_hsisource!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_hsisource)==2) {
      if(x & 512) return 2;
      else if(x&256) return 1;
    }
  }
  
  
  return 0;
}

uint32_t hsairpl_ap_loc_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_loc_captured!=NULL && hsairxpl_mcp_datarefs.x737_ap_loc_armed!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_captured))  return 2;
      else if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_loc_armed)) return 1;
      else return 0;
      
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_on)==3) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_roll_mode_arm)==3) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_on)==3) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_roll_mode_arm)==3) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_on)==3) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_roll_mode_arm)==3) return 1;
    }
    return 0;
  }
  
  
  
  
  
  if(hsairpl_mcp_get_loc_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  if(XPLMGetDatai(hsairxpl_mcp_datarefs.ap_hsisource)!=2) {
    if(x & 512) return 2;
    else if(x&256) return 1;
  }
  
  return 0;
}
uint32_t hsairpl_ap_gs_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==6) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==7) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==6) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==7) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==6) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==7) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==6) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==7) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==6) return 2;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==7) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==6) return 1;
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==7) return 1;
    }
    return 0;
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_gs_captured!=NULL && hsairxpl_mcp_datarefs.x737_ap_gs_armed!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_gs_captured))  return 2;
      else if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_gs_armed)) return 1;
      else return 0;
    }
    
  }
  
  if(hsairpl_mcp_get_app_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  if(x&2048) return 2;
  else if(x&1024) return 1;
  
  return 0;
}
uint32_t hsairpl_ap_lc_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_lcstatus!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_lcstatus)) {
        return 2;
      } else {
        return 0;
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_on)==8) return 2;
    }
    if(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_ap_pitch_mode_arm)==8) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_on)==8) return 2;
    }
    if(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_ap_pitch_mode_arm)==8) return 1;
    }
    return 0;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_on)==8) return 2;
    }
    if(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm!=NULL){
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_ap_pitch_mode_arm)==8) return 1;
    }
    return 0;
  }
  
  
  
  if(hsairpl_mcp_get_lchg_led())
    return 2;
  
  int x=0;
  if(hsairxpl_mcp_datarefs.ap_state!=NULL) {
    x=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_state);
  }
  if(x&64) return 2;
  
  return 0;
}
uint32_t hsairpl_ap_n1_status(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_ap_n1status!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.x737_ap_n1status)) {
        return 2;
      } else {
        return 0;
      }
    }
  }
  
  if(hsairpl_mcp_get_n1_led())
    return 2;
  
  return 0;
}

/* EFIS */

void hsairpl_efis1_press_wxr(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_wxr_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_wxr_act)>0.2)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_wxr_act,0.0);
      else
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_wxr_act,1.0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_wxr!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_wxr)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_wxr,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_wxr,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_wxr!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_wxr)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_wxr,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_wxr,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_wxr!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_wxr)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_wxr,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_wxr,1);
      }
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_wxr_press();
    return;
  }
  
  if(hsairxpl_mcp_datarefs.efis_swxr!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_swxr))
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_swxr, 0);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_swxr, 1);
  }
}


void hsairpl_efis1_press_sta(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_navaid_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act)>0.2)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act,0.0);
      else
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act,1.0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_sta!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_sta)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_sta,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_sta,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_sta!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_sta)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_sta,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_sta,1);
      }
      return;
    }
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_sta!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_sta)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_sta,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_sta,1);
      }
      return;
    }
    
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_sta_press();
    return;
  }
  
  if(hsairxpl_mcp_datarefs.efis_svor!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_svor)) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_svor, 0);
      if(hsairxpl_mcp_datarefs.efis_sndb!=NULL) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sndb, 0);
      }
    }
    else {
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_svor, 1);
      if(hsairxpl_mcp_datarefs.efis_sndb!=NULL) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sndb, 1);
      }
    }
  }
  
  
}


void hsairpl_efis1_press_wpt(void){
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_wpt_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act)>0.2)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act,0.0);
      else
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act,1.0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_wpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_wpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_wpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_wpt_press();
    return;
  }
  
  if(hsairxpl_mcp_datarefs.efis_sfix!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_sfix))
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sfix, 0);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sfix, 1);
  }
}


void hsairpl_efis1_press_arpt(void){
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_arpt_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act)>0.2)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act,0.0);
      else
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act,1.0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_arpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_arpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt,1);
      }
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_arpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt,1);
      }
      return;
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_arpt_press();
    return;
  }
  
  if(hsairxpl_mcp_datarefs.efis_sapt!=NULL){
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_sapt))
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sapt, 0);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sapt, 1);
  }
  
}

void hsairpl_efis1_press_data(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_data!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_data)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_data,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_data,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_data!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_data)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_data,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_data,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_data!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_data)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_data,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_data,1);
      }
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_data_press();
    return;
  }
}

void hsairpl_efis1_press_pos(void){
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_pos!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_pos)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_pos,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_pos,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_pos!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_pos)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_pos,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_pos,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_pos!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_pos)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_pos,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_pos,1);
      }
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_pos_press();
    return;
  }
}

void hsairpl_efis1_press_terr(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_terr!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_terr)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_terr,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_terr,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_terr!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_terr)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_terr,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_terr,1);
      }
    }
  }
  
  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_terr_press();
    return;
  }
}

void hsairpl_efis1_set_show(uint32_t v){
  
  
  hsairxpl_mcp_datarefs.efis1_local_mfdshow=v;
  if(hsairxpl_mcp_datarefs.efis_swxr!=NULL) {
    if(v & HSMP_MFD_SHOW_WXR)
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_swxr,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_swxr,0);
  }
  
  if(hsairxpl_mcp_datarefs.efis_stca!=NULL) {
    if(v & HSMP_MFD_SHOW_TCA)
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_stca,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_stca,0);
  }
  
  if(hsairxpl_mcp_datarefs.efis_sapt!=NULL) {
    if(v & HSMP_MFD_SHOW_APT)
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sapt,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sapt,0);
  }
  
  if(hsairxpl_mcp_datarefs.efis_sfix!=NULL) {
    if(v & HSMP_MFD_SHOW_WPT)
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sfix,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sfix,0);
  }
  
  if(hsairxpl_mcp_datarefs.efis_svor!=NULL) {
    if(v & HSMP_MFD_SHOW_VOR)
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_svor,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_svor,0);
  }
  
  if(hsairxpl_mcp_datarefs.efis_sndb!=NULL) {
    if(v & HSMP_MFD_SHOW_NDB)
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sndb,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_sndb,0);
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    
    if(v & HSMP_MFD_SHOW_APT) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act)<0.2){
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act,1.0);
      }
    } else {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act,0.0);
    }
    
    if(v & HSMP_MFD_SHOW_WPT) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act)<0.2){
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act,1.0);
      }
    } else {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act,0.0);
    }
    
    if(v & HSMP_MFD_SHOW_VOR) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act)<0.2){
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act,1.0);
      }
    } else {
      XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act,0.0);
    }
    
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    
    if(hsairxpl_mcp_datarefs.t7_efis_arpt!=NULL) {
      if(v & HSMP_MFD_SHOW_APT) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt,0);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.t7_efis_wpt!=NULL) {
      if(v & HSMP_MFD_SHOW_WPT) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt,0);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.t7_efis_sta!=NULL) {
      if(v & HSMP_MFD_SHOW_VOR) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_sta)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_sta,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_sta)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_sta,0);
        }
      }
    }
    
    
    if(hsairxpl_mcp_datarefs.t7_efis_nd_tfc!=NULL) {
      if(v & HSMP_MFD_SHOW_TCA) {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc,0);
        }
      } else {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc,1);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.t7_efis_nd_ctr!=NULL) {
      if(v & HSMP_MFD_SHOW_EXP) {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr,0);
        }
      } else {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr)) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr,1);
        }
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    if(hsairxpl_mcp_datarefs.b757_efis_arpt!=NULL) {
      if(v & HSMP_MFD_SHOW_APT) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt,0);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.b757_efis_wpt!=NULL) {
      if(v & HSMP_MFD_SHOW_WPT) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt,0);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.b757_efis_sta!=NULL) {
      if(v & HSMP_MFD_SHOW_VOR) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_sta)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_sta,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_sta)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_sta,0);
        }
      }
    }
    
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    if(hsairxpl_mcp_datarefs.b767_efis_arpt!=NULL) {
      if(v & HSMP_MFD_SHOW_APT) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt,0);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.b767_efis_wpt!=NULL) {
      if(v & HSMP_MFD_SHOW_WPT) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt,0);
        }
      }
    }
    
    if(hsairxpl_mcp_datarefs.b767_efis_sta!=NULL) {
      if(v & HSMP_MFD_SHOW_VOR) {
        if(!XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_sta)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_sta,1);
        }
      } else {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_sta)){
          XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_sta,0);
        }
      }
    }
    
    
    if(v & HSMP_MFD_SHOW_TCA) {
      hsairxpl_mcp_datarefs.b767_local_tfc_mode=1;
    } else {
      hsairxpl_mcp_datarefs.b767_local_tfc_mode=0;
    }
    
    
    
    
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode!=NULL) {
      if(v & HSMP_MFD_SHOW_EXP) {
        if(XPLMGetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode)==2) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,3);
        }
      } else if(XPLMGetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode)==3) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,2);
      }
    }
  }
  
  
}

uint32_t hsairpl_efis1_get_show(void){
  
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    
    if(hsairxpl_mcp_datarefs.x733_hsi_arpt_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_arpt_act)>0.2) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_APT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_APT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.x733_hsi_wpt_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_wpt_act)>0.2) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_WPT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_WPT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.x733_hsi_navaid_act!=NULL) {
      if(XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_navaid_act)>0.2) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_VOR;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_VOR;
      }
    }
  }
  
  else if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    
    if(hsairxpl_mcp_datarefs.t7_efis_wpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_wpt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_WPT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_WPT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.t7_efis_arpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_arpt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_APT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_APT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.t7_efis_sta!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_sta)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_VOR;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_VOR;
      }
    }
    
    if(hsairxpl_mcp_datarefs.t7_efis_nd_tfc!=NULL) {
      if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_TCA;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_TCA;
      }
    }
    
    /*
     if(hsairxpl_mcp_datarefs.t7_efis_nd_ctr!=NULL) {
     if(!XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr)) {
     hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_EXP;
     } else {
     hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_EXP;
     }
     }*/
    
    
    
  } else if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    if(hsairxpl_mcp_datarefs.b757_efis_wpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_wpt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_WPT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_WPT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.b757_efis_arpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_arpt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_APT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_APT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.b757_efis_sta!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_sta)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_VOR;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_VOR;
      }
    }
    
    if(hsairxpl_mcp_datarefs.efis_stca!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_stca)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_TCA;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_TCA;
      }
    }
    
  } else if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    if(hsairxpl_mcp_datarefs.b767_efis_wpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_wpt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_WPT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_WPT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.b767_efis_arpt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_arpt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_APT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_APT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.b767_efis_sta!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_sta)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_VOR;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_VOR;
      }
    }
    
    
    if(hsairxpl_mcp_datarefs.b767_local_tfc_mode) {
      hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_TCA;
    } else {
      hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_TCA;
    }
    
    
  } else {
    
    if(hsairxpl_mcp_datarefs.efis_sfix!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_sfix)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_WPT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_WPT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.efis_sapt!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_sapt)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_APT;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_APT;
      }
    }
    
    if(hsairxpl_mcp_datarefs.efis_svor!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_svor)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_VOR;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_VOR;
      }
    }
    
    if(hsairxpl_mcp_datarefs.efis_stca!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_stca)) {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_TCA;
      } else {
        hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_TCA;
      }
    }
    
    
  }
  
  
  if(hsairxpl_mcp_datarefs.efis_swxr!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_swxr)) {
      hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_WXR;
    } else {
      hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_WXR;
    }
  }
  
  
  if(hsairxpl_mcp_datarefs.efis_sndb!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_sndb)) {
      hsairxpl_mcp_datarefs.efis1_local_mfdshow |= HSMP_MFD_SHOW_NDB;
    } else {
      hsairxpl_mcp_datarefs.efis1_local_mfdshow &= ~HSMP_MFD_SHOW_NDB;
    }
  }
  
  
  return hsairxpl_mcp_datarefs.efis1_local_mfdshow;
  
}

void hsairpl_efis1_mins_select_radio(void) {
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_mins_type!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_mins_type, 0);
    }
    return;
  }
  
  hsairpl_efis1_dec_mode();
  
}
void hsairpl_efis1_mins_select_baro(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_mins_type!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_mins_type, 1);
    }
    return;
  }
  
  hsairpl_efis1_inc_mode();
  
}


void hsairpl_efis1_select_hpa(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_baro_type!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_baro_type, 1);
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_efis_barounits!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.x737_efis_barounits, 1);
    }
  }
  
}
void hsairpl_efis1_select_inhg(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_baro_type!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_baro_type, 0);
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_X737) {
    if(hsairxpl_mcp_datarefs.x737_efis_barounits!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.x737_efis_barounits, 0);
    }
  }
  
}

void hsairpl_efis1_inc_mins(float v){
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_mins_setting!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.t7_efis_mins_setting);
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_efis_mins_setting,f+(0.01*v));
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_mins_setting!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.b757_efis_mins_setting);
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_mins_setting,f+(0.01*v));
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_mins_setting!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.b767_efis_mins_setting);
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_mins_setting,f+(0.01*v));
      return;
    }
  }
  
  
}

void hsairpl_efis1_dec_mins(float v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_mins_setting!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.t7_efis_mins_setting);
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_efis_mins_setting,f-(0.01*v));
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_mins_setting!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.b757_efis_mins_setting);
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_mins_setting,f-(0.01*v));
      return;
    }
  }
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_mins_setting!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.b767_efis_mins_setting);
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_mins_setting,f-(0.01*v));
      return;
    }
  }
  
  
}

void hsairpl_efis1_set_baro(float v){
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_baro_setting!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.t7_efis_baro_setting,v-29.92+0.5);
      return;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_baro_setting1!=NULL) {
      XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_baro_setting1,v-29.92+0.5);
      if(hsairxpl_mcp_datarefs.b757_efis_baro_setting2!=NULL) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_baro_setting2,v-29.92+0.5);
      }
      if(hsairxpl_mcp_datarefs.b757_efis_baro_setting3!=NULL) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_baro_setting3,v-29.92+0.5);
      }
      return;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_baro_setting1!=NULL) {
      
      float x = (v - 26.96662842) / 5.9066723;
      XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_baro_setting1,x);
      if(hsairxpl_mcp_datarefs.b767_efis_baro_setting2!=NULL) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_baro_setting2,x);
      }
      if(hsairxpl_mcp_datarefs.b767_efis_baro_setting3!=NULL) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_baro_setting3,x);
      }
      return;
    }
  }
  
  
  
  if(hsairxpl_mcp_datarefs.efis_altimeter!=NULL) {
    XPLMSetDataf(hsairxpl_mcp_datarefs.efis_altimeter,v);
  }
  
}
float hsairpl_efis1_get_baro(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_baro_setting!=NULL) {
      return XPLMGetDataf(hsairxpl_mcp_datarefs.t7_efis_baro_setting)+29.92-0.5;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_baro_setting1!=NULL) {
      return XPLMGetDataf(hsairxpl_mcp_datarefs.b757_efis_baro_setting1)+29.92-0.5;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_baro_setting1!=NULL) {
      float baroS=(float)XPLMGetDataf(hsairxpl_mcp_datarefs.b767_efis_baro_setting1);
      return 26.96662842 + (baroS*5.9066723);
    }
  }
  
  
  if(hsairxpl_mcp_datarefs.efis_altimeter!=NULL) {
    return XPLMGetDataf(hsairxpl_mcp_datarefs.efis_altimeter);
  }
  return 0;
}


void hsairpl_efis1_inc_baro(float v){
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    hsairpl_efis1_set_baro(hsairpl_efis1_get_baro()+(0.002*v));
    return;
  }
  
  hsairpl_efis1_set_baro(hsairpl_efis1_get_baro()+(0.010*v));
}

void hsairpl_efis1_dec_baro(float v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    hsairpl_efis1_set_baro(hsairpl_efis1_get_baro()-(0.002*v));
    return;
  }
  hsairpl_efis1_set_baro(hsairpl_efis1_get_baro()-(0.010*v));
}

void hsairpl_efis1_mins_press_rst(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_mins_rst!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_mins_rst)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_mins_rst,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_mins_rst,1);
      }
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_mins_rst!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_mins_rst)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_mins_rst,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_mins_rst,1);
      }
    }
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_mins_rst!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_mins_rst)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_mins_rst,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_mins_rst,1);
      }
    }
    return;
  }
  
  
  
}

void hsairpl_efis1_press_std(void) {
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_baro_std!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_baro_std)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_baro_std,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_baro_std,1);
      }
    }
    hsairpl_efis1_set_baro(29.92);
    return;
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_baro_std_press();
    return;
  }
  
  hsairpl_efis1_set_baro(29.92);
  
}
void hsairpl_efis1_press_ctr(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_mode!=NULL) {
      float f=XPLMGetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode);
      int i = (int)f;
      if(i==0) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,1.0);
      } else if (i==1) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,0.0);
      } else if (i==2) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,3.0);
      } else if (i==3) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,2.0);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_nd_ctr!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_ctr,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode){
      hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode=0;
    } else {
      hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode=1;
    }
    hsairpl_efis1_set_mode(hsairpl_efis1_get_mode());
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode!=NULL) {
      uint32_t m=XPLMGetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode);
      if(m==3) XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,2);
      else if(m==2) XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,3);
    }
  }
  
  if(hsairxpl_mcp_datarefs.efis_hsimode!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_hsimode))
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_hsimode, 0);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_hsimode, 1);
  }
  
}
void hsairpl_efis1_press_tfc(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_nd_tfc!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_tfc,1);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_local_tfc_mode) hsairxpl_mcp_datarefs.b767_local_tfc_mode=0;
    else hsairxpl_mcp_datarefs.b767_local_tfc_mode=1;
    hsairpl_efis1_set_show(hsairpl_efis1_get_show());
    return;
  }
  
  
  if(hsairxpl_mcp_datarefs.efis_stca!=NULL) {
    if(XPLMGetDatai(hsairxpl_mcp_datarefs.efis_stca))
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_stca, 0);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_stca, 1);
  }
}

void hsairpl_efis1_press_fpv(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_fpv!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_fpv)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_fpv,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_fpv,1);
      }
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_fpv_press();
    return;
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_XPLANE) {
    hsairpl_ap_toggle_source();
  }
  return;
}

void hsairpl_efis1_press_mtrs(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_mtrs!=NULL) {
      if(XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_mtrs)) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_mtrs,0);
      } else {
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_mtrs,1);
      }
    }
  }

  if ( hsxpl_plane_type==HSXPL_PLANE_Z738 || hsxpl_plane_type==HSXPL_PLANE_B738) {
    hsairpl_efis1_b738_mtrs_press();
    return;
  }
}

void hsairpl_efis1_set_vas1(uint32_t v) {
  
  if(v<=2) {
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
      if(hsairxpl_mcp_datarefs.t7_efis_vas1!=NULL) {
        int x;
        if(v==0) x=-1;
        else if(v==1) x=0;
        else x=1;
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_vas1,x);
      }
    }
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
      
      if(hsairxpl_mcp_datarefs.b757_efis_vas1_sel!=NULL) {
        if(v==2)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_vas1_sel,0.0);
        else if(v==0)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_vas1_sel,1.0);
      }
    }
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
      
      if(hsairxpl_mcp_datarefs.b767_efis_vas1_sel!=NULL) {
        if(v==2)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_vas1_sel,0.0);
        else if(v==0)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_vas1_sel,1.0);
      }
    }
    
    
    if(hsairxpl_mcp_datarefs.efis_n1sel!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_n1sel,v);
    }
  }
}

uint32_t hsairpl_efis1_get_vas1(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_vas1!=NULL) {
      int x=XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_vas1);
      if(x<0) return 0;
      else if(x==0) return 1;
      else return 2;
    }
  }
  
  if(hsairxpl_mcp_datarefs.efis_n1sel!=NULL) {
    return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.efis_n1sel);
  }
  return 0;
}

void hsairpl_efis1_set_vas2(uint32_t v) {
  
  if(v<=2) {
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
      if(hsairxpl_mcp_datarefs.t7_efis_vas2!=NULL) {
        int x;
        if(v==0) x=-1;
        else if(v==1) x=0;
        else x=1;
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_vas2,x);
      }
    }
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
      if(hsairxpl_mcp_datarefs.b757_efis_vas2_sel!=NULL) {
        if(v==2)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_vas2_sel,0.0);
        else if(v==0)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b757_efis_vas2_sel,1.0);
      }
    }
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
      if(hsairxpl_mcp_datarefs.b767_efis_vas2_sel!=NULL) {
        if(v==2)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_vas2_sel,0.0);
        else if(v==0)
          XPLMSetDataf(hsairxpl_mcp_datarefs.b767_efis_vas2_sel,1.0);
      }
    }
    
    
    
    if(hsairxpl_mcp_datarefs.efis_n2sel!=NULL) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.efis_n2sel,v);
    }
  }
}

uint32_t hsairpl_efis1_get_vas2(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_vas2!=NULL) {
      int x=XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_vas2);
      if(x<0) return 0;
      else if(x==0) return 1;
      else return 2;
    }
  }
  
  if(hsairxpl_mcp_datarefs.efis_n2sel!=NULL) {
    return (uint32_t)XPLMGetDatai(hsairxpl_mcp_datarefs.efis_n2sel);
  }
  return 0;
}

void hsairpl_efis1_set_mode(uint32_t v) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_mode!=NULL) {
      if (v==0 || v==1) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,1.0);
      } else if (v==2 || v==3) {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,2.0);
      } else {
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_mode,4.0);
      }
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_nd_mode!=NULL) {
      uint32_t x=v;
      if(x==4) x=3;
      XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_mode,x);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode!=NULL) {
      if(v==4) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,4);
      } else if(v==2) {
        if(hsairxpl_mcp_datarefs.efis1_local_mfdshow & HSMP_MFD_SHOW_EXP) {
          XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,3);
        } else {
          XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,2);
        }
      } else if(v==0||v==1) {
        XPLMSetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode,v);
      }
      
      
      
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_nd_mode!=NULL) {
      uint32_t x;
      if(v==0 && hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode) x=0;
      else if(v==0 && !hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode) x=3;
      else if(v==1 && hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode) x=1;
      else if(v==1 && !hsairxpl_mcp_datarefs.b757_local_nd_ctr_mode) x=2;
      else if(v==2) x=4;
      else if(v==4) x=5;
      else x=4;
      XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_mode,x);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_nd_mode!=NULL) {
      uint32_t x;
      if(v==0) x=1;
      else if(v==1) x=0;
      else if(v==2) x=2;
      else if(v==3) x=3;
      else if(v==4) x=3;
      else x=2;
      XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_mode,x);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    hsairpl_efis1_z738_set_mode(v);
  }
  
  /* 0=APP, 1=VOR, 2=MAP, 3=NAV, 4=PLN */
  if(hsairxpl_mcp_datarefs.ap_mfdmode!=NULL) {
    if(v<=4) {
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdmode,v);
    }
  }
}

uint32_t hsairpl_efis1_get_mode(void) {
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_mode!=NULL) {
      float f =XPLMGetDatai(hsairxpl_mcp_datarefs.x733_hsi_mode);
      uint32_t m = (uint32_t)f;
      if(m==0) { return 1; }
      if(m==1) { return 1; }
      if(m==2) { return 2; }
      if(m==3) { return 2; }
      if(m==4) { return 4; }
      if(m==5) { return 4; }
      return m;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    if(hsairxpl_mcp_datarefs.t7_efis_nd_mode!=NULL) {
      uint32_t m=XPLMGetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_mode);
      if(m==3) m=4;
      return m;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_A320N) {
    if(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode!=NULL) {
      uint32_t m=XPLMGetDatai(hsairxpl_mcp_datarefs.a320n_fcu_nd_mode);
      if(m==3) m=2;
      return m;
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    if(hsairxpl_mcp_datarefs.b757_efis_nd_mode!=NULL) {
      uint32_t m=XPLMGetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_mode);
      if(m==0) { return 0; }
      if(m==1) { return 1; }
      if(m==3) { return 0; }
      if(m==2) { return 1; }
      if(m==4) { return 2; }
      if(m==5) { return 4; }
      return m;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_efis_nd_mode!=NULL) {
      uint32_t m=XPLMGetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_mode);
      if(m==0) { return 1; }
      if(m==1) { return 0; }
      if(m==2) { return 2; }
      if(m==3) { return 4; }
      if(m==4) { return 4; }
      if(m==5) { return 4; }
      return m;
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    return hsairpl_efis1_z738_get_mode();
  }
  
  if(hsairxpl_mcp_datarefs.ap_mfdmode!=NULL) {
    return XPLMGetDatai(hsairxpl_mcp_datarefs.ap_mfdmode);
  }
  
  return 0;
  
}
void hsairpl_efis1_inc_mode(void) {
  
  uint32_t m=hsairpl_efis1_get_mode();
  m++; if(m==3) m=4;
  if(m>4) m=0;
  hsairpl_efis1_set_mode(m);
}
void hsairpl_efis1_dec_mode(void) {
  
  uint32_t m=hsairpl_efis1_get_mode();
  m--; if(m==3) m=2;
  if(m>4) m=4;
  hsairpl_efis1_set_mode(m);
}


void hsairpl_efis1_set_range(float v) {
  
  hsairxpl_mcp_datarefs.efis1_local_mfdrange=v;
  
  if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
    if(hsairxpl_mcp_datarefs.x733_hsi_range!=NULL) {
      if(v>=320)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_range,5);
      else if(v>=160)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_range,4);
      else if(v>=80)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_range,3);
      else if(v>=40)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_range,2);
      else if(v>=20)
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_range,1);
      else
        XPLMSetDataf(hsairxpl_mcp_datarefs.x733_hsi_range,0);
    }
  }
  
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF777) {
    
    if(hsairxpl_mcp_datarefs.t7_efis_nd_range!=NULL) {
      
      if(v>=640)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,6);
      else if(v>=320)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,5);
      else if(v>=160)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,4);
      else if(v>=80)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,3);
      else if(v>=40)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,2);
      else if(v>=20)
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,1);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.t7_efis_nd_range,0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF757) {
    
    if(hsairxpl_mcp_datarefs.b757_efis_nd_range!=NULL) {
      
      if(v>=640)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,6);
      else if(v>=320)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,5);
      else if(v>=160)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,4);
      else if(v>=80)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,3);
      else if(v>=40)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,2);
      else if(v>=20)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,1);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.b757_efis_nd_range,0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    
    if(hsairxpl_mcp_datarefs.b767_efis_nd_range!=NULL) {
      
      if(v>=640)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,6);
      else if(v>=320)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,5);
      else if(v>=160)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,4);
      else if(v>=80)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,3);
      else if(v>=40)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,2);
      else if(v>=20)
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,1);
      else
        XPLMSetDatai(hsairxpl_mcp_datarefs.b767_efis_nd_range,0);
    }
  }
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    hsairpl_efis1_z738_set_range(v);
  }
  
  if(hsairxpl_mcp_datarefs.ap_mfdrange!=NULL) {
    
    if(v>=640)
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,6);
    else if(v>=320)
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,5);
    else if(v>=160)
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,4);
    else if(v>=80)
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,3);
    else if(v>=40)
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,2);
    else if(v>=20)
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,1);
    else
      XPLMSetDatai(hsairxpl_mcp_datarefs.ap_mfdrange,0);
  }
  
}

float hsairpl_efis1_get_range(void) {
  
  static int hsairpl_prev_local_mfd_range=-1;
  
  
  int current_mfd_range=-1;
  
  if(hsairxpl_mcp_datarefs.ap_mfdrange!=NULL)
    current_mfd_range=XPLMGetDatai(hsairxpl_mcp_datarefs.ap_mfdrange);
  
  if(hsxpl_plane_type==HSXPL_PLANE_Z738) {
    current_mfd_range=hsairpl_efis1_z738_get_range();
  }

  if(current_mfd_range!=hsairpl_prev_local_mfd_range || hsairxpl_mcp_datarefs.efis1_local_mfdrange==0) {
    switch(current_mfd_range) {
      case(0): hsairxpl_mcp_datarefs.efis1_local_mfdrange=10.0; break;
      case(1): hsairxpl_mcp_datarefs.efis1_local_mfdrange=20.0; break;
      case(2): hsairxpl_mcp_datarefs.efis1_local_mfdrange=40.0; break;
      case(3): hsairxpl_mcp_datarefs.efis1_local_mfdrange=80.0; break;
      case(4): hsairxpl_mcp_datarefs.efis1_local_mfdrange=160.0; break;
      case(5): hsairxpl_mcp_datarefs.efis1_local_mfdrange=320.0; break;
      case(6): hsairxpl_mcp_datarefs.efis1_local_mfdrange=640.0; break;
    }
  }
  hsairpl_prev_local_mfd_range=current_mfd_range;
  return hsairxpl_mcp_datarefs.efis1_local_mfdrange;
  
}

/* hsairpl_second_timer() is executed once per second from the main loop */
void hsairpl_mcp_second_timer(void) {
  
  /* hack, the 767 only loads all datarefs after the plugin has started, so test it and reload if necessary */
  if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
    if(hsairxpl_mcp_datarefs.b767_ap_hdg_dial==NULL) {
      hsxpl_set_datarefs();
    }
  }
}

