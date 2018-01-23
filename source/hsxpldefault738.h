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
 * Implementation of the X-Plane default Boeing 737-800 datarefs and
 * specific functions.
 *
 */
#ifndef ___HSAIRXPLB738__
#define ___HSAIRXPLB738__

#include <stdio.h>
#include "hsxpl.h"

/* Sets B737FMC keys to the corresponding command refs */
void hsxpl_set_b738_keys(void);

/* Sets the datarefs required specific to Laminar default 737-800 */
void hsxpl_set_default_738_datarefs(void);

/* MCP specifics */
void hsairpl_mcp_b738_at_arm_toggle(uint32_t);
uint32_t hsairpl_mcp_b738_get_at_arm(void);
void hsairpl_mcp_b738_fd_ca_toggle(uint32_t);
uint32_t hsairpl_mcp_b738_get_fd_ca(void);
void hsairpl_mcp_b738_fd_fo_toggle(uint32_t);
uint32_t hsairpl_mcp_b738_get_fd_fo(void);
void hsairpl_mcp_b738_n1_press(void);
void hsairpl_mcp_b738_speed_press(void);
void hsairpl_mcp_b738_co_press(void);
void hsairpl_mcp_b738_lvlchg_press(void);
void hsairpl_mcp_b738_vnav_press(void);
void hsairpl_mcp_b738_lnav_press(void);
void hsairpl_mcp_b738_vorloc_press(void);
void hsairpl_mcp_b738_app_press(void);
void hsairpl_mcp_b738_hdgsel_press(void);
void hsairpl_mcp_b738_althld_press(void);
void hsairpl_mcp_b738_vs_press(void);
void hsairpl_mcp_b738_cmd_a_press(void);
void hsairpl_mcp_b738_cmd_b_press(void);

/* EFIS captain side specifics */
void hsairpl_efis1_b738_wxr_press(void);
void hsairpl_efis1_b738_sta_press(void);
void hsairpl_efis1_b738_wpt_press(void);
void hsairpl_efis1_b738_arpt_press(void);
void hsairpl_efis1_b738_data_press(void);
void hsairpl_efis1_b738_pos_press(void);
void hsairpl_efis1_b738_terr_press(void);

#endif /* ___HSAIRXPLB738__ */
