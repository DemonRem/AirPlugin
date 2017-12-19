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

#include <dirent.h>
#include <sys/types.h>


#if !defined(_WIN32)
#include <sys/dir.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#include "HSAIRXPL.h"
#include "HSMPMSG.h"
#include "HSMPNET.h"
#include "HSMPTCP.h"
#include "HSAIRXPLMISC.h"

#include "HSAIRXPLCLIST.h"

/* A local pointer to a list of checklists */
hsairpl_clist_ref_t *__hsairpl_clist_ref_base__=NULL;

/* Clears the reference list so it can re-initialised. We probably
 * don't need this function as once read / loaded checklists don't change. */
void hsairpl_clist_clear_references(void) {

  hsxpl_log(HSXPLDEBUG_ACTION,"hsairpl_clist_clear_references()");

  hsairpl_clist_ref_t *p=__hsairpl_clist_ref_base__;
  hsairpl_clist_ref_t *p2;
  while(p!=NULL) {
    p2=p->next;
    free(p);
    p=p2;
  }
  __hsairpl_clist_ref_base__=NULL;
}

/* For each entry found in hsairpl_clist_read_references() this method
 * reads and creates a reference for the given list. */
void hsairpl_clist_create_ref_for(char *rpath) {

  char str[512];
  sprintf(str,"hsairpl_clist_create_ref_for(%s)",rpath);
  hsxpl_log(HSXPLDEBUG_ACTION,str);

  char line[1024]; line[1023]='\0';

  char checklist[32]; memset(checklist,0,32);
  int i=1;
  while(hsxpl_strentry(i,rpath,'/',NULL)!=NULL) {
    i++;
  }
  i--; i--;
  if(hsxpl_strentry(i,rpath,'/',line)!=NULL) {
    strncpy(checklist,line,31);
  } else {
    return;
  }

  /* Skip if we already have it, i.e. don't allow duplicates */
  hsairpl_clist_ref_t *p=__hsairpl_clist_ref_base__;
  while(p!=NULL) {
    if(!strcmp(p->checklist,checklist)) break;
    p=p->next;
  }

  if(p==NULL) {
    uint64_t fsize=0;
    struct stat fstat;
    if(!stat(rpath,&fstat)) {
      fsize=(uint64_t)fstat.st_size;
    }
    if(fsize>0) {
      hsairpl_clist_ref_t *np=(hsairpl_clist_ref_t *)calloc(sizeof(hsairpl_clist_ref_t), 1);
      if(np!=NULL) {
        strncpy(np->checklist,checklist,31);
        strncpy(np->path,rpath,511);
        np->fsize=fsize;

        np->next=__hsairpl_clist_ref_base__;
        __hsairpl_clist_ref_base__=np;
      }
    }
  }
}

/* hsairpl_clist_read_references() parses all clist.txt files under
 * $(X-Plane)/Aircraft and constructs a list of these which are
 * kept in memory so that they can be retrieved. This function calls
 * itself recursively and should only be called with rpath=NULL from
 * the outside. */
void hsairpl_clist_read_references(char *rpath) {

  char path[512]; memset(path,0,512);
  if(rpath==NULL) {

    /* Only read once */
    if(__hsairpl_clist_ref_base__!=NULL) return;

    hsxpl_log(HSXPLDEBUG_ACTION,"hsairpl_clist_read_references()");
    hsairpl_clist_clear_references();

    XPLMGetSystemPath(path);
    strncat(path, "Aircraft", 511);
  } else {
    strncpy(path,rpath,511);
  }

  DIR *dirp = opendir(path);
  if(dirp!=NULL) {
    struct dirent *dp;
    while((dp=readdir(dirp))!=NULL) {
      char dname[256];

#if APL
      if(dp->d_namlen > 0) {

        memcpy(dname,dp->d_name,dp->d_namlen);
        dname[dp->d_namlen]='\0';
#else
        if(1) {
          strcpy(dname,dp->d_name);
#endif
          if(!strcmp(dname,".")) continue;
          if(!strcmp(dname,"..")) continue;
          char rpath[512];
          sprintf(rpath,"%s/%s",path,dname);
          if(hsxpl_path_is_dir(rpath)) {
            hsairpl_clist_read_references(rpath);
          } else if(hsxpl_path_is_reg(rpath)) {
            if(!strcmp(dname,"clist.txt")) {
              hsairpl_clist_create_ref_for(rpath);
            }
          }
        }
      }
      closedir(dirp);
    }
  }

  /* Returns the list of references or NULL if none */
  hsairpl_clist_ref_t *hsairpl_clist_references(void) {
    return __hsairpl_clist_ref_base__;
  }

  /* Send checklist indexes to a specific client sa */
  void hsairpl_clist_send_indexes_to(struct sockaddr_in*to) {

    char str[512];
    sprintf(str,"hsairpl_clist_send_indexes_to(%s)",inet_ntoa(to->sin_addr));
    hsxpl_log(HSXPLDEBUG_ACTION,str);

    if(__hsairpl_clist_ref_base__==NULL) {
      return;
    }

    hsmp_net_tgt_list_t *peer = hsmp_peer_with_sa(to);
    if(peer==NULL) {
      return;
    }

    hsairpl_clist_ref_t *ap=__hsairpl_clist_ref_base__;

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    int n=0; int psize=1400;
    if(pkt!=NULL) {
      while(ap!=NULL) {
        hsmp_clist_idx_t idx;
        strcpy(idx.checklist,ap->checklist);
         idx.fsize=ap->fsize;
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_CLIST_IDX,&idx);
        n++;
        psize -= (int)sizeof(hsmp_clist_idx_t);
        if(psize < (int)sizeof(hsmp_clist_idx_t)) {
          if(n>0) {
            hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
          }
          free(pkt);
          n=0;
          pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
          if(pkt==NULL) break;
        }
        ap=ap->next;
      }
    }
    if(n>0 && pkt!=NULL) {
      hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
    }
    free(pkt);
  }

  /* Send a list over the TCP server connection */
  int __hsairpl_clist_file_being_sent_fd__=-1;

  /* Sends the next set of bytes for a clist.txt transfer that is ongoing */
  int hsairpl_clist_send_next_list_bytes(void) {

    int netfd=hsmp_tcp_server_accept_incoming();
    int t=0;

    if(__hsairpl_clist_file_being_sent_fd__>=0 && netfd>=0) {

      ssize_t n;ssize_t s;
      uint8_t buffer[65536];

      if((n=read(__hsairpl_clist_file_being_sent_fd__,buffer,65536))>0) {
        s=send(netfd,buffer,n,0);
        if(s!=n || s<0) {
          hsmp_tcp_close_server_client();
          close(__hsairpl_clist_file_being_sent_fd__);
          __hsairpl_clist_file_being_sent_fd__=-1;
          return -1;
        }
        t=(int)s;
      } else {
        hsmp_tcp_close_server_client();
        close(__hsairpl_clist_file_being_sent_fd__);
        __hsairpl_clist_file_being_sent_fd__=-1;
      }
    }
    return t;
  }

  struct sockaddr_in __hsairpl_clist_send_list_to_addr__;
  char __hsairpl_clist_send_list_to_list_id__[32];

  void hsairpl_clist_send_list_to(char *listid, struct sockaddr_in*to) {

      strcpy(__hsairpl_clist_send_list_to_list_id__,listid);
    memcpy(&__hsairpl_clist_send_list_to_addr__,to,sizeof(__hsairpl_clist_send_list_to_addr__));

    if(hsmp_tcp_network_server_is_open()) {
      if(__hsairpl_clist_file_being_sent_fd__<0) {
        hsairpl_clist_ref_t *p=__hsairpl_clist_ref_base__;
        while(p!=NULL) {
          if(!strcmp(p->checklist,listid)) {
            break;
          }
          p=p->next;
        }
        if(p!=NULL) {
#if IBM
          __hsairpl_clist_file_being_sent_fd__=open(p->path,O_RDONLY|O_BINARY);
#else
          __hsairpl_clist_file_being_sent_fd__=open(p->path,O_RDONLY);
#endif
          if(__hsairpl_clist_file_being_sent_fd__>=0) {

          } else {
            hsairpl_clist_send_req_fail();
          }
        } else {
          hsairpl_clist_send_req_fail();
        }
      } else {
        hsairpl_clist_send_req_fail();
      }
    } else {
      hsairpl_clist_send_req_fail();
    }
  }

  /* Sends a request fail back to the client app */
  void hsairpl_clist_send_req_fail(void) {

    if(__hsairpl_clist_ref_base__==NULL) return;
    hsmp_net_tgt_list_t *peer = hsmp_peer_with_sa(&__hsairpl_clist_send_list_to_addr__);
    if(peer==NULL) return;

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_CLIST_REQFAIL,__hsairpl_clist_send_list_to_list_id__);
      hsmp_net_send_to_target(pkt, pkt->hdr.dsize, peer);
      free(pkt);
    }
  }

