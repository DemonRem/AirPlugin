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
 * Specifics for the Zibo 737-800
 * https://forums.x-plane.org/index.php?/forums/topic/111305-b738-800-modified/
 *
 */

#ifndef ___HSAIRXPLZ738__
#define ___HSAIRXPLZ738__

#include <stdio.h>
#include "hsxpl.h"

/* Sets the datarefs required specific to zibo's 737-800 */
void hsxpl_set_z738_datarefs(void);

/* MCP specifics */
void hsairpl_mcp_z738_set_alt_dial(uint32_t v);
void hsairpl_mcp_z738_set_hdg_dial(uint32_t v);

/* Sends the z738 specific data to AirEFB i.e. its MCDU screen lines and states */
void hsxpl_send_z738_data(void);

/* Sets the FMC key commands  */
void hsxpl_set_z738_fmc_keys(void);

#endif /* ___HSAIRXPLZ738__ */
