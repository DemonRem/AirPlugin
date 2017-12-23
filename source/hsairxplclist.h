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

#ifndef __HSAIRXPLCHECKLIST_H__
#define __HSAIRXPLCHECKLIST_H__

#include <stdio.h>

/* hsairpl_clist_ref_t defines a structure to store a list of
 * clist.text references in the filesystem which can
 * be retrieved by clients */
typedef struct hsairpl_clist_ref_s {
  char path[512];
  char checklist[32];
  uint64_t fsize;
  struct hsairpl_clist_ref_s *next;
} hsairpl_clist_ref_t;

/* Clears the reference list so it can re-initialised. We probably
 * don't need this function as once read / loaded clists don't change */
void hsairpl_clist_clear_references(void);

/* hsairpl_clist_read_references() parses all apt.dat files under
 * $(X-Plane)/Aircraft and constructs a list of these which are
 * kept in memory so that they can be retrieved. This function calls
 * itself recursively and should only be called with rpath=NULL from
 * the outside. */
void hsairpl_clist_read_references(char *rpath);

/* For each entry found in hsairpl_clist_read_references() this method
 * reads and creates a reference for the given list. */
void hsairpl_clist_create_ref_for(char *rpath);

/* Returns the list of references or NULL if none */
hsairpl_clist_ref_t *hsairpl_clist_references(void);

/* Send list indexes to a specific client sa */
void hsairpl_clist_send_indexes_to(struct sockaddr_in*to);

/* Send a list over the TCP server connection */
void hsairpl_clist_send_list_to(char *listid, struct sockaddr_in*to);

/* Sends a request fail back to the client app */
void hsairpl_clist_send_req_fail(void);

/* Sends the next set of bytes for an apt.dat transfer that is ongoing */
int hsairpl_clist_send_next_list_bytes(void);

#endif /* __HSAIRXPLCHECKLIST_H__ */

