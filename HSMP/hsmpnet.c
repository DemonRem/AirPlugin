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
 *
 * The HSMPNET.x files contain code used to implement the UDP message protocol
 * which interacts with network sockets, maintains and manages a list of connected
 * peers.
 *
 */

#include "hsmpmsg.h"
#include "hsmpnet.h"
#include "hsmpatp.h"
#include "hsmptcp.h"

/* #define HSMPDEBUG 1 */

void (*hsmp_net_message_received_callback)(uint32_t mid,void *data,struct sockaddr_in *from);
void (*hsmp_net_packet_received_callback)(hsmp_pkt_t *pkt,struct sockaddr_in *from,uint32_t nbytes);
void (*hsmp_net_peer_created_callback)(struct sockaddr_in *addr,uint32_t idver);
void (*hsmp_net_peer_removed_callback)(struct sockaddr_in *addr);

/* Local private prototypes */

/* hsmp_add_target_to_list adds a given IP address and port to a target list
 * if one doesn't exist already and sets its timestamp to the value of ts.
 * If the target already exists on the list, it's texpire is updated to ts
 * only if its previous value is non-zero. In other words, targets which were
 * initially created with a timestamp of 0 are considered static and don't see
 * their texpire updated and consequently they cannot expire. */
void hsmp_add_target_to_list(char *addr,uint32_t port,hsmp_net_tgt_list_t **tlist,time_t ts,uint32_t idver);

/* Similarly the hsmp_remove_target_from_list function locations and removes a
 * given target from the specified list. */
void hsmp_remove_target_from_list(char *addr,uint32_t port,hsmp_net_tgt_list_t **tlist);


/* hsmp_net_read_packet() reads a packet from the network and returns it,
 * or NULL if an error occurs or there is nothing to be read.
 * sa is filled with the sender info. */
hsmp_pkt_t *hsmp_net_read_packet(struct sockaddr_in *sa);

/* The hsmp_net_process_packet() function takes a packet and processes it
 * by parsing all its messages and processing each individually. */
void hsmp_net_process_packet(hsmp_pkt_t *pk,struct sockaddr_in *from);

/* hsmp_net_process_message() processes an individual message of type
 * mid and contents data, if any. */
void hsmp_net_process_message(uint32_t mid,void *data,struct sockaddr_in *from,uint32_t idver);

/* Local log function if HSMPDEBUG is set */
void hsmp_net_log(char *s);

/* End of local prototypes section */

/* Our network socket */
#if defined(_WIN32)
WSADATA hsmp_wsa_data;
SOCKET hsmp_net_socket=INVALID_SOCKET;
#else
int hsmp_net_socket=INVALID_SOCKET;
#endif


/* Our node type ID */
uint32_t hsmp_node_type=0;

/* The original initialisation address */
char hsmp_init_addr[64];

/* The original initialisation port */
uint32_t hsmp_init_port=0;

/* The number of stream peers */
uint32_t hsmp_number_of_stream_peers=0;

/* The total number of peers */
uint32_t hsmp_peer_count=0;

/* The number of bytes read in a packet, when one is read */
uint32_t hsmp_net_packet_in_len=0;

/* The list of multicast addresses to which we send hello's */
hsmp_net_tgt_list_t    *hsmp_net_mcast_list=NULL;

/* The list of peers from which we have received valid hellos */
hsmp_net_tgt_list_t    *hsmp_net_peer_list=NULL;

void hsmp_add_target_to_list(char *addr,uint32_t port,hsmp_net_tgt_list_t **tlist,time_t ts,uint32_t idver) {

  struct in_addr a4;
  struct sockaddr_in sa4;

  /* Validate addr and port */
  if(port<1) return;
  a4.s_addr=inet_addr(addr);
  if(a4.s_addr==INADDR_NONE) return;

  /* Locate target, or not ...*/
  hsmp_net_tgt_list_t *xp=NULL;

  if(*tlist!=NULL) {
    xp = *tlist;

    while(xp!=NULL) {
      if(xp->sa.sin_port == htons(port)) {
        if(xp->sa.sin_addr.s_addr == a4.s_addr) {
          break;
        }
      }
      xp=xp->next;
    }
  }

  /* If located update texpire,  thello and idver and return here */
  if(xp!=NULL) {

    xp->idver=idver;
    /* Only update dynamic targets */
    if(xp->texpire) {
      xp->texpire=ts;
    }

    /* Update last hello to passed time */
    xp->thello=ts;

    return;
  }

  /* Otherwise we add it to the list */

  /* Prepare the sockaddr_in structure */
  sa4.sin_family=AF_INET;
  sa4.sin_port=htons(port);
  sa4.sin_addr=a4;

  /* Allocate space for new target */
  xp=(hsmp_net_tgt_list_t *)malloc(sizeof(hsmp_net_tgt_list_t));

  if(xp!=NULL) {

    /* Set new target values */
    memset(xp,0,sizeof(hsmp_net_tgt_list_t));
    xp->next=NULL;
    xp->thello=ts;
    xp->texpire=ts;
    xp->idver=idver;
    memcpy(&(xp->sa),&sa4,sizeof(struct sockaddr_in));

    /* Add new target to list */
    if(*tlist == NULL) {
      *tlist = xp;
    } else {
      hsmp_net_tgt_list_t *tk= *tlist;
      while(tk->next!=NULL) tk=tk->next;
      tk->next=xp;
    }

    if(*tlist==hsmp_net_peer_list) {
      hsmp_peer_count++;

      if(hsmp_net_peer_created_callback!=NULL) {
        hsmp_net_peer_created_callback(&(xp->sa),xp->idver);
      }
    }
  }
}

void hsmp_remove_target_from_list(char *addr,uint32_t port,hsmp_net_tgt_list_t **tlist) {

  struct in_addr a4;

  /* Validate addr and port */
  if(port<1) return;
  a4.s_addr=inet_addr(addr);
  if(a4.s_addr==INADDR_NONE) return;

  /* Locate multicast target, or not ...*/
  hsmp_net_tgt_list_t *xp= *tlist;
  hsmp_net_tgt_list_t *xpp=NULL;

  while(xp!=NULL) {
    if(xp->sa.sin_port == htons(port)) {
      if(xp->sa.sin_addr.s_addr == a4.s_addr) {
        break;
      }
    }
    xpp=xp;
    xp=xp->next;
  }

  /* If located remove target */
  if(xp!=NULL) {

    /* If we are removing from the peer list, decrement peer stream counter if
     * the peer was streaming */
    if(*tlist == hsmp_net_peer_list) {
      if(xp->flags & HSMP_TGT_FLAG_REQSTREAM) {
        if(hsmp_number_of_stream_peers) hsmp_number_of_stream_peers--;
        xp->flags &= ~HSMP_TGT_FLAG_REQSTREAM;
      }

      if(hsmp_peer_count>0) hsmp_peer_count--;

      if(hsmp_net_peer_removed_callback!=NULL) {
        hsmp_net_peer_removed_callback(&(xp->sa));
      }
    }

    if(xp== *tlist) {

      *tlist = xp->next;
      xp->next=NULL;
      free(xp);

    } else {

      if(xpp!=NULL) {
        xpp->next=xp->next;
      }
      xp->next=NULL;
      free(xp);
    }
  }

}

/* hsmp_peer_list returns the list of peers */
hsmp_net_tgt_list_t *hsmp_peer_list(void) {
  return hsmp_net_peer_list;
}

/* hsmp_peer_with_addr_port() looks for the given addr/port in the list
 * of peer targets and returns it if found, and NULL if not */
hsmp_net_tgt_list_t *hsmp_peer_with_addr_port(char *addr,uint32_t port) {

  /* Validate addr and port */
  struct in_addr a4;
  if(port<1) return NULL;
  a4.s_addr=inet_addr(addr);
  if(a4.s_addr==INADDR_NONE) return NULL;

  hsmp_net_tgt_list_t *xp= hsmp_net_peer_list;
  while(xp!=NULL) {
    if(xp->sa.sin_port == htons(port)) {
      if(xp->sa.sin_addr.s_addr == a4.s_addr) {
        return xp;
      }
    }
    xp=xp->next;
  }
  return NULL;
}


/* A similar function to hsmp_peer_with_addr_port but taking a sockaddr_in
 * as an argument. */
hsmp_net_tgt_list_t *hsmp_peer_with_sa(struct sockaddr_in *sa) {

  hsmp_net_tgt_list_t *xp= hsmp_net_peer_list;
  while(xp!=NULL) {
    if(xp->sa.sin_family==sa->sin_family) {
      if(xp->sa.sin_port == sa->sin_port) {
        if(xp->sa.sin_addr.s_addr == sa->sin_addr.s_addr) {
          return xp;
        }
      }
    }
    xp=xp->next;
  }
  return NULL;
}

/* hsmp_req_stream_data_from() sends a HSMP_MID_STREAM_START message to the
 * given peer if it is found on the list asking for data to be streamed. */
void hsmp_req_stream_data_from(char *addr,uint32_t port) {


  /* No point if there is no network */
#if IBM
  if(hsmp_net_socket==INVALID_SOCKET) return;
#else
  if(hsmp_net_socket<0 || hsmp_net_socket>2147483647) return;
#endif

  /* Look for peer first */
  hsmp_net_tgt_list_t *xp=hsmp_peer_with_addr_port(addr,port);

  /* If found, send request */
  if(xp!=NULL) {

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {
      if(hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_HELLO,&(pkt->hdr.idver))) {
        if(hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STREAM_START,NULL)) {

#ifdef HSMPDEBUG
          char str[256];
          sprintf(str,">>> REQUESTING STREAM DATA START FROM %s %d",addr,port);
          hsmp_net_log(str);
#endif
          hsmp_net_send_to_target(pkt,pkt->hdr.dsize,xp);
        }
      }
      free(pkt);

    }
  }
}

/* Inversely,hsmp_stop_stream_data_from() sends an HSMP_MID_STREAM_STOP message
 * asking for data sending to be stopped. */
void hsmp_stop_stream_data_from(char *addr,uint32_t port) {

  /* No point if there is no network */
#if IBM
  if(hsmp_net_socket==INVALID_SOCKET) return;
#else
  if(hsmp_net_socket<0 || hsmp_net_socket>2147483647) return;
#endif

  /* Look for peer first */
  hsmp_net_tgt_list_t *xp=hsmp_peer_with_addr_port(addr,port);

  /* If found, send request */
  if(xp!=NULL) {

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {
      if(hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_HELLO,&(pkt->hdr.idver))) {
        if(hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_STREAM_STOP,NULL)) {

#ifdef HSMPDEBUG
          char str[256];
          sprintf(str,">>> REQUESTING STREAM DATA STOP FROM %s %d",addr,port);
          hsmp_net_log(str);
#endif
          hsmp_net_send_to_target(pkt,pkt->hdr.dsize,xp);
        }
      }
      free(pkt);
    }

  }
}

/* The hsmp_number_of_stream_clients() function returns the number of
 * peers we have on our list that have asked to receive stream data.
 * This function may be used to query the engine and therefore avoid
 * unecessary processing should there be no listening clients waiting for
 * data. */
uint32_t hsmp_number_of_stream_clients(void) {
  return hsmp_number_of_stream_peers;
}

/* hsmp_number_of_peers() returns the total number of peers in the peer list */
uint32_t hsmp_number_of_peers(void) {
  return hsmp_peer_count;
}

/* The hsmp_initialise_network initialises the network socket to listen on lport
 * if specified, with source type ntype.
 *
 * If lport is 0 this function attempts to obtain a free port between 59000 and
 * 59999. This should be the default behaviour for sim sources whose ports are
 * dynamically allocated .
 *
 * If lport is non-zero, this function only tries to open the specified port.
 * In this case this function also issues a join to HSMP_DEF_MULTICAST_ADDR
 *
 * The node type should be one of hsmp_PKT_NT_* as defined in HSMPMSG.h
 *
 * On success, hsmp_net_socket will be >= 0. On error it will be < 0.
 *
 * The socket is bound to addr.
 *
 */
void hsmp_initialise_network(const char *addr,uint32_t lport,uint32_t ntype) {

  struct sockaddr_in receiver_sa;
  uint32_t from_port;
  uint32_t to_port;
  uint32_t i;

  /* int yes=1; */

#if defined(_WIN32)
  static int winInitialised=0;
#endif

#if IBM
  if(hsmp_net_socket!=INVALID_SOCKET) return;
#else
  if(hsmp_net_socket>=0 && hsmp_net_socket<=2147483647) return;
#endif

  if(lport) { from_port=lport; to_port=lport; }
  else { from_port=HSMP_DYN_UDP_PORT_START; to_port=HSMP_DYN_UDP_PORT_END; }

  /* Set source ID, filter out any noise on 3 LSBs */
  hsmp_node_type=ntype & 0xFF000000;

  /* Set local port for future initialisation */
  hsmp_init_port=lport;

  if(addr==NULL)
    sprintf(hsmp_init_addr,"0.0.0.0");
  else
  {
    /* Don't copy ourselves to ourselves */
    if(hsmp_init_addr!=addr) {
      strncpy(hsmp_init_addr,addr,63);
      hsmp_init_addr[63]='\0';
    }
  }



#if defined(_WIN32)
  if(!winInitialised) {
    if(WSAStartup(0x202,&hsmp_wsa_data)!=0)
      return;
  }
  winInitialised=1;
#endif

  /* Only attempt to open socket once per second and not more */
  static time_t hsxpl_last_init_attempt=0;
  time_t time_now=time(NULL);
  if(time_now==hsxpl_last_init_attempt) return;
  hsxpl_last_init_attempt=time_now;

  /* Create socket */
  hsmp_net_socket=socket(AF_INET,SOCK_DGRAM,0);

#if IBM
  if(hsmp_net_socket==INVALID_SOCKET) return;
#else
  if(hsmp_net_socket<0 || hsmp_net_socket>2147483647) return;
#endif

  /*
   setsockopt(hsmp_net_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(int));
   */

  struct in_addr a4;
  a4.s_addr=inet_addr(hsmp_init_addr);
  if(a4.s_addr!=INADDR_NONE)
    receiver_sa.sin_addr.s_addr=a4.s_addr;
  else
    receiver_sa.sin_addr.s_addr=INADDR_ANY;
  receiver_sa.sin_family=AF_INET;

  /* Try to bind to a port so that we can receive data on the socket */
  for(i=from_port;i<to_port+1;i++) {

    receiver_sa.sin_port=htons(i);
    if(!bind(hsmp_net_socket,(struct sockaddr *)&receiver_sa,sizeof(struct sockaddr))) break;

    /* If we couldn't bind, close the socket */
    if(i==to_port) {
      close(hsmp_net_socket);
      hsmp_net_socket=INVALID_SOCKET;
    }
  }


  /* Join multicast group if we have a static port */
  if(lport) {
    hsmp_join_multicast_group(HSMP_DEF_MULTICAST_ADDR);
  }

  return;

}

/* hsmp_join_multicast_group issues a multicast join for the given address.
 * It is called automatically by hsmp_initialise_network but may be called
 * subsequently in addition to. */
void hsmp_join_multicast_group(char *addr) {
  struct ip_mreq msa;
  msa.imr_multiaddr.s_addr=inet_addr(addr);
  msa.imr_interface.s_addr=INADDR_ANY;
  setsockopt(hsmp_net_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,
             (char *)&msa,sizeof(msa));
}

/* hsmp_close_network() closes the network socket */
void hsmp_close_network(void) {

#if IBM
  if(hsmp_net_socket!=INVALID_SOCKET) {
#else
  if(hsmp_net_socket>=0 && hsmp_net_socket<=2147483647) {
#endif

#if defined(_WIN32)
    closesocket(hsmp_net_socket);
#else
    close(hsmp_net_socket);
#endif

    hsmp_net_socket=INVALID_SOCKET;
  }
}

/* hsmp_network_is_open checks if the network is open and returns 1 if so
 * and 0 otherwise. */
int hsmp_network_is_open(void) {

#if IBM
  if(hsmp_net_socket!=INVALID_SOCKET) return 1;
#else
    if(hsmp_net_socket>=0 && hsmp_net_socket<=2147483647) return 1;
#endif

  return 0;
}

/* hsmp_add_multicast_target() adds a target to the list of multicast targets to
 * which to send hello packets */
void hsmp_add_multicast_target(char *addr,uint32_t port) {

  hsmp_add_target_to_list(addr,port,&hsmp_net_mcast_list,0,HSMP_PKT_NT_APP|HSMP_PKT_PROTO_VER);
}

/* Similarly, hsmp_remove_multicast_target() removes a target from the list */
void hsmp_remove_multicast_target(char *addr,uint32_t port) {

  hsmp_remove_target_from_list(addr,port,&hsmp_net_mcast_list);

}

/* hsmp_add_peer_target() adds a target to the list of static targets to
 * which to send helo packets */
void hsmp_add_peer_target(char *addr,uint32_t port,uint32_t idver) {
  hsmp_add_target_to_list(addr,port,&hsmp_net_peer_list,0,idver);
}

/* Similarly, hsmp_remove_peer_target() removes a target from the list */
void hsmp_remove_peer_target(char *addr,uint32_t port) {
  hsmp_remove_target_from_list(addr,port,&hsmp_net_peer_list);
}

/* hsmp_net_make_packet returns a newly allocated data buffer containing an
 * initialised packet. The packet's buffer is always 2048, allowing for 1500
 * byte packets to be filled. */
void *hsmp_net_make_packet(void) {

  void *pkt=(void *)malloc(2048);
  if(pkt!=NULL) {
    hsmp_pkt_t *hp=(hsmp_pkt_t *)pkt;
    memset(pkt,0,2048);
    hp->hdr.idver = hsmp_node_type | HSMP_PKT_PROTO_VER;
    hp->hdr.seqno = 0;
    hp->hdr.ackno = 0;
    hp->hdr.flags = 0;
    hp->hdr.dsize = sizeof(hsmp_pkt_hdr_t);
    hp->hdr.nomsgs = 0;
  }
  return pkt;
}

/* hsmp_net_add_msg_to_pkt() tries to add the given message to the
 * packet followed by its data. If this is not possible this function returns
 0, otherwise 1 is returned. */
int hsmp_net_add_msg_to_pkt(hsmp_pkt_t *pkt,uint32_t mid,void *mdata) {

#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,">>> ADD MID TO PACKET 0x%08X",mid);
  hsmp_net_log(str);
#endif

  uint32_t ms=HSMP_SIZE_FOR_MID(mid);

  /* Make sure that the message fits in the packet */
  if(pkt->hdr.dsize + sizeof(uint32_t) > HSMP_MAX_PACKET_SIZE)
    return 0;

  /* Append message */
  void *bp= (void *)pkt + pkt->hdr.dsize;
  memcpy(bp,&mid,sizeof(uint32_t));
  bp += sizeof(uint32_t);
  if(ms>0)
    memcpy(bp,mdata,ms);

  /* Update size and no msgs */
  pkt->hdr.dsize += (ms + sizeof(uint32_t));
  pkt->hdr.nomsgs++;

  return 1;

}

/* hsmp_net_send_to_target() sends a packet to a given target */
void hsmp_net_send_to_target(void *pkt,uint32_t nbytes,hsmp_net_tgt_list_t *tgt) {

#if IBM
  if(hsmp_net_socket!=INVALID_SOCKET) {
#else
  if(hsmp_net_socket>=0 && hsmp_net_socket<=2147483647) {
#endif

#ifdef HSMPDEBUG
    char str[256];
    hsmp_pkt_t *pkt2=(hsmp_pkt_t *)pkt;
    sprintf(str,">>> SENT PKT to %s %d (%d bytes)",inet_ntoa(tgt->sa.sin_addr),ntohs(tgt->sa.sin_port),pkt2->hdr.dsize);
    hsmp_net_log(str);
#endif

    if(pkt!=NULL && nbytes>0) {
      if(sendto(hsmp_net_socket,(void *)pkt,nbytes,0,(struct sockaddr *)&tgt->sa,sizeof(struct sockaddr_in))!=nbytes)
      {
        hsmp_close_network();
      }
    }
  }

}
/* hsmp_net_send_to_all_tgts() sends a packet to all targets of the list */
void hsmp_net_send_to_all_tgts(hsmp_pkt_t *pkt,hsmp_net_tgt_list_t *tlist)
{

#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,">>> hsmp_net_send_to_all_tgts()");
  hsmp_net_log(str);
#endif

  if(pkt!=NULL && tlist!=NULL) {
    hsmp_net_tgt_list_t *xp=tlist;
    while(xp!=NULL) {
      hsmp_net_send_to_target(pkt,pkt->hdr.dsize,xp);
      xp=xp->next;
    }
  }

}
/* hsmp_net_send_to_stream_peers() sends a packet to all active peers
 * that have subscribed to streaming services using HSMP_MID_STREAM_START.
 * If ntype is 0 the packet is sent to all peers. If ntype is non 0 the
 * packet is only sent to instances of the app specified by ntype. */
void hsmp_net_send_to_stream_peers(hsmp_pkt_t *pkt,uint32_t ntype) {

#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,">>> hsmp_net_send_to_stream_peers()");
  hsmp_net_log(str);
#endif

  if(pkt!=NULL && hsmp_net_peer_list!=NULL) {
    hsmp_net_tgt_list_t *xp=hsmp_net_peer_list;
    while(xp!=NULL) {
      if(xp->flags & HSMP_TGT_FLAG_REQSTREAM)
        if((!ntype) || (ntype == (xp->idver&0xFF000000))) {
          hsmp_net_send_to_target(pkt,pkt->hdr.dsize,xp);
        }
      xp=xp->next;
    }
  }
}

/* hsmp_net_read_packet() reads a packet from the network and returns it,
 * or NULL if an error occurs or there is nothing to be read.
 * sa is filled with the sender info.
 * If a packet is red, hsmp_net_packet_in_len is set to the number
 * of bytes read; otherwise it is set to 0 */
hsmp_pkt_t *hsmp_net_read_packet(struct sockaddr_in *sa) {

  fd_set readfds;
  struct timeval tv;
  int n;
  socklen_t sl;

  hsmp_net_packet_in_len=0;

#if IBM
  if(hsmp_net_socket==INVALID_SOCKET) return NULL;
#else
    if(hsmp_net_socket<0 || hsmp_net_socket>2147483647) return NULL;
#endif

  /* See if there are any packets to be read */
  FD_ZERO(&readfds);
  FD_SET(hsmp_net_socket,&readfds);
  tv.tv_sec=0;
  tv.tv_usec=0;
  if(select(hsmp_net_socket+1,&readfds,NULL,NULL,&tv)==-1) return NULL;

  /* If not, return here */
  if(!FD_ISSET(hsmp_net_socket,&readfds)) return NULL;

  /* If an error occurs, close the network so that it can be
   reinitialised and return */

  hsmp_pkt_t *pkt=hsmp_net_make_packet();
  if(pkt==NULL) {
    hsmp_close_network();
    return NULL;
  }


  sl=sizeof(struct sockaddr_in);
  n=(int)recvfrom(hsmp_net_socket,(void *)pkt,HSMP_MAX_PACKET_SIZE,0,(struct sockaddr *)sa,&sl);
  if(n<0) {
    hsmp_close_network();
    free(pkt);
    hsmp_net_packet_in_len=0;
    return NULL;
  }

  hsmp_net_packet_in_len=(uint32_t)n;

  /* Out of size packets are ignored */
  if(hsmp_net_packet_in_len==0 || hsmp_net_packet_in_len > HSMP_MAX_PACKET_SIZE ) {
    free(pkt);
    return NULL;
  }

  if(pkt->hdr.idver>0x000000FF) {
    if(hsmp_net_packet_in_len!=pkt->hdr.dsize) {
      free(pkt);
      return NULL;
    }
  }


#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,"<<< GOT PKT from %s %d <<< 0x%08X (%u bytes) (# %u msgs)",inet_ntoa(sa->sin_addr),ntohs(sa->sin_port),pkt->hdr.idver,pkt->hdr.dsize,pkt->hdr.nomsgs);
  hsmp_net_log(str);
#endif

  return pkt;
}

/* hsmp_net_process_message() processes an individual message of type
 * mid and contents data, if any. */
void hsmp_net_process_message(uint32_t mid,void *data,struct sockaddr_in *from,uint32_t idver) {


#ifdef HSMPDEBUG
  char str[256];
  uint32_t ms=HSMP_SIZE_FOR_MID(mid);
  sprintf(str,"<<< GOT MSG 0x%08X (%u bytes payload)",mid,ms);
  hsmp_net_log(str);
#endif

  uint32_t mclass = mid & 0xF0000000;
  uint32_t mgroup = mid & 0x0000FF00;

  /* Control messages */
  switch(mclass) {

      /* Control messages */
    case(HSMP_MSG_CLASS_CTL): {

      switch (mgroup) {

          /* Flow control group */
        case(HSMP_MSG_CGRP_FLOW): {

          switch(mid) {

              /* HELLO message */
            case(HSMP_MID_HELLO): {

              /* Add peer to our list of peers or update its timer */
              hsmp_add_target_to_list(inet_ntoa(from->sin_addr),ntohs(from->sin_port),&hsmp_net_peer_list,time(NULL),idver);


              break;
            }

            case(HSMP_MID_STREAM_START): {

#ifdef HSMPDEBUG
              char str[256];
              sprintf(str,">>> RCVD STREAM DATA START REQUEST FROM %s %d",inet_ntoa(from->sin_addr),ntohs(from->sin_port));
              hsmp_net_log(str);
#endif
              /* Look for peer first and update its flags if found */
              hsmp_net_tgt_list_t *xp=hsmp_peer_with_addr_port(inet_ntoa(from->sin_addr),ntohs(from->sin_port));
              if(xp!=NULL) {

                if(!(xp->flags & HSMP_TGT_FLAG_REQSTREAM)) {
                  hsmp_number_of_stream_peers++;
                  xp->flags |= HSMP_TGT_FLAG_REQSTREAM;
                }
              }

              break;
            }

            case(HSMP_MID_STREAM_STOP): {

#ifdef HSMPDEBUG
              char str[256];
              sprintf(str,">>> RCVD STREAM DATA STOP REQUEST FROM %s %d",inet_ntoa(from->sin_addr),ntohs(from->sin_port));
              hsmp_net_log(str);
#endif

              /* Look for peer first and update its flags if found */
              hsmp_net_tgt_list_t *xp=hsmp_peer_with_addr_port(inet_ntoa(from->sin_addr),ntohs(from->sin_port));

              if(xp!=NULL) {
                if(xp->flags & HSMP_TGT_FLAG_REQSTREAM) {
                  if(hsmp_number_of_stream_peers) hsmp_number_of_stream_peers--;
                  xp->flags &= ~HSMP_TGT_FLAG_REQSTREAM;
                }
              }

              break;


              break;
            }

          }

          break;
        }
      }

      break;
    }

      /* aerospace systems messages */
    case(HSMP_MSG_CLASS_AIR): {

      break;
    }

      /* marine systems messages */
    case(HSMP_MSG_CLASS_SEA): {

      break;
    }
  }

  /* Now tell the callback that we received a message so that something
   can be done about it if message callback is enabled. */
  if(hsmp_net_message_received_callback!=NULL) {
    hsmp_net_message_received_callback(mid,data,from);
  }

}

/* The hsmp_net_process_packet() function takes a packet and processes it
 * by parsing all its messages and processing each individually. */
void hsmp_net_process_packet(hsmp_pkt_t *pkt,struct sockaddr_in *from) {


  if(pkt==NULL) return;

#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,">>> RCVD PKT FROM %s %d",inet_ntoa(from->sin_addr),ntohs(from->sin_port));
  hsmp_net_log(str);
#endif

  /* ifdef COMPAT */
  /* If it is a packet from a previous ATP version, consider it an HELLO
   * if it is a POS report. The old protocol had the MSG ID in the front and the
   * version as its second integer. */
  if(pkt->hdr.idver <= 0x000000FF) {

    atp_data_t *ap=(atp_data_t *)pkt;
    if(ap->hdr.msgid==ATP_POSREPORT) {
      hsmp_add_target_to_list(inet_ntoa(from->sin_addr),ntohs(from->sin_port),&hsmp_net_peer_list,time(NULL),ap->hdr.version);
    }

    if(hsmp_net_packet_received_callback!=NULL) {
      hsmp_net_packet_received_callback(pkt,from,hsmp_net_packet_in_len);
    }

    return;
  }
  /* endif COMPAT */

  /* Make sure we support this versin of packet */
  if((pkt->hdr.idver & 0x00FFFFFFFF) >= HSMP_PKT_MIN_VER)

  {

    uint32_t i;

    /* Start parsing messages */
    void *bp=(void *)&pkt->data;
    int n=pkt->hdr.dsize - sizeof(hsmp_pkt_hdr_t);
    for(i=0;i<pkt->hdr.nomsgs;i++) {

      uint32_t mid; memcpy(&mid,bp,sizeof(uint32_t));
      uint32_t ms = HSMP_SIZE_FOR_MID(mid);

      bp += sizeof(uint32_t);
      n -= sizeof(uint32_t);

      if(n<0 || n<ms) break;

      hsmp_net_process_message(mid,bp,from,pkt->hdr.idver);

      /* Hack for AirTrack <= 3.7, if we receive an HSMP_MID_FD_MODE
       * we ignore the rest of the packet to avoid resetting other AP
       * variables */
      if(mid==HSMP_MID_FD_MODE) break;

      n -= ms;
      bp += ms;
    }
  }

  /* Now tell the callback that we received a packet so that something
   can be done about it if packet callback is enabled. */
  if(hsmp_net_packet_received_callback!=NULL) {
    hsmp_net_packet_received_callback(pkt,from,hsmp_net_packet_in_len);
  }

  return;
}



/* hsmp_send_hello_packets sends hello packets to all multicast targets and all
 * unicast targets  present on the corresponding lists. */
void hsmp_send_hello_packets(void) {

#if IBM
  if(hsmp_net_socket==INVALID_SOCKET) return;
#else
  if(hsmp_net_socket<0 || hsmp_net_socket>2147483647) return;
#endif


  hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
  if(pkt!=NULL) {
    if(hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_HELLO,&(pkt->hdr.idver))) {

      /* Send to all multicast peers */
      hsmp_net_send_to_all_tgts(pkt,hsmp_net_mcast_list);

      /* Send to all unicast peers */
      hsmp_net_send_to_all_tgts(pkt,hsmp_net_peer_list);

    }
    free(pkt);
  }

}

/* hsmp_remove_expired_peers() removes any peers on the peer list that have
 * expired, i.e. that haven't been heard of through hellos in the last 60
 * seconds. */
void hsmp_remove_expired_peers(void) {

  time_t tExpire=time(NULL)-60;

  /* Locate multicast target, or not ...*/
  hsmp_net_tgt_list_t *xp= hsmp_net_peer_list;
  hsmp_net_tgt_list_t *xpp=NULL;

  while(xp!=NULL) {
    if(xp->texpire) { /* Only expire dynamic peers */
      if(xp->texpire < tExpire) {


        /* If the peer was streaming, decrement number of clients */
        if(xp->flags & HSMP_TGT_FLAG_REQSTREAM) {
          if(hsmp_number_of_stream_peers) hsmp_number_of_stream_peers--;
          xp->flags &= ~HSMP_TGT_FLAG_REQSTREAM;
        }


#ifdef HSMPDEBUG
        char str[256];
        sprintf(str,"=== EXPIRE === PEER %s %d",inet_ntoa(xp->sa.sin_addr),ntohs(xp->sa.sin_port));
        hsmp_net_log(str);
#endif

        if(hsmp_peer_count>0) hsmp_peer_count--;

        hsmp_net_tgt_list_t *dp=xp;
        if(xpp!=NULL) xpp->next=xp->next;
        xp=xp->next;
        if(hsmp_net_peer_list==dp) hsmp_net_peer_list=xp;

        if(hsmp_net_peer_removed_callback!=NULL) {
          hsmp_net_peer_removed_callback(&(dp->sa));
        }
        free(dp);
      } else {
        xpp = xp;
        xp=xp->next;
      }
    } else {
      xpp = xp;
      xp=xp->next;
    }
  }
}


/* hsmp_showtime() is a function that performs all ATP pending tasks in one
 * cycle by calling all the appropriate functions. It can be used as a
 * convenience method instead of calling each function individually. */
void hsmp_showtime(void) {

  static time_t last_showtime=0;
  time_t time_now=time(NULL);

  /* Try to reinitialise network, just in case it has closed */
  hsmp_initialise_network(hsmp_init_addr,hsmp_init_port,hsmp_node_type);

  /* Process incoming messages */
  hsmp_pkt_t *pkt;
  struct sockaddr_in sa;
  while((pkt=hsmp_net_read_packet(&sa))!=NULL) {
    hsmp_net_process_packet(pkt,&sa);
    free(pkt);
  }


  /* Not more than once per second */
  if(time_now > last_showtime) {

    /* Send hello packets */
    hsmp_send_hello_packets();

    /* Remove expired peers */
    hsmp_remove_expired_peers();

    last_showtime=time_now;

  }

  hsmp_tcp_showtime();

}

#ifdef HSXPLDEBUG
#include <source/hsxplmisc.h>
#endif

void hsmp_net_log(char *str) {

#ifdef HSMPDEBUG

#ifdef HSXPLDEBUG
  hsxpl_log_str(str);
#else
  printf("%s\n",str);
#endif

#endif

}
