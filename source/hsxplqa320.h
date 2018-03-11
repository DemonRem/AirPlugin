/* Copyright (c) 2013-2017 Haversine Ltd
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
 * Old QPAC A320 / new ToLis A319 : toliss.com
*
*/

#ifndef __HSXPLQA320_H__
#define __HSXPLQA320_H__

#include <stdio.h>
#include "hsxpl.h"

/* Send A320 FMC data in ARINC format */

#define HSXPL_A320Q_FMC_FSIZE_DEFAULT     0
#define HSXPL_A320Q_FMC_FSIZE_LABEL       20
#define HSXPL_A320Q_FMC_COL_WHITE         0xFFFFFFFF
#define HSXPL_A320Q_FMC_COL_GREEN         0x00FF00FF
#define HSXPL_A320Q_FMC_COL_CYAN          0x00FFFFFF
#define HSXPL_A320Q_FMC_COL_MAGENTA       0xFF00FFFF
#define HSXPL_A320Q_FMC_COL_AMBER         0xFFBF00FF
#define HSXPL_A320Q_FMC_COL_YELLOW        0xFFFF00FF

/* A320 colour IDs */
#define HSXPL_A320Q_COL_IDX_WHITE       0
#define HSXPL_A320Q_COL_IDX_GREEN       1
#define HSXPL_A320Q_COL_IDX_BLUE        2
#define HSXPL_A320Q_COL_IDX_AMBER       3
#define HSXPL_A320Q_COL_IDX_YELLOW      4
#define HSXPL_A320Q_COL_IDX_MAGENTA     5
#define HSXPL_A320Q_COL_IDX_S           6

#define HSXPL_A320Q_NO_COLS             24

typedef struct hsxpl_a320q_datarefs_s {

  XPLMDataRef     title[7];
  XPLMDataRef     stitle[6];
  XPLMDataRef     label[6][7];
  XPLMDataRef     content_large[6][7];
  XPLMDataRef     content_small[6][7];
  XPLMDataRef     scratchpad[6];

} hsxpl_a320q_datarefs_t;

void hsxpl_set_a320q_datarefs(void);
void hsxpl_send_a320q_fmc_data(void);

#endif /* __HSXPLQA320_H__ */
