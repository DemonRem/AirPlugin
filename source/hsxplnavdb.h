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

#ifndef HSAIRXPLNAVDB_h
#define HSAIRXPLNAVDB_h

#include <stdio.h>

#include "hsmpmsg.h"
#include "hsmpnet.h"

/* The type of FMC, 1 for x-plane default, 2 for third party */
uint32_t hsxpl_navdb_fmc_type(void);

/* Resets the navdb fmc type so it can be determined again at runtime */
void hsxpl_navdb_reset_fmc_type(void);

/* An initialisation function, sets route to empty */
void hsxpl_navdb_clear_route(void);

/* Sets the current leg to a new value, -1 for none */
void hsxpl_navdb_set_current_leg(int32_t newleg);

/* Sets the number of points in route */
void hsxpl_navdb_set_nopoints(uint32_t nopoints);

/* Updates one given route point */
void hsxpl_navdb_set_route_point(uint32_t pindex,hsmp_route_pt_t *rp);

/* Parses x-plane's navdb and populates the in memory flight plan */
void hsxpl_navdb_update_from_xplane(void);

/* Returns the number of route points in the plan */
uint32_t hsxpl_navdb_number_of_route_points(void);

/* Returns the current leg index in the plan if any or -1 if none is active */
int32_t hsxpl_current_leg(void);

/* Returns the route point at the given index if valid, or NULL if invalid */
hsmp_route_pt_t *hsxpl_route_point_at_index(uint32_t pindex);

#endif /* HSAIRXPLNAVDB_h */
