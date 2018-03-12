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
 * Description:     	The X-Plane plugin for the Haversine Message Protocol
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "hsxpl.h"
#include "hsxplmisc.h"
#include "hsxplnavdb.h"
#include "hsmpmsg.h"
#include "hsmpnet.h"
#include "hsmptcp.h"
#include "hsxplclist.h"
#include "hsxplmcp.h"
#include "hsxplcoms.h"
#include "hsxplatc.h"
#include "hsxplairports.h"
#include "hsxpldref-requests.h"
#include "hsxplfmc.h"
#include "hsxpldefault738.h"
#include "hsxplzibo738.h"
#include "hsxpleadtx738.h"
#include "hsxplixeg733.h"
#include "hsxpljara320.h"
#include "hsxplsettings.h"
#include "hsxpldatarefs.h"
#include "hsxplff757.h"
#include "hsxplff767.h"
#include "hsxplff777.h"
#include "hsxplcrj200.h"
#include "hsxplxfmc.h"
#include "hsxplufmc.h"
#include "hsxplqa320.h"
#include "hsxplxsquawkbox.h"

#ifdef CPFLIGHT
#include "hsxplcpflight.h"
#endif

/* How often the engine handler is called to do stuff in secs */
#define	HSXPL_ENGINE_TICTAC		0.1	/* 10 times per second */

#define	EARTH_RADIUS	6371		/* In kms */

#ifndef M_PI
#define	M_PI	3.1415926535
#endif

#define HSFTOI(a) ((int32_t)a)
#define HSITOF(a) (a<0?(float)(0-fabsf((float)(0-a))):(float)(a))

/* For Windows, define DLLMain */
#if IBM
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}
#endif
#pragma mark Global Variables

struct sockaddr_in hsxpl_unicast_sa;

/* FMC type, as in HSMP_FMC_TYPE_* */
uint32_t hsxpl_fmc_type=HSMP_FMC_TYPE_XPLANE;
uint32_t hsxpl_plane_type=HSXPL_PLANE_XPLANE;
uint32_t hsxpl_idver=0;

extern uint32_t hsxpl_cpflight_enabled;

/* Local hack to fix AirTrack N1 press twice bug until it's fixed */
uint32_t hsxpl_n1_pressed=0;

/* FMC datarefs */
extern hsxpl_fmc_t hsxpl_fmc;

extern hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;



#pragma mark X-Plane Callbacks

/* The default/required plugin handlers */
PLUGIN_API int XPluginStart ( char * outName, char * outSig, char * outDesc ) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPluginStart()");
#endif

  /* Register our credentials */
  sprintf(outName,"Haversine Air Plugin v%s",HSAIRXPL_VERSION);
  strcpy(outSig, "com.haversine.air.xpl");
  sprintf(outDesc,"Haversine Air Plugin v%s",HSAIRXPL_VERSION);

  /* Log the start of the plugin regardless of the state of HSXPLDEBUG so
   * that its version gets into Log.txt */
  char logstr[64];
  sprintf(logstr,"%s XPluginStart()\n",outName);
  XPLMDebugString(logstr);


  /* Determine node type, windows, mac or linux */
  hsxpl_idver=HSMP_PKT_NT_XPG;
#if defined(_WIN32)
  hsxpl_idver=HSMP_PKT_NT_XPW;
#else
  struct utsname ut;
  if(!uname(&ut)) {
    if(strstr(ut.sysname,"Darwin")!=NULL)
      hsxpl_idver=HSMP_PKT_NT_XPM;
    else if(strstr(ut.sysname,"Linux")!=NULL)
      hsxpl_idver=HSMP_PKT_NT_XPL;
  }
#endif

  /* Initialise network */

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_initialise_network()");
#endif
  hsmp_initialise_network("0.0.0.0",0,hsxpl_idver);


  /* Register AirTrack multicast receiver */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_add_multicast_target()");
#endif
  hsmp_add_multicast_target(HSMP_DEF_MULTICAST_ADDR,HSMP_AIRTRACK_PORT);

  /* Register AirFMC multicast receiver */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_add_multicast_target()");
#endif
  hsmp_add_multicast_target(HSMP_DEF_MULTICAST_ADDR,HSMP_AIRFMC_PORT);

  /* Register AirEFB multicast receiver */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_add_multicast_target()");
#endif
  hsmp_add_multicast_target(HSMP_DEF_MULTICAST_ADDR,HSMP_AIREFB_PORT);

  /* Enable message callback */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_net_message_received_callback=");
#endif
  hsmp_net_message_received_callback = hsxpl_hsmp_message_callback;

  /* Setup datarefs */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_set_datarefs()");
#endif
  hsxpl_set_datarefs();

  /* Clear route */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_navdb_clear_route()");
#endif
  hsxpl_navdb_clear_route();

  /* Setup settings menu entries */
  hsxpl_settings_configure_menu();

  /* Load saved settings */
  hsxpl_load_settings();

#ifdef CPFLIGHT
  if(hsxpl_cpflight_enabled) {
#ifdef HSXPLDEBUG
    hsxpl_log(HSXPLDEBUG_ACTION,"hsaircpf_open_serial_port()");
#endif
    hsaircpf_open_serial_port();
#ifdef HSXPLDEBUG
    hsxpl_log(HSXPLDEBUG_ACTION,"hsaircpf_initialise_hardware()");
#endif
    hsaircpf_initialise_hardware();
  }
#endif

  /* Setup AirEFB tcp server */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_tcp_start_server()");
#endif
  hsmp_tcp_start_server("0.0.0.0",HSMP_DYN_TCP_PORT_START);

  /* Register main loopback call */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPLMRegisterFlightLoopCallback()");
#endif
  XPLMRegisterFlightLoopCallback(hsxpl_runtime,-1,NULL);

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPLMRegisterFlightLoopCallback()");
#endif

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPluginStart() finishing");
#endif
  return 1;
}

PLUGIN_API void XPluginStop ( void ) {
  XPLMUnregisterFlightLoopCallback(hsxpl_runtime,NULL);

#ifdef CPFLIGHT
  if(hsxpl_cpflight_enabled) {
    hsaircpf_close_serial_port();
  }
#endif

}

PLUGIN_API int XPluginEnable ( void ) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPluginEnable()");
#endif

  /* Setup datarefs */
  hsxpl_set_datarefs();

  return 1;
}

PLUGIN_API void XPluginDisable ( void ) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPluginDisable()");
#endif

  hsxpl_set_datarefs();

}

#define XPLM_MSG_PLANE_CRASHED          101
#define XPLM_MSG_PLANE_LOADED           102
#define XPLM_MSG_AIRPORT_LOADED         103
#define XPLM_MSG_SCENERY_LOADED         104
#define XPLM_MSG_AIRPLANE_COUNT_CHANGED 105
#define XPLM_MSG_PLANE_UNLOADED         106
#define XPLM_MSG_WILL_WRITE_PREFS       107
#define XPLM_MSG_LIVERY_LOADED          108

PLUGIN_API void XPluginReceiveMessage ( XPLMPluginID inFrom, long inMessage,
                                       void * inParam ) {


#ifdef HSXPLDEBUG
/*  hsxpl_log(HSXPLDEBUG_ACTION,"XPluginReceiveMessage()"); */
#endif

  switch(inMessage) {

    case(XPLM_MSG_PLANE_CRASHED):
    case(XPLM_MSG_PLANE_LOADED):
    case(XPLM_MSG_AIRPLANE_COUNT_CHANGED):
    case(XPLM_MSG_PLANE_UNLOADED):
    case(XPLM_MSG_LIVERY_LOADED):

      hsxpl_set_datarefs();
      break;

  }

  /*
   if(inMessage<0x00FFFFFF) {
   hsxpl_set_datarefs();
   }
   */
}

#pragma mark Plugin Implementation

int32_t _hsxpl_runtime_initialised_ = 0;

/* hsxpl_runtime - This function is called every HSXPL_ENGINE_TICTAC interval
 * to process stuff under the plugin; it is the core engine that recurringly
 * sends data to apps such as AirTrack */
float hsxpl_runtime(float                inElapsedSinceLastCall,
                    float                inElapsedTimeSinceLastFlightLoop,
                    int                  inCounter,
                    void *               inRefcon)
{

#ifdef CPFLIGHT
  if(hsxpl_cpflight_enabled) {
    hsaircpf_runloop();
  }
#endif

  static int _hsxpl_runtime_two_tictacs_=1;
  static int _hsxpl_runtime_three_tictacs_=1;
  static int _hsxpl_runtime_second_tictacs_=1;

  /* Reset datarefs once again in the first run */
  if(_hsxpl_runtime_initialised_ == 0) {
    _hsxpl_runtime_initialised_ = 1;
#ifdef HSXPLDEBUG
    hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_runtime() initialise");
#endif
    hsxpl_set_datarefs();
  }

  /* Perform HSMP control tasks */
  hsmp_showtime();

  /* Send data reports */
  hsxpl_send_subsecond_data();

  /* Clean up stuff */
  if(hsairpl_apt_send_next_airport_bytes()<0) {
    hsairpl_apt_send_req_fail();
  }

  if(hsairpl_clist_send_next_list_bytes()<0) {
    hsairpl_clist_send_req_fail();
  }

  /* Send datarefs to those who requested them and clean up old expired peers */
  hsairpl_dref_showtime_tictac();

    /* One in every two cycles */
  _hsxpl_runtime_two_tictacs_--;
  if(_hsxpl_runtime_two_tictacs_<1) {
    _hsxpl_runtime_two_tictacs_=2;

    /* Release x737 FMC key if pressed */
    hsxpl_x737_release_key_pressed();

  }

  /* One in every three cycles */
  _hsxpl_runtime_three_tictacs_--;
  if(_hsxpl_runtime_three_tictacs_<1) {
    _hsxpl_runtime_three_tictacs_=3;
    hsxpl_send_fmc_data();

  }

  /* One in every second or roughly */
  _hsxpl_runtime_second_tictacs_--;
  if(_hsxpl_runtime_second_tictacs_<1) {
    _hsxpl_runtime_second_tictacs_=(int)((float)(1.0/HSXPL_ENGINE_TICTAC));

    hsxpl_n1_pressed=0;
    hsxpl_navdb_update_from_xplane();
    hsxpl_send_second_data();

    /* Send ATC traffic */
    hsairpl_atc_send_traffic();

    /* Send per second datarefs and cleanup old peers */
    hsairpl_mcp_second_timer();

    hsairpl_dref_showtime_sec();
  }

  /* Re-check ufmc because it sometimes takes a while for it to come up */

  int ufmc_present=0;
  if(XPLMFindDataRef("FJCC/UFMC/PRESENT")) if(XPLMGetDataf(XPLMFindDataRef("FJCC/UFMC/PRESENT"))!=0) ufmc_present=1;
  if(XPLMFindDataRef("FJCC/UFMC_1/PRESENT")) if(XPLMGetDataf(XPLMFindDataRef("FJCC/UFMC_1/PRESENT"))!=0) ufmc_present=1;

  if(ufmc_present && (hsxpl_fmc_type!=HSMP_FMC_TYPE_XP_X737 && hsxpl_fmc_type!=HSMP_FMC_TYPE_XP_UFMC && hsxpl_fmc_type!=HSMP_FMC_TYPE_XP_X737V5)) {
    hsxpl_set_datarefs();
  } /* else if(!ufmc_present && (hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737 || hsxpl_fmc_type==HSMP_FMC_TYPE_XP_UFMC || hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737V5)) {
    hsxpl_set_datarefs();
  }
   */


  return ((float)HSXPL_ENGINE_TICTAC);
}


/* The message receiving callback */
void hsxpl_hsmp_message_callback(uint32_t mid,void *data,struct sockaddr_in *from)
{

  /* char str[512]; */

  /* Only process AIR messages */
  if(!(mid & HSMP_MSG_CLASS_AIR)) return;

  uint32_t mgroup=mid&0x0000FF00;

  switch(mgroup) {

    case(HSMP_MSG_AGRP_ROUTE): {

      switch(mid) {

          /* Update current route leg */
        case(HSMP_MID_ROUTE_CLEG): {
          uint32_t *i=(uint32_t *)data;
          hsxpl_navdb_set_current_leg(*i);
          break;
        }

          /* Update current route number of points */
        case(HSMP_MID_ROUTE_NOPTS): {
          uint32_t *i=(uint32_t *)data;
          hsxpl_navdb_set_nopoints(*i);
          break;
        }

          /* Updates one given route point with a point message */
        case(HSMP_MID_ROUTE_POINT): {
          hsmp_route_pt_msg_t *mp=(hsmp_route_pt_msg_t *)data;
          hsxpl_navdb_set_route_point(mp->pindex,&mp->pt);
          break;
        }

        default: break;
      }
      break;
    }


    case(HSMP_MSG_AGRP_RC): {
      switch(mid) {
        case(HSMP_MID_RC_ROLL): {
          float *f=(float *)data;
          *f /= 90.0;
          if(*f <-1) *f=-1;
          else if(*f>1) *f=1;
          XPLMSetDataf(hsxpl_xplane_datarefs.joyroll,*f);
          break;
        }
        case(HSMP_MID_RC_PITCH): {
          float *f=(float *)data;
          *f /= 90.0;
          if(*f <-1) *f=-1;
          else if(*f>1) *f=1;
          XPLMSetDataf(hsxpl_xplane_datarefs.joypitch,*f);
          break;
        }
        case(HSMP_MID_RC_YAW): {
          float *f=(float *)data;
          *f /= 90.0;
          if(*f <-1) *f=-1;
          else if(*f>1) *f=1;
          XPLMSetDataf(hsxpl_xplane_datarefs.joyyaw,*f);
          break;
        }

        default: break;
      }
      break;
    }

    case(HSMP_MSG_AGRP_WXR): {
      switch(mid) {
        case(HSMP_MID_WXR_ALTM): {
          float *f=(float *)data;
          hsairpl_efis1_set_baro(*f);
          break;
        }
        default: break;
      }
      break;
    }
    case(HSMP_MSG_AGRP_FD): {
      switch(mid) {
        case(HSMP_MID_FD_MODE): {
          uint32_t *i=(uint32_t *)data;

          if(*i == 2) {
            hsairpl_mcp_set_fd1(1);
            hsairpl_mcp_set_fd2(1);
            if(!hsairpl_mcp_get_cmdb_led())
              hsairpl_mcp_press_cmdb();
            if(!hsairpl_mcp_get_cmda_led())
              hsairpl_mcp_press_cmda();
          }
          else if(*i == 1) {
            if(hsairpl_mcp_get_cmdb_led())
              hsairpl_mcp_press_cmdb();
            if(hsairpl_mcp_get_cmda_led())
              hsairpl_mcp_press_cmda();
            hsairpl_mcp_set_fd1(1);
            hsairpl_mcp_set_fd2(1);
          }
          else if (*i == 0) {
            if(hsairpl_mcp_get_cmdb_led())
              hsairpl_mcp_press_cmdb();
            if(hsairpl_mcp_get_cmda_led())
              hsairpl_mcp_press_cmda();
            hsairpl_mcp_set_fd1(0);
            hsairpl_mcp_set_fd2(0);
          }

          break;
        }
        default: break;
      }
      break;
    }

    case(HSMP_MSG_AGRP_RAD): {

      switch(mid) {
        case(HSMP_MID_RAD_STACK): {
          hsmp_radio_stack_t *rs=(hsmp_radio_stack_t *)data;
          hsairpl_coms_set_nav1(rs->nav1);
          hsairpl_coms_set_snav1(rs->nav1s);
          hsairpl_coms_set_nav2(rs->nav2);
          hsairpl_coms_set_snav2(rs->nav2s);
          hsairpl_coms_set_com1(rs->com1);
          hsairpl_coms_set_scom1(rs->com1s);
          hsairpl_coms_set_com2(rs->com2);
          hsairpl_coms_set_scom2(rs->com2s);
          hsairpl_coms_set_adf1(rs->adf1/100);
          hsairpl_coms_set_sadf1(rs->adf1s/100);
          hsairpl_coms_set_adf2(rs->adf2/100);
          hsairpl_coms_set_sadf2(rs->adf2s/100);
          break;
        }
        case(HSMP_MID_RAD_N1FREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_nav1(*i);
          break;
        }
        case(HSMP_MID_RAD_N1SFREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_snav1(*i);
          break;
        }
        case(HSMP_MID_RAD_N2FREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_nav2(*i);
          break;
        }
        case(HSMP_MID_RAD_N2SFREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_snav2(*i);
          break;
        }

        case(HSMP_MID_RAD_C1FREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_com1(*i);
          break;
        }
        case(HSMP_MID_RAD_C1SFREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_scom1(*i);
          break;
        }
        case(HSMP_MID_RAD_C2FREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_com2(*i);
          break;
        }
        case(HSMP_MID_RAD_C2SFREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_scom2(*i);
          break;
        }

        case(HSMP_MID_RAD_A1FREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_adf1(*i / 100);
          break;
        }
        case(HSMP_MID_RAD_A1SFREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_sadf1(*i / 100);
          break;
        }
        case(HSMP_MID_RAD_A2FREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_adf2(*i / 100);
          break;
        }
        case(HSMP_MID_RAD_A2SFREQ): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_sadf2(*i / 100);
          break;
        }
        case(HSMP_MID_RAD_XPDRCODE): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_xponder_code(*i);
          break;
        }
        case(HSMP_MID_RAD_XPDRMODE): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_coms_set_xponder_mode(*i);
          break;
        }

        default: break;
      }
      break;
    }

    case(HSMP_MSG_AGRP_MFD): {
      switch(mid) {
        case(HSMP_MID_MFD_MODE): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_efis1_set_mode(*i);
          break;
        }
        case(HSMP_MID_MFD_SHOW): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_efis1_set_show(*i);
          break;
        }
        case(HSMP_MID_MFD_RANGE): {
          float *f=(float *)data;
          hsairpl_efis1_set_range(*f);
          break;
        }
        case(HSMP_MID_MFD_N1SEL): {
          uint32_t *i=(uint32_t *)data;
          switch(*i) {
            case(0): hsairpl_efis1_set_vas1(1); break;
            case(1): hsairpl_efis1_set_vas1(2); break;
            case(2): hsairpl_efis1_set_vas1(0); break;
          }
          break;
        }
        case(HSMP_MID_MFD_N2SEL): {
          uint32_t *i=(uint32_t *)data;
          switch(*i) {
            case(0): hsairpl_efis1_set_vas2(1); break;
            case(1): hsairpl_efis1_set_vas2(2); break;
            case(2): hsairpl_efis1_set_vas2(0); break;
          }
          break;
        }
        default:break;
      } /* switch MID */
      break;
    } /* case MFD */


    case(HSMP_MSG_AGRP_AP): {
      switch(mid) {
        case(HSMP_MID_AP_SOURCE): {
          uint32_t *i=(uint32_t *)data;
          hsairpl_ap_set_source(*i);
          break;
        }

        case(HSMP_MID_AP_VSSSTATUS): {
          hsairpl_mcp_press_vs();
          break;
        }

        case(HSMP_MID_AP_LOCSTATUS): {
          hsairpl_mcp_press_loc();
          break;
        }

        case(HSMP_MID_AP_GSSTATUS): {
          hsairpl_mcp_press_app();
          break;
        }

        case(HSMP_MID_AP_VNAVSTATUS): {
          hsairpl_mcp_press_vnav();
          break;
        }
        case(HSMP_MID_AP_LNAVSTATUS): {
          hsairpl_mcp_press_lnav();
          break;
        }
        case(HSMP_MID_AP_LCSTATUS): {
          hsairpl_mcp_press_lchg();
          break;
        }
        case(HSMP_MID_AP_N1STATUS): {
          if(!hsxpl_n1_pressed){
            hsairpl_mcp_press_n1();
            hsxpl_n1_pressed=1;
          }
          break;
        }
        case(HSMP_MID_AP_SPDSTATUS): {
          uint32_t *i=(uint32_t *)data;
          if(*i==1) {
            hsairpl_mcp_set_atarm(1);
            if(hsairpl_mcp_get_spd_led()) {
              hsairpl_mcp_press_spd();
            }

          } else if(*i==2) {
            hsairpl_mcp_set_atarm(1);
            if(!hsairpl_mcp_get_spd_led()){
              hsairpl_mcp_press_spd();
            }
          } else {
            if(hsairpl_mcp_get_spd_led()) {
              hsairpl_mcp_press_spd();
            }
            hsairpl_mcp_set_atarm(0);
          }
          break;
        }

        case(HSMP_MID_AP_HDGSTATUS): {
          hsairpl_mcp_press_hdg();
          break;
        }
        case(HSMP_MID_AP_ALTSTATUS): {
          hsairpl_mcp_press_alt();
          break;
        }

        case(HSMP_MID_AP_SISMACH): {
          /* hsairpl_mcp_press_co(); */
          uint32_t *i=(uint32_t *)data;
          uint32_t x;if(*i) x=1; else x=0;
          hsairpl_mcp_set_speed_is_mach(x);
          break;
        }

        case(HSMP_MID_AP_OBS1DIAL): {
          float *f=(float *)data;
          hsairpl_mcp_set_crs1_dial(HSFTOI(*f));
          break;
        }
        case(HSMP_MID_AP_OBS2DIAL): {
          float *f=(float *)data;
          hsairpl_mcp_set_crs2_dial(HSFTOI(*f));
          break;
        }
        case(HSMP_MID_AP_HDGDIAL): {
          float *f=(float *)data;
          hsairpl_mcp_set_hdg_dial(HSFTOI(*f));
          break;
        }
        case(HSMP_MID_AP_SPDDIAL): {
          float *f=(float *)data;
          hsairpl_mcp_set_speed_dial(*f);
          break;
        }
        case(HSMP_MID_AP_ALTDIAL): {
          float *f=(float *)data;
          hsairpl_mcp_set_alt_dial(HSFTOI(*f));
          break;
        }
        case(HSMP_MID_AP_VSDIAL): {
          float *f=(float *)data;
          hsairpl_mcp_set_vvm_dial(HSFTOI(*f));
          break;
        }



        default: break;
      }
      break;
    }

    case(HSMP_MSG_AGRP_FMC): {

      if(hsxpl_fmc_type!=HSMP_FMC_TYPE_XPLANE || hsxpl_navdb_fmc_type()==1) {
        switch(mid) {
          case(HSMP_MID_FMC_BUT_L01): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lk1);
            break;
          }
          case(HSMP_MID_FMC_BUT_L02): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lk2);
            break;
          }
          case(HSMP_MID_FMC_BUT_L03): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lk3);
            break;
          }
          case(HSMP_MID_FMC_BUT_L04): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lk4);
            break;
          }
          case(HSMP_MID_FMC_BUT_L05): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lk5);
            break;
          }
          case(HSMP_MID_FMC_BUT_L06): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lk6);
            break;
          }
          case(HSMP_MID_FMC_BUT_R01): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rk1);
            break;
          }
          case(HSMP_MID_FMC_BUT_R02): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rk2);
            break;
          }
          case(HSMP_MID_FMC_BUT_R03): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rk3);
            break;
          }
          case(HSMP_MID_FMC_BUT_R04): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rk4);
            break;
          }
          case(HSMP_MID_FMC_BUT_R05): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rk5);
            break;
          }
          case(HSMP_MID_FMC_BUT_R06): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rk6);
            break;
          }

          case(HSMP_MID_FMC_BUT_D1): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_1);
            break;
          }
          case(HSMP_MID_FMC_BUT_D2): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_2);
            break;
          }
          case(HSMP_MID_FMC_BUT_D3): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_3);
            break;
          }
          case(HSMP_MID_FMC_BUT_D4): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_4);
            break;
          }
          case(HSMP_MID_FMC_BUT_D5): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_5);
            break;
          }
          case(HSMP_MID_FMC_BUT_D6): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_6);
            break;
          }
          case(HSMP_MID_FMC_BUT_D7): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_7);
            break;
          }
          case(HSMP_MID_FMC_BUT_D8): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_8);
            break;
          }
          case(HSMP_MID_FMC_BUT_D9): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_9);
            break;
          }
          case(HSMP_MID_FMC_BUT_D0): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_0);
            break;
          }
          case(HSMP_MID_FMC_BUT_DOT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_dot);
            break;
          }
          case(HSMP_MID_FMC_BUT_PLMN): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_plusminus);
            break;
          }


            /* Alphabet */

          case(HSMP_MID_FMC_BUT_A): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_a);
            break;
          }
          case(HSMP_MID_FMC_BUT_B): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_b);
            break;
          }
          case(HSMP_MID_FMC_BUT_C): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_c);
            break;
          }
          case(HSMP_MID_FMC_BUT_D): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_d);
            break;
          }
          case(HSMP_MID_FMC_BUT_E): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_e);
            break;
          }
          case(HSMP_MID_FMC_BUT_F): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_f);
            break;
          }
          case(HSMP_MID_FMC_BUT_G): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_g);
            break;
          }
          case(HSMP_MID_FMC_BUT_H): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_h);
            break;
          }
          case(HSMP_MID_FMC_BUT_I): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_i);
            break;
          }
          case(HSMP_MID_FMC_BUT_J): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_j);
            break;
          }
          case(HSMP_MID_FMC_BUT_K): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_k);
            break;
          }
          case(HSMP_MID_FMC_BUT_L): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_l);
            break;
          }
          case(HSMP_MID_FMC_BUT_M): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_m);
            break;
          }
          case(HSMP_MID_FMC_BUT_N): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_n);
            break;
          }
          case(HSMP_MID_FMC_BUT_O): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_o);
            break;
          }
          case(HSMP_MID_FMC_BUT_P): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_p);
            break;
          }
          case(HSMP_MID_FMC_BUT_Q): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_q);
            break;
          }
          case(HSMP_MID_FMC_BUT_R): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_r);
            break;
          }
          case(HSMP_MID_FMC_BUT_S): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_s);
            break;
          }
          case(HSMP_MID_FMC_BUT_T): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_t);
            break;
          }
          case(HSMP_MID_FMC_BUT_U): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_u);
            break;
          }
          case(HSMP_MID_FMC_BUT_V): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_v);
            break;
          }
          case(HSMP_MID_FMC_BUT_W): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_w);
            break;
          }
          case(HSMP_MID_FMC_BUT_X): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_x);
            break;
          }
          case(HSMP_MID_FMC_BUT_Y): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_y);
            break;
          }
          case(HSMP_MID_FMC_BUT_Z): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_z);
            break;
          }
          case(HSMP_MID_FMC_BUT_SPC): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_space);
            break;
          }
          case(HSMP_MID_FMC_BUT_DEL): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_del);
            break;
          }
          case(HSMP_MID_FMC_BUT_SLASH): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_slash);
            break;
          }
          case(HSMP_MID_FMC_BUT_CLR): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_clr);
            break;
          }
          case(HSMP_MID_FMC_BUT_OVFY): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_ovfy);
            break;
          }


            /* Control keys */
          case(HSMP_MID_FMC_BUT_AIRPT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_airp);
            break;
          }
          case(HSMP_MID_FMC_BUT_DATA): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_data);
            break;
          }
          case(HSMP_MID_FMC_BUT_BLANK): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_blank);
            break;
          }
          case(HSMP_MID_FMC_BUT_PERF): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_perf);
            break;
          }
          case(HSMP_MID_FMC_BUT_FUELP): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_fuel);
            break;
          }
          case(HSMP_MID_FMC_BUT_RADNAV): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_radnav);
            break;
          }
          case(HSMP_MID_FMC_BUT_DIR): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_dir);
            break;
          }
          case(HSMP_MID_FMC_BUT_FPLN): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_fpln);
            break;
          }
          case(HSMP_MID_FMC_BUT_UP): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_up);
            break;
          }
          case(HSMP_MID_FMC_BUT_DOWN): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_down);
            break;
          }
          case(HSMP_MID_FMC_BUT_RIGHT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_right);
            break;
          }
          case(HSMP_MID_FMC_BUT_LEFT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_left);
            break;
          }

          case(HSMP_MID_FMC_BUT_INIT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_init);
            break;
          }
          case(HSMP_MID_FMC_BUT_MENU): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_menu);
            break;
          }
          case(HSMP_MID_FMC_BUT_N1LIM): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_n1limit);
            break;
          }
          case(HSMP_MID_FMC_BUT_PREVP): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_prevpage);
            break;
          }
          case(HSMP_MID_FMC_BUT_RTE): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_rte);
            break;
          }
          case(HSMP_MID_FMC_BUT_LEGS): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_legs);
            break;
          }
          case(HSMP_MID_FMC_BUT_FIX): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_fix);
            break;
          }
          case(HSMP_MID_FMC_BUT_NEXTP): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_nextpage);
            break;
          }
          case(HSMP_MID_FMC_BUT_CLB): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_clb);
            break;
          }

          case(HSMP_MID_FMC_BUT_DEPARR): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_deparr);
            break;
          }

          case(HSMP_MID_FMC_BUT_CRZ): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_crz);
            break;
          }
          case(HSMP_MID_FMC_BUT_HOLD): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_hold);
            break;
          }
          case(HSMP_MID_FMC_BUT_DES): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_des);
            break;
          }
          case(HSMP_MID_FMC_BUT_PROG): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_prog);
            break;
          }
          case(HSMP_MID_FMC_BUT_EXEC): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_exec);
            break;
          }

            /* X-Plane specific */
          case(HSMP_MID_FMC_BUT_LOAD): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_load);
            break;
          }
          case(HSMP_MID_FMC_BUT_SAVE): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_save);
            break;
          }
          case(HSMP_MID_FMC_BUT_APT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_apt);
            break;
          }
          case(HSMP_MID_FMC_BUT_VOR): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_vor);
            break;
          }
          case(HSMP_MID_FMC_BUT_NDB): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_ndb);
            break;
          }
          case(HSMP_MID_FMC_BUT_WPT): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_wpt);
            break;
          }
          case(HSMP_MID_FMC_BUT_LATLON): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_latlon);
            break;
          }
          case(HSMP_MID_FMC_BUT_INDEX): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_index);
            break;
          }
          case(HSMP_MID_FMC_BUT_RADIO): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_radio);
            break;
          }
          case(HSMP_MID_FMC_BUT_MFDMENU): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_mfdmenu);
            break;
          }
          case(HSMP_MID_FMC_BUT_MSG): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_msg);
            break;
          }
          case(HSMP_MID_FMC_BUT_LNAV): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_lnav);
            break;
          }
          case(HSMP_MID_FMC_BUT_VNAV): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_vnav);
            break;
          }
          case(HSMP_MID_FMC_BUT_AP): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_ap);
            break;
          }
          case(HSMP_MID_FMC_BUT_ATC): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_atc);
            break;
          }
          case(HSMP_MID_FMC_BUT_FMCCOM): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_fmccom);
            break;
          }
          case(HSMP_MID_FMC_BUT_ALTN): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_altn);
            break;
          }
          case(HSMP_MID_FMC_BUT_SFPLN): {
            hsxpl_fmc_press_key(mid,hsxpl_fmc.key_sfpln);
            break;
          }
          case(HSMP_MID_FMC_MCDU_TOGGLE): {
            hsxpl_mcdu_toggle();
            break;
          }

          default: break;
        }
      }

      hsxpl_send_fmc_data();
      break;

    } /* End FMC group */

    case(HSMP_MSG_AGRP_APT): {
      switch(mid) {
        case(HSMP_MID_APT_LIST_REQ): {
          hsairpl_apt_send_indexes_to(from);
          break;
        }
        case(HSMP_MID_APT_REQDL): {
          char *airportid=(char *)data;
          hsairpl_apt_send_airport_to(airportid,from);
          break;
        }
        default:
          break;
      }
      break;
    }

    case(HSMP_MSG_AGRP_CLIST): {
      switch(mid) {
        case(HSMP_MID_CLIST_LIST_REQ): {
          hsairpl_clist_send_indexes_to(from);
          break;
        }
        case(HSMP_MID_CLIST_REQDL): {
          char *listid=(char *)data;
          hsairpl_clist_send_list_to(listid,from);
          break;
        }
        default:
          break;
      }
      break;
    }

    case(HSMP_MSG_AGRP_DREF): {
      if((mid & 0xFF000000) == HSMP_MID_DREF_CLASSTYPE) {
        hsairpl_dref_process_message(mid,data,from);
      }
      break;
    }

    default: break;
  } /* End switch mgroup */
}


/* Send a packet with instant data several times per second */
void hsxpl_send_subsecond_data(void) {

  /* Avoid building packet if we don't have anyone to send it to */
  if(hsmp_number_of_stream_clients()<1) return;

  /* AirTrack */
  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();

  /* AirEFB */
  hsmp_pkt_t *pkt2=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL && pkt2!=NULL) {

    /* POS */
    if(hsxpl_xplane_datarefs.latitude!=NULL) {
      double d=XPLMGetDatad(hsxpl_xplane_datarefs.latitude);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_LAT,&d);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_LAT,&d);
    }
    if(hsxpl_xplane_datarefs.longitude!=NULL) {
      double d=XPLMGetDatad(hsxpl_xplane_datarefs.longitude);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_LON,&d);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_LON,&d);
    }
    if(hsxpl_xplane_datarefs.elevation!=NULL) {
      /*
       double d=(double)XPLMGetDataf(hsxpl_xplane_datarefs.elevation) / 3.2808399;
       */
      double d=(double)XPLMGetDataf(hsxpl_xplane_datarefs.elevation) / 3.2808399;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_ELEV,&d);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_ELEV,&d);
    }
    if(hsxpl_xplane_datarefs.theta!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.theta);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_THETA,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_THETA,&f);
    }
    if(hsxpl_xplane_datarefs.psi!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.psi);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_PSI,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_PSI,&f);
    }
    if(hsxpl_xplane_datarefs.phi!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.phi);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_PHI,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_PHI,&f);
    }
    if(hsxpl_xplane_datarefs.mvar!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.mvar);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_MVAR,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_MVAR,&f);
    }

    if(hsxpl_xplane_datarefs.theta!=NULL && hsxpl_xplane_datarefs.vpath!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.theta) - XPLMGetDataf(hsxpl_xplane_datarefs.vpath);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_ALPHA,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_ALPHA,&f);
    }

    if(hsxpl_xplane_datarefs.gs!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.gs);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_GS,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_GS,&f);
      if(f>2.0 && hsxpl_xplane_datarefs.hpath!=NULL && hsxpl_xplane_datarefs.psi!=NULL) {
        f=XPLMGetDataf(hsxpl_xplane_datarefs.psi) - XPLMGetDataf(hsxpl_xplane_datarefs.hpath);
      } else {
        f=0;
      }
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_BETA,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_BETA,&f);
    }
    if(hsxpl_xplane_datarefs.ias!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.ias);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_IAS,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_IAS,&f);
    }
    if(hsxpl_xplane_datarefs.tas!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.tas);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_TAS,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_TAS,&f);
    }
    if(hsxpl_xplane_datarefs.mach!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.mach);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_MACH,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_MACH,&f);
    }
    if(hsxpl_xplane_datarefs.vvi!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.vvi);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_VVI,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_VVI,&f);
    }
    if(hsxpl_xplane_datarefs.msl!=NULL) {
      float f=(float)XPLMGetDatad(hsxpl_xplane_datarefs.msl);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_MSL,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_MSL,&f);
    }
    if(hsxpl_xplane_datarefs.agl!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.agl);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_AGL,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_AGL,&f);
    }

    /* WXR */
    float f=hsairpl_efis1_get_baro();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_ALTM,&f);
    hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_ALTM,&f);

    if(hsxpl_xplane_datarefs.winddir!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.winddir);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_WDIR,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_WDIR,&f);
    }
    if(hsxpl_xplane_datarefs.windspeed!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.windspeed);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_WSPEED,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_WSPEED,&f);
    }
    if(hsxpl_xplane_datarefs.outsidetemp!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.outsidetemp);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_OTEMP,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_OTEMP,&f);
    }

    /* BFLG */
    if(hsxpl_xplane_datarefs.lgear!=NULL) {
      uint32_t i=XPLMGetDatai(hsxpl_xplane_datarefs.lgear);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_LGEAR,&i);
    }
    if(hsxpl_xplane_datarefs.pbreak!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.pbreak);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_PBRK,&f);
    }
    if(hsxpl_xplane_datarefs.flaps!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.flaps);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_FLAPS,&f);
    }
    if(hsxpl_xplane_datarefs.spdbreak!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.spdbreak);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_SPDBRK,&f);
    }


    /* RADIO */
    hsmp_radio_stack_t rs; memset(&rs,0,sizeof(hsmp_radio_stack_t));
    rs.nav1=hsairpl_coms_get_nav1();
    rs.nav1s=hsairpl_coms_get_snav1();
    rs.nav2=hsairpl_coms_get_nav2();
    rs.nav2s=hsairpl_coms_get_snav2();
    rs.com1=hsairpl_coms_get_com1();
    rs.com1s=hsairpl_coms_get_scom1();
    rs.com2=hsairpl_coms_get_com2();
    rs.com2s=hsairpl_coms_get_scom2();
    rs.adf1=hsairpl_coms_get_adf1()*100;
    rs.adf1s=hsairpl_coms_get_sadf1()*100;
    rs.adf2=hsairpl_coms_get_adf2()*100;
    rs.adf2s=hsairpl_coms_get_sadf2()*100;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_STACK,&rs);

    uint32_t u=hsairpl_coms_get_xponder_code();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_XPDRCODE,&u);


    u=hsairpl_coms_get_xponder_mode();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_XPDRMODE,&u);


    /* FD */
    f=hsairpl_ap_get_fdpitch();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FD_PITCH,&f);

    f=hsairpl_ap_get_fdroll();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FD_ROLL,&f);


    u=hsairpl_mcp_get_fd1_mode();
    if(hsairpl_mcp_get_cmda_led() || hsairpl_mcp_get_cmdb_led()) {
      u=2;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FD_MODE,&u);

    /* MFD */
    u=hsairpl_efis1_get_show();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_MFD_SHOW,&u);

    u=hsairpl_efis1_get_mode();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_MFD_MODE,&u);

    u=hsairpl_efis1_get_vas1();
    switch(u) {
      case(0): u=2; break;
      case(1): u=0; break;
      case(2): u=1; break;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_MFD_N1SEL,&u);

    u=hsairpl_efis1_get_vas2();
    switch(u) {
      case(0): u=2; break;
      case(1): u=0; break;
      case(2): u=1; break;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_MFD_N2SEL,&u);

    f=hsairpl_efis1_get_range();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_MFD_RANGE,&f);

    /* AP */

    f=HSITOF(hsairpl_mcp_get_alt_dial());
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_ALTDIAL,&f);

    f=hsairpl_mcp_get_speed_dial();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_SPDDIAL,&f);

    f=HSITOF(hsairpl_mcp_get_hdg_dial());
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_HDGDIAL,&f);

    f=HSITOF(hsairpl_mcp_get_vvm_dial());
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_VSDIAL,&f);

    u=hsairpl_mcp_get_speed_is_mach();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_SISMACH,&u);


    f=HSITOF(hsairpl_mcp_get_crs1_dial());
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_OBS1DIAL,&f);

    f=HSITOF(hsairpl_mcp_get_crs2_dial());
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_OBS2DIAL,&f);

    u=HSITOF(hsairpl_ap_get_source());
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_SOURCE,&u);

    u=hsairpl_ap_speed_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_SPDSTATUS,&u);

    u=hsairpl_ap_vnav_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_VNAVSTATUS,&u);

    u=hsairpl_ap_hdg_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_HDGSTATUS,&u);

    u=hsairpl_ap_vs_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_VSSSTATUS,&u);

    u=hsairpl_ap_alt_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_ALTSTATUS,&u);

    u=hsairpl_ap_lnav_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_LNAVSTATUS,&u);

    u=hsairpl_ap_loc_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_LOCSTATUS,&u);

    u=hsairpl_ap_gs_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_GSSTATUS,&u);

    u=hsairpl_ap_lc_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_LCSTATUS,&u);

    u=hsairpl_ap_n1_status();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AP_N1STATUS,&u);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRTRACK);
    hsmp_net_send_to_stream_peers(pkt2,HSMP_PKT_NT_AIREFB);
    if(pkt!=NULL) free(pkt);
    if(pkt2!=NULL) free(pkt2);
  }
}

/* Send a packet with instant data several times per second */
void hsxpl_send_second_data(void) {

  /* Avoid building packets if we don't have anyone to send them to */
  if(hsmp_number_of_stream_clients()<1) return;

  /* Or do it ... */
  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {


    /* STATIC ACF DATA */
    char acfstr[8];memset(acfstr,0,8);
    if(hsxpl_xplane_datarefs.tailno!=NULL) {
      XPLMGetDatab(hsxpl_xplane_datarefs.tailno,acfstr,0,7);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STAT_TAILNO,acfstr);
    }
    memset(acfstr,0,8);
    if(hsxpl_xplane_datarefs.acficao!=NULL) {
      XPLMGetDatab(hsxpl_xplane_datarefs.acficao,acfstr,0,7);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STAT_ICAO,acfstr);
    }
    memset(acfstr,0,8);
    if(hsxpl_xplane_datarefs.callsign!=NULL) {
      XPLMGetDatab(hsxpl_xplane_datarefs.callsign,acfstr,0,7);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STAT_CALLSGN,acfstr);
    }

    /* VSPEEDS */
    if(hsxpl_xplane_datarefs.vso!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.vso);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VS0,&f);
    }
    if(hsxpl_xplane_datarefs.vs!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.vs);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VS,&f);
    }
    if(hsxpl_xplane_datarefs.vfe!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.vfe);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VFE,&f);
    }
    if(hsxpl_xplane_datarefs.vno!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.vno);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VNO,&f);
    }
    if(hsxpl_xplane_datarefs.vne!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.vne);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VNE,&f);
    }
    if(hsxpl_xplane_datarefs.mmo!=NULL) {
      float f=XPLMGetDataf(hsxpl_xplane_datarefs.mmo);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_MMO,&f);
    }

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRTRACK);

    free(pkt);
  }


  /* Now send route */
  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t i;
    uint32_t points_in_packet=0;
    uint32_t nopoints=hsxpl_navdb_number_of_route_points();
    int32_t cleg=hsxpl_current_leg();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_ROUTE_NOPTS,&nopoints);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_ROUTE_CLEG,&(cleg));

    for(i=0;i<nopoints;i++) {
      hsmp_route_pt_t *rp=hsxpl_route_point_at_index(i);
      if(rp != NULL) {
        hsmp_route_pt_msg_t msg;
        msg.pindex=i;
        memcpy(&msg.pt,rp,sizeof(hsmp_route_pt_t));
        if(pkt==NULL) { pkt=(hsmp_pkt_t *)hsmp_net_make_packet(); }
        if(pkt==NULL) break;
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_ROUTE_POINT,&(msg));
        points_in_packet++;
        if(points_in_packet>30) {
          points_in_packet=0;
          hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRTRACK);
          free(pkt);
          pkt=NULL;
        }
      }
    }
    if(pkt!=NULL) {
      hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRTRACK);
      free(pkt);
    }
  }
}

#pragma mark Dataref sets

void hsxpl_set_datarefs(void) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_set_datarefs()");
#endif

  XPLMEnableFeature("XPLM_USE_NATIVE_PATHS",1);

  hsxpl_fmc_type=HSMP_FMC_TYPE_XPLANE;
  hsxpl_plane_type=HSXPL_PLANE_XPLANE;

  hsxpl_set_xsb_datarefs();
  hsxpl_set_xplane_datarefs();

  hsxpl_set_xfmc_datarefs();

  hsxpl_set_x737_datarefs();
  hsxpl_set_ufmc_datarefs();
  hsxpl_set_ix733_datarefs();
  hsxpl_set_default_738_datarefs();
  hsxpl_set_z738_datarefs();

  hsxpl_set_pcrj200_datarefs();
  hsxpl_set_p777_datarefs();
  hsxpl_set_p757_datarefs();
  hsxpl_set_p767_datarefs();
  hsxpl_set_a320q_datarefs();
  hsxpl_set_a320n_datarefs();

  /* If X-Plane, re-read datarefs for FMC since they will have been overwritten */
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XPLANE)
    hsxpl_set_xplane_datarefs();

  hsairpl_mcp_update_datarefs();
  hsairpl_coms_update_datarefs();
  hsairpl_atc_update_datarefs();

  hsairpl_apt_read_references();
  hsairpl_clist_read_references(NULL);

  hsxpl_navdb_reset_fmc_type();
}

