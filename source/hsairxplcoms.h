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


#ifndef __HS__HSAIRXPLCOMS__
#define __HS__HSAIRXPLCOMS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "hsairxpl.h"

void hsairpl_coms_update_datarefs(void);

uint32_t hsairpl_coms_get_nav1(void);
uint32_t hsairpl_coms_get_snav1(void);
uint32_t hsairpl_coms_get_nav2(void);
uint32_t hsairpl_coms_get_snav2(void);

uint32_t hsairpl_coms_get_com1(void);
uint32_t hsairpl_coms_get_scom1(void);
uint32_t hsairpl_coms_get_com2(void);
uint32_t hsairpl_coms_get_scom2(void);

uint32_t hsairpl_coms_get_adf1(void);
uint32_t hsairpl_coms_get_sadf1(void);
uint32_t hsairpl_coms_get_adf2(void);
uint32_t hsairpl_coms_get_sadf2(void);

void hsairpl_coms_set_nav1(uint32_t v);
void hsairpl_coms_set_snav1(uint32_t v);
void hsairpl_coms_set_nav2(uint32_t v);
void hsairpl_coms_set_snav2(uint32_t v);

void hsairpl_coms_set_com1(uint32_t v);
void hsairpl_coms_set_scom1(uint32_t v);
void hsairpl_coms_set_com2(uint32_t v);
void hsairpl_coms_set_scom2(uint32_t v);

void hsairpl_coms_set_adf1(uint32_t v);
void hsairpl_coms_set_sadf1(uint32_t v);
void hsairpl_coms_set_adf2(uint32_t v);
void hsairpl_coms_set_sadf2(uint32_t v);

uint32_t hsairpl_coms_get_xponder_mode(void);
uint32_t hsairpl_coms_get_xponder_code(void);
void hsairpl_coms_set_xponder_mode(uint32_t v);
void hsairpl_coms_set_xponder_code(uint32_t v);

#endif /* defined(__HS__HSAIRXPLCOMS__) */
