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

#include "hsxpldref-requests.h"
#include "hsxpl.h"
#include "hsmpmsg.h"
#include "hsmpnet.h"
#include "hsmptcp.h"
#include "hsxplmisc.h"

hsairpl_dref_read_req_t *hsairpl_dref_tictacbase=NULL;
hsairpl_dref_read_req_t *hsairpl_dref_secondbase=NULL;

/* Find a request in a queue */
hsairpl_dref_read_req_t *hsairpl_dref_find_read_req(hsairpl_dref_read_req_t *req,hsairpl_dref_read_req_t *queue) {

  hsairpl_dref_read_req_t *p=queue;
  while(p!=NULL) {

    if(ntohs(p->from.sin_port)==ntohs(req->from.sin_port)) {
      if(p->from.sin_addr.s_addr == req->from.sin_addr.s_addr) {
        if(p->data.drid==req->data.drid) {
          return p;
        }
      }
    }
    p=p->next;
  }
  if(p==NULL) {
    while(p!=NULL) {
      if(ntohs(p->from.sin_port)==ntohs(req->from.sin_port)) {
        if(p->from.sin_addr.s_addr == req->from.sin_addr.s_addr) {
          if(!strcmp(p->dref,req->dref)) {
            return p;
          }
        }
        p=p->next;
      }
    }
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

  char dataref[128];
  int arrayindex=-1;
  int dlen;

  strncpy(dataref,req->dref,128);

  /* See if it ends in [idx] for array types */
  dlen=(int)strlen(dataref);
  int i=dlen-1;
  if(dataref[i]==']') {
    char *openbrackets=&dataref[i];
    while(i>=0) {
      openbrackets--;
      if(*openbrackets=='[')  {
        i--;
        break;
      }
      i--;
    }
    if(i>=0) {
      *openbrackets='\0';openbrackets++;
      dataref[dlen-1]='\0';
      arrayindex=atoi(openbrackets);
    }
  }

  /* Process dataref */
  XPLMDataRef dref=XPLMFindDataRef(dataref);
  uint32_t datasize=0;
  if(dref!=NULL) {
    uint32_t dtype=req->dtype;
    if(dtype==HSMP_MID_DREF_T_UNDEF) {
      XPLMDataTypeID dt=XPLMGetDataRefTypes(dref);
      if(dt==0) return 0;
      if(dt&xplmType_Int) {
        dtype=HSMP_MID_DREF_T_INT32;
      } else if(dt&xplmType_Double) {
        dtype=HSMP_MID_DREF_T_DOUBLE;
      } else if(dt&xplmType_Float) {
        dtype=HSMP_MID_DREF_T_FLOAT;
      } else if(dt&xplmType_FloatArray) {
        dtype=HSMP_MID_DREF_T_AFLOAT;
      } else if(dt&xplmType_IntArray) {
        dtype=HSMP_MID_DREF_T_AINT32;
      } else if(dt&xplmType_Data) {
        dtype=HSMP_MID_DREF_T_STR;
      } else {
        return 0;
      }
    }
    switch (dtype) {
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
      case (HSMP_MID_DREF_T_AINT32): {
        mid |= HSMP_MID_DREF_T_INT32;
        if(arrayindex>=0) {
          int32_t i;
          if(XPLMGetDatavi(dref,&i,arrayindex,1)==1) {
            memcpy(req->data.dval,&i,sizeof(int32_t));
            datasize=sizeof(int32_t);
          }
        }
        break;
      }
      case (HSMP_MID_DREF_T_AFLOAT): {
        mid |= HSMP_MID_DREF_T_FLOAT;
        if(arrayindex>=0) {
          float i;
          if(XPLMGetDatavf(dref,&i,arrayindex,1)==1) {
            memcpy(req->data.dval,&i,sizeof(float));
            datasize=sizeof(float);
          }
        }
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

    /* Try to find peer */
    peer=hsmp_peer_with_sa(&p->from);

    if(peer==NULL) {  /* Peer no longer valid, remove request from list */
      p2=p->next;
      hsairpl_dref_delete_read_req(p,base);
      p=p2;
      continue;
    }

    /* If we have no packet, create one */
    if(pkt==NULL) {
      psize=1400; msgcount=0;
      pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    }

    /* If we have a different peer from the previous, send prev packet and
     * create a new one */
    if(previouspeer!=NULL) {
      if(ntohs(previouspeer->sa.sin_port)!=ntohs(peer->sa.sin_port) ||
         previouspeer->sa.sin_addr.s_addr != peer->sa.sin_addr.s_addr) {

        if(msgcount>0) {
          hsmp_net_send_to_target(pkt,pkt->hdr.dsize,previouspeer);
          free(pkt);
          msgcount=0;
          psize=1400;
          pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
          if(pkt==NULL) break;
        }
      }
    }

    uint32_t mid=hsairpl_dref_process_dref_read_request(p);
    if(!mid)  {
      p=p->next;
      continue;

    }

    /* If we don't have enough space for the new packet, dispatch
     * packet first and create a new one */
    if(psize<HSMP_MESSAGE_SIZE_FOR_MID(mid)) {
      if(msgcount>0) {
        hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
        free(pkt);
        msgcount=0;
        psize=1400;
        pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
        if(pkt==NULL) break;
      }
    }

    /* Now add message */
    hsmp_net_add_msg_to_pkt(pkt,mid,(void *)&p->data);
    msgcount++;
    psize -= HSMP_MESSAGE_SIZE_FOR_MID(mid);

    /* Update previous peer to us */
    previouspeer = peer;

    /* Move to next dataref */
    p=p->next;
  }

  if(pkt!=NULL) {
    if(msgcount>0 && peer!=NULL) {
      hsmp_net_send_to_target(pkt,pkt->hdr.dsize,peer);
    }
    free(pkt);
  }
}

void hsairpl_dref_showtime_tictac(void) {
  hsairpl_dref_showtime_base(&hsairpl_dref_tictacbase);
}

void hsairpl_dref_showtime_sec(void) {
  hsairpl_dref_showtime_base(&hsairpl_dref_secondbase);
}
