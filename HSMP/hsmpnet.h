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

#ifndef __HSMP_NET_H__
#define __HSMP_NET_H__

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <io.h>
#else
#include <sys/time.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#if LIN
#ifndef _BSD_SOURCE
#ifndef ip_mreq
struct ip_mreq  {
  struct in_addr imr_multiaddr;   /* IP multicast address of group */
  struct in_addr imr_interface;   /* local IP address of interface */
};
#endif
#endif
#endif

#if !IBM
#define INVALID_SOCKET  -1
#endif

#include <HSMP/hsmpmsg.h>

/* HSMP protocol version */
#define	HSMP_PKT_PROTO_VER	0x00040800
#define	HSMP_PKT_MIN_VER    0x00040000

/* Some defaults for service advertisement */
#define HSMP_DEF_MULTICAST_ADDR            "239.224.224.1"
#define HSMP_AIRTRACK_PORT                  2424
#define HSMP_AIRFMC_PORT                    2425
#define HSMP_AIREFB_PORT                    2426

#define HSMP_DYN_UDP_PORT_START             59000
#define HSMP_DYN_UDP_PORT_END               59999

/* Maximum bytes we fill a packet with, don't want to get close to 1500 */
#define HSMP_MAX_PACKET_SIZE                1472

/*HSMP_SIZE_FOR_MID extracts the size of a MID in bytes */
#define HSMP_SIZE_FOR_MID(a)    (((a>>16)&0xFF)*4)

typedef struct hsmp_net_pkt_s {
  void *pkt;
  struct hsmp_net_pkt_s *next;
} hsmp_net_pkt_t;

/* The hsmp_net_tgt_list_t type serves as a prototype for holding a list
 * of receiving targets. Targets may be static, in which case they have a
 * timestamp of 0, or dynamic, in which case timestamp is set to the time at
 * which they were last processed. */
typedef struct hsmp_net_tgt_list_s {

  struct sockaddr_in sa;      /* peer's IP address and port */
  time_t texpire;             /* last update or 0 for static */
  time_t thello;              /* The time of the last hello received or 0 */
  uint32_t flags;             /* Target flags */
  uint32_t idver;             /* The ID and version of peer */
  hsmp_net_pkt_t *outqhead;   /* output queue of packets head */
  hsmp_net_pkt_t *outqtail;   /* output queue of packets tail */
  hsmp_net_pkt_t *inqhead;    /* input queue of packets head */
  hsmp_net_pkt_t *inqtail;    /* input queue of packets tail */
  struct hsmp_net_tgt_list_s *next;   /* Next target */

} hsmp_net_tgt_list_t;

/* Whether the target wants streamed regular data or not */
#define HSMP_TGT_FLAG_REQSTREAM 0x00000001

/* The hsmp_initialise_network initialises the network socket to listen on lport
 * if specified, with source type ntype.
 *
 * If lport is 0 this function attempts to obtain a free port between 56000 and
 * 56999. This should be the default behaviour for sim sources whose ports are
 * dynamically allocated.
 *
 * If lport is non-zero, this function only tries to open the specified port.
 * In this case this function also issues a join to hsmp_DEF_MULTICAST_ADDR
 *
 * The node type should be one of hsmp_PKT_NT_* as defined in HSMPMSG.h
 *
 * On success, hsmp_net_socket will be >= 0. On error it will be < 0.
 *
 * The socket is bound to addr.
 */
void hsmp_initialise_network(const char *addr,uint32_t lport,uint32_t ntype);

/* hsmp_close_network() closes the network socket */
void hsmp_close_network(void);

/* hsmp_network_is_open checks if the network is open and returns 1 if so
 * and 0 otherwise. */
int hsmp_network_is_open(void);

/* hsmp_add_multicast_target() adds a target to the list of multicast targets to
 * which to send helo packets */
void hsmp_add_multicast_target(char *addr,uint32_t port);

/* Similarly, hsmp_remove_multicast_target() removes a target from the list */
void hsmp_remove_multicast_target(char *addr,uint32_t port);

/* hsmp_join_multicast_group issues a multicast join for the given address.
 * It is called automatically by hsmp_initialise_network but may be called
 * subsequently in addition to. */
void hsmp_join_multicast_group(char *addr);

/* hsmp_add_peer_target() adds a target to the list of static targets to
 * which to send helo packets */
void hsmp_add_peer_target(char *addr,uint32_t port,uint32_t idver);

/* Similarly, hsmp_remove_peer_target() removes a target from the list */
void hsmp_remove_peer_target(char *addr,uint32_t port);

/* hsmp_peer_with_addr_port() looks for the given addr/port in the list
 * of peer targets and returns it if found, and NULL if not */
hsmp_net_tgt_list_t *hsmp_peer_with_addr_port(char *addr,uint32_t port);

/* A similar function to hsmp_peer_with_addr_port but taking a sockaddr_in
 * as an argument. */
hsmp_net_tgt_list_t *hsmp_peer_with_sa(struct sockaddr_in *sa);


/* hsmp_peer_list returns the list of peers */
hsmp_net_tgt_list_t *hsmp_peer_list(void);

/* hsmp_req_stream_data_from() sends a HSMP_MID_STREAM_START message to the
 * given peer if it is found on the list asking for data to be streamed. */
void hsmp_req_stream_data_from(char *addr,uint32_t port);

/* Inversely,hsmp_stop_stream_data_from() sends an HSMP_MID_STREAM_STOP message
 * asking for data sending to be stopped. */
void hsmp_stop_stream_data_from(char *addr,uint32_t port);

/* hsmp_net_send_to_stream_peers() sends a packet to all active peers
 * that have subscribed to streaming services using HSMP_MID_STREAM_START.
 * If ntype is 0 the packet is sent to all peers. If ntype is non 0 the
 * packet is only sent to instances of the app specified by ntype. */
void hsmp_net_send_to_stream_peers(hsmp_pkt_t *pkt,uint32_t ntype);

/* hsmp_net_send_to_target() sends a packet to a given target */
void hsmp_net_send_to_target(void *pkt,uint32_t nbytes,hsmp_net_tgt_list_t *tgt);

/* hsmp_net_send_to_all_tgts() sends a packet to all targets of the list */
void hsmp_net_send_to_all_tgts(hsmp_pkt_t *pkt,hsmp_net_tgt_list_t *tlist);

/* The hsmp_number_of_stream_clients() function returns the number of
 * peers we have on our list that have asked to receive stream data.
 * This function may be used to query the engine and therefore avoid
 * unecessary processing should there be no listening clients waiting for
 * data. */
uint32_t hsmp_number_of_stream_clients(void);

/* hsmp_number_of_peers() returns the total number of peers in the peer list */
uint32_t hsmp_number_of_peers(void);

/* hsmp_net_make_packet returns a newly allocated data buffer containing an
 * initialised packet. The packet's buffer is always 2048, allowing for 1500
 * byte packets to be filled. */
void *hsmp_net_make_packet(void);

/* hsmp_net_add_msg_to_pkt() tries to add the given message to the
 * packet followed by its data. If this is not possible this function returns
 0, otherwise 1 is returned. */
int hsmp_net_add_msg_to_pkt(hsmp_pkt_t *pkt,uint32_t mid,void *mdata);

/* hsmp_send_hello_packets sends hello packets to all multicast targets and all
 * unicast targets  present on the corresponding lists. */
void hsmp_send_hello_packets(void);

/* hsmp_remove_expired_peers() removes any peers on the peer list that have
 * expired, i.e. that haven't been heard of through hellos in the last 60
 * seconds. */
void hsmp_remove_expired_peers(void);

/* hsmp_showtime() is a function that performs all ATP pending tasks in one
 * cycle by calling the other appropriate functions in order of which they are
 * due. */
void hsmp_showtime(void);

/* Logging function */
void hsmp_net_log(char *str);

/* Everytime a packet arrives or a message is processed, the library has
 * the capability of calling a callback function which should be defined
 * elsewhere in order to handle messages or packets. Depending on whether
 * packets contain one or many messages you may choose to enable one or the
 * other. The advantage of enabling message callbacks is that messages are
 * already decoded when they reach the callback so it is easier to implement.
 * The drawback however is that if a packet arrives with many messages,
 * one callback will be made per message which may be a burden in terms of
 * processing; in such cases processing a packet in one go and decoding them
 * messages oneself might be preferred.
 */

extern void (*hsmp_net_message_received_callback)(uint32_t mid,void *data,struct sockaddr_in *from);
extern void (*hsmp_net_packet_received_callback)(hsmp_pkt_t *pkt,struct sockaddr_in *from,uint32_t nbytes);

extern void (*hsmp_net_peer_created_callback)(struct sockaddr_in *addr,uint32_t idver);
extern void (*hsmp_net_peer_removed_callback)(struct sockaddr_in *addr);

#endif /* __HSMP_NET_H__ */
