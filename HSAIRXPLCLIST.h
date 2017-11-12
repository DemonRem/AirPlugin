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

#ifndef __HS_HSAIRXPLCLIST_H__
#define __HS_HSAIRXPLCLIST_H__

#include <XPLMProcessing.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMNavigation.h>
#include <XPLMMenus.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>

#include <XPWidgets.h>
#include <XPStandardWidgets.h>
#include <XPUIGraphics.h>
#include <XPLMPlanes.h>

/* HSCStringEntry() returns the nth element of a string where elements are separated
 * by character <delim>.
 * If <delim> is a space or a tab, <delim> is expanded to both space or tab.
 * If <delim> is a CR or NL, <delim> is expanded to both CR or NL.
 * If <entry> is non-null (a valid string), strentry() copies the nth
 * element to entry and returns a pointer to it.
 * If <entry> is null, strentry() simply returns a pointer to the nth
 * element of the original string, including all subsequent elements.
 * strentry() returns NULL if str is NULL or element is invalid.
 */
char *HSCStringEntry(int32_t element,char *str,char delim,char *entry);

/* HSCStringQuotedEntry is like strentry but if it happens that a field is quoted between
 * quotechar characters, all the characters inbetween are considered even if
 * the match delim.
 *
 * In other words, if delim is ',' and there is "Something, something else"
 * and quotechar is '"' then the whole sentence is returned as an entry.
 *
 * If quotechar is \0 strqentry behaves like strentry.
 * As a matter of fact strentry calls strqentry with this character as quote.
 */
char *HSCStringQuotedEntry(int32_t element,char *str,char delim,char *entry,char quotechar);


#define HSXPL_CLIST_TITLE_LEN       128
#define HSXPL_CLIST_PAGE_NAME_LEN   128
#define HSXPL_CLIST_ITEM_NAME_LEN   128
#define HSXPL_CLIST_ITEM_DREF_LEN   128
#define HSXPL_CLIST_ITEM_COND_LEN   32

/* The following are structure types to hold a complete checklist
 * in memory. A checklist comprises a hsxpl_clist_t reference
 * with multiple pages of type hsxpl_clist_page_t each with multiple
 * items of type hsxpl_clist_item_t. */
typedef struct hsxpl_clist_item_s {
    int state;
    int item_no;
    char name[HSXPL_CLIST_ITEM_NAME_LEN];
    char dref[HSXPL_CLIST_ITEM_DREF_LEN];
    char cond[HSXPL_CLIST_ITEM_COND_LEN];
    XPLMDataRef xpref;
    struct hsxpl_clist_item_s *next;
} hsxpl_clist_item_t;

typedef struct hsxpl_clist_page_s {
    int page_no;
    int no_items;
    char name[HSXPL_CLIST_PAGE_NAME_LEN];
    struct hsxpl_clist_item_s *items;
    struct hsxpl_clist_page_s *next;
} hsxpl_clist_page_t;

typedef struct hsxpl_clist_s {
    int no_pages;
    int no_items;
    char title[HSXPL_CLIST_TITLE_LEN];
    struct hsxpl_clist_page_s *pages;
} hsxpl_clist_t;

/* hsxpl_clist_getpath attempts to construct the name of a clist.txt file
 * containing a checklist for the current plane and returns it in path. If the
 * filename could not be created (no plane loaded) path will be returned with 
 * an empty string.
 */
void hsxpl_clist_getpath(char *path);

/* hsxpl_read_checklist_if_needed attempts to find a checklist file
 * and loads it if needed. This happens either at startup or when a new
 * checklist filename is detected, indicating that a new plane has been
 * loaded and hence a new checklist is available.
 */
void hsxpl_clist_getpath(char *path);

/* hsxpl_read_checklist_if_needed attempts to find a checklist file
 * and loads it if needed. This happens either at startup or when a new
 * checklist filename is detected, indicating that a new plane has been
 * loaded and hence a new checklist is available.
 */
void hsxpl_read_checklist_if_needed(void);

/* hsxpl_release_checklist parses the current loaded checklist and releases 
 * all the memory associated to it.
 */
void hsxpl_release_checklist(void);

/* hsxpl_read_checklist attempts to read the given checklist from the file on 
 * disk
 */
void hsxpl_read_checklist(char *path);

/* hsxpl_send_checklist_description is sent to connected clients with the
 * description of the current checklist, i.e. its number of pages and items 
 * per page
 */
void hsxpl_send_checklist_description(void);

/* hsxpl_clist_showtime is called from the runtime method periodically in 
 * order to perform all tasks related to the checklist module of the plugin.
 */
void hsxpl_clist_showtime(void);

#endif /* __HS_HSAIRXPLCLIST_H__ */
