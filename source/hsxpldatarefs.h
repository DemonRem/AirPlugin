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

#ifndef __HSXPLDATAREFS_H__
#define __HSXPLDATAREFS_H__

#include <stdio.h>
#include "hsxpl.h"

/* The list of X-Plane datarefs */
typedef struct hsxpl_xplane_datarefs_s {

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

} hsxpl_xplane_datarefs_t;

void hsxpl_set_xplane_datarefs(void);


char *hsxpl_acf_icao(void);
char *hsxpl_acf_tailno(void);

#endif /* __HSXPLDATAREFS_H__ */
