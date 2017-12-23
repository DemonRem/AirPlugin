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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMNavigation.h>

#include "hsmpnet.h"
#include "hsmpmsg.h"
#include "hsairxplnavdb.h"
#include "hsairxplmisc.h"

/* Our flight plan */
hsmp_route_t hsxpl_route;
char logstr[512];
extern uint32_t hsxpl_fmc_type;

/* Local variable for storing type of FMC DV */
uint32_t hsxpl_navdb_fmc_db_type=0;

uint32_t hsxpl_navdb_route_feedback_enabled=0;

/* The type of FMC, 1 for x-plane default, 2 x-plane advanced, 3 for third party */
uint32_t hsxpl_navdb_fmc_type(void) {

  if(hsxpl_navdb_fmc_db_type == 0) {
    hsxpl_navdb_route_feedback_enabled=0;
    if(hsxpl_fmc_type==HSMP_FMC_TYPE_XPLANE) {
      hsxpl_navdb_fmc_db_type=2;
      float lat,lon;
      int altitude;
      XPLMNavRef navref;
      XPLMNavType navtype;
      XPLMGetFMSEntryInfo(0,&navtype,NULL,&navref,&altitude,&lat,&lon);
       if(navref != XPLM_NAV_NOT_FOUND) {
        if(navtype==0 && altitude==0 && lat==0.0 && lon==0.0) {
          hsxpl_navdb_fmc_db_type=1;
        }
      }
    } else {
      hsxpl_navdb_fmc_db_type=3;
    }
  }
  return hsxpl_navdb_fmc_db_type;
}

/* Resets the navdb fmc type so it can be determined again at runtime */
void hsxpl_navdb_reset_fmc_type(void) {
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_navdb_reset_fmc_type()");
#endif
  hsxpl_navdb_fmc_db_type=0;
  hsxpl_navdb_route_feedback_enabled=0;
}

/* An initialisation function, sets route to empty */
void hsxpl_navdb_clear_route(void) {

  hsxpl_log(HSXPLDEBUG_ACTION,"hsmp_navdb_clear_route()");

  if(hsxpl_navdb_fmc_db_type==1 || hsxpl_navdb_fmc_db_type==2) {
    int32_t j,nentries=(int32_t)XPLMCountFMSEntries();
    for(j=nentries-1;j>=0;j--) {
      XPLMClearFMSEntry(j);
    }
  }

  memset(&hsxpl_route,0,sizeof(hsmp_route_t));
  if(hsxpl_navdb_fmc_db_type==3) {
    hsxpl_navdb_route_feedback_enabled=0;
  }
}

/* Sets the current leg to a new value, -1 for none */
void hsxpl_navdb_set_current_leg(int32_t newleg) {

#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_navdb_set_current_leg() %d -> %d",hsxpl_route.cleg,newleg);
  hsxpl_log(HSXPLDEBUG_ACTION,logstr);
#endif

  /* Auto-correct current leg to -1 if 0, we should never have 0 */
  if(newleg==0) newleg=-1;

  hsxpl_route.cleg=newleg;

  if(hsxpl_navdb_fmc_db_type==1 || hsxpl_navdb_fmc_db_type==2) {
    if(hsxpl_route.cleg>0) {
      XPLMSetDestinationFMSEntry(hsxpl_route.cleg);
      XPLMSetDisplayedFMSEntry(hsxpl_route.cleg);
    } else {
      /*
      XPLMSetDestinationFMSEntry(0);
      XPLMSetDisplayedFMSEntry(0);
       */
    }
  }
}

/* Sets the number of points in route */
void hsxpl_navdb_set_nopoints(uint32_t nopoints) {

#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_navdb_set_nopoints() %d -> %d",hsxpl_route.nopoints,nopoints);
  hsxpl_log(HSXPLDEBUG_ACTION,logstr);
#endif

  /* If we have more points than the new ones, clear exceeding entries */
  if(hsxpl_navdb_fmc_db_type==1 || hsxpl_navdb_fmc_db_type==2) {
    int32_t noentries=(int32_t)XPLMCountFMSEntries();
    if(noentries>nopoints) {
      int32_t j;
      for(j=noentries-1;j>=nopoints;j--) {
        if(j>=0) {
          XPLMClearFMSEntry(j);
#ifdef HSXPLDEBUG
          sprintf(logstr,"hsxpl_navdb_set_nopoints() delete XP point %d",j);
          hsxpl_log(HSXPLDEBUG_ACTION,logstr);
#endif
        } else {
          break; /* Needed because of type casting */
        }
      }
    }
  }
  hsxpl_route.nopoints=nopoints;
  if(nopoints>0) {
    memset(&hsxpl_route.pts[nopoints],0,(HSMP_ROUTE_MAX_POINTS-nopoints)*sizeof(hsmp_route_pt_t));
  }

  if(hsxpl_navdb_fmc_db_type==3) {
    if(nopoints>0) {
      hsxpl_navdb_route_feedback_enabled=1;
    } else {
      hsxpl_navdb_route_feedback_enabled=0;
    }
  }



#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_navdb_set_nopoints() returning");
  hsxpl_log(HSXPLDEBUG_INFO,logstr);
#endif

}

/* Updates one given route point */
void hsxpl_navdb_set_route_point(uint32_t pindex,hsmp_route_pt_t *rp) {

#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_navdb_set_route_point() %d : %s -> %s",pindex,hsxpl_route.pts[pindex].pname,rp->pname);
  hsxpl_log(HSXPLDEBUG_ACTION,logstr);
#endif

  if(pindex >= HSMP_ROUTE_MAX_POINTS) {
    hsxpl_log(HSXPLDEBUG_ERROR,"hsxpl_navdb_set_route_point() error: point index exceeds maximum allowed");
  } else {
    memcpy(&hsxpl_route.pts[pindex],rp,sizeof(hsmp_route_pt_t));
    if(hsxpl_navdb_fmc_db_type==1 || hsxpl_navdb_fmc_db_type==2) {

      uint32_t ptype=rp->ptype;
      if(ptype!=xplm_Nav_Airport && ptype!=xplm_Nav_NDB && ptype!=xplm_Nav_VOR && ptype!=xplm_Nav_Fix) {
        ptype=xplm_Nav_LatLon;
      }

      XPLMNavRef nr=XPLMFindNavAid(NULL,rp->pname,
                                   &(rp->lat),&(rp->lon),
                                   NULL,(XPLMNavType)(ptype));

      if(nr!=XPLM_NAV_NOT_FOUND) {
        XPLMSetFMSEntryInfo(pindex,nr,(int)rp->elev);
      } else {
        XPLMSetFMSEntryLatLon(pindex,rp->lat,rp->lon,(int)rp->elev);
      }
    }
  }

}

/* Parses x-plane's navdb and populates the in memory flight plan */
void hsxpl_navdb_update_from_xplane(void) {

  int32_t i;

  if(hsxpl_navdb_fmc_db_type!=1 && hsxpl_navdb_fmc_db_type!=3 && hsxpl_navdb_fmc_db_type!=2) {
    return;
  }

  if(hsxpl_navdb_route_feedback_enabled) {
    return;
  }


#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_LOOP,"hsxpl_navdb_update_from_xplane()");
#endif

  uint32_t nopoints=XPLMCountFMSEntries();

  if(nopoints<2) {
    return;
  }

  if(nopoints>1) {
    hsxpl_route.nopoints=nopoints;
  }

  int32_t cleg=XPLMGetDestinationFMSEntry();
  if(cleg>0)   hsxpl_route.cleg=cleg;

  if(nopoints<2 || cleg<1) {
    hsxpl_route.cleg=-1;
  }

  for(i=0; i < hsxpl_route.nopoints ;i++) {

    XPLMNavType     fms_pt_type;
    char            fms_pt_id[512];
    int             fms_pt_altitude;
    float           fms_pt_latitude;
    float           fms_pt_longitude;
    XPLMNavRef      fms_pt_reference;

    /* We can only hold so many points in memory, don't blow things up ... */
    if(i>=HSMP_ROUTE_MAX_POINTS) {
      hsxpl_route.nopoints=HSMP_ROUTE_MAX_POINTS;
      break;
    }

    /* Fetch FMS entry */
    XPLMGetFMSEntryInfo(i,&fms_pt_type,fms_pt_id,&fms_pt_reference,&fms_pt_altitude,&fms_pt_latitude,&fms_pt_longitude);

    /* Replace anything unknown by latlon */
    if(fms_pt_type==xplm_Nav_Unknown) fms_pt_type=xplm_Nav_LatLon;
    
    /* Detect if the point name changed if it is of a supported type by X-Plane */
    int names_dont_match=0;
    if(hsxpl_route.pts[i].ptype<xplm_Nav_LatLon && fms_pt_type<xplm_Nav_LatLon) {
      if(strncmp(hsxpl_route.pts[i].pname,fms_pt_id,7)) {
        names_dont_match=1;
      }
    }

    /* If something has changed, we set up a new point in the route plan */
    if(names_dont_match || (hsxpl_route.pts[i].lat!=fms_pt_latitude || hsxpl_route.pts[i].lon!=fms_pt_longitude || hsxpl_route.pts[i].elev!=(float)fms_pt_altitude))
    {
      strncpy(hsxpl_route.pts[i].pname,fms_pt_id,7);
      hsxpl_route.pts[i].pname[7]='\0';
      hsxpl_route.pts[i].elev=(float) fms_pt_altitude;
      hsxpl_route.pts[i].lat = (float) fms_pt_latitude;
      hsxpl_route.pts[i].lon= (float) fms_pt_longitude;

      hsxpl_route.pts[i].ptype= (uint32_t)fms_pt_type;

      if(hsxpl_route.pts[i].ptype==2048) {
        hsxpl_point_name_for_position(hsxpl_route.pts[i].lat,hsxpl_route.pts[i].lon,hsxpl_route.pts[i].pname);
        hsxpl_route.pts[i].pname[7]='\0';
      }

      hsxpl_route.pts[i].ta = 0;
    }
  }
}

/* Returns the number of route points in the plan */
uint32_t hsxpl_navdb_number_of_route_points(void) {

#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_navdb_number_of_route_points() -> %d",hsxpl_route.nopoints);
  hsxpl_log(HSXPLDEBUG_DETAIL,logstr);
#endif

  return hsxpl_route.nopoints;
}

/* Returns the current leg index in the plan if any or -1 if none is active */
int32_t hsxpl_current_leg(void) {

  if(hsxpl_route.cleg >= hsxpl_route.nopoints || hsxpl_route.nopoints < 2) {
    if(hsxpl_route.cleg != -1) {
#ifdef HSXPLDEBUG
        hsxpl_log(HSXPLDEBUG_ERROR,"hsxpl_current_leg() error: reseting current leg to -1");
#endif
      hsxpl_navdb_set_current_leg(-1);
    }
  }
#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_current_leg() -> %d",hsxpl_route.cleg);
  hsxpl_log(HSXPLDEBUG_DETAIL,logstr);
#endif
  return hsxpl_route.cleg;
}

/* Returns the route point at the given index if valid, or NULL if invalid */
hsmp_route_pt_t *hsxpl_route_point_at_index(uint32_t pindex) {

  if(pindex >= hsxpl_route.nopoints || pindex >= HSMP_ROUTE_MAX_POINTS) {
#ifdef HSXPLDEBUG
    sprintf(logstr,"hsxpl_route_point_at_index() %d : NULL",pindex);
    hsxpl_log(HSXPLDEBUG_LOOP,logstr);
#endif
    return NULL;
  }

#ifdef HSXPLDEBUG
  sprintf(logstr,"hsxpl_route_point_at_index() %d : %s",pindex,hsxpl_route.pts[pindex].pname);
  hsxpl_log(HSXPLDEBUG_DETAIL,logstr);
#endif

  return &hsxpl_route.pts[pindex];
}


