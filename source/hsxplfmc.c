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
 * FMC specifics.
 *
 */

#include <ctype.h>

#include "hsxpl.h"
#include "hsxplfmc.h"
#include "hsxplmisc.h"
#include "hsxplnavdb.h"
#include "hsmpnet.h"

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

extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;

extern hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;

/* Shared global var to hold FMC specific datarefs */
hsxpl_fmc_t hsxpl_fmc;

/* The active MCDU, 1 or 2 */
uint32_t hsxpl_active_mcdu=1;

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
        hsxpl_x737_fmc_press_key(k);
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
