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
 * EADT 737-800 stuff : https://www.eadt.eu
 *
 */

#include "hsxpleadtx738.h"
#include "hsxpl.h"
#include "hsxplmisc.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;

hsxpl_x737_datarefs_t hsxpl_x737_datarefs;

/* Release x737 FMC key if pressed */
void hsxpl_x737_release_key_pressed(void) {
  if(hsxpl_x737_datarefs.key_down!=NULL) {
    XPLMSetDataf(hsxpl_x737_datarefs.key_down,0);
    hsxpl_x737_datarefs.key_down=NULL;

  }
}

void hsxpl_x737_fmc_press_key(XPLMDataRef k) {

  if(hsxpl_x737_datarefs.key_down!=NULL) {
    XPLMSetDataf(hsxpl_x737_datarefs.key_down,0);
    hsxpl_x737_datarefs.key_down=NULL;
  }

  XPLMSetDataf(k,1);

  if(hsxpl_fmc_type==HSMP_FMC_TYPE_XP_X737V5) {
    hsxpl_x737_datarefs.key_down=k;
  }
  
}

void hsxpl_set_x737_datarefs(void) {

  /* x737 datarefs */
  memset(&hsxpl_x737_datarefs,0,sizeof(struct hsxpl_x737_datarefs_s));

  /* Make sure we have an x737 status dataref */
  hsxpl_x737_datarefs.pluginstatus=XPLMFindDataRef("x737/systems/afds/plugin_status");
  if(hsxpl_x737_datarefs.pluginstatus==NULL) {
    return;
  }

  /* And that it is enabled */
  if(!(XPLMGetDatai(hsxpl_x737_datarefs.pluginstatus))) {
    return;
  }

  hsxpl_plane_type = HSXPL_PLANE_X737;

}


