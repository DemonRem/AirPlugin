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
 * Generic datarefs used throughout the plugin
 *
 */

#include "hsxpldatarefs.h"
#include "hsxpl.h"
#include "hsxplmisc.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;
extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;

/* Pre-set the references to the datarefs for quicker access later */
void hsxpl_set_xplane_datarefs(void) {

  memset(&hsxpl_xplane_datarefs,0,sizeof(struct hsxpl_xplane_datarefs_s));
  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_xplane_datarefs.latitude=XPLMFindDataRef("sim/flightmodel/position/latitude");
  hsxpl_xplane_datarefs.longitude=XPLMFindDataRef("sim/flightmodel/position/longitude");
  /* pos_elev=XPLMFindDataRef("sim/flightmodel/position/elevation"); */
  hsxpl_xplane_datarefs.elevation=XPLMFindDataRef("sim/flightmodel/misc/h_ind");
  hsxpl_xplane_datarefs.theta=XPLMFindDataRef("sim/flightmodel/position/theta");
  hsxpl_xplane_datarefs.phi=XPLMFindDataRef("sim/flightmodel/position/phi");
  hsxpl_xplane_datarefs.psi=XPLMFindDataRef("sim/flightmodel/position/psi");
  hsxpl_xplane_datarefs.mvar=XPLMFindDataRef("sim/flightmodel/position/magnetic_variation");
  hsxpl_xplane_datarefs.hpath=XPLMFindDataRef("sim/flightmodel/position/hpath");
  hsxpl_xplane_datarefs.vpath=XPLMFindDataRef("sim/flightmodel/position/vpath");
  hsxpl_xplane_datarefs.gs=XPLMFindDataRef("sim/flightmodel/position/groundspeed");
  hsxpl_xplane_datarefs.ias=XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
  hsxpl_xplane_datarefs.tas=XPLMFindDataRef("sim/flightmodel/position/true_airspeed");
  hsxpl_xplane_datarefs.vvi=XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm2");
  hsxpl_xplane_datarefs.mach=XPLMFindDataRef("sim/flightmodel/misc/machno");
  hsxpl_xplane_datarefs.msl=XPLMFindDataRef("sim/flightmodel/position/elevation");
  hsxpl_xplane_datarefs.agl=XPLMFindDataRef("sim/flightmodel/position/y_agl");

  hsxpl_xplane_datarefs.winddir=XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_heading_deg_mag");
  hsxpl_xplane_datarefs.windspeed=XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_speed_kts");
  hsxpl_xplane_datarefs.outsidetemp=XPLMFindDataRef("sim/weather/temperature_ambient_c");

  hsxpl_xplane_datarefs.tailno=XPLMFindDataRef("sim/aircraft/view/acf_tailnum");
  hsxpl_xplane_datarefs.acficao=XPLMFindDataRef("sim/aircraft/view/acf_ICAO");

  hsxpl_xplane_datarefs.lgear=XPLMFindDataRef("sim/cockpit/switches/gear_handle_status");
  hsxpl_xplane_datarefs.pbreak=XPLMFindDataRef("sim/cockpit2/controls/parking_brake_ratio");
  hsxpl_xplane_datarefs.flaps=XPLMFindDataRef("sim/cockpit2/controls/flap_ratio");
  hsxpl_xplane_datarefs.spdbreak=XPLMFindDataRef("sim/cockpit2/controls/speedbrake_ratio");

  hsxpl_xplane_datarefs.vso=XPLMFindDataRef("sim/aircraft/view/acf_Vso");
  hsxpl_xplane_datarefs.vs=XPLMFindDataRef("sim/aircraft/view/acf_Vs");
  hsxpl_xplane_datarefs.vfe=XPLMFindDataRef("sim/aircraft/view/acf_Vfe");
  hsxpl_xplane_datarefs.vno=XPLMFindDataRef("sim/aircraft/view/acf_Vno");
  hsxpl_xplane_datarefs.vne=XPLMFindDataRef("sim/aircraft/view/acf_Vne");
  hsxpl_xplane_datarefs.mmo=XPLMFindDataRef("sim/aircraft/view/acf_Mmo");

  hsxpl_xplane_datarefs.joyroll=XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
  hsxpl_xplane_datarefs.joypitch=XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
  hsxpl_xplane_datarefs.joyyaw=XPLMFindDataRef("sim/joystick/yoke_heading_ratio");

  hsxpl_fmc.key_0=XPLMFindCommand("sim/FMS/key_0");
  hsxpl_fmc.key_1=XPLMFindCommand("sim/FMS/key_1");
  hsxpl_fmc.key_2=XPLMFindCommand("sim/FMS/key_2");
  hsxpl_fmc.key_3=XPLMFindCommand("sim/FMS/key_3");
  hsxpl_fmc.key_4=XPLMFindCommand("sim/FMS/key_4");
  hsxpl_fmc.key_5=XPLMFindCommand("sim/FMS/key_5");
  hsxpl_fmc.key_6=XPLMFindCommand("sim/FMS/key_6");
  hsxpl_fmc.key_7=XPLMFindCommand("sim/FMS/key_7");
  hsxpl_fmc.key_8=XPLMFindCommand("sim/FMS/key_8");
  hsxpl_fmc.key_9=XPLMFindCommand("sim/FMS/key_9");

  hsxpl_fmc.key_a=XPLMFindCommand("sim/FMS/key_A");
  hsxpl_fmc.key_b=XPLMFindCommand("sim/FMS/key_B");
  hsxpl_fmc.key_c=XPLMFindCommand("sim/FMS/key_C");
  hsxpl_fmc.key_d=XPLMFindCommand("sim/FMS/key_D");
  hsxpl_fmc.key_e=XPLMFindCommand("sim/FMS/key_E");
  hsxpl_fmc.key_f=XPLMFindCommand("sim/FMS/key_F");
  hsxpl_fmc.key_g=XPLMFindCommand("sim/FMS/key_G");
  hsxpl_fmc.key_h=XPLMFindCommand("sim/FMS/key_H");
  hsxpl_fmc.key_i=XPLMFindCommand("sim/FMS/key_I");
  hsxpl_fmc.key_j=XPLMFindCommand("sim/FMS/key_J");
  hsxpl_fmc.key_k=XPLMFindCommand("sim/FMS/key_K");
  hsxpl_fmc.key_l=XPLMFindCommand("sim/FMS/key_L");
  hsxpl_fmc.key_m=XPLMFindCommand("sim/FMS/key_M");
  hsxpl_fmc.key_n=XPLMFindCommand("sim/FMS/key_N");
  hsxpl_fmc.key_o=XPLMFindCommand("sim/FMS/key_O");
  hsxpl_fmc.key_p=XPLMFindCommand("sim/FMS/key_P");
  hsxpl_fmc.key_q=XPLMFindCommand("sim/FMS/key_Q");
  hsxpl_fmc.key_r=XPLMFindCommand("sim/FMS/key_R");
  hsxpl_fmc.key_s=XPLMFindCommand("sim/FMS/key_S");
  hsxpl_fmc.key_t=XPLMFindCommand("sim/FMS/key_T");
  hsxpl_fmc.key_u=XPLMFindCommand("sim/FMS/key_U");
  hsxpl_fmc.key_v=XPLMFindCommand("sim/FMS/key_V");
  hsxpl_fmc.key_w=XPLMFindCommand("sim/FMS/key_W");
  hsxpl_fmc.key_x=XPLMFindCommand("sim/FMS/key_X");
  hsxpl_fmc.key_y=XPLMFindCommand("sim/FMS/key_Y");
  hsxpl_fmc.key_z=XPLMFindCommand("sim/FMS/key_Z");

  hsxpl_fmc.key_lk1=XPLMFindCommand("sim/FMS/ls_6l");
  hsxpl_fmc.key_lk2=XPLMFindCommand("sim/FMS/ls_5l");
  hsxpl_fmc.key_lk3=XPLMFindCommand("sim/FMS/ls_4l");
  hsxpl_fmc.key_lk4=XPLMFindCommand("sim/FMS/ls_3l");
  hsxpl_fmc.key_lk5=XPLMFindCommand("sim/FMS/ls_2l");
  hsxpl_fmc.key_lk6=XPLMFindCommand("sim/FMS/ls_1l");

  hsxpl_fmc.key_rk1=XPLMFindCommand("sim/FMS/ls_6r");
  hsxpl_fmc.key_rk2=XPLMFindCommand("sim/FMS/ls_5r");
  hsxpl_fmc.key_rk3=XPLMFindCommand("sim/FMS/ls_4r");
  hsxpl_fmc.key_rk4=XPLMFindCommand("sim/FMS/ls_3r");
  hsxpl_fmc.key_rk5=XPLMFindCommand("sim/FMS/ls_2r");
  hsxpl_fmc.key_rk6=XPLMFindCommand("sim/FMS/ls_1r");

  hsxpl_fmc.key_init=XPLMFindCommand("sim/FMS/init");
  hsxpl_fmc.key_prevpage=XPLMFindCommand("sim/FMS/prev");
  hsxpl_fmc.key_nextpage=XPLMFindCommand("sim/FMS/next");
  hsxpl_fmc.key_clr=XPLMFindCommand("sim/FMS/clear");
  hsxpl_fmc.key_exec=XPLMFindCommand("sim/FMS/direct");
  hsxpl_fmc.key_plusminus=XPLMFindCommand("sim/FMS/sign");
  hsxpl_fmc.key_dot=XPLMFindCommand("sim/FMS/key_period");
  hsxpl_fmc.key_del=XPLMFindCommand("sim/FMS/key_back");
  hsxpl_fmc.key_space=XPLMFindCommand("sim/FMS/key_space");
  hsxpl_fmc.key_load=XPLMFindCommand("sim/FMS/key_load");
  hsxpl_fmc.key_save=XPLMFindCommand("sim/FMS/key_save");

  hsxpl_fmc.key_apt=XPLMFindCommand("sim/FMS/type_apt");
  hsxpl_fmc.key_vor=XPLMFindCommand("sim/FMS/type_vor");
  hsxpl_fmc.key_ndb=XPLMFindCommand("sim/FMS/type_ndb");
  hsxpl_fmc.key_wpt=XPLMFindCommand("sim/FMS/type_fix");
  hsxpl_fmc.key_latlon=XPLMFindCommand("sim/FMS/type_latlon");

}


#pragma mark Dataref retrievers

char *hsxpl_acf_icao(void) {

  static char acficao[32]; memset(acficao,0,8);
  if(hsxpl_xplane_datarefs.acficao!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.acficao,acficao,0,31);
  return acficao;
}

char *hsxpl_acf_tailno(void) {
  static char actfailno[8]; memset(actfailno,0,8);
  if(hsxpl_xplane_datarefs.tailno!=NULL)
    XPLMGetDatab(hsxpl_xplane_datarefs.tailno,actfailno,0,31);
  return actfailno;
}

