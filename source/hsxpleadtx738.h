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

#ifndef __HSXPLEADTX738__H__
#define __HSXPLEADTX738__H__

#include <stdio.h>
#include "hsxpl.h"

/* The x737 set of functions are meant to overcome some dataref
 * differences between the normal aircraft and the EADT x737 project
 * Boeing 737-NG. This is only relevant for X-Plane so if you try to
 * port this code to another flight simulator you may forget these  */
typedef struct hsxpl_x737_datarefs_s {

  XPLMDataRef pluginstatus;

  /* For xFMC and x737 V5 */
  XPLMDataRef key_down;

} hsxpl_x737_datarefs_t;

void hsxpl_x737_fmc_press_key(XPLMDataRef k);
void hsxpl_x737_release_key_pressed(void);
void hsxpl_set_x737_datarefs(void);

#endif /* __HSXPLEADTX738__H__ */
