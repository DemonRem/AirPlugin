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


#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "HSMPMSG.h"
#include "HSMPNET.h"
#include "HSAIRXPLCLIST.h"

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

#define	XPLM200	1
#define	XPLM210	1
#define	XPLM211	1

/* Auxiliary functions for string extraction, from HSF/FSCString */

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
char *HSCStringEntry(int32_t element,char *str,char delim,char *entry) {
	return HSCStringQuotedEntry(element,str,delim,entry,'\0');
}


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
char *HSCStringQuotedEntry(int32_t element,char *str,char delim,char *entry,char quotechar) {
    
    char *pb;
    char d1,d2;
    char e1,e2,e3;
    int32_t i;
	
    /* Perform some validy tests */
    if(element<1) return NULL;
    if(str==NULL) return NULL;
    if(entry!=NULL) entry[0]='\0';
	
    /* Initialise EOL and delimiter characters */
    e1='\0'; e2='\n'; e3='\r';
    if(delim=='\r' || delim=='\n') { e2=e1; e3=e1; }
	
    if(delim=='\t' || delim==' ') { d1='\t'; d2=' '; }
    else if(delim=='\r' || delim=='\n') { d1='\r'; d2='\n'; }
    else { d1=delim; d2=delim; }
    /* If space / tab based, move to beginning of valid content */
    while(d1=='\t' && (*str==d1 || *str==d2) && *str!=e1 && *str!=e2  && *str!=e3)
        str++;
	
    /* If we are at the end, return here */
    if(*str==e1 || *str==e2 || *str==e3) {
        if(entry!=NULL) return entry;
        else return NULL;
    }
	
    /* Position str at the start of the nth element */
    for(i=0;i<element-1;i++) {
		
        /* Move to next delimiter */
        while(*str!=d1 && *str!=d2 && *str!=e1 && *str!=e2  && *str!=e3) {
			
            if(quotechar=='\0') str++;
            else {
                if((*str) != quotechar)
                    str++;
                else {                                        str++;
                    while(*str!=quotechar && *str!=e1 && *str!=e2  && *str!=e3) {
                        str++;
                    }
                    if(*str==quotechar) str++;
					
                }
            }
        }
		
        /* If we reached the end, return here */
        if(*str==e1 || *str==e2 || *str==e3) {
            if(entry!=NULL)  return entry;
            else return NULL;
        }
		/* Move to beginning of next entry */
		if(d1=='\t') /* Tab / space based or not */
            while((*str==d1 || *str==d2) && *str!=e1 && *str!=e2  && *str!=e3)
                str++;
        else
            str++;
		
        /* If we reached the end, return here */
        if(*str==e1 || *str==e2 || *str==e3) {
            if(entry!=NULL)  return entry;
            else return NULL;
        }
		
    } /* end for() positioning */
	
    /* If no entry is passed, return current pointer */
    if(entry==NULL) return str;
	
    pb=entry;
	
    if(quotechar!='\0' && *str==quotechar) {
        d1=quotechar; d2=quotechar;
        str++;
    }
	
    /* Copy to entry */
    while((*str!=d1 && *str!=d2) && *str!=e1 && *str!=e2  && *str!=e3)
        *pb++ = *str++;
	
    *pb='\0';
    return entry;
}

/* The current checklist */
hsxpl_clist_t *hsxpl_checklist=NULL;

/* hsxpl_clist_getpath attempts to construct the name of a clist.txt file
 * containing a checklist for the current plane and returns it in path. If the
 * filename could not be created (no plane loaded) path will be returned with an
 * empty string.
 */
void hsxpl_clist_getpath(char *path) {
    
    char outFile[512];
    char outPath[1024];

    path[0]='\0';

    XPLMGetNthAircraftModel(0,outFile,outPath);
    
    /* Locate first : */
    char *cp,*bp;
    cp=outPath;

    char ch;
#if IBM
    ch='\\';
#else
    ch='/';
#endif
    
    while(*cp!='\0') {
        if(*cp==':') break;
        cp++;
    }
    if(*cp=='\0') return;
    
    /* Set beggining of string at first : */
    *cp=ch;

    /* Replace : with / */
    bp=cp;


    while(*cp!='\0') {
        if(*cp==':') {
            *cp = ch;
        }
        cp++;
    }
    
    /* Now remove last filename */
    while(*cp!=ch && cp!=bp) {
        cp--;
    }
    if(cp==bp) return;
    
    /* And add clist.txt */
    cp++;
    sprintf(cp,"clist.txt");
    
    /* Copy name */
    strncpy(path,bp,1023);

    return;
    
}

/* hsxpl_read_checklist_if_needed attempts to find a checklist file
 * and loads it if needed. This happens either at startup or when a new
 * checklist filename is detected, indicating that a new plane has been
 * loaded and hence a new checklist is available.
 */
void hsxpl_read_checklist_if_needed(void) {
    
    static int hsxpl_read_checklist_if_needed_initialised=0;
    static char old_path[1024];
    char new_path[1024];
    
    /* Initialise old path */
    if(!hsxpl_read_checklist_if_needed_initialised) {
        memset(old_path,0,1024);
        hsxpl_read_checklist_if_needed_initialised=1;
    }
    
    hsxpl_clist_getpath(new_path);
    
    if(strcmp(new_path,old_path)) {
        hsxpl_release_checklist();
        if(new_path[0]!='\0') {
            hsxpl_read_checklist(new_path);
            strncpy(old_path,new_path,1023);
        }
    }
    
    
}

/* hsxpl_release_checklist parses the current loaded checklist and releases all
 * the memory associated to it.
 */
void hsxpl_release_checklist(void) {
    
    if(hsxpl_checklist!=NULL) {
        hsxpl_clist_page_t *p=hsxpl_checklist->pages;
        while(p!=NULL) {
            
            hsxpl_clist_item_t *i=p->items;
            while(i!=NULL) {
                hsxpl_clist_item_t *i2=i;
                i=i->next;
                free(i2);
            }

            hsxpl_clist_page_t *p2=p;
            p=p->next;
            free(p2);
        }
        free(hsxpl_checklist);
    }
    hsxpl_checklist=NULL;
    
}

/* hsxpl_read_checklist attempts to read the given checklist from the file on disk */
void hsxpl_read_checklist(char *path) {
    
    if(hsxpl_checklist!=NULL)
        hsxpl_release_checklist();
    
    FILE *fp=fopen(path,"r");
    if(fp!=NULL) {
        
        hsxpl_clist_page_t *page=NULL;
        hsxpl_clist_item_t *item=NULL;
        char str[1024];
        int item_no=0;
        
        hsxpl_checklist=(hsxpl_clist_t *)malloc(sizeof(hsxpl_clist_t));
        memset(hsxpl_checklist,0,sizeof(hsxpl_clist_t));
        if(hsxpl_checklist!=NULL) {
            hsxpl_checklist->no_pages=0;
            hsxpl_checklist->no_items=0;
            sprintf(hsxpl_checklist->title,"X-PLANE CHECKLIST");
            while(fgets(str,1023,fp)!=NULL) {
                
                /* Strip \r\n */
                char *cp=str;
                while(cp!='\0' && *cp!='\r' && *cp!='\n' && (cp-str)<1023) cp++;
                *cp='\0';
                
                char w1[1024];w1[0]='\0';
                char w2[1024];w2[0]='\0';
                char w3[1024];w3[0]='\0';
                char w4[1024];w4[0]='\0';
                
                if(HSCStringEntry(1,str,':',w1)!=NULL)
                {
                    if(!strcasecmp(w1,"sw_checklist")) {
                        
                        hsxpl_clist_page_t *cpage=page;
                        
                        page=(hsxpl_clist_page_t *)malloc(sizeof(hsxpl_clist_page_t));
                        memset(page,0,sizeof(hsxpl_clist_page_t));
                        item_no=0;
                        if(page!=NULL) {
                            if(HSCStringEntry(2,str,':',w2)!=NULL) {
                                strncpy(page->name,w2,HSXPL_CLIST_PAGE_NAME_LEN-1);
                            }
                            page->no_items=0;
                            hsxpl_checklist->no_pages++;
                        }
                        if(cpage!=NULL) cpage->next=page;
                        else hsxpl_checklist->pages=page;
                    }
                    
                    
                    if(!strcasecmp(w1,"sw_item") && page!=NULL) {
                        hsxpl_clist_item_t *citem=item;
                        item=(hsxpl_clist_item_t *)malloc(sizeof(hsxpl_clist_item_t));
                        memset(item,0,sizeof(hsxpl_clist_item_t));
                        if(item!=NULL) {
                            page->no_items++;
                            hsxpl_checklist->no_items++;
                            if(HSCStringEntry(2,str,':',w2)!=NULL) {
                                strncpy(item->name,w2,HSXPL_CLIST_ITEM_NAME_LEN-1);
                            }
                            if(HSCStringEntry(3,str,':',w3)!=NULL) {
                                strncpy(item->dref,w2,HSXPL_CLIST_ITEM_DREF_LEN-1);
                            }
                            if(HSCStringEntry(4,str,':',w4)!=NULL) {
                                strncpy(item->cond,w2,HSXPL_CLIST_ITEM_COND_LEN-1);
                            }
                            item->item_no=item_no;
                            item_no++;
                        }
                        if(citem!=NULL) citem->next=item;
                        else page->items=item;
                    }
                    
                }
                
                
            }
        }
        
        fclose(fp);
        
    }
    
    hsxpl_send_checklist_description();
    
}

/* hsxpl_send_checklist_description is sent to connected clients with the
 * description of the current checklist, i.e. its number of pages and items per page
 */
void hsxpl_send_checklist_description(void)
{
    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {
        uint32_t n=0;
        if(hsxpl_checklist!=NULL) n=hsxpl_checklist->no_pages;
        hsmp_net_add_msg_to_pkt(pkt,HSMP_CLIST_DESC_NOPAGES,&n);
        
        if(hsxpl_checklist!=NULL) {
            int i;
            hsxpl_clist_page_t *page=NULL;
            for(i=0;i<hsxpl_checklist->no_pages;i++) {
                if(page==NULL) page=hsxpl_checklist->pages;
                else page=page->next;
                if(page!=NULL) {
                    uint64_t x = (uint64_t)i; x = x << 32;
                    uint64_t y = (uint64_t) page->no_items;
                    uint64_t z = x|y;
                    hsmp_net_add_msg_to_pkt(pkt,HSMP_CLIST_DESC_NOITEMS,&z);
                }
            }
        }
        
        hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIREFB);
        free(pkt);
    }
}

/* hsxpl_clist_showtime is called from the runtime method periodically in order
 * to perform all tasks related to the checklist module of the plugin.
 */
void hsxpl_clist_showtime(void) {

    return;
    
    /* char path[1024]; */
    /* hsxpl_read_checklist_if_needed(); */
    
}
