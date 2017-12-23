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


#include "hsxplcoms.h"
#include "hsxpl.h"
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
struct hsairxpl_coms_datarefs_s {

    XPLMDataRef	nav1freq;
    XPLMDataRef	nav2freq;
    XPLMDataRef	com1freq;
    XPLMDataRef	com2freq;
    XPLMDataRef	adf1freq;
    XPLMDataRef	adf2freq;
    XPLMDataRef	snav1freq;
    XPLMDataRef	snav2freq;
    XPLMDataRef	scom1freq;
    XPLMDataRef	scom2freq;
    XPLMDataRef	sadf1freq;
    XPLMDataRef	sadf2freq;
    XPLMDataRef	xpderc;
    XPLMDataRef	xpdermode;
    
    XPLMDataRef b767_xpdermode;
    XPLMDataRef b767_adf1_small_rotary;
    XPLMDataRef b767_adf1_mid_rotary;
    XPLMDataRef b767_adf1_big_rotary;
    XPLMDataRef b767_adf2_small_rotary;
    XPLMDataRef b767_adf2_mid_rotary;
    XPLMDataRef b767_adf2_big_rotary;
    
    XPLMDataRef x733_rad_adf1_100;
    XPLMDataRef x733_rad_adf1_010;
    XPLMDataRef x733_rad_adf1_001;
    XPLMDataRef x733_rad_adf2_100;
    XPLMDataRef x733_rad_adf2_010;
    XPLMDataRef x733_rad_adf2_001;
    XPLMDataRef x733_rad_xpdr_mode;

} hsairxpl_coms_datarefs;


void hsairpl_coms_update_datarefs(void) {
    
     memset(&hsairxpl_coms_datarefs,0,sizeof(hsairxpl_coms_datarefs));
    
    hsairxpl_coms_datarefs.nav1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_frequency_hz");
    hsairxpl_coms_datarefs.nav2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_frequency_hz");
    hsairxpl_coms_datarefs.com1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_frequency_hz");
    hsairxpl_coms_datarefs.com2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_frequency_hz");
    hsairxpl_coms_datarefs.adf1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_frequency_hz");
    hsairxpl_coms_datarefs.adf2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_frequency_hz");
    hsairxpl_coms_datarefs.snav1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_hz");
    hsairxpl_coms_datarefs.snav2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_standby_frequency_hz");
    hsairxpl_coms_datarefs.scom1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_hz");
    hsairxpl_coms_datarefs.scom2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_hz");
    hsairxpl_coms_datarefs.sadf1freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_standby_frequency_hz");
    hsairxpl_coms_datarefs.sadf2freq=XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");
    hsairxpl_coms_datarefs.xpdermode=XPLMFindDataRef("sim/cockpit/radios/transponder_mode");
    hsairxpl_coms_datarefs.xpderc=XPLMFindDataRef("sim/cockpit/radios/transponder_code");

     if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
         hsairxpl_coms_datarefs.b767_xpdermode=XPLMFindDataRef("1-sim/transponder/systemMode");
         hsairxpl_coms_datarefs.b767_adf1_small_rotary=XPLMFindDataRef("1-sim/adf/leftSmallRotary");
         hsairxpl_coms_datarefs.b767_adf1_mid_rotary=XPLMFindDataRef("1-sim/adf/leftMidRotary");
         hsairxpl_coms_datarefs.b767_adf1_big_rotary=XPLMFindDataRef("1-sim/adf/leftBigRotary");
         hsairxpl_coms_datarefs.b767_adf2_small_rotary=XPLMFindDataRef("1-sim/adf/rightSmallRotary");
         hsairxpl_coms_datarefs.b767_adf2_mid_rotary=XPLMFindDataRef("1-sim/adf/rightMidRotary");
         hsairxpl_coms_datarefs.b767_adf2_big_rotary=XPLMFindDataRef("1-sim/adf/rightBigRotary");
    }
    
    if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
        hsairxpl_coms_datarefs.x733_rad_adf1_100=XPLMFindDataRef("ixeg/733/radios/radios_adf1_100_act");
        hsairxpl_coms_datarefs.x733_rad_adf1_010=XPLMFindDataRef("ixeg/733/radios/radios_adf1_010_act");
        hsairxpl_coms_datarefs.x733_rad_adf1_001=XPLMFindDataRef("ixeg/733/radios/radios_adf1_001_act");
        hsairxpl_coms_datarefs.x733_rad_adf2_100=XPLMFindDataRef("ixeg/733/radios/radios_adf2_100_act");
        hsairxpl_coms_datarefs.x733_rad_adf2_010=XPLMFindDataRef("ixeg/733/radios/radios_adf2_010_act");
        hsairxpl_coms_datarefs.x733_rad_adf2_001=XPLMFindDataRef("ixeg/733/radios/radios_adf2_001_act");
        
        /* Doesn't seem to work */
        /*
         hsairxpl_coms_datarefs.x733_rad_xpdr_mode=XPLMFindDataRef("ixeg/733/radios/actuators/transponder_mode");
         */
    }
}

uint32_t hsairpl_coms_get_nav1(void) {
    
    if(hsairxpl_coms_datarefs.nav1freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.nav1freq);
    }
    return 0;
}

uint32_t hsairpl_coms_get_snav1(void) {
    
    if(hsairxpl_coms_datarefs.snav1freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.snav1freq);
    }
    return 0;
}

uint32_t hsairpl_coms_get_nav2(void) {
    
    if(hsairxpl_coms_datarefs.nav2freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.nav2freq);
    }
    return 0;
}

uint32_t hsairpl_coms_get_snav2(void) {
    
    if(hsairxpl_coms_datarefs.snav2freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.snav2freq);
    }
    return 0;
}


uint32_t hsairpl_coms_get_com1(void) {
    
    if(hsairxpl_coms_datarefs.com1freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.com1freq);
    }
    return 0;
}
uint32_t hsairpl_coms_get_scom1(void) {
    
    if(hsairxpl_coms_datarefs.scom1freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.scom1freq);
    }
    return 0;
}
uint32_t hsairpl_coms_get_com2(void) {
    
    if(hsairxpl_coms_datarefs.com2freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.com2freq);
    }
    return 0;
}
uint32_t hsairpl_coms_get_scom2(void) {
    
    if(hsairxpl_coms_datarefs.scom2freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.scom2freq);
    }
    return 0;
}

uint32_t hsairpl_coms_get_adf1(void) {
    
    if(hsairxpl_coms_datarefs.adf1freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.adf1freq);
    }
    return 0;

}
uint32_t hsairpl_coms_get_sadf1(void) {
    
    if(hsairxpl_coms_datarefs.sadf1freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.sadf1freq);
    }
    return 0;
    

}
uint32_t hsairpl_coms_get_adf2(void) {
    
    if(hsairxpl_coms_datarefs.adf2freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.adf2freq);
    }
    return 0;
    

}
uint32_t hsairpl_coms_get_sadf2(void) {
    
    if(hsairxpl_coms_datarefs.sadf2freq!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.sadf2freq);
    }
    return 0;
}

void hsairpl_coms_set_nav1(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.nav1freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.nav1freq,v);
    }
    
}
void hsairpl_coms_set_snav1(uint32_t v) {

    if(hsairxpl_coms_datarefs.snav1freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.snav1freq,v);
    }
}
void hsairpl_coms_set_nav2(uint32_t v) {

    if(hsairxpl_coms_datarefs.nav2freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.nav2freq,v);
    }

}
void hsairpl_coms_set_snav2(uint32_t v) {

    if(hsairxpl_coms_datarefs.snav2freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.snav2freq,v);
    }

}

void hsairpl_coms_set_com1(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.com1freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.com1freq,v);
    }
    
}
void hsairpl_coms_set_scom1(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.scom1freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.scom1freq,v);
    }
}
void hsairpl_coms_set_com2(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.com2freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.com2freq,v);
    }
}
void hsairpl_coms_set_scom2(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.scom2freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.scom2freq,v);
    }
}

void hsairpl_coms_set_adf1(uint32_t v) {
    
   
    if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
        if(hsairxpl_coms_datarefs.x733_rad_adf1_001!=NULL &&
           hsairxpl_coms_datarefs.x733_rad_adf1_010!=NULL &&
           hsairxpl_coms_datarefs.x733_rad_adf1_100!=NULL) {
            int i = v%10;
            XPLMSetDataf(hsairxpl_coms_datarefs.x733_rad_adf1_001,(float)i);
            i = v/10; i = i % 10;
            XPLMSetDataf(hsairxpl_coms_datarefs.x733_rad_adf1_010,(float)i);
            i = v/100;
            XPLMSetDataf(hsairxpl_coms_datarefs.x733_rad_adf1_100,(float)i);

        }
    }
    
    /* This is the most ridiculous way of setting an ADF frequency
     * but here it goes, the best we could get... shame on you FF767! */
    if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
        if(hsairxpl_coms_datarefs.b767_adf1_small_rotary!=NULL &&
           hsairxpl_coms_datarefs.b767_adf1_mid_rotary!=NULL &&
           hsairxpl_coms_datarefs.b767_adf1_big_rotary!=NULL) {

            uint32_t sV=v%10;float sF= 1.0 - ((1.0/9.0) *(float)sV);
            uint32_t mV=(v/10)%10;float mF= 0.9 - ((1.0/9.0) *(float)mV);
            uint32_t bV=(v/100);float bF= 1.0 - ((1.0/16.0) *(float)(bV-1));
            
            XPLMSetDataf(hsairxpl_coms_datarefs.b767_adf1_small_rotary,sF);
            XPLMSetDataf(hsairxpl_coms_datarefs.b767_adf1_mid_rotary,mF);
            XPLMSetDataf(hsairxpl_coms_datarefs.b767_adf1_big_rotary,bF);
            
            
        }
      
    } else {
        if(hsairxpl_coms_datarefs.adf1freq!=NULL) {
            XPLMSetDatai(hsairxpl_coms_datarefs.adf1freq,v);
        }
        
    }


}
void hsairpl_coms_set_sadf1(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.sadf1freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.sadf1freq,v);
    }
    
}
void hsairpl_coms_set_adf2(uint32_t v) {
    
    if(hsxpl_plane_type==HSXPL_PLANE_IX733) {
        if(hsairxpl_coms_datarefs.x733_rad_adf2_001!=NULL &&
           hsairxpl_coms_datarefs.x733_rad_adf2_010!=NULL &&
           hsairxpl_coms_datarefs.x733_rad_adf2_100!=NULL) {
            int i = v%10;
            XPLMSetDataf(hsairxpl_coms_datarefs.x733_rad_adf2_001,(float)i);
            i = v/10; i = i % 10;
            XPLMSetDataf(hsairxpl_coms_datarefs.x733_rad_adf2_010,(float)i);
            i = v/100;
            XPLMSetDataf(hsairxpl_coms_datarefs.x733_rad_adf2_100,(float)i);
            
        }
    }

    /* This is the most ridiculous way of setting an ADF frequency
     * but here it goes, the best we could get... shame on you FF767! */
    if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
        if(hsairxpl_coms_datarefs.b767_adf2_small_rotary!=NULL &&
           hsairxpl_coms_datarefs.b767_adf2_mid_rotary!=NULL &&
           hsairxpl_coms_datarefs.b767_adf2_big_rotary!=NULL) {
            
            uint32_t sV=v%10;float sF= ((1.0/9.0) *(float)sV);
            uint32_t mV=(v/10)%10;float mF= ((1.0/9.0) *(float)mV);
            uint32_t bV=(v/100);float bF=((1.0/16.0) *(float)(bV-1));
            
            XPLMSetDataf(hsairxpl_coms_datarefs.b767_adf2_small_rotary,sF);
            XPLMSetDataf(hsairxpl_coms_datarefs.b767_adf2_mid_rotary,mF);
            XPLMSetDataf(hsairxpl_coms_datarefs.b767_adf2_big_rotary,bF);
            
        }
        
    } else {
        if(hsairxpl_coms_datarefs.adf2freq!=NULL) {
            XPLMSetDatai(hsairxpl_coms_datarefs.adf2freq,v);
        }
    }

    
}
void hsairpl_coms_set_sadf2(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.sadf2freq!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.sadf2freq,v);
    }
    
}

uint32_t hsairpl_coms_get_xponder_mode(void) {
    
    if(hsairxpl_coms_datarefs.xpdermode!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.xpdermode);
    }
    return 0;
}

uint32_t hsairpl_coms_get_xponder_code(void) {
    
    if(hsairxpl_coms_datarefs.xpderc!=NULL) {
        return XPLMGetDatai(hsairxpl_coms_datarefs.xpderc);
    }
    return 0;
}

void hsairpl_coms_set_xponder_mode(uint32_t v) {
    
    if(hsairxpl_coms_datarefs.xpdermode!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.xpdermode,v);
    }
    
    if(hsxpl_plane_type==HSXPL_PLANE_FF767) {
         if(hsairxpl_coms_datarefs.b767_xpdermode!=NULL) {
             switch(v) {
                 case(0):XPLMSetDataf(hsairxpl_coms_datarefs.b767_xpdermode,2.0);break;
                 case(1):XPLMSetDataf(hsairxpl_coms_datarefs.b767_xpdermode,1.0);break;
                 case(2):XPLMSetDataf(hsairxpl_coms_datarefs.b767_xpdermode,5.0);break;
                 case(3):XPLMSetDataf(hsairxpl_coms_datarefs.b767_xpdermode,0.0);break;
             }
         }
    }
}

void hsairpl_coms_set_xponder_code(uint32_t v) {
    if(hsairxpl_coms_datarefs.xpderc!=NULL) {
        XPLMSetDatai(hsairxpl_coms_datarefs.xpderc,v);
    }
}


