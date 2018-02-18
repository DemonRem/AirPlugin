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

/**** WIDGET RELATED PROTOTYPES AND GLOBAL VARS *****/
XPLMMenuID hsxpl_main_menu=NULL;
int hsxpl_settings_menu_item;
XPLMMenuID hsxpl_settingsmenu=NULL;
XPWidgetID hsxpl_settings_widget=NULL;
XPWidgetID hsxpl_ipsubwindow=NULL;
XPWidgetID hsxpl_iplabel=NULL;
XPWidgetID hsxpl_iptxinput=NULL;
XPWidgetID hsxpl_iplabel2=NULL;
XPWidgetID hsxpl_iplabel3=NULL;
XPWidgetID hsxpl_iplabel4=NULL;
XPWidgetID hsxpl_iplabel5=NULL;
XPWidgetID hsxpl_ipapply=NULL;

#pragma mark Global Variables

struct sockaddr_in hsxpl_unicast_sa;

/* FMC type, as in HSMP_FMC_TYPE_* */
uint32_t hsxpl_fmc_type=HSMP_FMC_TYPE_XPLANE;

uint32_t hsxpl_plane_type=HSXPL_PLANE_XPLANE;


int32_t hsxpl_local_ap_source=-1;

uint32_t hsxpl_idver=0;

uint32_t hsxpl_active_mcdu=1;

uint32_t hsxpl_cpflight_enabled=0;

/* Local hack to fix AirTrack N1 press twice bug until it's fixed */
uint32_t hsxpl_n1_pressed=0;

/* FMC datarefs */
extern hsxpl_fmc_t hsxpl_fmc;

/* The list of X-Plane datarefs */
struct xplane_datarefs_s {

  /* STATIC DATA */
  XPLMDataRef tailno;
  XPLMDataRef acficao;
  XPLMDataRef callsign;

  /* IPOS */
  XPLMDataRef latitude;
  XPLMDataRef longitude;
  XPLMDataRef elevation;
  XPLMDataRef theta;
  XPLMDataRef phi;
  XPLMDataRef psi;
  XPLMDataRef mvar;
  XPLMDataRef vpath;
  XPLMDataRef hpath;
  XPLMDataRef gs;
  XPLMDataRef ias;
  XPLMDataRef tas;
  XPLMDataRef vvi;
  XPLMDataRef mach;
  XPLMDataRef msl;
  XPLMDataRef agl;

  /* WXR */
  XPLMDataRef outsidetemp;
  XPLMDataRef winddir;
  XPLMDataRef windspeed;

  /* BFLG */
  XPLMDataRef pbreak;
  XPLMDataRef flaps;
  XPLMDataRef spdbreak;
  XPLMDataRef lgear;

  /* VSPEEDS */
  XPLMDataRef vso;
  XPLMDataRef vs;
  XPLMDataRef vfe;
  XPLMDataRef vno;
  XPLMDataRef vne;
  XPLMDataRef mmo;

  /* JOYSTICK */
  XPLMDataRef joyroll;
  XPLMDataRef joypitch;
  XPLMDataRef joyyaw;

} xpdatarefs;

/* The x737 set of functions are meant to overcome some dataref
 * differences between the normal aircraft and the EADT x737 project
 * Boeing 737-NG. This is only relevant for X-Plane so if you try to
 * port this code to another flight simulator you may forget these  */
struct x737_datarefs_s {

  XPLMDataRef pluginstatus;

  /* For xFMC and x737 V5 */
  XPLMDataRef key_down;

} x7datarefs;

struct ix733_datarefs_s {
  XPLMDataRef     title;
  XPLMDataRef     scratchPad;
  XPLMDataRef     pageNo;
  XPLMDataRef     llines[12];
  XPLMDataRef     rlines[12];
} ix733_datarefs;




/* Data refs for X737FMC */
struct ufmc_datarefs_s {

  XPLMDataRef         line[14];

} ufmcdatarefs;

/* Data refs for XFMC */
struct xfmc_datarefs_s {

  XPLMDataRef         line[14];
  XPLMDataRef         status;

} xfmcdatarefs;

struct a320ndatarefs_s {

  XPLMDataRef     label1;
  XPLMDataRef     label2;
  XPLMDataRef     label3;
  XPLMDataRef     label4;
  XPLMDataRef     label5;
  XPLMDataRef     label6;

  XPLMDataRef     line1a;
  XPLMDataRef     line1b;
  XPLMDataRef     line1g;
  XPLMDataRef     line1m;
  XPLMDataRef     line1w;
  XPLMDataRef     line1y;

  XPLMDataRef     line2a;
  XPLMDataRef     line2b;
  XPLMDataRef     line2g;
  XPLMDataRef     line2m;
  XPLMDataRef     line2w;
  XPLMDataRef     line2y;

  XPLMDataRef     line3a;
  XPLMDataRef     line3b;
  XPLMDataRef     line3g;
  XPLMDataRef     line3m;
  XPLMDataRef     line3w;
  XPLMDataRef     line3y;

  XPLMDataRef     line4a;
  XPLMDataRef     line4b;
  XPLMDataRef     line4g;
  XPLMDataRef     line4m;
  XPLMDataRef     line4w;
  XPLMDataRef     line4y;

  XPLMDataRef     line5a;
  XPLMDataRef     line5b;
  XPLMDataRef     line5g;
  XPLMDataRef     line5m;
  XPLMDataRef     line5w;
  XPLMDataRef     line5y;

  XPLMDataRef     line6a;
  XPLMDataRef     line6b;
  XPLMDataRef     line6g;
  XPLMDataRef     line6m;
  XPLMDataRef     line6w;
  XPLMDataRef     line6y;

  XPLMDataRef     scratchpad;
  XPLMDataRef     scratchpad_a;

  XPLMDataRef     title_g;
  XPLMDataRef     title_w;
  XPLMDataRef     title_y;


} a320ndatarefs;

struct a320qdatarefs_s {

  XPLMDataRef     title[7];
  XPLMDataRef     stitle[6];
  XPLMDataRef     label[6][7];
  XPLMDataRef     content_large[6][7];
  XPLMDataRef     content_small[6][7];
  XPLMDataRef     scratchpad[6];

} a320qdatarefs;


/* XSquawkbox datarefs */
struct xsb_datarefs_s {

  XPLMDataRef callsign;

} xsbdatarefs;


#pragma mark Dataref retrievers

char *hsxpl_acf_icao(void) {

  static char acficao[32]; memset(acficao,0,8);
  if(xpdatarefs.acficao!=NULL)
    XPLMGetDatab(xpdatarefs.acficao,acficao,0,31);
  return acficao;
}

char *hsxpl_acf_tailno(void) {
  static char actfailno[8]; memset(actfailno,0,8);
  if(xpdatarefs.tailno!=NULL)
    XPLMGetDatab(xpdatarefs.tailno,actfailno,0,31);
  return actfailno;
}

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
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPLMCreateMenu()");
#endif
  hsxpl_main_menu=XPLMCreateMenu("Haversine Air",NULL,0,hsxpl_select_menu_option,0);
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPLMAppendMenuItem()");
#endif
  hsxpl_settings_menu_item=XPLMAppendMenuItem(hsxpl_main_menu,"Settings",(void *)"Settings",1);

  /* Load saved settings */
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_load_settings()");
#endif
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
    if(x7datarefs.key_down!=NULL) {
      XPLMSetDataf(x7datarefs.key_down,0);
      x7datarefs.key_down=NULL;

    }
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
    hsairpl_dref_showtime_sec();

    hsairpl_mcp_second_timer();
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
          XPLMSetDataf(xpdatarefs.joyroll,*f);
          break;
        }
        case(HSMP_MID_RC_PITCH): {
          float *f=(float *)data;
          *f /= 90.0;
          if(*f <-1) *f=-1;
          else if(*f>1) *f=1;
          XPLMSetDataf(xpdatarefs.joypitch,*f);
          break;
        }
        case(HSMP_MID_RC_YAW): {
          float *f=(float *)data;
          *f /= 90.0;
          if(*f <-1) *f=-1;
          else if(*f>1) *f=1;
          XPLMSetDataf(xpdatarefs.joyyaw,*f);
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
    if(xpdatarefs.latitude!=NULL) {
      double d=XPLMGetDatad(xpdatarefs.latitude);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_LAT,&d);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_LAT,&d);
    }
    if(xpdatarefs.longitude!=NULL) {
      double d=XPLMGetDatad(xpdatarefs.longitude);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_LON,&d);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_LON,&d);
    }
    if(xpdatarefs.elevation!=NULL) {
      /*
       double d=(double)XPLMGetDataf(xpdatarefs.elevation) / 3.2808399;
       */
      double d=(double)XPLMGetDataf(xpdatarefs.elevation) / 3.2808399;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_ELEV,&d);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_ELEV,&d);
    }
    if(xpdatarefs.theta!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.theta);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_THETA,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_THETA,&f);
    }
    if(xpdatarefs.psi!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.psi);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_PSI,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_PSI,&f);
    }
    if(xpdatarefs.phi!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.phi);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_PHI,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_PHI,&f);
    }
    if(xpdatarefs.mvar!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.mvar);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_MVAR,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_MVAR,&f);
    }

    if(xpdatarefs.theta!=NULL && xpdatarefs.vpath!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.theta) - XPLMGetDataf(xpdatarefs.vpath);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_ALPHA,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_ALPHA,&f);
    }

    if(xpdatarefs.gs!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.gs);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_GS,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_GS,&f);
      if(f>2.0 && xpdatarefs.hpath!=NULL && xpdatarefs.psi!=NULL) {
        f=XPLMGetDataf(xpdatarefs.psi) - XPLMGetDataf(xpdatarefs.hpath);
      } else {
        f=0;
      }
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_BETA,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_BETA,&f);
    }
    if(xpdatarefs.ias!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.ias);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_IAS,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_IAS,&f);
    }
    if(xpdatarefs.tas!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.tas);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_TAS,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_TAS,&f);
    }
    if(xpdatarefs.mach!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.mach);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_MACH,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_MACH,&f);
    }
    if(xpdatarefs.vvi!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.vvi);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_VVI,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_VVI,&f);
    }
    if(xpdatarefs.msl!=NULL) {
      float f=(float)XPLMGetDatad(xpdatarefs.msl);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_MSL,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_MSL,&f);
    }
    if(xpdatarefs.agl!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.agl);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_IPOS_AGL,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_IPOS_AGL,&f);
    }

    /* WXR */
    float f=hsairpl_efis1_get_baro();
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_ALTM,&f);
    hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_ALTM,&f);

    if(xpdatarefs.winddir!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.winddir);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_WDIR,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_WDIR,&f);
    }
    if(xpdatarefs.windspeed!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.windspeed);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_WSPEED,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_WSPEED,&f);
    }
    if(xpdatarefs.outsidetemp!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.outsidetemp);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_WXR_OTEMP,&f);
      hsmp_net_add_msg_to_pkt(pkt2,HSMP_MID_WXR_OTEMP,&f);
    }

    /* BFLG */
    if(xpdatarefs.lgear!=NULL) {
      uint32_t i=XPLMGetDatai(xpdatarefs.lgear);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_LGEAR,&i);
    }
    if(xpdatarefs.pbreak!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.pbreak);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_PBRK,&f);
    }
    if(xpdatarefs.flaps!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.flaps);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_BFLG_FLAPS,&f);
    }
    if(xpdatarefs.spdbreak!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.spdbreak);
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

    /* Alternative method, based on individual messages, we send a stack instead a single message
     if(xpdatarefs.nav1freq!=NULL) {
     uint32_t i=hsairpl_coms_get_nav1()
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_N1FREQ,&i);
     }
     if(xpdatarefs.nav2freq!=NULL) {
     uint32_t i=hsairpl_coms_get_nav2();
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_N2FREQ,&i);
     }
     if(xpdatarefs.com1freq!=NULL) {
     uint32_t i=hsairpl_coms_get_com1();
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_C1FREQ,&i);
     }
     if(xpdatarefs.com2freq!=NULL) {
     uint32_t i=hsairpl_coms_get_com2();
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_C2FREQ,&i);
     }
     if(xpdatarefs.adf1freq!=NULL) {
     uint32_t i=hsairpl_coms_get_adf1()*100;
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_A1FREQ,&i);
     }
     if(xpdatarefs.adf2freq!=NULL) {
     uint32_t i=hsairpl_coms_get_adf2()*100;
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_A2FREQ,&i);
     }
     if(xpdatarefs.nav1sfreq!=NULL) {
     uint32_t i=hsairpl_coms_get_snav1()
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_N1SFREQ,&i);
     }
     if(xpdatarefs.nav2sfreq!=NULL) {
     uint32_t i=hsairpl_coms_get_snav2();
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_N2SFREQ,&i);
     }
     if(xpdatarefs.com1sfreq!=NULL) {
     uint32_t i=hsairpl_coms_get_scom1()
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_C1SFREQ,&i);
     }
     if(xpdatarefs.com2sfreq!=NULL) {
     uint32_t i=hsairpl_coms_get_scom2();
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_C2SFREQ,&i);
     }
     if(xpdatarefs.adf1sfreq!=NULL) {
     uint32_t i=hsairpl_coms_get_sadf1()*100;
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_A1SFREQ,&i);
     }
     if(xpdatarefs.adf2sfreq!=NULL) {
     uint32_t i=hsairpl_coms_get_sadf2()*100;
     hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_RAD_A2SFREQ,&i);
     }
     */

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
    if(xpdatarefs.tailno!=NULL) {
      XPLMGetDatab(xpdatarefs.tailno,acfstr,0,7);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STAT_TAILNO,acfstr);
    }
    memset(acfstr,0,8);
    if(xpdatarefs.acficao!=NULL) {
      XPLMGetDatab(xpdatarefs.acficao,acfstr,0,7);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STAT_ICAO,acfstr);
    }
    memset(acfstr,0,8);
    if(xpdatarefs.callsign!=NULL) {
      XPLMGetDatab(xpdatarefs.callsign,acfstr,0,7);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STAT_CALLSGN,acfstr);
    }

    /* VSPEEDS */
    if(xpdatarefs.vso!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.vso);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VS0,&f);
    }
    if(xpdatarefs.vs!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.vs);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VS,&f);
    }
    if(xpdatarefs.vfe!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.vfe);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VFE,&f);
    }
    if(xpdatarefs.vno!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.vno);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VNO,&f);
    }
    if(xpdatarefs.vne!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.vne);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_VSPD_VNE,&f);
    }
    if(xpdatarefs.mmo!=NULL) {
      float f=XPLMGetDataf(xpdatarefs.mmo);
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



/* Generic FMC send data function, calls others if it is the case */
void hsxpl_send_fmc_data(void) {

  hsxpl_navdb_fmc_type();

  if(hsmp_number_of_stream_clients()<1) return;

  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_XFMC) {
    hsxpl_send_xfmc_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_A320N) {
    hsxpl_send_a320n_fmc_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_A320Q) {
    hsxpl_send_a320q_fmc_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_XP738) {
    hsxpl_send_z738_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737) {
    hsxpl_send_ufmc_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737V5) {
    hsxpl_send_ufmc_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_IX733) {
    hsxpl_send_ix733_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_UFMC) {
    hsxpl_send_ufmc_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_PB757) {
    hsxpl_send_p757_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_PB777) {
    hsxpl_send_p777_data();
    return;
  }
  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_PCRJ200) {
    hsxpl_send_pcrj200_data();
    return;
  }

  /* Default x-plane */

  if(hsxpl_navdb_fmc_type()!=1) {
    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {

      char line[32];
      memset(line,0,32);
      strncpy(line,hsxpl_acf_icao(),31);
      strncat(line," / ",31);
      strncat(line,hsxpl_acf_tailno(),31);


      uint32_t n=HSMP_FMC_TYPE_XPLANE;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L01,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L02,line);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L03,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L04,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L05,"INCOMPATIBLE AIRCRAFT");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L07,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L08,"UNABLE TO ACCESS THE ACF");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L09,"FLIGHT MANAGEMENT SYSTEM");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L10,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L11,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L12,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L13,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L14,"");

      hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
      free(pkt);
      return;
    }
  }

  if(hsxpl_navdb_number_of_route_points()==0) {
    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {

      char line[32];
      memset(line,0,32);
      strncpy(line,hsxpl_acf_icao(),31);
      strncat(line," / ",31);
      strncat(line,hsxpl_acf_tailno(),31);


      uint32_t n=HSMP_FMC_TYPE_XPLANE;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L01,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L02,"NO FLIGHT PLAN LOADED");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L03,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L04,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L05,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L07,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L08,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L09,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L10,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L11,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L12,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L13,"");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L14,"");

      hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
      free(pkt);
      return;
    }
  }

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t n=HSMP_FMC_TYPE_XPLANE;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    char line[32];
    memset(line,0,32);

    /* Empty lines */
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L01,line);

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L04,line);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L09,line);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L10,line);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L12,line);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L13,line);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L14,line);

    int di=XPLMGetDisplayedFMSEntry();
    int da=XPLMGetDestinationFMSEntry();

    if(di>100) di=0; if(di<0) di=0;
    sprintf(line,"  PLAN SEGMENT %02d",di);
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L02,line);

    if(di==da) {
      sprintf(line,"  ACTIVE LEG");
    } else {
      memset(line,0,32);
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L03,line);

    XPLMNavType outType;char outID[32];XPLMNavRef outRef;
    int outAltitude;float outLat;float outLon;
    XPLMGetFMSEntryInfo(di,&outType,outID,&outRef,&outAltitude,&outLat,&outLon);

    switch(outType) {
      case(xplm_Nav_Airport): sprintf(line,"< APT ");break;
      case(xplm_Nav_NDB): sprintf(line,"< NDB ");break;
      case(xplm_Nav_VOR): sprintf(line,"< VOR ");break;
      case(xplm_Nav_ILS): sprintf(line,"< ILS ");break;
      case(xplm_Nav_Localizer): sprintf(line,"< LOC ");break;
      case(xplm_Nav_GlideSlope): sprintf(line,"< GS ");break;
      case(xplm_Nav_OuterMarker): sprintf(line,"< OM ");break;
      case(xplm_Nav_MiddleMarker): sprintf(line,"< MM ");break;
      case(xplm_Nav_InnerMarker): sprintf(line,"< IM ");break;
      case(xplm_Nav_Fix): sprintf(line,"< FIX ");break;
      case(xplm_Nav_DME): sprintf(line,"< DME ");break;
      case(xplm_Nav_LatLon): sprintf(line,"< L-L ");break;
      default: memset(line,0,32);
    }
    if(outID[0]!='\0') {
      outID[22]='\0';
      strcat(line,outID);
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L05,line);

    if(outType) {

      if(outRef!=0 && outID[0]!='\0') {

        XPLMNavType nouttype;float noutLat;float noutLon;int noutFreq; /* float noutHeight; */
        float noutHeading;char noutID[32];char noutName[256];char noutReg;
        nouttype=outType;
        XPLMGetNavAidInfo(outRef,&nouttype,&noutLat,&noutLon,&noutHeading,&noutFreq,&noutHeading,noutID,noutName,&noutReg);

        if(noutName[0]!='\0') {
          if(strcmp(noutName,"NOT FOUND")) {
            memset(line,0,32);
            snprintf(line,31,"  %s",noutName);
          }
          else
            memset(line,0,32);
          hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,line);
        } else {
          memset(line,0,32);
          hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,line);
        }

      } else {

        memset(line,0,32);
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,line);
      }

      sprintf(line,"< FLY AT %05d FT",outAltitude>=0?outAltitude:0);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L11,line);

    } else {

      memset(line,0,32);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,line);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L11,line);
    }


    if(outLat!=0) {
      char lx[32]; IPONAVLatitudeCStringFor(outLat,lx,IPONAV_LATLON_FMT_DMM);
      sprintf(line,"  LAT %s",lx);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L07,line);

    } else {
      memset(line,0,32);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L07,line);
    }
    if(outLon!=0) {
      char lx[32]; IPONAVLongitudeCStringFor(outLon,lx,IPONAV_LATLON_FMT_DMM);
      sprintf(line,"  LON %s",lx);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L08,line);

    } else {
      memset(line,0,32);
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L08,line);
    }

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);

  }

}


/* hsxpl_send_xfmc_data is used to send xfmc data */
void hsxpl_send_xfmc_data(void) {


  uint8_t i;
  uint8_t j;
  uint8_t si;

  /* Build screen, prepare matrix  */

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
    for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=0xFFFFFFFF;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }


  unsigned char line[256];line[255]='\0';
  unsigned char *cp;

  for(i=0;i<14;i++) {
    if(xfmcdatarefs.line[i]!=NULL) {

      int j=1;
      unsigned char str[256];

      memset(line,0,256);
      XPLMGetDatab(xfmcdatarefs.line[i],line,0,255);
      cp=line; while(*cp!='\0') { if(*cp>127) *cp = *cp & 0x7F; cp++;  if(cp-line >80) break; }

      /* Move to the part after the slash */
      cp=line;
      while(*cp!='\0' && *cp!='/') cp++;
      if(*cp=='/') cp++;

      do {
        if(hsxpl_strentry(j,(char *)cp,';',(char *)str)==NULL) break;
        if(str[0]=='\0') break;

        else {

          char font_size[256];
          char pixel_offset[256];
          char text[256];

          if(hsxpl_strentry(1,(char *)str,',',(char *)font_size)==NULL) break;
          if(font_size[0]=='\0') break;
          if(hsxpl_strentry(2,(char *)str,',',(char *)pixel_offset)==NULL) break;
          if(pixel_offset[0]=='\0') break;
          if(hsxpl_strentry(3,(char *)str,',',(char *)text)==NULL) break;
          if(text[0]=='\0') break;

          uint8_t fsize=0;
          if(font_size[0]=='0' && font_size[1]=='\0') fsize=20;

          uint8_t scol=0xFF;
          float f=atof(pixel_offset);
          if(f>=0.0) {
            float k = f * 30.0 / 199.0;
            scol = (uint8_t) floor(k);
            if(scol>=HSMP_FMC_MAX_SCREEN_NOCOLS) scol=0xFF;
          }

          if(scol!=0xFF) {

            char *cx=text;
            while(*cx!='\0') {
              if(scol>=HSMP_FMC_MAX_SCREEN_NOCOLS) break;

              if(*cx == 31) screen.matrix[i][scol].chr='_'; /* Box symbol */
              else if(*cx == 30) screen.matrix[i][scol].chr=(char)161; /* Degree symbol */
              else
                screen.matrix[i][scol].chr = toupper(*cx);

              screen.matrix[i][scol].fsize = fsize;
              if(fsize) screen.matrix[i][scol].colour=0xCCE5FFFF;
              /* LIGHT BLUE */
              cx++;scol++;
            }
          }


        }
        j++;

      } while(1);

    }
  }

  for(si=0;si<HSMP_FMC_MAX_SCREEN_NOROWS;si+=3) {

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {

      uint8_t i,j;

      uint32_t n=HSMP_FMC_TYPE_XP_XFMC;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);



      char xstr[8];
      memset(xstr,0,8);
      sprintf(xstr,"LNAV");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSLU_TXT,xstr);
      sprintf(xstr,"VNAV");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSLD_TXT,xstr);
      sprintf(xstr,"ATHR");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSRU_TXT,xstr);
      sprintf(xstr,"MCP");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSRD_TXT,xstr);

      uint32_t an=0;
      if(xfmcdatarefs.status!=NULL) {
        uint32_t st=XPLMGetDatai(xfmcdatarefs.status);
        if(st&(1<<5))
          an |= HSMP_FMC_ANNUNC_EXEC;
        if(st&(1<<1))
          an |= HSMP_FMC_ANNUNC_VSLU;
        if(!(st&(1<<2)))
          an |= HSMP_FMC_ANNUNC_VSLD;
        if(st&(1<<3))
          an |= HSMP_FMC_ANNUNC_VSRU;
        if(st&(1<<4))
          an |= HSMP_FMC_ANNUNC_VSRD;
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&an);
      }

      for(i=0;i<3;i++) {

        if(si+i >= HSMP_FMC_MAX_SCREEN_NOROWS) break;

        for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
          hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[si+i][j]));
        }
      }
      hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
      free(pkt);
    }

  }


}







/* Send 757 data ; most data is retreived by web services so it isn't sent here */
void hsxpl_send_p757_data(void) {

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t n=HSMP_FMC_TYPE_XP_PB757;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    uint32_t i=0;
    if(hsxpl_fmc.exec_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.exec_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_EXEC;
    }
    if(hsxpl_fmc.dspy_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.dspy_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSLU;
    }
    if(hsxpl_fmc.fail_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.fail_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSLD;
    }
    if(hsxpl_fmc.msg_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.msg_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSRU;
    }
    if(hsxpl_fmc.ofst_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.ofst_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSRD;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&i);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

}



void hsxpl_send_p777_data(void) {

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t n=HSMP_FMC_TYPE_XP_PB777;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    uint32_t i=0;
    if(hsxpl_fmc.exec_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.exec_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_EXEC;
    }
    if(hsxpl_fmc.dspy_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.dspy_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSLU;
    }
    if(hsxpl_fmc.fail_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.fail_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSLD;
    }
    if(hsxpl_fmc.msg_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.msg_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSRU;
    }
    if(hsxpl_fmc.ofst_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.ofst_light_on);
      if(f>0.3) i |= HSMP_FMC_ANNUNC_VSRD;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&i);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

}

/* Likewise CRJ200 */
void hsxpl_send_pcrj200_data(void) {

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint32_t n=HSMP_FMC_TYPE_XP_PCRJ200;
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

}

#define HSXPL_A320N_FMC_FSIZE_DEFAULT     0
#define HSXPL_A320N_FMC_FSIZE_LABEL       20
#define HSXPL_A320N_FMC_COL_WHITE         0xFFFFFFFF
#define HSXPL_A320N_FMC_COL_GREEN         0x00FF00FF
#define HSXPL_A320N_FMC_COL_CYAN          0x00FFFFFF
#define HSXPL_A320N_FMC_COL_MAGENTA       0xFF00FFFF
#define HSXPL_A320N_FMC_COL_AMBER         0xFFBF00FF
#define HSXPL_A320N_FMC_COL_YELLOW        0xFFFF00FF

void hsxpl_build_fmc_screen_line(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size,uint32_t colour) {

  char *cp=line;
  uint8_t i;

  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOCOLS;i++) {

    if(*cp=='\0') break;
    if(*cp==' ') { cp++; continue; }

    matrix[i].fsize=font_size;
    matrix[i].chr= toupper(*cp);
    matrix[i].colour=colour;
    cp++;
  }
}

/* For the IX733 */
void hsxpl_build_ix737_screen_left_line(hsmp_fmc_screen_c_t *matrix,char *line,uint32_t colour) {

  unsigned char *cp=(unsigned char *)line;
  uint8_t i;
  uint8_t fSize=26;
  uint32_t fColour=colour;

  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOCOLS;i++) {

    if(*cp=='\0') break;
    if(*cp==' ') { cp++; continue; }

    if(*cp == 164) { fSize=26; cp++; i--; continue; }
    else if(*cp == 163) { fSize=22; cp++; i--; continue; }
    else if(*cp == '&') {
      if(fColour==colour) {
        fColour=0xFFFFFFFF;
      } else {
        fColour=colour;
      }
      cp++; i--;
      continue;

    };

    matrix[i].fsize=fSize;
    matrix[i].chr= toupper(*cp);
    matrix[i].colour=fColour;
    cp++;
  }
}
void hsxpl_build_ix737_screen_right_line(hsmp_fmc_screen_c_t *matrix,char *line,uint32_t colour) {

  unsigned char *bp=(unsigned char *)line;
  unsigned char *cp=bp;
  uint8_t i;
  uint8_t fSize=26;
  uint32_t fColour=colour;

  while(*cp!='\0' && (cp-bp) < 32) cp++;
  cp--;

  for(i=23;i>=0;i--) {

    if(cp<bp) break;
    if(*cp==' ') { cp--; continue; }

    if(*cp == 164) { cp--; i++; continue; }
    else if(*cp == 163) { cp--; i++; continue;  }
    else if(*cp == '&') { cp--; i++; continue;  }

    matrix[i].chr= toupper(*cp);

    cp--;
  }
  cp++; i++;
  for(;i<24;i++) {
    if(*cp == 164) { fSize=26; cp++; i--; continue; }
    else if(*cp == 163) { fSize=22; cp++; i--; continue; }
    else if(*cp == '&') {
      if(fColour==colour) {
        fColour=0xFFFFFFFF;
      } else {
        fColour=colour;
      }
      cp++; i--;
      continue;
    };
    matrix[i].colour=fColour;
    matrix[i].fsize=fSize;
  }

}



/* Send A320 FMC data in ARINC format */
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
  if(a320ndatarefs.title_g!=NULL) {
    XPLMGetDatab(a320ndatarefs.title_g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.title_w!=NULL) {
    XPLMGetDatab(a320ndatarefs.title_w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.title_y!=NULL) {
    XPLMGetDatab(a320ndatarefs.title_y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }


  /* Scratchpad is line 13 */
  if(a320ndatarefs.scratchpad!=NULL) {
    XPLMGetDatab(a320ndatarefs.scratchpad,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.scratchpad_a!=NULL) {
    XPLMGetDatab(a320ndatarefs.scratchpad_a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }

  /* Line1 = row 2 */
  if(a320ndatarefs.line1w!=NULL) {
    XPLMGetDatab(a320ndatarefs.line1w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.line1g!=NULL) {
    XPLMGetDatab(a320ndatarefs.line1g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.line1b!=NULL) {
    XPLMGetDatab(a320ndatarefs.line1b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(a320ndatarefs.line1a!=NULL) {
    XPLMGetDatab(a320ndatarefs.line1a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(a320ndatarefs.line1m!=NULL) {
    XPLMGetDatab(a320ndatarefs.line1m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(a320ndatarefs.line1y!=NULL) {
    XPLMGetDatab(a320ndatarefs.line1y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }


  /* Line2 = row 4*/
  if(a320ndatarefs.line2w!=NULL) {
    XPLMGetDatab(a320ndatarefs.line2w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.line2g!=NULL) {
    XPLMGetDatab(a320ndatarefs.line2g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.line2b!=NULL) {
    XPLMGetDatab(a320ndatarefs.line2b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(a320ndatarefs.line2a!=NULL) {
    XPLMGetDatab(a320ndatarefs.line2a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(a320ndatarefs.line2m!=NULL) {
    XPLMGetDatab(a320ndatarefs.line2m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(a320ndatarefs.line2y!=NULL) {
    XPLMGetDatab(a320ndatarefs.line2y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[4],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Line3 = row 6 */
  if(a320ndatarefs.line3w!=NULL) {
    XPLMGetDatab(a320ndatarefs.line3w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.line3g!=NULL) {
    XPLMGetDatab(a320ndatarefs.line3g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.line3b!=NULL) {
    XPLMGetDatab(a320ndatarefs.line3b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(a320ndatarefs.line3a!=NULL) {
    XPLMGetDatab(a320ndatarefs.line3a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(a320ndatarefs.line3m!=NULL) {
    XPLMGetDatab(a320ndatarefs.line3m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(a320ndatarefs.line3y!=NULL) {
    XPLMGetDatab(a320ndatarefs.line3y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[6],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }


  /* Line4 = row 8 */
  if(a320ndatarefs.line4w!=NULL) {
    XPLMGetDatab(a320ndatarefs.line4w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.line4g!=NULL) {
    XPLMGetDatab(a320ndatarefs.line4g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.line4b!=NULL) {
    XPLMGetDatab(a320ndatarefs.line4b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(a320ndatarefs.line4a!=NULL) {
    XPLMGetDatab(a320ndatarefs.line4a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(a320ndatarefs.line4m!=NULL) {
    XPLMGetDatab(a320ndatarefs.line4m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(a320ndatarefs.line4y!=NULL) {
    XPLMGetDatab(a320ndatarefs.line4y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[8],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Line5 = row 10 */
  if(a320ndatarefs.line5w!=NULL) {
    XPLMGetDatab(a320ndatarefs.line5w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.line5g!=NULL) {
    XPLMGetDatab(a320ndatarefs.line5g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.line5b!=NULL) {
    XPLMGetDatab(a320ndatarefs.line5b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(a320ndatarefs.line5a!=NULL) {
    XPLMGetDatab(a320ndatarefs.line5a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(a320ndatarefs.line5m!=NULL) {
    XPLMGetDatab(a320ndatarefs.line5m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(a320ndatarefs.line5y!=NULL) {
    XPLMGetDatab(a320ndatarefs.line5y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[10],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Line6 = row 12 */
  if(a320ndatarefs.line6w!=NULL) {
    XPLMGetDatab(a320ndatarefs.line6w,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.line6g!=NULL) {
    XPLMGetDatab(a320ndatarefs.line6g,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_GREEN);
  }
  if(a320ndatarefs.line6b!=NULL) {
    XPLMGetDatab(a320ndatarefs.line6b,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_CYAN);
  }
  if(a320ndatarefs.line6a!=NULL) {
    XPLMGetDatab(a320ndatarefs.line6a,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_AMBER);
  }
  if(a320ndatarefs.line6m!=NULL) {
    XPLMGetDatab(a320ndatarefs.line6m,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_MAGENTA);
  }
  if(a320ndatarefs.line6y!=NULL) {
    XPLMGetDatab(a320ndatarefs.line6y,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[12],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320N_FMC_COL_YELLOW);
  }

  /* Labels are line 1,3,5,7,9,11 */
  if(a320ndatarefs.label1!=NULL) {
    XPLMGetDatab(a320ndatarefs.label1,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.label2!=NULL) {
    XPLMGetDatab(a320ndatarefs.label2,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[3],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.label3!=NULL) {
    XPLMGetDatab(a320ndatarefs.label3,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[5],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.label4!=NULL) {
    XPLMGetDatab(a320ndatarefs.label4,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[7],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.label5!=NULL) {
    XPLMGetDatab(a320ndatarefs.label5,line,0,26);
    hsxpl_build_fmc_screen_line(screen.matrix[9],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320N_FMC_COL_WHITE);
  }
  if(a320ndatarefs.label6!=NULL) {
    XPLMGetDatab(a320ndatarefs.label6,line,0,26);
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

#define HSXPL_A320Q_FMC_FSIZE_DEFAULT     0
#define HSXPL_A320Q_FMC_FSIZE_LABEL       20
#define HSXPL_A320Q_FMC_COL_WHITE         0xFFFFFFFF
#define HSXPL_A320Q_FMC_COL_GREEN         0x00FF00FF
#define HSXPL_A320Q_FMC_COL_CYAN          0x00FFFFFF
#define HSXPL_A320Q_FMC_COL_MAGENTA       0xFF00FFFF
#define HSXPL_A320Q_FMC_COL_AMBER         0xFFBF00FF
#define HSXPL_A320Q_FMC_COL_YELLOW        0xFFFF00FF

void hsxpl_build_a320q_screen_line(hsmp_fmc_screen_c_t *matrix,char *line,uint8_t row,uint8_t font_size,uint32_t colour) {

  char *cp=line;
  uint8_t i;

  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOCOLS;i++) {

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

  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOCOLS;i++) {

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


void hsxpl_send_a320q_fmc_data(void) {


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
      screen.matrix[i][j].colour=HSXPL_A320Q_FMC_COL_WHITE;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }

  char line[26];line[25]='\0';

  /* Line 0 is title */

  if(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
    XPLMGetDatab(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_WHITE);
  }
  if(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
    XPLMGetDatab(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_GREEN);
  }
  if(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
    XPLMGetDatab(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_CYAN);
  }
  if(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
    XPLMGetDatab(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_AMBER);
  }
  if(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
    XPLMGetDatab(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_YELLOW);
  }
  if(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
    XPLMGetDatab(a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_MAGENTA);
  }


  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_WHITE],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_WHITE);
  }
  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_GREEN],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_GREEN);
  }
  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_BLUE],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_CYAN);
  }
  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_AMBER],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_AMBER);
  }
  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_YELLOW],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_YELLOW);
  }
  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[0],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_MAGENTA);
  }
  if(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_S]!=NULL) {
    XPLMGetDatab(a320qdatarefs.title[HSXPL_A320Q_COL_IDX_S],line,0,26);
    hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT);
  }

  /* Scratchpad is line 13 */
  if(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
    XPLMGetDatab(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_WHITE);
  }
  if(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
    XPLMGetDatab(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_GREEN);
  }
  if(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
    XPLMGetDatab(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_CYAN);
  }
  if(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
    XPLMGetDatab(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_AMBER);
  }
  if(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
    XPLMGetDatab(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_YELLOW);
  }
  if(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
    XPLMGetDatab(a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA],line,0,26);
    hsxpl_build_a320q_screen_line(screen.matrix[13],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_MAGENTA);
  }

  /* Now for the rest */
  for(i=0;i<6;i++) {


    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_WHITE],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_WHITE);
    }
    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_GREEN],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_GREEN);
    }
    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_BLUE],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_CYAN);
    }
    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_AMBER],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_AMBER);
    }
    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_YELLOW],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_YELLOW);
    }
    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_MAGENTA],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+1],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_MAGENTA);
    }
    if(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_S]!=NULL) {
      XPLMGetDatab(a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_S],line,0,26);
      hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL);
    }

    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_WHITE],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_WHITE);
    }
    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_GREEN],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_GREEN);
    }
    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_BLUE],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_CYAN);
    }
    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_AMBER],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_AMBER);
    }
    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_S]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_S],line,0,26);
      hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL);
    }
    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_YELLOW],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_YELLOW);
    }
    if(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_MAGENTA],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_LABEL,HSXPL_A320Q_FMC_COL_MAGENTA);
    }

    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_WHITE]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_WHITE],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_WHITE);
    }
    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_GREEN]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_GREEN],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_GREEN);
    }
    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_BLUE]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_BLUE],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_CYAN);
    }
    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_AMBER]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_AMBER],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_AMBER);
    }
    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_S]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_S],line,0,26);
      hsxpl_build_a320q_screen_sline(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT);
    }
    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_YELLOW]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_YELLOW],line,0,26);
      hsxpl_build_a320q_screen_line(screen.matrix[i*2+2],line,0,HSXPL_A320N_FMC_FSIZE_DEFAULT,HSXPL_A320Q_FMC_COL_YELLOW);
    }
    if(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_MAGENTA]!=NULL) {
      XPLMGetDatab(a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_MAGENTA],line,0,26);
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

    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
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

    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
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
    uint32_t n=HSMP_FMC_TYPE_XP_A320Q;
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

void hsxpl_send_ix733_data(void) {



  uint8_t i;
  uint8_t j;

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
    for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=0x00FF00FF;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }

  char line[32];

  memset(line,0,32);
  XPLMGetDatab(ix733_datarefs.title,line,0,31);
  hsxpl_build_ix737_screen_left_line(screen.matrix[0],line,0x00FF00FF);

  memset(line,0,32);
  XPLMGetDatab(ix733_datarefs.pageNo,line,0,31);
  hsxpl_build_ix737_screen_right_line(screen.matrix[0],line,0x00FF00FF);

  for(i=0;i<12;i++) {
    if (ix733_datarefs.llines[i]!=NULL) {
      memset(line,0,32);
      XPLMGetDatab(ix733_datarefs.llines[i],line,0,31);
      hsxpl_build_ix737_screen_left_line(screen.matrix[i+1],line,0x00FF00FF);
      memset(line,0,32);
      XPLMGetDatab(ix733_datarefs.rlines[i],line,0,31);
      hsxpl_build_ix737_screen_right_line(screen.matrix[i+1],line,0x00FF00FF);

    }
  }

  memset(line,0,32);
  XPLMGetDatab(ix733_datarefs.scratchPad,line,0,31);
  hsxpl_build_ix737_screen_left_line(screen.matrix[13],line,0x00FF00FF);

  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    uint8_t i,j;

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

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

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

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

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

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

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    for(i=12;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
      for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[i][j]));
      }
    }
    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }

  pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&hsxpl_fmc_type);

    uint32_t i=0;
    if(hsxpl_fmc.exec_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.exec_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_EXEC;
    }
    if(hsxpl_fmc.fail_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.fail_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_VSLD;
    }
    if(hsxpl_fmc.msg_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.msg_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_VSRU;
    }
    if(hsxpl_fmc.ofst_light_on!=NULL) {
      float f=XPLMGetDataf(hsxpl_fmc.ofst_light_on);
      if(f>0.2) i |= HSMP_FMC_ANNUNC_VSRD;
    }
    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&i);

    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ACTIVE_MCDU,&hsxpl_active_mcdu);

    hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
    free(pkt);
  }


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
      XPLMGetDatab(ufmcdatarefs.line[i],line,0,63);

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
      XPLMGetDatab(ufmcdatarefs.line[i],line,0,63);

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
      XPLMGetDatab(ufmcdatarefs.line[i],line,0,63);

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
      XPLMGetDatab(ufmcdatarefs.line[i],line,0,63);

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

   if(ufmcdatarefs.line1!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line1,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L01,line);
   }
   if(ufmcdatarefs.line2!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line2,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L02,line);
   }
   if(ufmcdatarefs.line3!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line3,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L03,line);
   }
   if(ufmcdatarefs.line4!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line4,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L04,line);
   }
   if(ufmcdatarefs.line5!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line5,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L05,line);
   }
   if(ufmcdatarefs.line6!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line6,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L06,line);
   }
   if(ufmcdatarefs.line7!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line7,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L07,line);
   }
   if(ufmcdatarefs.line8!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line8,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L08,line);
   }
   if(ufmcdatarefs.line9!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line9,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L09,line);
   }
   if(ufmcdatarefs.line10!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line10,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L10,line);
   }
   if(ufmcdatarefs.line11!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line11,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L11,line);
   }
   if(ufmcdatarefs.line12!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line12,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L12,line);
   }
   if(ufmcdatarefs.line13!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line13,line,0,25);
   cp=line; while(*cp!='\0') { if(*cp=='=') *cp=(char)161; *cp = toupper(*cp); cp++; }
   hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_LINE_L13,line);
   }
   if(ufmcdatarefs.line14!=NULL) {
   memset(line,0,HSXPL_UFMC_LINE_SIZE);
   XPLMGetDatab(ufmcdatarefs.line14,line,0,25);
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

/* Pre-set the references to the datarefs for quicker access later */
void hsxpl_set_xplane_datarefs(void) {

  memset(&xpdatarefs,0,sizeof(struct xplane_datarefs_s));
  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  xpdatarefs.latitude=XPLMFindDataRef("sim/flightmodel/position/latitude");
  xpdatarefs.longitude=XPLMFindDataRef("sim/flightmodel/position/longitude");
  /* pos_elev=XPLMFindDataRef("sim/flightmodel/position/elevation"); */
  xpdatarefs.elevation=XPLMFindDataRef("sim/flightmodel/misc/h_ind");
  xpdatarefs.theta=XPLMFindDataRef("sim/flightmodel/position/theta");
  xpdatarefs.phi=XPLMFindDataRef("sim/flightmodel/position/phi");
  xpdatarefs.psi=XPLMFindDataRef("sim/flightmodel/position/psi");
  xpdatarefs.mvar=XPLMFindDataRef("sim/flightmodel/position/magnetic_variation");
  xpdatarefs.hpath=XPLMFindDataRef("sim/flightmodel/position/hpath");
  xpdatarefs.vpath=XPLMFindDataRef("sim/flightmodel/position/vpath");
  xpdatarefs.gs=XPLMFindDataRef("sim/flightmodel/position/groundspeed");
  xpdatarefs.ias=XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
  xpdatarefs.tas=XPLMFindDataRef("sim/flightmodel/position/true_airspeed");
  xpdatarefs.vvi=XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm2");
  xpdatarefs.mach=XPLMFindDataRef("sim/flightmodel/misc/machno");
  xpdatarefs.msl=XPLMFindDataRef("sim/flightmodel/position/elevation");
  xpdatarefs.agl=XPLMFindDataRef("sim/flightmodel/position/y_agl");

  xpdatarefs.winddir=XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_heading_deg_mag");
  xpdatarefs.windspeed=XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_speed_kts");
  xpdatarefs.outsidetemp=XPLMFindDataRef("sim/weather/temperature_ambient_c");

  xpdatarefs.tailno=XPLMFindDataRef("sim/aircraft/view/acf_tailnum");
  xpdatarefs.acficao=XPLMFindDataRef("sim/aircraft/view/acf_ICAO");

  xpdatarefs.lgear=XPLMFindDataRef("sim/cockpit/switches/gear_handle_status");
  xpdatarefs.pbreak=XPLMFindDataRef("sim/cockpit2/controls/parking_brake_ratio");
  xpdatarefs.flaps=XPLMFindDataRef("sim/cockpit2/controls/flap_ratio");
  xpdatarefs.spdbreak=XPLMFindDataRef("sim/cockpit2/controls/speedbrake_ratio");

  xpdatarefs.vso=XPLMFindDataRef("sim/aircraft/view/acf_Vso");
  xpdatarefs.vs=XPLMFindDataRef("sim/aircraft/view/acf_Vs");
  xpdatarefs.vfe=XPLMFindDataRef("sim/aircraft/view/acf_Vfe");
  xpdatarefs.vno=XPLMFindDataRef("sim/aircraft/view/acf_Vno");
  xpdatarefs.vne=XPLMFindDataRef("sim/aircraft/view/acf_Vne");
  xpdatarefs.mmo=XPLMFindDataRef("sim/aircraft/view/acf_Mmo");

  xpdatarefs.joyroll=XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
  xpdatarefs.joypitch=XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
  xpdatarefs.joyyaw=XPLMFindDataRef("sim/joystick/yoke_heading_ratio");

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

void hsxpl_set_a320q_datarefs(void) {


  int i;
  char dref[64];

  if(XPLMFindDataRef("AirbusFBW/MCDU1label1m") == NULL) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_A320Q;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_A320Q;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));
  memset(&a320qdatarefs,0,sizeof(struct a320qdatarefs_s));

  if(hsxpl_active_mcdu==2) {

    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU2titlew");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU2titleg");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU2titleb");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU2titlea");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU2titley");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU2titlem");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef("AirbusFBW/MCDU2titles");

    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU2stitlew");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU2stitleg");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU2stitleb");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU2stitlea");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU2stitley");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU2stitlem");

    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU2spw");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU2spg");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU2spb");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU2spa");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU2spy");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU2spm");

  } else {

    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU1titlew");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU1titleg");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU1titleb");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU1titlea");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU1titley");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU1titlem");
    a320qdatarefs.title[HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef("AirbusFBW/MCDU1titles");

    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU1stitlew");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU1stitleg");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU1stitleb");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU1stitlea");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU1stitley");
    a320qdatarefs.stitle[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU1stitlem");

    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef("AirbusFBW/MCDU1spw");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef("AirbusFBW/MCDU1spg");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef("AirbusFBW/MCDU1spb");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef("AirbusFBW/MCDU1spa");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef("AirbusFBW/MCDU1spy");
    a320qdatarefs.scratchpad[HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef("AirbusFBW/MCDU1spm");
  }

  for(i=0;i<6;i++) {

    sprintf(dref,"AirbusFBW/MCDU%dscont%dw",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%dg",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%db",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%da",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%dy",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%dm",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dscont%ds",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_small[i][HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef(dref);

    sprintf(dref,"AirbusFBW/MCDU%dcont%dw",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%dg",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%db",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%da",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%dy",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%dm",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dcont%ds",hsxpl_active_mcdu,i+1);
    a320qdatarefs.content_large[i][HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef(dref);


    sprintf(dref,"AirbusFBW/MCDU%dlabel%dw",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_WHITE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%dg",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_GREEN]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%db",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_BLUE]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%da",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_AMBER]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%dy",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_YELLOW]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%dm",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_MAGENTA]=XPLMFindDataRef(dref);
    sprintf(dref,"AirbusFBW/MCDU%dlabel%ds",hsxpl_active_mcdu,i+1);
    a320qdatarefs.label[i][HSXPL_A320Q_COL_IDX_S]=XPLMFindDataRef(dref);

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

void hsxpl_set_a320n_datarefs(void) {

  if(strcmp(hsxpl_acf_icao(),"A320") && strcmp(hsxpl_acf_icao(),"A330")) return;
  if(XPLMFindDataRef("sim/custom/xap/mcdu/click_0") == NULL) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_A320N;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_A320N;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));
  memset(&a320ndatarefs,0,sizeof(struct a320ndatarefs_s));

  hsxpl_fmc.key_0=XPLMFindDataRef("sim/custom/xap/mcdu/click_0");
  hsxpl_fmc.key_1=XPLMFindDataRef("sim/custom/xap/mcdu/click_1");
  hsxpl_fmc.key_2=XPLMFindDataRef("sim/custom/xap/mcdu/click_2");
  hsxpl_fmc.key_3=XPLMFindDataRef("sim/custom/xap/mcdu/click_3");
  hsxpl_fmc.key_4=XPLMFindDataRef("sim/custom/xap/mcdu/click_4");
  hsxpl_fmc.key_5=XPLMFindDataRef("sim/custom/xap/mcdu/click_5");
  hsxpl_fmc.key_6=XPLMFindDataRef("sim/custom/xap/mcdu/click_6");
  hsxpl_fmc.key_7=XPLMFindDataRef("sim/custom/xap/mcdu/click_7");
  hsxpl_fmc.key_8=XPLMFindDataRef("sim/custom/xap/mcdu/click_8");
  hsxpl_fmc.key_9=XPLMFindDataRef("sim/custom/xap/mcdu/click_9");
  hsxpl_fmc.key_dot=XPLMFindDataRef("sim/custom/xap/mcdu/click_dot");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("sim/custom/xap/mcdu/plusmin");

  hsxpl_fmc.key_a=XPLMFindDataRef("sim/custom/xap/mcdu/click_a");
  hsxpl_fmc.key_b=XPLMFindDataRef("sim/custom/xap/mcdu/click_b");
  hsxpl_fmc.key_c=XPLMFindDataRef("sim/custom/xap/mcdu/click_c");
  hsxpl_fmc.key_d=XPLMFindDataRef("sim/custom/xap/mcdu/click_d");
  hsxpl_fmc.key_e=XPLMFindDataRef("sim/custom/xap/mcdu/click_e");
  hsxpl_fmc.key_f=XPLMFindDataRef("sim/custom/xap/mcdu/click_f");
  hsxpl_fmc.key_g=XPLMFindDataRef("sim/custom/xap/mcdu/click_g");
  hsxpl_fmc.key_h=XPLMFindDataRef("sim/custom/xap/mcdu/click_h");
  hsxpl_fmc.key_i=XPLMFindDataRef("sim/custom/xap/mcdu/click_i");
  hsxpl_fmc.key_j=XPLMFindDataRef("sim/custom/xap/mcdu/click_j");
  hsxpl_fmc.key_k=XPLMFindDataRef("sim/custom/xap/mcdu/click_k");
  hsxpl_fmc.key_l=XPLMFindDataRef("sim/custom/xap/mcdu/click_l");
  hsxpl_fmc.key_m=XPLMFindDataRef("sim/custom/xap/mcdu/click_m");
  hsxpl_fmc.key_n=XPLMFindDataRef("sim/custom/xap/mcdu/click_n");
  hsxpl_fmc.key_o=XPLMFindDataRef("sim/custom/xap/mcdu/click_o");
  hsxpl_fmc.key_p=XPLMFindDataRef("sim/custom/xap/mcdu/click_p");
  hsxpl_fmc.key_q=XPLMFindDataRef("sim/custom/xap/mcdu/click_q");
  hsxpl_fmc.key_r=XPLMFindDataRef("sim/custom/xap/mcdu/click_r");
  hsxpl_fmc.key_s=XPLMFindDataRef("sim/custom/xap/mcdu/click_s");
  hsxpl_fmc.key_t=XPLMFindDataRef("sim/custom/xap/mcdu/click_t");
  hsxpl_fmc.key_u=XPLMFindDataRef("sim/custom/xap/mcdu/click_u");
  hsxpl_fmc.key_v=XPLMFindDataRef("sim/custom/xap/mcdu/click_v");
  hsxpl_fmc.key_w=XPLMFindDataRef("sim/custom/xap/mcdu/click_w");
  hsxpl_fmc.key_x=XPLMFindDataRef("sim/custom/xap/mcdu/click_x");
  hsxpl_fmc.key_y=XPLMFindDataRef("sim/custom/xap/mcdu/click_y");
  hsxpl_fmc.key_z=XPLMFindDataRef("sim/custom/xap/mcdu/click_z");

  hsxpl_fmc.key_clr=XPLMFindDataRef("sim/custom/xap/mcdu/click_clr");
  hsxpl_fmc.key_space=XPLMFindDataRef("sim/custom/xap/mcdu/click_sp");
  hsxpl_fmc.key_slash=XPLMFindDataRef("sim/custom/xap/mcdu/click_slash");
  hsxpl_fmc.key_ovfy=XPLMFindDataRef("sim/custom/xap/mcdu/click_ovfy");

  hsxpl_fmc.key_airp=XPLMFindDataRef("sim/custom/xap/mcdu/click_airp");
  hsxpl_fmc.key_data=XPLMFindDataRef("sim/custom/xap/mcdu/click_data");
  hsxpl_fmc.key_dir=XPLMFindDataRef("sim/custom/xap/mcdu/click_dir");
  hsxpl_fmc.key_fpln=XPLMFindDataRef("sim/custom/xap/mcdu/click_fpln");
  hsxpl_fmc.key_init=XPLMFindDataRef("sim/custom/xap/mcdu/click_int");
  hsxpl_fmc.key_fuel=XPLMFindDataRef("sim/custom/xap/mcdu/click_fuel");
  hsxpl_fmc.key_menu=XPLMFindDataRef("sim/custom/xap/mcdu/click_mcdumenu");
  hsxpl_fmc.key_perf=XPLMFindDataRef("sim/custom/xap/mcdu/click_perf");
  hsxpl_fmc.key_radnav=XPLMFindDataRef("sim/custom/xap/mcdu/click_radnav");
  hsxpl_fmc.key_prog=XPLMFindDataRef("sim/custom/xap/mcdu/click_prog");
  hsxpl_fmc.key_blank=XPLMFindDataRef("sim/custom/xap/mcdu/click_blank");

  hsxpl_fmc.key_down=XPLMFindDataRef("sim/custom/xap/mcdu/click_up");
  hsxpl_fmc.key_up=XPLMFindDataRef("sim/custom/xap/mcdu/click_down");

  hsxpl_fmc.key_left=XPLMFindDataRef("sim/custom/xap/mcdu/click_left");
  hsxpl_fmc.key_right=XPLMFindDataRef("sim/custom/xap/mcdu/click_right");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("sim/custom/xap/mcdu/click_l1");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("sim/custom/xap/mcdu/click_l2");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("sim/custom/xap/mcdu/click_l3");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("sim/custom/xap/mcdu/click_l4");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("sim/custom/xap/mcdu/click_l5");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("sim/custom/xap/mcdu/click_l6");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("sim/custom/xap/mcdu/click_r1");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("sim/custom/xap/mcdu/click_r2");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("sim/custom/xap/mcdu/click_r3");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("sim/custom/xap/mcdu/click_r4");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("sim/custom/xap/mcdu/click_r5");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("sim/custom/xap/mcdu/click_r6");

  a320ndatarefs.label1=XPLMFindDataRef("sim/custom/xap/mcdu/label_1");
  a320ndatarefs.label2=XPLMFindDataRef("sim/custom/xap/mcdu/label_2");
  a320ndatarefs.label3=XPLMFindDataRef("sim/custom/xap/mcdu/label_3");
  a320ndatarefs.label4=XPLMFindDataRef("sim/custom/xap/mcdu/label_4");
  a320ndatarefs.label5=XPLMFindDataRef("sim/custom/xap/mcdu/label_5");
  a320ndatarefs.label6=XPLMFindDataRef("sim/custom/xap/mcdu/label_6");

  a320ndatarefs.line1a=XPLMFindDataRef("sim/custom/xap/mcdu/line_1a");
  a320ndatarefs.line1b=XPLMFindDataRef("sim/custom/xap/mcdu/line_1b");
  a320ndatarefs.line1g=XPLMFindDataRef("sim/custom/xap/mcdu/line_1g");
  a320ndatarefs.line1m=XPLMFindDataRef("sim/custom/xap/mcdu/line_1m");
  a320ndatarefs.line1w=XPLMFindDataRef("sim/custom/xap/mcdu/line_1w");
  a320ndatarefs.line1y=XPLMFindDataRef("sim/custom/xap/mcdu/line_1y");

  a320ndatarefs.line2a=XPLMFindDataRef("sim/custom/xap/mcdu/line_2a");
  a320ndatarefs.line2b=XPLMFindDataRef("sim/custom/xap/mcdu/line_2b");
  a320ndatarefs.line2g=XPLMFindDataRef("sim/custom/xap/mcdu/line_2g");
  a320ndatarefs.line2m=XPLMFindDataRef("sim/custom/xap/mcdu/line_2m");
  a320ndatarefs.line2w=XPLMFindDataRef("sim/custom/xap/mcdu/line_2w");
  a320ndatarefs.line2y=XPLMFindDataRef("sim/custom/xap/mcdu/line_2y");

  a320ndatarefs.line3a=XPLMFindDataRef("sim/custom/xap/mcdu/line_3a");
  a320ndatarefs.line3b=XPLMFindDataRef("sim/custom/xap/mcdu/line_3b");
  a320ndatarefs.line3g=XPLMFindDataRef("sim/custom/xap/mcdu/line_3g");
  a320ndatarefs.line3m=XPLMFindDataRef("sim/custom/xap/mcdu/line_3m");
  a320ndatarefs.line3w=XPLMFindDataRef("sim/custom/xap/mcdu/line_3w");
  a320ndatarefs.line3y=XPLMFindDataRef("sim/custom/xap/mcdu/line_3y");

  a320ndatarefs.line4a=XPLMFindDataRef("sim/custom/xap/mcdu/line_4a");
  a320ndatarefs.line4b=XPLMFindDataRef("sim/custom/xap/mcdu/line_4b");
  a320ndatarefs.line4g=XPLMFindDataRef("sim/custom/xap/mcdu/line_4g");
  a320ndatarefs.line4m=XPLMFindDataRef("sim/custom/xap/mcdu/line_4m");
  a320ndatarefs.line4w=XPLMFindDataRef("sim/custom/xap/mcdu/line_4w");
  a320ndatarefs.line4y=XPLMFindDataRef("sim/custom/xap/mcdu/line_4y");

  a320ndatarefs.line5a=XPLMFindDataRef("sim/custom/xap/mcdu/line_5a");
  a320ndatarefs.line5b=XPLMFindDataRef("sim/custom/xap/mcdu/line_5b");
  a320ndatarefs.line5g=XPLMFindDataRef("sim/custom/xap/mcdu/line_5g");
  a320ndatarefs.line5m=XPLMFindDataRef("sim/custom/xap/mcdu/line_5m");
  a320ndatarefs.line5w=XPLMFindDataRef("sim/custom/xap/mcdu/line_5w");
  a320ndatarefs.line5y=XPLMFindDataRef("sim/custom/xap/mcdu/line_5y");

  a320ndatarefs.line6a=XPLMFindDataRef("sim/custom/xap/mcdu/line_6a");
  a320ndatarefs.line6b=XPLMFindDataRef("sim/custom/xap/mcdu/line_6b");
  a320ndatarefs.line6g=XPLMFindDataRef("sim/custom/xap/mcdu/line_6g");
  a320ndatarefs.line6m=XPLMFindDataRef("sim/custom/xap/mcdu/line_6m");
  a320ndatarefs.line6w=XPLMFindDataRef("sim/custom/xap/mcdu/line_6w");
  a320ndatarefs.line6y=XPLMFindDataRef("sim/custom/xap/mcdu/line_6y");

  a320ndatarefs.scratchpad=XPLMFindDataRef("sim/custom/xap/mcdu/scratchpad");
  a320ndatarefs.scratchpad_a=XPLMFindDataRef("sim/custom/xap/mcdu/scratchpad_a");

  a320ndatarefs.title_g=XPLMFindDataRef("sim/custom/xap/mcdu/title_g");
  a320ndatarefs.title_w=XPLMFindDataRef("sim/custom/xap/mcdu/title_w");
  a320ndatarefs.title_y=XPLMFindDataRef("sim/custom/xap/mcdu/title_y");





}


void hsxpl_set_ix733_datarefs(void) {

  if (XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1L")==NULL)
    return;

  char acficao[8]; memset(acficao,0,8);
  if(xpdatarefs.acficao!=NULL)
    XPLMGetDatab(xpdatarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(xpdatarefs.tailno!=NULL)
    XPLMGetDatab(xpdatarefs.tailno,actfailno,0,7);

  int isIX733=0;
  if(XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1L") != NULL) {
    if(!strncmp(acficao,"B733",7)) {
      if(!strncmp(actfailno,"737300",7)) {
        isIX733=1;
      } else if(!strncmp(actfailno,"D-IXEG",7)) {
        isIX733=1;
      }
    }
  }
  if(!isIX733) {
    return;
  }

  /* x737 datarefs */
  memset(&ix733_datarefs,0,sizeof(struct ix733_datarefs_s));

  hsxpl_plane_type = HSXPL_PLANE_IX733;

  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_IX733;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  if(hsxpl_active_mcdu==1) {

    hsxpl_fmc.key_lk1=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1L");
    hsxpl_fmc.key_lk2=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_2L");
    hsxpl_fmc.key_lk3=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_3L");
    hsxpl_fmc.key_lk4=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_4L");
    hsxpl_fmc.key_lk5=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_5L");
    hsxpl_fmc.key_lk6=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_6L");

    hsxpl_fmc.key_rk1=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_1R");
    hsxpl_fmc.key_rk2=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_2R");
    hsxpl_fmc.key_rk3=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_3R");
    hsxpl_fmc.key_rk4=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_4R");
    hsxpl_fmc.key_rk5=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_5R");
    hsxpl_fmc.key_rk6=XPLMFindDataRef("ixeg/733/FMC/cdu1_lsk_6R");

    hsxpl_fmc.key_1=XPLMFindDataRef("ixeg/733/FMC/cdu1_1");
    hsxpl_fmc.key_2=XPLMFindDataRef("ixeg/733/FMC/cdu1_2");
    hsxpl_fmc.key_3=XPLMFindDataRef("ixeg/733/FMC/cdu1_3");
    hsxpl_fmc.key_4=XPLMFindDataRef("ixeg/733/FMC/cdu1_4");
    hsxpl_fmc.key_5=XPLMFindDataRef("ixeg/733/FMC/cdu1_5");
    hsxpl_fmc.key_6=XPLMFindDataRef("ixeg/733/FMC/cdu1_6");
    hsxpl_fmc.key_7=XPLMFindDataRef("ixeg/733/FMC/cdu1_7");
    hsxpl_fmc.key_8=XPLMFindDataRef("ixeg/733/FMC/cdu1_8");
    hsxpl_fmc.key_9=XPLMFindDataRef("ixeg/733/FMC/cdu1_9");
    hsxpl_fmc.key_0=XPLMFindDataRef("ixeg/733/FMC/cdu1_0");

    hsxpl_fmc.key_a=XPLMFindDataRef("ixeg/733/FMC/cdu1_A");
    hsxpl_fmc.key_b=XPLMFindDataRef("ixeg/733/FMC/cdu1_B");
    hsxpl_fmc.key_c=XPLMFindDataRef("ixeg/733/FMC/cdu1_C");
    hsxpl_fmc.key_d=XPLMFindDataRef("ixeg/733/FMC/cdu1_D");
    hsxpl_fmc.key_e=XPLMFindDataRef("ixeg/733/FMC/cdu1_E");
    hsxpl_fmc.key_f=XPLMFindDataRef("ixeg/733/FMC/cdu1_F");
    hsxpl_fmc.key_g=XPLMFindDataRef("ixeg/733/FMC/cdu1_G");
    hsxpl_fmc.key_h=XPLMFindDataRef("ixeg/733/FMC/cdu1_H");
    hsxpl_fmc.key_i=XPLMFindDataRef("ixeg/733/FMC/cdu1_I");
    hsxpl_fmc.key_j=XPLMFindDataRef("ixeg/733/FMC/cdu1_J");
    hsxpl_fmc.key_k=XPLMFindDataRef("ixeg/733/FMC/cdu1_K");
    hsxpl_fmc.key_l=XPLMFindDataRef("ixeg/733/FMC/cdu1_L");
    hsxpl_fmc.key_m=XPLMFindDataRef("ixeg/733/FMC/cdu1_M");
    hsxpl_fmc.key_n=XPLMFindDataRef("ixeg/733/FMC/cdu1_N");
    hsxpl_fmc.key_o=XPLMFindDataRef("ixeg/733/FMC/cdu1_O");
    hsxpl_fmc.key_p=XPLMFindDataRef("ixeg/733/FMC/cdu1_P");
    hsxpl_fmc.key_q=XPLMFindDataRef("ixeg/733/FMC/cdu1_Q");
    hsxpl_fmc.key_r=XPLMFindDataRef("ixeg/733/FMC/cdu1_R");
    hsxpl_fmc.key_s=XPLMFindDataRef("ixeg/733/FMC/cdu1_S");
    hsxpl_fmc.key_t=XPLMFindDataRef("ixeg/733/FMC/cdu1_T");
    hsxpl_fmc.key_u=XPLMFindDataRef("ixeg/733/FMC/cdu1_U");
    hsxpl_fmc.key_v=XPLMFindDataRef("ixeg/733/FMC/cdu1_V");
    hsxpl_fmc.key_w=XPLMFindDataRef("ixeg/733/FMC/cdu1_W");
    hsxpl_fmc.key_x=XPLMFindDataRef("ixeg/733/FMC/cdu1_X");
    hsxpl_fmc.key_y=XPLMFindDataRef("ixeg/733/FMC/cdu1_Y");
    hsxpl_fmc.key_z=XPLMFindDataRef("ixeg/733/FMC/cdu1_Z");

    hsxpl_fmc.key_slash=XPLMFindDataRef("ixeg/733/FMC/cdu1_slash");
    hsxpl_fmc.key_dot=XPLMFindDataRef("ixeg/733/FMC/cdu1_dot");
    hsxpl_fmc.key_plusminus=XPLMFindDataRef("ixeg/733/FMC/cdu1_plus");

    hsxpl_fmc.key_clr=XPLMFindCommand("ixeg/733/FMC/cdu1_clr");
    hsxpl_fmc.key_del=XPLMFindDataRef("ixeg/733/FMC/cdu1_del");

    hsxpl_fmc.key_init=XPLMFindDataRef("ixeg/733/FMC/cdu1_initref");
    hsxpl_fmc.key_rte=XPLMFindDataRef("ixeg/733/FMC/cdu1_rte");
    hsxpl_fmc.key_clb=XPLMFindDataRef("ixeg/733/FMC/cdu1_clb");
    hsxpl_fmc.key_crz=XPLMFindDataRef("ixeg/733/FMC/cdu1_crz");
    hsxpl_fmc.key_des=XPLMFindDataRef("ixeg/733/FMC/cdu1_des");
    hsxpl_fmc.key_menu=XPLMFindDataRef("ixeg/733/FMC/cdu1_menu");
    hsxpl_fmc.key_legs=XPLMFindDataRef("ixeg/733/FMC/cdu1_legs");
    hsxpl_fmc.key_deparr=XPLMFindDataRef("ixeg/733/FMC/cdu1_deparr");
    hsxpl_fmc.key_hold=XPLMFindDataRef("ixeg/733/FMC/cdu1_hold");
    hsxpl_fmc.key_prog=XPLMFindDataRef("ixeg/733/FMC/cdu1_prog");
    hsxpl_fmc.key_n1limit=XPLMFindDataRef("ixeg/733/FMC/cdu1_n1limit");
    hsxpl_fmc.key_fix=XPLMFindDataRef("ixeg/733/FMC/cdu1_fix");
    hsxpl_fmc.key_prevpage=XPLMFindDataRef("ixeg/733/FMC/cdu1_prev");
    hsxpl_fmc.key_nextpage=XPLMFindDataRef("ixeg/733/FMC/cdu1_next");
    hsxpl_fmc.key_exec=XPLMFindDataRef("ixeg/733/FMC/cdu1_exec");

    hsxpl_fmc.exec_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_exec_ann");
    hsxpl_fmc.ofst_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_ofst_ann");
    hsxpl_fmc.msg_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_msg_ann");
    hsxpl_fmc.fail_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu1_fail_ann");

    ix733_datarefs.title=XPLMFindDataRef("ixeg/733/FMC/cdu1D_title");
    ix733_datarefs.scratchPad=XPLMFindDataRef("ixeg/733/FMC/cdu1D_scrpad");
    ix733_datarefs.pageNo=XPLMFindDataRef("ixeg/733/FMC/cdu1D_pg_number");

    ix733_datarefs.llines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1L_t");
    ix733_datarefs.llines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1L_d");
    ix733_datarefs.llines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2L_t");
    ix733_datarefs.llines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2L_d");
    ix733_datarefs.llines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3L_t");
    ix733_datarefs.llines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3L_d");
    ix733_datarefs.llines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4L_t");
    ix733_datarefs.llines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4L_d");
    ix733_datarefs.llines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5L_t");
    ix733_datarefs.llines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5L_d");
    ix733_datarefs.llines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6L_t");
    ix733_datarefs.llines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6L_d");

    ix733_datarefs.rlines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1R_t");
    ix733_datarefs.rlines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line1R_d");
    ix733_datarefs.rlines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2R_t");
    ix733_datarefs.rlines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line2R_d");
    ix733_datarefs.rlines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3R_t");
    ix733_datarefs.rlines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line3R_d");
    ix733_datarefs.rlines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4R_t");
    ix733_datarefs.rlines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line4R_d");
    ix733_datarefs.rlines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5R_t");
    ix733_datarefs.rlines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line5R_d");
    ix733_datarefs.rlines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6R_t");
    ix733_datarefs.rlines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu1D_line6R_d");

  } else if (hsxpl_active_mcdu == 2) {

    hsxpl_fmc.key_lk1=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_1L");
    hsxpl_fmc.key_lk2=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_2L");
    hsxpl_fmc.key_lk3=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_3L");
    hsxpl_fmc.key_lk4=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_4L");
    hsxpl_fmc.key_lk5=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_5L");
    hsxpl_fmc.key_lk6=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_6L");

    hsxpl_fmc.key_rk1=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_1R");
    hsxpl_fmc.key_rk2=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_2R");
    hsxpl_fmc.key_rk3=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_3R");
    hsxpl_fmc.key_rk4=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_4R");
    hsxpl_fmc.key_rk5=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_5R");
    hsxpl_fmc.key_rk6=XPLMFindDataRef("ixeg/733/FMC/cdu2_lsk_6R");

    hsxpl_fmc.key_1=XPLMFindDataRef("ixeg/733/FMC/cdu2_1");
    hsxpl_fmc.key_2=XPLMFindDataRef("ixeg/733/FMC/cdu2_2");
    hsxpl_fmc.key_3=XPLMFindDataRef("ixeg/733/FMC/cdu2_3");
    hsxpl_fmc.key_4=XPLMFindDataRef("ixeg/733/FMC/cdu2_4");
    hsxpl_fmc.key_5=XPLMFindDataRef("ixeg/733/FMC/cdu2_5");
    hsxpl_fmc.key_6=XPLMFindDataRef("ixeg/733/FMC/cdu2_6");
    hsxpl_fmc.key_7=XPLMFindDataRef("ixeg/733/FMC/cdu2_7");
    hsxpl_fmc.key_8=XPLMFindDataRef("ixeg/733/FMC/cdu2_8");
    hsxpl_fmc.key_9=XPLMFindDataRef("ixeg/733/FMC/cdu2_9");
    hsxpl_fmc.key_0=XPLMFindDataRef("ixeg/733/FMC/cdu2_0");

    hsxpl_fmc.key_a=XPLMFindDataRef("ixeg/733/FMC/cdu2_A");
    hsxpl_fmc.key_b=XPLMFindDataRef("ixeg/733/FMC/cdu2_B");
    hsxpl_fmc.key_c=XPLMFindDataRef("ixeg/733/FMC/cdu2_C");
    hsxpl_fmc.key_d=XPLMFindDataRef("ixeg/733/FMC/cdu2_D");
    hsxpl_fmc.key_e=XPLMFindDataRef("ixeg/733/FMC/cdu2_E");
    hsxpl_fmc.key_f=XPLMFindDataRef("ixeg/733/FMC/cdu2_F");
    hsxpl_fmc.key_g=XPLMFindDataRef("ixeg/733/FMC/cdu2_G");
    hsxpl_fmc.key_h=XPLMFindDataRef("ixeg/733/FMC/cdu2_H");
    hsxpl_fmc.key_i=XPLMFindDataRef("ixeg/733/FMC/cdu2_I");
    hsxpl_fmc.key_j=XPLMFindDataRef("ixeg/733/FMC/cdu2_J");
    hsxpl_fmc.key_k=XPLMFindDataRef("ixeg/733/FMC/cdu2_K");
    hsxpl_fmc.key_l=XPLMFindDataRef("ixeg/733/FMC/cdu2_L");
    hsxpl_fmc.key_m=XPLMFindDataRef("ixeg/733/FMC/cdu2_M");
    hsxpl_fmc.key_n=XPLMFindDataRef("ixeg/733/FMC/cdu2_N");
    hsxpl_fmc.key_o=XPLMFindDataRef("ixeg/733/FMC/cdu2_O");
    hsxpl_fmc.key_p=XPLMFindDataRef("ixeg/733/FMC/cdu2_P");
    hsxpl_fmc.key_q=XPLMFindDataRef("ixeg/733/FMC/cdu2_Q");
    hsxpl_fmc.key_r=XPLMFindDataRef("ixeg/733/FMC/cdu2_R");
    hsxpl_fmc.key_s=XPLMFindDataRef("ixeg/733/FMC/cdu2_S");
    hsxpl_fmc.key_t=XPLMFindDataRef("ixeg/733/FMC/cdu2_T");
    hsxpl_fmc.key_u=XPLMFindDataRef("ixeg/733/FMC/cdu2_U");
    hsxpl_fmc.key_v=XPLMFindDataRef("ixeg/733/FMC/cdu2_V");
    hsxpl_fmc.key_w=XPLMFindDataRef("ixeg/733/FMC/cdu2_W");
    hsxpl_fmc.key_x=XPLMFindDataRef("ixeg/733/FMC/cdu2_X");
    hsxpl_fmc.key_y=XPLMFindDataRef("ixeg/733/FMC/cdu2_Y");
    hsxpl_fmc.key_z=XPLMFindDataRef("ixeg/733/FMC/cdu2_Z");

    hsxpl_fmc.key_slash=XPLMFindDataRef("ixeg/733/FMC/cdu2_slash");
    hsxpl_fmc.key_dot=XPLMFindDataRef("ixeg/733/FMC/cdu2_dot");
    hsxpl_fmc.key_plusminus=XPLMFindDataRef("ixeg/733/FMC/cdu2_plus");
    hsxpl_fmc.key_clr=XPLMFindCommand("ixeg/733/FMC/cdu2_clr");
    hsxpl_fmc.key_del=XPLMFindDataRef("ixeg/733/FMC/cdu2_del");

    hsxpl_fmc.key_init=XPLMFindDataRef("ixeg/733/FMC/cdu2_initref");
    hsxpl_fmc.key_rte=XPLMFindDataRef("ixeg/733/FMC/cdu2_rte");
    hsxpl_fmc.key_clb=XPLMFindDataRef("ixeg/733/FMC/cdu2_clb");
    hsxpl_fmc.key_crz=XPLMFindDataRef("ixeg/733/FMC/cdu2_crz");
    hsxpl_fmc.key_des=XPLMFindDataRef("ixeg/733/FMC/cdu2_des");
    hsxpl_fmc.key_menu=XPLMFindDataRef("ixeg/733/FMC/cdu2_menu");
    hsxpl_fmc.key_legs=XPLMFindDataRef("ixeg/733/FMC/cdu2_legs");
    hsxpl_fmc.key_deparr=XPLMFindDataRef("ixeg/733/FMC/cdu2_deparr");
    hsxpl_fmc.key_hold=XPLMFindDataRef("ixeg/733/FMC/cdu2_hold");
    hsxpl_fmc.key_prog=XPLMFindDataRef("ixeg/733/FMC/cdu2_prog");
    hsxpl_fmc.key_n1limit=XPLMFindDataRef("ixeg/733/FMC/cdu2_n1limit");
    hsxpl_fmc.key_fix=XPLMFindDataRef("ixeg/733/FMC/cdu2_fix");
    hsxpl_fmc.key_prevpage=XPLMFindDataRef("ixeg/733/FMC/cdu2_prev");
    hsxpl_fmc.key_nextpage=XPLMFindDataRef("ixeg/733/FMC/cdu2_next");
    hsxpl_fmc.key_exec=XPLMFindDataRef("ixeg/733/FMC/cdu2_exec");

    hsxpl_fmc.exec_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_exec_ann");
    hsxpl_fmc.ofst_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_ofst_ann");
    hsxpl_fmc.msg_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_msg_ann");
    hsxpl_fmc.fail_light_on=XPLMFindDataRef("ixeg/733/FMC/cdu2_fail_ann");


    ix733_datarefs.title=XPLMFindDataRef("ixeg/733/FMC/cdu2D_title");
    ix733_datarefs.scratchPad=XPLMFindDataRef("ixeg/733/FMC/cdu2D_scrpad");
    ix733_datarefs.pageNo=XPLMFindDataRef("ixeg/733/FMC/cdu2D_pg_number");

    ix733_datarefs.llines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1L_t");
    ix733_datarefs.llines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1L_d");
    ix733_datarefs.llines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2L_t");
    ix733_datarefs.llines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2L_d");
    ix733_datarefs.llines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3L_t");
    ix733_datarefs.llines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3L_d");
    ix733_datarefs.llines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4L_t");
    ix733_datarefs.llines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4L_d");
    ix733_datarefs.llines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5L_t");
    ix733_datarefs.llines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5L_d");
    ix733_datarefs.llines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6L_t");
    ix733_datarefs.llines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6L_d");

    ix733_datarefs.rlines[0]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1R_t");
    ix733_datarefs.rlines[1]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line1R_d");
    ix733_datarefs.rlines[2]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2R_t");
    ix733_datarefs.rlines[3]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line2R_d");
    ix733_datarefs.rlines[4]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3R_t");
    ix733_datarefs.rlines[5]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line3R_d");
    ix733_datarefs.rlines[6]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4R_t");
    ix733_datarefs.rlines[7]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line4R_d");
    ix733_datarefs.rlines[8]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5R_t");
    ix733_datarefs.rlines[9]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line5R_d");
    ix733_datarefs.rlines[10]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6R_t");
    ix733_datarefs.rlines[11]=XPLMFindDataRef("ixeg/733/FMC/cdu2D_line6R_d");

  }
}


void hsxpl_set_x737_datarefs(void) {

  /* x737 datarefs */
  memset(&x7datarefs,0,sizeof(struct x737_datarefs_s));

  /* Make sure we have an x737 status dataref */
  x7datarefs.pluginstatus=XPLMFindDataRef("x737/systems/afds/plugin_status");
  if(x7datarefs.pluginstatus==NULL) {
    return;
  }

  /* And that it is enabled */
  if(!(XPLMGetDatai(x7datarefs.pluginstatus))) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_X737;

}

void hsxpl_set_xfmc_datarefs(void) {

  if(XPLMFindDataRef("xfmc/Status") == NULL) {
    return;
  }
  if(XPLMGetDatai(XPLMFindDataRef("xfmc/Status"))==0) {
    return;
  }

  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_XFMC;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));
  memset(&xfmcdatarefs,0,sizeof(struct xfmc_datarefs_s));

  xfmcdatarefs.line[0]=XPLMFindDataRef("xfmc/Upper");
  xfmcdatarefs.line[2]=XPLMFindDataRef("xfmc/Panel_1");
  xfmcdatarefs.line[1]=XPLMFindDataRef("xfmc/Panel_2");
  xfmcdatarefs.line[4]=XPLMFindDataRef("xfmc/Panel_3");
  xfmcdatarefs.line[3]=XPLMFindDataRef("xfmc/Panel_4");
  xfmcdatarefs.line[6]=XPLMFindDataRef("xfmc/Panel_5");
  xfmcdatarefs.line[5]=XPLMFindDataRef("xfmc/Panel_6");
  xfmcdatarefs.line[8]=XPLMFindDataRef("xfmc/Panel_7");
  xfmcdatarefs.line[7]=XPLMFindDataRef("xfmc/Panel_8");
  xfmcdatarefs.line[10]=XPLMFindDataRef("xfmc/Panel_9");
  xfmcdatarefs.line[9]=XPLMFindDataRef("xfmc/Panel_10");
  xfmcdatarefs.line[12]=XPLMFindDataRef("xfmc/Panel_11");
  xfmcdatarefs.line[11]=XPLMFindDataRef("xfmc/Panel_12");
  xfmcdatarefs.line[13]=XPLMFindDataRef("xfmc/Scratch");

  xfmcdatarefs.status=XPLMFindDataRef("xfmc/Status");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("xfmc/Keypath");


  hsxpl_fmc.key_0=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_1=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_2=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_3=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_4=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_5=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_6=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_7=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_8=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_9=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_dot=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_a=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_b=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_c=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_d=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_e=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_f=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_g=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_h=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_i=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_j=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_k=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_l=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_m=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_n=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_o=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_p=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_q=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_r=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_s=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_t=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_u=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_v=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_w=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_x=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_y=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_z=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_space=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_del=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_slash=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_clr=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_init=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_fix=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_menu=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_prevpage=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rte=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_legs=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_radnav=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_hold=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_ap=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_perf=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_vnav=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_prog=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_exec=XPLMFindDataRef("xfmc/Keypath");
}


void hsxpl_set_ufmc_datarefs(void) {


  if(XPLMFindDataRef("SSG/UFMC/PRESENT") != NULL) {
    if(XPLMGetDataf(XPLMFindDataRef("SSG/UFMC/PRESENT"))!=0) {

      hsxpl_fmc_type = HSMP_FMC_TYPE_XP_UFMC;

      memset(&ufmcdatarefs,0,sizeof(struct ufmc_datarefs_s));
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

      ufmcdatarefs.line[0]=XPLMFindDataRef("SSG/UFMC/LINE_1");
      ufmcdatarefs.line[1]=XPLMFindDataRef("SSG/UFMC/LINE_2");
      ufmcdatarefs.line[2]=XPLMFindDataRef("SSG/UFMC/LINE_3");
      ufmcdatarefs.line[3]=XPLMFindDataRef("SSG/UFMC/LINE_4");
      ufmcdatarefs.line[4]=XPLMFindDataRef("SSG/UFMC/LINE_5");
      ufmcdatarefs.line[5]=XPLMFindDataRef("SSG/UFMC/LINE_6");
      ufmcdatarefs.line[6]=XPLMFindDataRef("SSG/UFMC/LINE_7");
      ufmcdatarefs.line[7]=XPLMFindDataRef("SSG/UFMC/LINE_8");
      ufmcdatarefs.line[8]=XPLMFindDataRef("SSG/UFMC/LINE_9");
      ufmcdatarefs.line[9]=XPLMFindDataRef("SSG/UFMC/LINE_10");
      ufmcdatarefs.line[10]=XPLMFindDataRef("SSG/UFMC/LINE_11");
      ufmcdatarefs.line[11]=XPLMFindDataRef("SSG/UFMC/LINE_12");
      ufmcdatarefs.line[12]=XPLMFindDataRef("SSG/UFMC/LINE_13");
      ufmcdatarefs.line[13]=XPLMFindDataRef("SSG/UFMC/LINE_14");

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

  memset(&ufmcdatarefs,0,sizeof(struct ufmc_datarefs_s));
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



    ufmcdatarefs.line[0]=XPLMFindDataRef("FJCC/UFMC/LINE_1");
    ufmcdatarefs.line[1]=XPLMFindDataRef("FJCC/UFMC/LINE_2");
    ufmcdatarefs.line[2]=XPLMFindDataRef("FJCC/UFMC/LINE_3");
    ufmcdatarefs.line[3]=XPLMFindDataRef("FJCC/UFMC/LINE_4");
    ufmcdatarefs.line[4]=XPLMFindDataRef("FJCC/UFMC/LINE_5");
    ufmcdatarefs.line[5]=XPLMFindDataRef("FJCC/UFMC/LINE_6");
    ufmcdatarefs.line[6]=XPLMFindDataRef("FJCC/UFMC/LINE_7");
    ufmcdatarefs.line[7]=XPLMFindDataRef("FJCC/UFMC/LINE_8");
    ufmcdatarefs.line[8]=XPLMFindDataRef("FJCC/UFMC/LINE_9");
    ufmcdatarefs.line[9]=XPLMFindDataRef("FJCC/UFMC/LINE_10");
    ufmcdatarefs.line[10]=XPLMFindDataRef("FJCC/UFMC/LINE_11");
    ufmcdatarefs.line[11]=XPLMFindDataRef("FJCC/UFMC/LINE_12");
    ufmcdatarefs.line[12]=XPLMFindDataRef("FJCC/UFMC/LINE_13");
    ufmcdatarefs.line[13]=XPLMFindDataRef("FJCC/UFMC/LINE_14");
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


    ufmcdatarefs.line[0]=XPLMFindDataRef("FJCC/UFMC_1/LINE_1");
    ufmcdatarefs.line[1]=XPLMFindDataRef("FJCC/UFMC_1/LINE_2");
    ufmcdatarefs.line[2]=XPLMFindDataRef("FJCC/UFMC_1/LINE_3");
    ufmcdatarefs.line[3]=XPLMFindDataRef("FJCC/UFMC_1/LINE_4");
    ufmcdatarefs.line[4]=XPLMFindDataRef("FJCC/UFMC_1/LINE_5");
    ufmcdatarefs.line[5]=XPLMFindDataRef("FJCC/UFMC_1/LINE_6");
    ufmcdatarefs.line[6]=XPLMFindDataRef("FJCC/UFMC_1/LINE_7");
    ufmcdatarefs.line[7]=XPLMFindDataRef("FJCC/UFMC_1/LINE_8");
    ufmcdatarefs.line[8]=XPLMFindDataRef("FJCC/UFMC_1/LINE_9");
    ufmcdatarefs.line[9]=XPLMFindDataRef("FJCC/UFMC_1/LINE_10");
    ufmcdatarefs.line[10]=XPLMFindDataRef("FJCC/UFMC_1/LINE_11");
    ufmcdatarefs.line[11]=XPLMFindDataRef("FJCC/UFMC_1/LINE_12");
    ufmcdatarefs.line[12]=XPLMFindDataRef("FJCC/UFMC_1/LINE_13");
    ufmcdatarefs.line[13]=XPLMFindDataRef("FJCC/UFMC_1/LINE_14");
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


void hsxpl_set_pcrj200_datarefs(void) {

  char acficao[8]; memset(acficao,0,8);
  if(xpdatarefs.acficao!=NULL)
    XPLMGetDatab(xpdatarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(xpdatarefs.tailno!=NULL)
    XPLMGetDatab(xpdatarefs.tailno,actfailno,0,7);

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

void hsxpl_set_p757_datarefs(void) {

  char acficao[8]; memset(acficao,0,8);
  if(xpdatarefs.acficao!=NULL)
    XPLMGetDatab(xpdatarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(xpdatarefs.tailno!=NULL)
    XPLMGetDatab(xpdatarefs.tailno,actfailno,0,7);

  int is757=0;
  if(XPLMFindDataRef("757Avionics/CDU/exec") != NULL) {
    if(!strncmp(acficao,"B752",7)) {
      /* if(!strncmp(actfailno,"RA7541",7)) { */
      is757=1;
      /* } */
    }
  }
  if(!is757) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_FF757;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_PB757;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_fmc.exec_light_on=XPLMFindDataRef("war/main/58");
  hsxpl_fmc.dspy_light_on=XPLMFindDataRef("war/main/51");
  hsxpl_fmc.fail_light_on=XPLMFindDataRef("war/main/52");
  hsxpl_fmc.msg_light_on=XPLMFindDataRef("war/main/53");
  hsxpl_fmc.ofst_light_on=XPLMFindDataRef("war/main/54");

  hsxpl_fmc.key_0=XPLMFindDataRef("757Avionics/CDU/0");
  hsxpl_fmc.key_1=XPLMFindDataRef("757Avionics/CDU/1");
  hsxpl_fmc.key_2=XPLMFindDataRef("757Avionics/CDU/2");
  hsxpl_fmc.key_3=XPLMFindDataRef("757Avionics/CDU/3");
  hsxpl_fmc.key_4=XPLMFindDataRef("757Avionics/CDU/4");
  hsxpl_fmc.key_5=XPLMFindDataRef("757Avionics/CDU/5");
  hsxpl_fmc.key_6=XPLMFindDataRef("757Avionics/CDU/6");
  hsxpl_fmc.key_7=XPLMFindDataRef("757Avionics/CDU/7");
  hsxpl_fmc.key_8=XPLMFindDataRef("757Avionics/CDU/8");
  hsxpl_fmc.key_9=XPLMFindDataRef("757Avionics/CDU/9");
  hsxpl_fmc.key_dot=XPLMFindDataRef("757Avionics/CDU/point");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("757Avionics/CDU/plusminus");

  hsxpl_fmc.key_a=XPLMFindDataRef("757Avionics/CDU/A");
  hsxpl_fmc.key_b=XPLMFindDataRef("757Avionics/CDU/B");
  hsxpl_fmc.key_c=XPLMFindDataRef("757Avionics/CDU/C");
  hsxpl_fmc.key_d=XPLMFindDataRef("757Avionics/CDU/D");
  hsxpl_fmc.key_e=XPLMFindDataRef("757Avionics/CDU/E");
  hsxpl_fmc.key_f=XPLMFindDataRef("757Avionics/CDU/F");
  hsxpl_fmc.key_g=XPLMFindDataRef("757Avionics/CDU/G");
  hsxpl_fmc.key_h=XPLMFindDataRef("757Avionics/CDU/H");
  hsxpl_fmc.key_i=XPLMFindDataRef("757Avionics/CDU/I");
  hsxpl_fmc.key_j=XPLMFindDataRef("757Avionics/CDU/J");
  hsxpl_fmc.key_k=XPLMFindDataRef("757Avionics/CDU/K");
  hsxpl_fmc.key_l=XPLMFindDataRef("757Avionics/CDU/L");
  hsxpl_fmc.key_m=XPLMFindDataRef("757Avionics/CDU/M");
  hsxpl_fmc.key_n=XPLMFindDataRef("757Avionics/CDU/N");
  hsxpl_fmc.key_o=XPLMFindDataRef("757Avionics/CDU/O");
  hsxpl_fmc.key_p=XPLMFindDataRef("757Avionics/CDU/P");
  hsxpl_fmc.key_q=XPLMFindDataRef("757Avionics/CDU/Q");
  hsxpl_fmc.key_r=XPLMFindDataRef("757Avionics/CDU/R");
  hsxpl_fmc.key_s=XPLMFindDataRef("757Avionics/CDU/S");
  hsxpl_fmc.key_t=XPLMFindDataRef("757Avionics/CDU/T");
  hsxpl_fmc.key_u=XPLMFindDataRef("757Avionics/CDU/U");
  hsxpl_fmc.key_v=XPLMFindDataRef("757Avionics/CDU/V");
  hsxpl_fmc.key_w=XPLMFindDataRef("757Avionics/CDU/W");
  hsxpl_fmc.key_x=XPLMFindDataRef("757Avionics/CDU/X");
  hsxpl_fmc.key_y=XPLMFindDataRef("757Avionics/CDU/Y");
  hsxpl_fmc.key_z=XPLMFindDataRef("757Avionics/CDU/Z");
  hsxpl_fmc.key_space=XPLMFindDataRef("757Avionics/CDU/space");
  hsxpl_fmc.key_slash=XPLMFindDataRef("757Avionics/CDU/slash");
  hsxpl_fmc.key_clr=XPLMFindDataRef("757Avionics/CDU/clear");
  hsxpl_fmc.key_del=XPLMFindDataRef("757Avionics/CDU/delete");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("757Avionics/CDU/LLSK1");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("757Avionics/CDU/LLSK2");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("757Avionics/CDU/LLSK3");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("757Avionics/CDU/LLSK4");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("757Avionics/CDU/LLSK5");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("757Avionics/CDU/LLSK6");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("757Avionics/CDU/RLSK1");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("757Avionics/CDU/RLSK2");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("757Avionics/CDU/RLSK3");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("757Avionics/CDU/RLSK4");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("757Avionics/CDU/RLSK5");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("757Avionics/CDU/RLSK6");

  hsxpl_fmc.key_init=XPLMFindDataRef("757Avionics/CDU/init_ref");
  hsxpl_fmc.key_rte=XPLMFindDataRef("757Avionics/CDU/rte");
  hsxpl_fmc.key_clb=XPLMFindDataRef("757Avionics/CDU/clb");
  hsxpl_fmc.key_crz=XPLMFindDataRef("757Avionics/CDU/crz");
  hsxpl_fmc.key_des=XPLMFindDataRef("757Avionics/CDU/des");

  hsxpl_fmc.key_dir=XPLMFindDataRef("757Avionics/CDU/dir");
  hsxpl_fmc.key_legs=XPLMFindDataRef("757Avionics/CDU/legs");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("757Avionics/CDU/dep_arr");
  hsxpl_fmc.key_hold=XPLMFindDataRef("757Avionics/CDU/hold");
  hsxpl_fmc.key_prog=XPLMFindDataRef("757Avionics/CDU/prog");
  hsxpl_fmc.key_exec=XPLMFindDataRef("757Avionics/CDU/exec");

  hsxpl_fmc.key_menu=XPLMFindDataRef("757Avionics/CDU/mcdu_menu");
  hsxpl_fmc.key_fix=XPLMFindDataRef("757Avionics/CDU/fix");
  hsxpl_fmc.key_prevpage=XPLMFindDataRef("757Avionics/CDU/prev_page");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("757Avionics/CDU/next_page");

}

void hsxpl_set_p767_datarefs(void) {

  if(XPLMFindDataRef("757Avionics/CDU/exec") == NULL) return;
  if(strncmp(hsxpl_acf_icao(),"B763",7)) return;

  hsxpl_plane_type = HSXPL_PLANE_FF767;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_PB757;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_fmc.exec_light_on=XPLMFindDataRef("war/main/58");
  hsxpl_fmc.dspy_light_on=XPLMFindDataRef("war/main/51");
  hsxpl_fmc.fail_light_on=XPLMFindDataRef("war/main/52");
  hsxpl_fmc.msg_light_on=XPLMFindDataRef("war/main/53");
  hsxpl_fmc.ofst_light_on=XPLMFindDataRef("war/main/54");

  hsxpl_fmc.key_0=XPLMFindDataRef("757Avionics/CDU/0");
  hsxpl_fmc.key_1=XPLMFindDataRef("757Avionics/CDU/1");
  hsxpl_fmc.key_2=XPLMFindDataRef("757Avionics/CDU/2");
  hsxpl_fmc.key_3=XPLMFindDataRef("757Avionics/CDU/3");
  hsxpl_fmc.key_4=XPLMFindDataRef("757Avionics/CDU/4");
  hsxpl_fmc.key_5=XPLMFindDataRef("757Avionics/CDU/5");
  hsxpl_fmc.key_6=XPLMFindDataRef("757Avionics/CDU/6");
  hsxpl_fmc.key_7=XPLMFindDataRef("757Avionics/CDU/7");
  hsxpl_fmc.key_8=XPLMFindDataRef("757Avionics/CDU/8");
  hsxpl_fmc.key_9=XPLMFindDataRef("757Avionics/CDU/9");
  hsxpl_fmc.key_dot=XPLMFindDataRef("757Avionics/CDU/point");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("757Avionics/CDU/plusminus");

  hsxpl_fmc.key_a=XPLMFindDataRef("757Avionics/CDU/A");
  hsxpl_fmc.key_b=XPLMFindDataRef("757Avionics/CDU/B");
  hsxpl_fmc.key_c=XPLMFindDataRef("757Avionics/CDU/C");
  hsxpl_fmc.key_d=XPLMFindDataRef("757Avionics/CDU/D");
  hsxpl_fmc.key_e=XPLMFindDataRef("757Avionics/CDU/E");
  hsxpl_fmc.key_f=XPLMFindDataRef("757Avionics/CDU/F");
  hsxpl_fmc.key_g=XPLMFindDataRef("757Avionics/CDU/G");
  hsxpl_fmc.key_h=XPLMFindDataRef("757Avionics/CDU/H");
  hsxpl_fmc.key_i=XPLMFindDataRef("757Avionics/CDU/I");
  hsxpl_fmc.key_j=XPLMFindDataRef("757Avionics/CDU/J");
  hsxpl_fmc.key_k=XPLMFindDataRef("757Avionics/CDU/K");
  hsxpl_fmc.key_l=XPLMFindDataRef("757Avionics/CDU/L");
  hsxpl_fmc.key_m=XPLMFindDataRef("757Avionics/CDU/M");
  hsxpl_fmc.key_n=XPLMFindDataRef("757Avionics/CDU/N");
  hsxpl_fmc.key_o=XPLMFindDataRef("757Avionics/CDU/O");
  hsxpl_fmc.key_p=XPLMFindDataRef("757Avionics/CDU/P");
  hsxpl_fmc.key_q=XPLMFindDataRef("757Avionics/CDU/Q");
  hsxpl_fmc.key_r=XPLMFindDataRef("757Avionics/CDU/R");
  hsxpl_fmc.key_s=XPLMFindDataRef("757Avionics/CDU/S");
  hsxpl_fmc.key_t=XPLMFindDataRef("757Avionics/CDU/T");
  hsxpl_fmc.key_u=XPLMFindDataRef("757Avionics/CDU/U");
  hsxpl_fmc.key_v=XPLMFindDataRef("757Avionics/CDU/V");
  hsxpl_fmc.key_w=XPLMFindDataRef("757Avionics/CDU/W");
  hsxpl_fmc.key_x=XPLMFindDataRef("757Avionics/CDU/X");
  hsxpl_fmc.key_y=XPLMFindDataRef("757Avionics/CDU/Y");
  hsxpl_fmc.key_z=XPLMFindDataRef("757Avionics/CDU/Z");
  hsxpl_fmc.key_space=XPLMFindDataRef("757Avionics/CDU/space");
  hsxpl_fmc.key_slash=XPLMFindDataRef("757Avionics/CDU/slash");
  hsxpl_fmc.key_clr=XPLMFindDataRef("757Avionics/CDU/clear");
  hsxpl_fmc.key_del=XPLMFindDataRef("757Avionics/CDU/delete");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("757Avionics/CDU/LLSK1");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("757Avionics/CDU/LLSK2");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("757Avionics/CDU/LLSK3");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("757Avionics/CDU/LLSK4");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("757Avionics/CDU/LLSK5");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("757Avionics/CDU/LLSK6");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("757Avionics/CDU/RLSK1");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("757Avionics/CDU/RLSK2");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("757Avionics/CDU/RLSK3");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("757Avionics/CDU/RLSK4");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("757Avionics/CDU/RLSK5");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("757Avionics/CDU/RLSK6");

  hsxpl_fmc.key_init=XPLMFindDataRef("757Avionics/CDU/init_ref");
  hsxpl_fmc.key_rte=XPLMFindDataRef("757Avionics/CDU/rte");
  hsxpl_fmc.key_clb=XPLMFindDataRef("757Avionics/CDU/clb");
  hsxpl_fmc.key_crz=XPLMFindDataRef("757Avionics/CDU/crz");
  hsxpl_fmc.key_des=XPLMFindDataRef("757Avionics/CDU/des");

  hsxpl_fmc.key_dir=XPLMFindDataRef("757Avionics/CDU/dir");
  hsxpl_fmc.key_legs=XPLMFindDataRef("757Avionics/CDU/legs");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("757Avionics/CDU/dep_arr");
  hsxpl_fmc.key_hold=XPLMFindDataRef("757Avionics/CDU/hold");
  hsxpl_fmc.key_prog=XPLMFindDataRef("757Avionics/CDU/prog");
  hsxpl_fmc.key_exec=XPLMFindDataRef("757Avionics/CDU/exec");

  hsxpl_fmc.key_menu=XPLMFindDataRef("757Avionics/CDU/mcdu_menu");
  hsxpl_fmc.key_fix=XPLMFindDataRef("757Avionics/CDU/fix");
  hsxpl_fmc.key_prevpage=XPLMFindDataRef("757Avionics/CDU/prev_page");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("757Avionics/CDU/next_page");

}


void hsxpl_set_p777_datarefs(void) {

  char acficao[8]; memset(acficao,0,8);
  if(xpdatarefs.acficao!=NULL)
    XPLMGetDatab(xpdatarefs.acficao,acficao,0,7);

  char actfailno[8]; memset(actfailno,0,8);
  if(xpdatarefs.tailno!=NULL)
    XPLMGetDatab(xpdatarefs.tailno,actfailno,0,7);

  int isT7=0;
  if(XPLMFindDataRef("T7Avionics/CDU/exec") != NULL) {
    if(!strncmp(acficao,"B77L",7) || !strncmp(acficao,"B77W",7) || !strncmp(acficao,"B772",7)) {
      /*if(!strncmp(actfailno,"YNKV41",7)) { */
      isT7=1;
      /* } */
    }
  }
  if(!isT7) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_FF777;
  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_PB777;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));

  hsxpl_fmc.exec_light_on=XPLMFindDataRef("lamps/208");
  hsxpl_fmc.dspy_light_on=XPLMFindDataRef("lamps/205");
  hsxpl_fmc.msg_light_on =XPLMFindDataRef("lamps/206");
  hsxpl_fmc.ofst_light_on=XPLMFindDataRef("lamps/207");

  hsxpl_fmc.key_0=XPLMFindDataRef("T7Avionics/CDU/0");
  hsxpl_fmc.key_1=XPLMFindDataRef("T7Avionics/CDU/1");
  hsxpl_fmc.key_2=XPLMFindDataRef("T7Avionics/CDU/2");
  hsxpl_fmc.key_3=XPLMFindDataRef("T7Avionics/CDU/3");
  hsxpl_fmc.key_4=XPLMFindDataRef("T7Avionics/CDU/4");
  hsxpl_fmc.key_5=XPLMFindDataRef("T7Avionics/CDU/5");
  hsxpl_fmc.key_6=XPLMFindDataRef("T7Avionics/CDU/6");
  hsxpl_fmc.key_7=XPLMFindDataRef("T7Avionics/CDU/7");
  hsxpl_fmc.key_8=XPLMFindDataRef("T7Avionics/CDU/8");
  hsxpl_fmc.key_9=XPLMFindDataRef("T7Avionics/CDU/9");
  hsxpl_fmc.key_dot=XPLMFindDataRef("T7Avionics/CDU/point");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("T7Avionics/CDU/plusminus");

  hsxpl_fmc.key_a=XPLMFindDataRef("T7Avionics/CDU/A");
  hsxpl_fmc.key_b=XPLMFindDataRef("T7Avionics/CDU/B");
  hsxpl_fmc.key_c=XPLMFindDataRef("T7Avionics/CDU/C");
  hsxpl_fmc.key_d=XPLMFindDataRef("T7Avionics/CDU/D");
  hsxpl_fmc.key_e=XPLMFindDataRef("T7Avionics/CDU/E");
  hsxpl_fmc.key_f=XPLMFindDataRef("T7Avionics/CDU/F");
  hsxpl_fmc.key_g=XPLMFindDataRef("T7Avionics/CDU/G");
  hsxpl_fmc.key_h=XPLMFindDataRef("T7Avionics/CDU/H");
  hsxpl_fmc.key_i=XPLMFindDataRef("T7Avionics/CDU/I");
  hsxpl_fmc.key_j=XPLMFindDataRef("T7Avionics/CDU/J");
  hsxpl_fmc.key_k=XPLMFindDataRef("T7Avionics/CDU/K");
  hsxpl_fmc.key_l=XPLMFindDataRef("T7Avionics/CDU/L");
  hsxpl_fmc.key_m=XPLMFindDataRef("T7Avionics/CDU/M");
  hsxpl_fmc.key_n=XPLMFindDataRef("T7Avionics/CDU/N");
  hsxpl_fmc.key_o=XPLMFindDataRef("T7Avionics/CDU/O");
  hsxpl_fmc.key_p=XPLMFindDataRef("T7Avionics/CDU/P");
  hsxpl_fmc.key_q=XPLMFindDataRef("T7Avionics/CDU/Q");
  hsxpl_fmc.key_r=XPLMFindDataRef("T7Avionics/CDU/R");
  hsxpl_fmc.key_s=XPLMFindDataRef("T7Avionics/CDU/S");
  hsxpl_fmc.key_t=XPLMFindDataRef("T7Avionics/CDU/T");
  hsxpl_fmc.key_u=XPLMFindDataRef("T7Avionics/CDU/U");
  hsxpl_fmc.key_v=XPLMFindDataRef("T7Avionics/CDU/V");
  hsxpl_fmc.key_w=XPLMFindDataRef("T7Avionics/CDU/W");
  hsxpl_fmc.key_x=XPLMFindDataRef("T7Avionics/CDU/X");
  hsxpl_fmc.key_y=XPLMFindDataRef("T7Avionics/CDU/Y");
  hsxpl_fmc.key_z=XPLMFindDataRef("T7Avionics/CDU/Z");
  hsxpl_fmc.key_space=XPLMFindDataRef("T7Avionics/CDU/space");
  hsxpl_fmc.key_slash=XPLMFindDataRef("T7Avionics/CDU/slash");
  hsxpl_fmc.key_clr=XPLMFindDataRef("T7Avionics/CDU/clear");
  hsxpl_fmc.key_del=XPLMFindDataRef("T7Avionics/CDU/delete");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("T7Avionics/CDU/LLSK1");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("T7Avionics/CDU/LLSK2");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("T7Avionics/CDU/LLSK3");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("T7Avionics/CDU/LLSK4");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("T7Avionics/CDU/LLSK5");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("T7Avionics/CDU/LLSK6");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("T7Avionics/CDU/RLSK1");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("T7Avionics/CDU/RLSK2");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("T7Avionics/CDU/RLSK3");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("T7Avionics/CDU/RLSK4");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("T7Avionics/CDU/RLSK5");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("T7Avionics/CDU/RLSK6");

  hsxpl_fmc.key_init=XPLMFindDataRef("T7Avionics/CDU/init_ref");
  hsxpl_fmc.key_rte=XPLMFindDataRef("T7Avionics/CDU/rte");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("T7Avionics/CDU/dep_arr");
  hsxpl_fmc.key_altn=XPLMFindDataRef("T7Avionics/CDU/altn");
  hsxpl_fmc.key_vnav=XPLMFindDataRef("T7Avionics/CDU/vnav");

  hsxpl_fmc.key_fix=XPLMFindDataRef("T7Avionics/CDU/fix");
  hsxpl_fmc.key_legs=XPLMFindDataRef("T7Avionics/CDU/legs");
  hsxpl_fmc.key_hold=XPLMFindDataRef("T7Avionics/CDU/hold");
  hsxpl_fmc.key_fmccom=XPLMFindDataRef("T7Avionics/CDU/fmc_comm");
  hsxpl_fmc.key_prog=XPLMFindDataRef("T7Avionics/CDU/prog");
  hsxpl_fmc.key_exec=XPLMFindDataRef("T7Avionics/CDU/exec");

  hsxpl_fmc.key_menu=XPLMFindDataRef("T7Avionics/CDU/mcdu_menu");
  hsxpl_fmc.key_radnav=XPLMFindDataRef("T7Avionics/CDU/nav_rad");

  hsxpl_fmc.key_prevpage=XPLMFindDataRef("T7Avionics/CDU/prev_page");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("T7Avionics/CDU/next_page");

}

void hsxpl_set_xsb_datarefs(void) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_set_xsb_datarefs()");
#endif

  memset(&xsbdatarefs,0,sizeof(struct xsb_datarefs_s));
  xsbdatarefs.callsign=XPLMFindDataRef("xsquawkbox/login/callsign");
}

void hsxpl_mcdu_toggle(void) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_mcdu_toggle()");
#endif

  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_A320Q || hsxpl_fmc_type==HSMP_FMC_TYPE_XP_IX733) {
    if(hsxpl_active_mcdu==1)
      hsxpl_active_mcdu=2;
    else
      hsxpl_active_mcdu=1;
    hsxpl_set_datarefs();
  }
}


void hsxpl_fmc_press_key(uint32_t mid,XPLMDataRef k) {

  if(k!=NULL) {

    if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_IX733) {

      if (k == hsxpl_fmc.key_clr) {
        XPLMCommandOnce(k);
      } else {
        XPLMSetDataf(k,1);
        XPLMSetDataf(k,0);
      }

    } else if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737 || hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737V5 || hsxpl_fmc_type==HSMP_FMC_TYPE_XP_UFMC) {

      if (hsxpl_plane_type == HSXPL_PLANE_IX733 ) {

        XPLMSetDataf(k,1);
        XPLMSetDataf(k,0);

      } else {

        if(x7datarefs.key_down!=NULL) {
          XPLMSetDataf(x7datarefs.key_down,0);
          x7datarefs.key_down=NULL;
        }

        XPLMSetDataf(k,1);

        if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737V5) {
          x7datarefs.key_down=k;
        }
      }

    } else if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_A320Q) {

      XPLMCommandOnce(k);

    } else if(hsxpl_fmc_type==HSMP_FMC_TYPE_XPLANE || hsxpl_fmc_type==HSMP_FMC_TYPE_XP_XP738) {

      XPLMCommandOnce(k);

    } else if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_XFMC) {

      switch(mid) {
        case(HSMP_MID_FMC_BUT_L01): XPLMSetDatai(k,0);break;
        case(HSMP_MID_FMC_BUT_L02): XPLMSetDatai(k,1);break;
        case(HSMP_MID_FMC_BUT_L03): XPLMSetDatai(k,2);break;
        case(HSMP_MID_FMC_BUT_L04): XPLMSetDatai(k,3);break;
        case(HSMP_MID_FMC_BUT_L05): XPLMSetDatai(k,4);break;
        case(HSMP_MID_FMC_BUT_L06): XPLMSetDatai(k,5);break;
        case(HSMP_MID_FMC_BUT_R01): XPLMSetDatai(k,6);break;
        case(HSMP_MID_FMC_BUT_R02): XPLMSetDatai(k,7);break;
        case(HSMP_MID_FMC_BUT_R03): XPLMSetDatai(k,8);break;
        case(HSMP_MID_FMC_BUT_R04): XPLMSetDatai(k,9);break;
        case(HSMP_MID_FMC_BUT_R05): XPLMSetDatai(k,10);break;
        case(HSMP_MID_FMC_BUT_R06): XPLMSetDatai(k,11);break;

        case(HSMP_MID_FMC_BUT_D0): XPLMSetDatai(k,67);break;
        case(HSMP_MID_FMC_BUT_D1): XPLMSetDatai(k,57);break;
        case(HSMP_MID_FMC_BUT_D2): XPLMSetDatai(k,58);break;
        case(HSMP_MID_FMC_BUT_D3): XPLMSetDatai(k,59);break;
        case(HSMP_MID_FMC_BUT_D4): XPLMSetDatai(k,60);break;
        case(HSMP_MID_FMC_BUT_D5): XPLMSetDatai(k,61);break;
        case(HSMP_MID_FMC_BUT_D6): XPLMSetDatai(k,62);break;
        case(HSMP_MID_FMC_BUT_D7): XPLMSetDatai(k,63);break;
        case(HSMP_MID_FMC_BUT_D8): XPLMSetDatai(k,64);break;
        case(HSMP_MID_FMC_BUT_D9): XPLMSetDatai(k,65);break;
        case(HSMP_MID_FMC_BUT_DOT): XPLMSetDatai(k,66);break;
        case(HSMP_MID_FMC_BUT_PLMN): XPLMSetDatai(k,68);break;

        case(HSMP_MID_FMC_BUT_A): XPLMSetDatai(k,27);break;
        case(HSMP_MID_FMC_BUT_B): XPLMSetDatai(k,28);break;
        case(HSMP_MID_FMC_BUT_C): XPLMSetDatai(k,29);break;
        case(HSMP_MID_FMC_BUT_D): XPLMSetDatai(k,30);break;
        case(HSMP_MID_FMC_BUT_E): XPLMSetDatai(k,31);break;
        case(HSMP_MID_FMC_BUT_F): XPLMSetDatai(k,32);break;
        case(HSMP_MID_FMC_BUT_G): XPLMSetDatai(k,33);break;
        case(HSMP_MID_FMC_BUT_H): XPLMSetDatai(k,34);break;
        case(HSMP_MID_FMC_BUT_I): XPLMSetDatai(k,35);break;
        case(HSMP_MID_FMC_BUT_J): XPLMSetDatai(k,36);break;
        case(HSMP_MID_FMC_BUT_K): XPLMSetDatai(k,37);break;
        case(HSMP_MID_FMC_BUT_L): XPLMSetDatai(k,38);break;
        case(HSMP_MID_FMC_BUT_M): XPLMSetDatai(k,39);break;
        case(HSMP_MID_FMC_BUT_N): XPLMSetDatai(k,40);break;
        case(HSMP_MID_FMC_BUT_O): XPLMSetDatai(k,41);break;
        case(HSMP_MID_FMC_BUT_P): XPLMSetDatai(k,42);break;
        case(HSMP_MID_FMC_BUT_Q): XPLMSetDatai(k,43);break;
        case(HSMP_MID_FMC_BUT_R): XPLMSetDatai(k,44);break;
        case(HSMP_MID_FMC_BUT_S): XPLMSetDatai(k,45);break;
        case(HSMP_MID_FMC_BUT_T): XPLMSetDatai(k,46);break;
        case(HSMP_MID_FMC_BUT_U): XPLMSetDatai(k,47);break;
        case(HSMP_MID_FMC_BUT_V): XPLMSetDatai(k,48);break;
        case(HSMP_MID_FMC_BUT_W): XPLMSetDatai(k,49);break;
        case(HSMP_MID_FMC_BUT_X): XPLMSetDatai(k,50);break;
        case(HSMP_MID_FMC_BUT_Y): XPLMSetDatai(k,51);break;
        case(HSMP_MID_FMC_BUT_Z): XPLMSetDatai(k,52);break;
        case(HSMP_MID_FMC_BUT_SPC): XPLMSetDatai(k,53);break;
        case(HSMP_MID_FMC_BUT_DEL): XPLMSetDatai(k,54);break;
        case(HSMP_MID_FMC_BUT_SLASH): XPLMSetDatai(k,55);break;
        case(HSMP_MID_FMC_BUT_CLR): XPLMSetDatai(k,56);break;

        case(HSMP_MID_FMC_BUT_INIT): XPLMSetDatai(k,12);break;
        case(HSMP_MID_FMC_BUT_FIX): XPLMSetDatai(k,17);break;
        case(HSMP_MID_FMC_BUT_MENU): XPLMSetDatai(k,23);break;
        case(HSMP_MID_FMC_BUT_PREVP): XPLMSetDatai(k,25);break;
        case(HSMP_MID_FMC_BUT_RTE): XPLMSetDatai(k,13);break;
        case(HSMP_MID_FMC_BUT_LEGS): XPLMSetDatai(k,18);break;
        case(HSMP_MID_FMC_BUT_RADNAV): XPLMSetDatai(k,24); break;
        case(HSMP_MID_FMC_BUT_NEXTP): XPLMSetDatai(k,26);break;

        case(HSMP_MID_FMC_BUT_DEPARR): XPLMSetDatai(k,14);break;
        case(HSMP_MID_FMC_BUT_HOLD): XPLMSetDatai(k,19);break;
        case(HSMP_MID_FMC_BUT_AP): { XPLMSetDatai(k,15); break; }
        case(HSMP_MID_FMC_BUT_PERF): XPLMSetDatai(k,20);break;
        case(HSMP_MID_FMC_BUT_VNAV): XPLMSetDatai(k,16);break;
        case(HSMP_MID_FMC_BUT_PROG): XPLMSetDatai(k,21);break;
        case(HSMP_MID_FMC_BUT_EXEC): XPLMSetDatai(k,22);break;
      }


    } else {
      XPLMSetDatai(k,1);
    }
  }
}

#pragma mark Widget functions

void hsxpl_select_menu_option(void *inMenuRef,void *inItemRef)
{

  if (!strcmp((char *) inItemRef, "Settings")) {

    if(hsxpl_settings_widget==NULL) {
      hsxpl_create_settings_widget(300,650,300,220);
      if(hsxpl_settings_widget==NULL) return;
    }

    if(!XPIsWidgetVisible(hsxpl_settings_widget))
      XPShowWidget(hsxpl_settings_widget);


  }

}

void hsxpl_create_settings_widget(int x, int y, int w, int h)
{


  /* AirTrack IP address part */

  char vstr[64];
  sprintf(vstr,"Haversine Air %s Settings",HSAIRXPL_VERSION);
  hsxpl_settings_widget = XPCreateWidget(x, y, x+w, y-h,1,vstr,1,NULL, xpWidgetClass_MainWindow);
  XPSetWidgetProperty(hsxpl_settings_widget, xpProperty_MainWindowHasCloseBoxes, 1);

  hsxpl_ipsubwindow = XPCreateWidget(x+15, y-30,x+w-15,y-200,1,"",0,hsxpl_settings_widget,xpWidgetClass_SubWindow);
  XPSetWidgetProperty(hsxpl_ipsubwindow, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);


  hsxpl_iplabel=XPCreateWidget(x+25,y-50,x+120,y-70,1,"AirApp IP Address: ",0,hsxpl_settings_widget, xpWidgetClass_Caption);

  if(hsxpl_unicast_sa.sin_addr.s_addr)
    hsxpl_iptxinput=XPCreateWidget(x+140, y-50, x+w-25, y-70,1, inet_ntoa(hsxpl_unicast_sa.sin_addr), 0, hsxpl_settings_widget,xpWidgetClass_TextField);
  else
    hsxpl_iptxinput=XPCreateWidget(x+140, y-50, x+w-25, y-70,1, "AUTO", 0, hsxpl_settings_widget,xpWidgetClass_TextField);
  XPSetWidgetProperty(hsxpl_iptxinput, xpProperty_TextFieldType, xpTextEntryField);

  hsxpl_iplabel2=XPCreateWidget(x+25,y-80,x+w-25,y-90,1,"This sets the IP address of the device",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_iplabel3=XPCreateWidget(x+25,y-100,x+w-25,y-110,1,"running an AirApp. You should leave this with",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_iplabel4=XPCreateWidget(x+25,y-120,x+w-25,y-130,1,"AUTO and set it only if you experience",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_iplabel5=XPCreateWidget(x+25,y-140,x+w-25,y-150,1,"connectivity problems.",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_ipapply = XPCreateWidget(x+50,y-170,x+w-50,y-180,1,"Apply",0,hsxpl_settings_widget,xpWidgetClass_Button);

  XPAddWidgetCallback(hsxpl_settings_widget,(XPWidgetFunc_t)hsxpl_settings_widget_handler);
  XPAddWidgetCallback(hsxpl_ipapply,(XPWidgetFunc_t)hsxpl_settings_widget_handler);
}



int hsxpl_settings_widget_handler(XPWidgetMessage inMessage,XPWidgetID inWidget,long inParam1,long inParam2)
{


  /* Close button pressed, only hide the widget, rather than destropying it. */
  if (inMessage == xpMessage_CloseButtonPushed && inWidget==hsxpl_settings_widget)
  {
    if (hsxpl_settings_widget!=NULL)
    {
      XPHideWidget(hsxpl_settings_widget);
    }
    return 1;
  }

  /* IP addr apply button pressed */
  if (inMessage == xpMsg_PushButtonPressed && inWidget==hsxpl_ipapply)
  {

    char buffer[256];
    struct in_addr a4;

    /* Get the IP address from the text box */
    XPGetWidgetDescriptor(hsxpl_iptxinput, buffer, sizeof(buffer));
    if(!strncmp(buffer,"AUTO",4)) {
      if(hsxpl_unicast_sa.sin_addr.s_addr) {
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
      }
      hsxpl_unicast_sa.sin_addr.s_addr=0;
      hsxpl_save_settings();
      return 1;
    }


    a4.s_addr=inet_addr(buffer);
    if(a4.s_addr==INADDR_NONE) {
      if(hsxpl_unicast_sa.sin_addr.s_addr) {
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
      }
      hsxpl_unicast_sa.sin_addr.s_addr=0;
      XPSetWidgetDescriptor(hsxpl_iptxinput, "AUTO");
      hsxpl_save_settings();
      return 1;
    }
    
    hsxpl_unicast_sa.sin_family=AF_INET;
    hsxpl_unicast_sa.sin_port=htons(HSMP_AIRTRACK_PORT);
    hsxpl_unicast_sa.sin_addr=a4;
    hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT,HSMP_PKT_NT_AIRTRACK|HSMP_PKT_PROTO_VER);
    hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT,HSMP_PKT_NT_AIRFMC|HSMP_PKT_PROTO_VER);
    hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT,HSMP_PKT_NT_AIREFB|HSMP_PKT_PROTO_VER);
    hsxpl_save_settings();
    return 1;
    
  }
  
  return 0;
}

#pragma mark Settings functions

#ifdef CPFLIGHT
extern char hsaircpf_serial_port[256];
#endif


void hsxpl_load_settings(void)
{
  
#ifdef CPFLIGHT
  memset(hsaircpf_serial_port,0,256);
#endif
  
#if !defined(_WIN32)
  FILE *fp=fopen("Resources/plugins/HaversineAir/hsairxpl.cfg","r");
  if(fp==NULL) fp=fopen("Resources/plugins/HaversineAir/hsairxpl.cfg","r");
#else
  FILE *fp=fopen("Resources\\plugins\\HaversineAir\\hsairxpl.cfg","r");
  if(fp==NULL) fp=fopen("Resources\\plugins\\HaversineAir\\hsairxpl.cfg","r");
#endif
  
  
  if(fp!=NULL) {
    char buffer[1024]; buffer[1023]='\0';
    while(fgets(buffer,1023,fp)!=NULL) {
      
      
      /* Remove leading \r\n */
      char *c=buffer;
      while(*c!='\0' && *c!='\r' && *c!='\n') c++;
      *c='\0';
      
#ifdef CPFLIGHT
      /* Test CPFLIGHT serial port setting */
      if(!memcmp(buffer,"CPFLIGHT-SERIAL-PORT=",strlen("CPFLIGHT-SERIAL-PORT="))) {
        
        hsxpl_cpflight_enabled=1;
        
        c=&buffer[strlen("CPFLIGHT-SERIAL-PORT=")];
        
        if(*c!='\0') {
          strncpy(hsaircpf_serial_port,c,255);
          if(!strcmp(hsaircpf_serial_port,"AUTO"))
            memset(hsaircpf_serial_port,0,256);
        }
      }
#endif
      
      /* Test broadcast mode */
      if(!memcmp(buffer,"DESTINATION-IP=",strlen("DESTINATION-IP="))) {
        
        c=&buffer[strlen("DESTINATION-IP=")];
        
        if(*c!='\0') {
          if(!memcmp(c,"AUTO",4)) {
            if(hsxpl_unicast_sa.sin_addr.s_addr) {
              hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
              hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
              hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
            }
            hsxpl_unicast_sa.sin_addr.s_addr=0;
          }
          else
          {
            struct in_addr a4;
            a4.s_addr=inet_addr(c);
            
            if(a4.s_addr==INADDR_NONE) {
              if(hsxpl_unicast_sa.sin_addr.s_addr) {
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
              }
              hsxpl_unicast_sa.sin_addr.s_addr=0;
              hsxpl_save_settings();
            } else {
              if(hsxpl_unicast_sa.sin_addr.s_addr) {
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
              }
              hsxpl_unicast_sa.sin_addr.s_addr=a4.s_addr;
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT,HSMP_PKT_NT_AIRTRACK|HSMP_PKT_PROTO_VER);
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT,HSMP_PKT_NT_AIRFMC|HSMP_PKT_PROTO_VER);
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT,HSMP_PKT_NT_AIREFB|HSMP_PKT_PROTO_VER);

              hsxpl_save_settings();
              
            }
          }
        }
      }
      
      
      
    }
    fclose(fp);
  }
  
}
void hsxpl_save_settings(void)
{
  
#if !defined(_WIN32)
  FILE *fp=fopen("Resources/plugins/HaversineAir/hsairxpl.cfg","w");
#else
  FILE *fp=fopen("Resources\\plugins\\HaversineAir\\hsairxpl.cfg","w");
#endif
  if(fp!=NULL) {
    
    /* Save broadcast mode */
    if(hsxpl_unicast_sa.sin_addr.s_addr)
      fprintf(fp,"DESTINATION-IP=%s\r\n",inet_ntoa(hsxpl_unicast_sa.sin_addr));
    else
      fprintf(fp,"DESTINATION-IP=AUTO\r\n");
    
#ifdef CPFLIGHT
    if(hsaircpf_serial_port[0]!='\0') {
      fprintf(fp,"CPFLIGHT-SERIAL-PORT=%s\r\n",hsaircpf_serial_port);
    }
#endif
    
    fclose(fp);
  }
}

