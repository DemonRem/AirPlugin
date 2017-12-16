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

#include "HSAIRXPLDREF.h"

#include "HSAIRXPL.h"
#include "HSMPMSG.h"
#include "HSMPNET.h"
#include "HSMPTCP.h"
#include "HSAIRXPLMISC.h"

hsairpl_dref_read_req_t *hsairpl_dref_tictacbase=NULL;
hsairpl_dref_read_req_t *hsairpl_dref_secondbase=NULL;

/* Find a request in a queue */
hsairpl_dref_read_req_t *hsairpl_dref_find_read_req(hsairpl_dref_read_req_t *req,hsairpl_dref_read_req_t *queue) {

  hsairpl_dref_read_req_t *p=queue;
  while(p!=NULL) {
    if(p->data.drid==req->data.drid) {
      if(p->dtype==req->dtype) {
        if(!strcmp(p->dref,req->dref)) {
          if(!memcmp(&p->from,&req->from,sizeof(struct sockaddr_in))) {
            return p;
          }
        }
      }
    }
    p=p->next;
  }
  return NULL;
}

/* Remove a request from a queue */
void hsairpl_dref_delete_read_req(hsairpl_dref_read_req_t *req,hsairpl_dref_read_req_t **base) {

  hsairpl_dref_read_req_t *p=hsairpl_dref_find_read_req(req,*base);
  if(p!=NULL) {
    if(p == *base) {
      *base = p->next;
      free(p);
    } else {
      hsairpl_dref_read_req_t *p2 = *base;
      while(p2!=NULL) {
        if(p2->next == p) {
          p2->next = p->next;
          free(p);
          break;
        }
        p2=p2->next;
      }
    }
  }
}

/* Returns the MID for the given request or 0 if failed */
uint32_t hsairpl_dref_process_dref_read_request(hsairpl_dref_read_req_t *req) {

  uint32_t mid=HSMP_MID_DREF_CLASSTYPE|HSMP_MID_DREF_GROUP|HSMP_MID_DREF_O_RD_REP;

  XPLMDataRef dref=XPLMFindDataRef(req->dref);
  uint32_t datasize=0;
  if(dref!=NULL) {
      switch (req->dtype) {
        case (HSMP_MID_DREF_T_INT32): {
          mid |= HSMP_MID_DREF_T_INT32;
          int32_t i=XPLMGetDatai(dref);
          memcpy(req->data.dval,&i,sizeof(int32_t));
          datasize=sizeof(int32_t);
          break;
        }
        case (HSMP_MID_DREF_T_FLOAT): {
          mid |= HSMP_MID_DREF_T_FLOAT;
          float i=XPLMGetDataf(dref);
          memcpy(req->data.dval,&i,sizeof(float));
          datasize=sizeof(float);
          break;
        }
        case (HSMP_MID_DREF_T_DOUBLE): {
          mid |= HSMP_MID_DREF_T_DOUBLE;
          double i=XPLMGetDataf(dref);
          memcpy(req->data.dval,&i,sizeof(double));
          datasize=sizeof(double);
          break;
        }
        case (HSMP_MID_DREF_T_STR): {
          mid |= HSMP_MID_DREF_T_STR;
          XPLMGetDatab(dref,req->data.dval,0,127);
          datasize=(uint32_t)strlen(req->data.dval)+1;
          break;
        }
        default:
          break;
      }
  }
  if(datasize>0) {
    while(datasize%4) datasize+=1;        /* Round to 4 byte alignment */
    datasize+=sizeof(req->data.drid);     /* drid */
    datasize/=4;                          /* Convert to no of 4 byte values */
    if(datasize<256) {
      mid |= (datasize<<16);
      return mid;
    }
  }
  return 0;
}

void hsairpl_dref_process_message(uint32_t mid,void *data,struct sockaddr_in *from) {

  uint32_t mop = (mid & 0xFF) & 0xC0;
  uint32_t mfreq = (mid & 0xFF) & 0x38;
  uint32_t mdtype = (mid & 0xFF) & 0x07;

  char str[512];
  sprintf(str,"hsairpl_dref_process_message(mid:0x%08X from:%s)",mid,inet_ntoa(from->sin_addr));
  hsxpl_log(HSXPLDEBUG_ACTION,str);

  switch(mop) {
    case(HSMP_MID_DREF_O_RD_REQ): {
      hsmp_dref_read_req_t *hreq=(hsmp_dref_read_req_t *)data;
      hsairpl_dref_read_req_t *req=(hsairpl_dref_read_req_t *)calloc(1, sizeof(hsairpl_dref_read_req_t));
      if(req!=NULL) {
        memcpy(req->dref,hreq->dref,127);
        req->data.drid=hreq->drid;
        req->dtype=mdtype;
        memcpy(&req->from,from,sizeof(struct sockaddr_in));

        if(mfreq==HSMP_MID_DREF_F_DISABLE) {
          hsairpl_dref_delete_read_req(req,&hsairpl_dref_tictacbase);
          hsairpl_dref_delete_read_req(req,&hsairpl_dref_secondbase);
          free(req);
        } else if(mfreq==HSMP_MID_DREF_F_ONCE) {
          hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
          uint32_t mid=hsairpl_dref_process_dref_read_request(req);
          if(mid) {
            hsmp_net_add_msg_to_pkt(pkt,mid,(void *)&req->data);
            hsmp_net_tgt_list_t *peer=hsmp_peer_with_sa(from);
            if(peer!=NULL) {
              hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
            }
          }
          free(pkt);
          free(req);
        } else if(mfreq==HSMP_MID_DREF_F_TICTAC) {
           hsairpl_dref_delete_read_req(req,&hsairpl_dref_secondbase);
          if(hsairpl_dref_find_read_req(req,hsairpl_dref_tictacbase)==NULL) {
            req->next=hsairpl_dref_tictacbase;
            hsairpl_dref_tictacbase=req;
          }
        } else if(mfreq==HSMP_MID_DREF_F_SECOND) {
          hsairpl_dref_delete_read_req(req,&hsairpl_dref_tictacbase);
          if(hsairpl_dref_find_read_req(req,hsairpl_dref_secondbase)==NULL) {
            req->next=hsairpl_dref_secondbase;
            hsairpl_dref_secondbase=req;
          }
        } else {
          free(req);
        }
      }
      break;
    }
    case(HSMP_MID_DREF_O_WR_REQ): {
      hsmp_dref_write_req_t *hreq=(hsmp_dref_write_req_t *)data;

      XPLMDataRef dref=XPLMFindDataRef(hreq->dref);
      if(dref!=NULL) {
        if(XPLMCanWriteDataRef(dref)) {
          switch (mdtype) {
            case (HSMP_MID_DREF_T_INT32): {

              int32_t *i=(int32_t *)hreq->dval;
              XPLMSetDatai(dref,*i);
              break;
            }
            case (HSMP_MID_DREF_T_FLOAT): {
              float *i=(float *)hreq->dval;
              XPLMSetDataf(dref,*i);
              break;
            }
            case (HSMP_MID_DREF_T_DOUBLE): {
              double *i=(double *)hreq->dval;
              XPLMSetDatad(dref,*i);
              break;
            }
            case (HSMP_MID_DREF_T_STR): {
              char *s=(char *)hreq->dval;
              XPLMSetDatab(dref,s,0,(int)strlen(s)+1);
              break;
            }
            default:
              break;
          }
        }
      }
      break;
    }
  }
}

void hsairpl_dref_showtime_base(hsairpl_dref_read_req_t **base) {

  hsairpl_dref_read_req_t *p = *base;
  hsairpl_dref_read_req_t *p2 = *base;

  hsmp_net_tgt_list_t *peer=NULL;
  hsmp_net_tgt_list_t *previouspeer=NULL;

  /* Build an initial packet */
  hsmp_pkt_t *pkt=NULL;
  int psize=1400; int msgcount=0;

  while(p!=NULL) {
    if(pkt==NULL) pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    /* Try to find peer */
    peer=hsmp_peer_with_sa(&p->from);
    if(previouspeer==NULL) previouspeer=peer;
    if(peer==NULL) {  /* Peer no longer valid, remove request from list */
      p2=p->next;
      hsairpl_dref_delete_read_req(p,base);
      p=p2;
      continue;
    } else {
      uint32_t mid=hsairpl_dref_process_dref_read_request(p);
      if(mid) {
        int peersdiffer=1;
        if(!memcmp(&previouspeer->sa,&peer->sa,sizeof(struct sockaddr_in))) {
          peersdiffer=0;
        }
        if(psize<HSMP_MESSAGE_SIZE_FOR_MID(mid) || peersdiffer) {
          hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
          msgcount=0;
          free(pkt);
          pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
          if(pkt==NULL) break;
          psize=1400;
        }
        hsmp_net_add_msg_to_pkt(pkt,mid,(void *)&p->data);
        msgcount++;
        psize -= HSMP_MESSAGE_SIZE_FOR_MID(mid);
        previouspeer = peer;
      }
    }
    p=p->next;
  }
  if(msgcount>0 && peer!=NULL) {
    hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
  }
  free(pkt);
}

void hsairpl_dref_showtime_tictac(void) {
  hsairpl_dref_showtime_base(&hsairpl_dref_tictacbase);
}

void hsairpl_dref_showtime_sec(void) {
  hsairpl_dref_showtime_base(&hsairpl_dref_secondbase);
}
