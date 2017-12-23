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
 */

#include "hsxplatc.h"

#include "hsxpl.h"
#include "hsmpmsg.h"
#include "hsmpnet.h"
#include "hsxplmisc.h"
#include <string.h>
#include <sys/time.h>
#include <dirent.h>

/* The X-Plane SDK include files */

#include <XPLMProcessing.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMNavigation.h>
#include <XPLMMenus.h>
#include <XPLMDisplay.h>
#include <XPLMPlanes.h>
#include <XPLMPlugin.h>

/* A structure to hold all the datarefs in memory */
struct hsairxpl_atc_datarefs_s {
  XPLMDataRef rbearings;
  XPLMDataRef rdistances;
  XPLMDataRef raltitudes;
  XPLMDataRef latitudes[HSAIRPL_ATC_MAX_ACF];
  XPLMDataRef longitudes[HSAIRPL_ATC_MAX_ACF];
  XPLMDataRef elevations[HSAIRPL_ATC_MAX_ACF];
  XPLMDataRef headings[HSAIRPL_ATC_MAX_ACF];
  char callsigns[HSAIRPL_ATC_MAX_ACF][16];
  char icaos[HSAIRPL_ATC_MAX_ACF][8];
} hsairxpl_atc_datarefs;

struct hsairxpl_atc_wt_datarefs_s {
  XPLMDataRef numaircraft;
  XPLMDataRef acfid;
  XPLMDataRef acfelev;
  XPLMDataRef acfspeed;
  XPLMDataRef acfhdg;
  XPLMDataRef acflat;
  XPLMDataRef acflon;
  XPLMDataRef acftext;
} hsairxpl_atc_wt_datarefs;

struct hsairxpl_atc_xl_datarefs_s {
  XPLMDataRef gndTraffic;
  char *gndBuffer;
  size_t gndBufferSize;
  int numberOfAircraft;
} hsairxpl_atc_xl_datarefs;
int hsairxpl_atc_xl_datarefs_initialised=0;

/* The plugin in control of aircraft */
int hsairpl_atc_plugin_id(void) {
  XPLMPluginID plugid;
  int totalacf,activeacf;
  XPLMCountAircraft(&totalacf,&activeacf,&plugid);
  return (int)plugid;
}

/* The plugin signature in control of the aircraft */
char *hsairpl_atc_plugin_sig(void) {
  int pid=hsairpl_atc_plugin_id();
  static char sig[256];
  if(pid>=0) {
    XPLMGetPluginInfo(pid,NULL,NULL,sig,NULL);
    return sig;
  }
  return NULL;
}

/* The source of AI traffic */
int hsairpl_atc_source(void) {

  if(hsairpl_atc_wt_total_aircrafts()>0) {

    if(hsairxpl_atc_wt_datarefs.acfid!=NULL &&
       hsairxpl_atc_wt_datarefs.acfelev!=NULL &&
       hsairxpl_atc_wt_datarefs.acfspeed!=NULL &&
       hsairxpl_atc_wt_datarefs.acfhdg!=NULL &&
       hsairxpl_atc_wt_datarefs.acflat!=NULL &&
       hsairxpl_atc_wt_datarefs.acflon!=NULL) {
      return HSAIRPL_ATC_SOURCE_WORLDTRAFFIC;
    }
  }


  if(hsairpl_atc_plugin_id()<0) {
    if(hsairpl_atc_xpai_active_aircrafts()>0) {
      return HSAIRPL_ATC_SOURCE_XPLANE_AI;
    }
  } else {
    char *sig = hsairpl_atc_plugin_sig();
    if(!strcmp(sig,HSAIRPL_ATC_PG_SIGNATURE_XLIFE)) {
      return HSAIRPL_ATC_SOURCE_XLIFE;
    } else if(!strcmp(sig,HSAIRPL_ATC_PG_SIGNATURE_VATSIM)) {
      return HSAIRPL_ATC_SOURCE_VATSIM;
    }
  }

  return HSAIRPL_ATC_SOURCE_UNDEFINED;
}

/* The number of total user aircraft */
int hsairpl_atc_total_aircrafts() {
  switch(hsairpl_atc_source()) {
    case(HSAIRPL_ATC_SOURCE_XPLANE_AI): return hsairpl_atc_xpai_total_aircrafts(); break;
    case(HSAIRPL_ATC_SOURCE_VATSIM): return hsairpl_atc_xpai_total_aircrafts(); break;
    case(HSAIRPL_ATC_SOURCE_WORLDTRAFFIC): return hsairpl_atc_wt_total_aircrafts(); break;
    case(HSAIRPL_ATC_SOURCE_XLIFE): return hsairxpl_atc_xl_datarefs.numberOfAircraft; break;
  }
  return 0;
}

/* The number of active user aircraft */
int hsairpl_atc_active_aircrafts() {
  switch(hsairpl_atc_source()) {
    case(HSAIRPL_ATC_SOURCE_XPLANE_AI): return hsairpl_atc_xpai_active_aircrafts(); break;
    case(HSAIRPL_ATC_SOURCE_VATSIM): return hsairpl_atc_xpai_active_aircrafts(); break;
    case(HSAIRPL_ATC_SOURCE_WORLDTRAFFIC): return hsairpl_atc_wt_active_aircrafts(); break;
    case(HSAIRPL_ATC_SOURCE_XLIFE): return hsairxpl_atc_xl_datarefs.numberOfAircraft; break;
  }
  return 0;
}

int hsairpl_atc_wt_total_aircrafts(void) {
  if(hsairxpl_atc_wt_datarefs.numaircraft!=NULL) {
    return XPLMGetDatai(hsairxpl_atc_wt_datarefs.numaircraft);
  }
  return 0;
}
int hsairpl_atc_wt_active_aircrafts(void) {
  return hsairpl_atc_wt_total_aircrafts();
  /*
   if(hsairpl_atc_wt_total_aircrafts()>0) {
   if(hsairxpl_atc_wt_datarefs.acfid!=NULL) {
   return XPLMGetDatavi(hsairxpl_atc_wt_datarefs.acfid,NULL,0,0);
   }
   }
   return 0;
   */
}


int hsairpl_atc_xpai_total_aircrafts(void) {

  XPLMPluginID plugid;
  int totalacf,activeacf;
  XPLMCountAircraft(&totalacf,&activeacf,&plugid);
  if(totalacf>HSAIRPL_ATC_MAX_ACF) return HSAIRPL_ATC_MAX_ACF;
  return totalacf - 1;
}

/* The number of active aircraft */
int hsairpl_atc_xpai_active_aircrafts(void) {
  return hsairpl_atc_xpai_total_aircrafts();
/*  XPLMPluginID plugid;
  int totalacf,activeacf;
  XPLMCountAircraft(&totalacf,&activeacf,&plugid);
  if(activeacf>HSAIRPL_ATC_MAX_ACF) return HSAIRPL_ATC_MAX_ACF;
  return activeacf - 1;
*/

}


int hsairpl_atc_read_acf_info_read=0;

void hsairpl_atc_read_acf_info(void) {

  int source=hsairpl_atc_source();
  if(source==HSAIRPL_ATC_SOURCE_XPLANE_AI || source==HSAIRPL_ATC_SOURCE_VATSIM) {

    int i;
    int total_acfs=hsairpl_atc_total_aircrafts();

    for(i=0;i<total_acfs;i++) {
      char outname[256];char outpath[512];char str[512];
      XPLMGetNthAircraftModel(i+1,outname,outpath);
      if(outpath[0]!='\0') {
        FILE *fp=fopen(str,"r");
        if(fp==NULL) {
          char *cp=outpath; while(*cp!='\0') { if (*cp==':') *cp='/'; cp++; }
          fp=fopen(outpath,"r");
          if(fp==NULL) {
            cp=outpath; while(*cp!='/' && *cp!='\0') cp++;
            if(*cp=='/') {
              cp++;
              fp=fopen(cp,"r");
              if(fp==NULL && *cp!='/' && *cp!='\0') {
                char outpath2[1024];
                sprintf(outpath2,"/%s",cp);
                fp=fopen(outpath2,"r");
              }
            }
          }
        }
        if(fp!=NULL) {
          char line[1024]; line[1023]='\0';
          while(fgets(line,1023,fp)) {
            char w[1024];char w2[1024];
            if(hsxpl_strentry(2,line,' ',w)!=NULL) {
              if(!strncmp(w,"acf/_ICAO",16)) {
                if(hsxpl_strentry(3,line,' ',w2)!=NULL) {
                  strncpy(hsairxpl_atc_datarefs.icaos[i],w2,7);
                }
              } else if(!strncmp(w,"acf/_callsign",16)) {
                if(hsxpl_strentry(3,line,' ',w2)!=NULL) {
                  strncpy(hsairxpl_atc_datarefs.callsigns[i],w2,15);
                }
              }
            }
            if(hsairxpl_atc_datarefs.callsigns[i][0]!='\0' &&
               hsairxpl_atc_datarefs.icaos[i][0]!='\0')  {
              break;
            }
          }
          fclose(fp);
        }
      }
    }
    hsairpl_atc_read_acf_info_read=1;
  }
}


/* Updates or configures the datarefs */
void hsairpl_atc_update_datarefs(void) {

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsairpl_atc_update_datarefs()");
#endif

  /* X-Plane AI ; default */
  memset(&hsairxpl_atc_datarefs,0,sizeof(struct hsairxpl_atc_datarefs_s ));

  hsairxpl_atc_datarefs.rbearings=XPLMFindDataRef("sim/cockpit2/tcas/indicators/relative_bearing_degs");
  hsairxpl_atc_datarefs.rdistances=XPLMFindDataRef("sim/cockpit2/tcas/indicators/relative_distance_mtrs");
  hsairxpl_atc_datarefs.raltitudes=XPLMFindDataRef("sim/cockpit2/tcas/indicators/relative_altitude_mtrs");


  int i; char str[512];
  for(i=0;i<HSAIRPL_ATC_MAX_ACF;i++) {
    sprintf(str,"sim/multiplayer/position/plane%d_lat",i+1);
    hsairxpl_atc_datarefs.latitudes[i]=XPLMFindDataRef(str);
    sprintf(str,"sim/multiplayer/position/plane%d_lon",i+1);
    hsairxpl_atc_datarefs.longitudes[i]=XPLMFindDataRef(str);
    sprintf(str,"sim/multiplayer/position/plane%d_el",i+1);
    hsairxpl_atc_datarefs.elevations[i]=XPLMFindDataRef(str);
    sprintf(str,"sim/multiplayer/position/plane%d_psi",i+1);
    hsairxpl_atc_datarefs.headings[i]=XPLMFindDataRef(str);
  }

  hsairpl_atc_read_acf_info_read=0;

  /* World Traffic */
  memset(&hsairxpl_atc_wt_datarefs,0,sizeof(struct hsairxpl_atc_wt_datarefs_s ));
  hsairxpl_atc_wt_datarefs.numaircraft=XPLMFindDataRef("cjs/world_traffic/num_aircraft");
  hsairxpl_atc_wt_datarefs.acfid=XPLMFindDataRef("cjs/world_traffic/id");
  hsairxpl_atc_wt_datarefs.acfelev=XPLMFindDataRef("cjs/world_traffic/alt_asl");
  hsairxpl_atc_wt_datarefs.acfspeed=XPLMFindDataRef("cjs/world_traffic/speed_kias");
  hsairxpl_atc_wt_datarefs.acfhdg=XPLMFindDataRef("cjs/world_traffic/heading_degT");
  hsairxpl_atc_wt_datarefs.acflat=XPLMFindDataRef("cjs/world_traffic/aircraft_lat");
  hsairxpl_atc_wt_datarefs.acflon=XPLMFindDataRef("cjs/world_traffic/aircraft_lon");
  hsairxpl_atc_wt_datarefs.acftext=XPLMFindDataRef("cjs/world_traffic/num_aircraft");

  /* X-Life */
  if(!hsairxpl_atc_xl_datarefs_initialised) {
    memset(&hsairxpl_atc_xl_datarefs,0,sizeof(struct hsairxpl_atc_xl_datarefs_s ));
    hsairxpl_atc_xl_datarefs_initialised=1;
  } else {
    if(hsairxpl_atc_xl_datarefs.gndBuffer!=NULL) {
      free(hsairxpl_atc_xl_datarefs.gndBuffer);
      hsairxpl_atc_xl_datarefs.gndBuffer=NULL;
    }
    hsairxpl_atc_xl_datarefs.gndBufferSize=0;
  }

  hsairxpl_atc_xl_datarefs.gndTraffic=XPLMFindDataRef("jd/xlife/gnd_trf_data");
  if(hsairxpl_atc_xl_datarefs.gndTraffic==NULL) {
    hsairxpl_atc_xl_datarefs.gndTraffic= XPLMRegisterDataAccessor("jd/xlife/gnd_trf_data",
                                                                  xplmType_Data,                                  /* The types we support */
                                                                  1,                                              /* Writable */
                                                                  NULL, NULL,                                     /* Integer accessors */
                                                                  NULL, NULL,                                     /* Float accessors */
                                                                  NULL, NULL,                                     /* Doubles accessors */
                                                                  NULL, NULL,                                     /* Int array accessors */
                                                                  NULL, NULL,                                     /* Float array accessors */
                                                                  hsairpl_atc_xl_get_tfc, hsairpl_atc_xl_set_tfc, /* Raw data accessors */
                                                                  NULL, NULL);                                    /* Refcons not used */
  }
}


int hsairpl_atc_xl_get_tfc(void *inRefcon,void *outValue,int inOffset,int inMaxLength) {

  return 1048576; /* 1024*1024 */
  /*
  if(outValue==NULL) {
    return 1048576; // 1024*1024
  }
  return 0;
 */

}


void	hsairpl_atc_xl_set_tfc(void *inRefcon,void *inValue,int inOffset,int inLength) {
  if(hsairxpl_atc_xl_datarefs.gndBufferSize<(size_t)inLength) {
    if(hsairxpl_atc_xl_datarefs.gndBuffer!=NULL) {
      free(hsairxpl_atc_xl_datarefs.gndBuffer);
    }
    hsairxpl_atc_xl_datarefs.gndBuffer=calloc(1,(size_t)inLength+1);
    if(hsairxpl_atc_xl_datarefs.gndBuffer!=NULL) {
      hsairxpl_atc_xl_datarefs.gndBufferSize=inLength;
    } else {
      hsairxpl_atc_xl_datarefs.gndBufferSize=0;
    }
  }
  if(hsairxpl_atc_xl_datarefs.gndBuffer!=NULL) {
    memcpy(hsairxpl_atc_xl_datarefs.gndBuffer,inValue,inLength);
  }

}

/* Returns a timestamp in microseconds since 1970 */
uint64_t hsairpl_atc_get_timestamp(void) {
  uint64_t timestamp=0;
  struct timeval tp;
  if(!gettimeofday(&tp,NULL)) {
    uint64_t tsec=(uint64_t)tp.tv_sec;
    uint64_t tusec=(uint64_t)tp.tv_usec;
    timestamp = tusec + (tsec * 1000000);
  }
  return timestamp;
}


/* Sends all surrounding acf to peers */
void hsairpl_atc_send_traffic(void) {


  int source = hsairpl_atc_source();

  if(source==HSAIRPL_ATC_SOURCE_WORLDTRAFFIC) {
    hsairpl_atc_wt_send_traffic();
  } else if(source==HSAIRPL_ATC_SOURCE_XLIFE) {
    hsairpl_atc_xl_send_traffic();
  } else if(source==HSAIRPL_ATC_SOURCE_XPLANE_AI || source==HSAIRPL_ATC_SOURCE_VATSIM) {
    hsairpl_atc_xpai_send_traffic();
  }

}

void hsairpl_atc_xl_send_traffic(void) {


  if(hsairxpl_atc_xl_datarefs.gndTraffic!=NULL) {
    if(hsairxpl_atc_xl_datarefs.gndBufferSize>0 && hsairxpl_atc_xl_datarefs.gndBuffer!=NULL) {
      char buffer[hsairxpl_atc_xl_datarefs.gndBufferSize];
      int i=1;
      hsairxpl_atc_xl_datarefs.numberOfAircraft=0;

      hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
      int n=0; int psize=1400;
      if(pkt!=NULL) {

      do {
        if(hsxpl_strentry(i,hsairxpl_atc_xl_datarefs.gndBuffer,'/',buffer)!=NULL) {
          if(buffer[0]!='\0') {
            char word[strlen(buffer)+1];
            if(hsxpl_strentry(7,buffer,',',word)!=NULL) {
              hsmp_atc_pos_t acf; memset(&acf,0,sizeof(acf));
              acf.xprcode=atoi(word);
              if(hsxpl_strentry(8,buffer,',',word)!=NULL) {
                int i = atoi(word);
                switch(i) {
                  case(12): acf.stage=HSMP_ATC_POS_STAGE_START; break;
                  case(10): acf.stage=HSMP_ATC_POS_STAGE_PARK; break;
                  case(9): acf.stage=HSMP_ATC_POS_STAGE_LANDING; break;
                  case(8): acf.stage=HSMP_ATC_POS_STAGE_LANDING|HSMP_ATC_POS_STAGE_AIRBORNE; break;
                  case(7): acf.stage=HSMP_ATC_POS_STAGE_AIRBORNE; break;
                  case(6): acf.stage=HSMP_ATC_POS_STAGE_TAXI; break;
                  case(5): acf.stage=HSMP_ATC_POS_STAGE_PUSHBACK; break;
                  case(4): acf.stage=HSMP_ATC_POS_STAGE_PUSHBACK; break;
                  case(3): acf.stage=HSMP_ATC_POS_STAGE_PUSHBACK; break;
                  case(2): acf.stage=HSMP_ATC_POS_STAGE_GATE|HSMP_ATC_POS_STAGE_PARK; break;
                  default: acf.stage=HSMP_ATC_POS_STAGE_UNDEF;break;
                }
              } else {
                acf.stage=HSMP_ATC_POS_STAGE_NA;
              }
              if(hsxpl_strentry(6,buffer,',',word)!=NULL) {
                strncpy(acf.icao,word,7);
              }
              if(hsxpl_strentry(5,buffer,',',word)!=NULL) {
                strncpy(acf.callsign,word,15);
              }
              if(hsxpl_strentry(4,buffer,',',word)!=NULL) {
                acf.elevation=(float)atof(word);
              }
              if(hsxpl_strentry(3,buffer,',',word)!=NULL) {
                acf.heading=(float)atof(word);
              }
              if(hsxpl_strentry(2,buffer,',',word)!=NULL) {
                acf.longitude=(double)atof(word);
              }
              if(hsxpl_strentry(1,buffer,',',word)!=NULL) {
                acf.latitude=(double)atof(word);
              }
              acf.speed=-1;
              acf.reserved=0;
              acf.timestamp=hsairpl_atc_get_timestamp();
              hsairxpl_atc_xl_datarefs.numberOfAircraft++;

              hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AT_POS_ACF,&acf);
              n++;
              psize -= (int)sizeof(hsmp_atc_pos_t);
              if(psize < (int)sizeof(hsmp_atc_pos_t)) {
                if(n>0) {
                  hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIREFB);
                }
                free(pkt);
                n=0;
                pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
                if(pkt==NULL) break;
              }
            }
          } else {
            break;
          }
        } else {
          break;
        }
        i++;
      } while(1);
        if(n>0 && pkt!=NULL) {
          hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIREFB);
        }
        free(pkt);
      }
    }
  }
}

void hsairpl_atc_wt_send_traffic(void) {

  /* char str[512]; */
  int number_of_acfs=hsairpl_atc_wt_active_aircrafts();

  if(number_of_acfs>0) {

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    int n=0; int psize=1400;
    if(pkt!=NULL) {
      int i;
      for(i=1;i<number_of_acfs;i++) {
        int aid=0; float lat=0.0; float lon=0.0; float elev=0.0; float hdg=0.0; float spd=0.0;
        if(XPLMGetDatavi(hsairxpl_atc_wt_datarefs.acfid,&aid,i,1)==1) {
          if(XPLMGetDatavf(hsairxpl_atc_wt_datarefs.acflat,&lat,i,1)==1) {
            if(XPLMGetDatavf(hsairxpl_atc_wt_datarefs.acflon,&lon,i,1)==1) {
              if(lat==0.0 && lon==0.0) break; /* End of array detected */
              if(aid==0) continue; /* Skip ourselves */
              if(XPLMGetDatavf(hsairxpl_atc_wt_datarefs.acfelev,&elev,i,1)==1) {
                if(XPLMGetDatavf(hsairxpl_atc_wt_datarefs.acfhdg,&hdg,i,1)==1) {
                  if(XPLMGetDatavf(hsairxpl_atc_wt_datarefs.acfspeed,&spd,i,1)==1) {
                    char text[128]; memset(text,0,128);
                    char *ptr=NULL;
                    if(XPLMGetDatab(hsairxpl_atc_wt_datarefs.acftext,&ptr,i,1)==1) {
                      if(ptr!=NULL) {
                        strncpy(text,ptr,127);
                      }
                    }
                    hsmp_atc_pos_t acf;
                    acf.xprcode=aid;
                    acf.callsign[0]='\0';
                    acf.icao[0]='\0';
                    acf.latitude=(double)lat;
                    acf.longitude=(double)lon;
                    acf.elevation=elev*0.3048;
                    acf.heading=hdg;
                    acf.speed=spd;
                    acf.stage=HSMP_ATC_POS_STAGE_NA;
                    acf.reserved=0;
                    acf.timestamp=hsairpl_atc_get_timestamp();
                    hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AT_POS_ACF,&acf);
                    n++;
                    psize -= (int)sizeof(hsmp_atc_pos_t);
                    if(psize < (int)sizeof(hsmp_atc_pos_t)) {
                      if(n>0) {
                        hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIREFB);
                      }
                      free(pkt);
                      n=0;
                      pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
                      if(pkt==NULL) break;
                    }
                  }
                }
              }
            }
          }
        }
      }
      if(n>0 && pkt!=NULL) {
        hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIREFB);
      }
      free(pkt);
    }
  }
}


void hsairpl_atc_xpai_send_traffic(void) {

    int number_of_acfs=hsairpl_atc_active_aircrafts();

    if(!hsairpl_atc_read_acf_info_read)
      hsairpl_atc_read_acf_info();

    if(hsairxpl_atc_datarefs.rbearings!=NULL && hsairxpl_atc_datarefs.rdistances!=NULL &&
       hsairxpl_atc_datarefs.raltitudes!=NULL) {

      int n1=0; int n2=0; int n3=0;
      float rbearings[number_of_acfs];
      float raltitudes[number_of_acfs];
      float rdistances[number_of_acfs];

      n1=XPLMGetDatavf(hsairxpl_atc_datarefs.rbearings,rbearings,0,number_of_acfs);
      n2=XPLMGetDatavf(hsairxpl_atc_datarefs.rdistances,rdistances,0,number_of_acfs);
      n3=XPLMGetDatavf(hsairxpl_atc_datarefs.raltitudes,raltitudes,0,number_of_acfs);

      if(n1>0 && n2>0 && n3>0) {
        if(n1==n2 && n2==n3) {
          hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
          if(pkt!=NULL) {
            int i;
            for(i=0;i<n1;i++) {
              if(rdistances[i]>0) {
                hsmp_tca_acf_t acf;
                acf.xprcode=i;
                acf.callsign[0]='\0';strcpy(acf.callsign,hsairxpl_atc_datarefs.callsigns[i]);
                acf.icao[0]='\0';strcpy(acf.icao,hsairxpl_atc_datarefs.icaos[i]);
                acf.rbearing=rbearings[i];
                acf.rdistance=rdistances[i];
                acf.raltitude=raltitudes[i];
                acf.timestamp=hsairpl_atc_get_timestamp();
                hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AT_TCA_ACF,&acf);
              }
            }
            hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRTRACK);
            free(pkt);
          }
        }
      }
    }

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {

      int i; int n=0;
      for(i=0;i<number_of_acfs;i++) {
        if(hsairxpl_atc_datarefs.latitudes[i]!=NULL && hsairxpl_atc_datarefs.longitudes[i]!=NULL &&
           hsairxpl_atc_datarefs.elevations[i]!=NULL && hsairxpl_atc_datarefs.headings[i]!=NULL) {
          hsmp_atc_pos_t acf;
          acf.xprcode=i;
          acf.callsign[0]='\0';strcpy(acf.callsign,hsairxpl_atc_datarefs.callsigns[i]);
          acf.icao[0]='\0';strcpy(acf.icao,hsairxpl_atc_datarefs.icaos[i]);
          acf.latitude=XPLMGetDatad(hsairxpl_atc_datarefs.latitudes[i]);
          acf.longitude=XPLMGetDatad(hsairxpl_atc_datarefs.longitudes[i]);
          acf.elevation=(float)XPLMGetDatad(hsairxpl_atc_datarefs.elevations[i]);
          acf.speed=-1;
          acf.stage=HSMP_ATC_POS_STAGE_NA;
          acf.reserved=0;
          acf.heading=XPLMGetDataf(hsairxpl_atc_datarefs.headings[i]);
          acf.timestamp=hsairpl_atc_get_timestamp();
          hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_AT_POS_ACF,&acf);
          n++;
        }
      }
      if(n>0) {
        hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIREFB);
      }
      free(pkt);
    }
}
