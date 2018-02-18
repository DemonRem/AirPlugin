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
 * JA Design A320 (and A330) support: http://jardesign.org/
 *
 */

#ifndef __HSXPLJARA320__H__
#define __HSXPLJARA320__H__

#include "hsxpl.h"

#define HSXPL_A320N_FMC_FSIZE_DEFAULT     0
#define HSXPL_A320N_FMC_FSIZE_LABEL       20
#define HSXPL_A320N_FMC_COL_WHITE         0xFFFFFFFF
#define HSXPL_A320N_FMC_COL_GREEN         0x00FF00FF
#define HSXPL_A320N_FMC_COL_CYAN          0x00FFFFFF
#define HSXPL_A320N_FMC_COL_MAGENTA       0xFF00FFFF
#define HSXPL_A320N_FMC_COL_AMBER         0xFFBF00FF
#define HSXPL_A320N_FMC_COL_YELLOW        0xFFFF00FF

typedef struct hsxpl_a320n_datarefs_s {

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

} hsxpl_a320n_datarefs_t;

void hsxpl_set_a320n_datarefs(void);
void hsxpl_send_a320n_fmc_data(void);

#endif /* __HSXPLJARA320__H__ */
