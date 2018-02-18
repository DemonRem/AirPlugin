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
 * The HSMPTCP.x files contain code used to implement specific client-server
 * or plugin <-> app communications that due to their nature (large file/data
 * transfers for example) require TCP instead of UDP sockets.
 *
 */

#include "hsmptcp.h"
#include "hsmpmsg.h"
#include "hsmpnet.h"
#include "hsmpatp.h"

void (*hsmp_tcp_server_data_received_callback)(uint8_t *data,struct sockaddr_in from,uint64_t nbytes) = NULL;

/* Our network socket */
#if defined(_WIN32)
WSADATA hsmp_wsa_tcp_data;
SOCKET hsmp_tcp_server_socket=INVALID_SOCKET;
#else
int hsmp_tcp_server_socket=INVALID_SOCKET;
#endif

/* Our network client socket */
#if defined(_WIN32)
SOCKET hsmp_tcp_server_client_socket=INVALID_SOCKET;
#else
int hsmp_tcp_server_client_socket=INVALID_SOCKET;
#endif
struct sockaddr_in hsmp_tcp_server_client_addr;


/* The original initialisation address */
char hsmp_tcp_init_addr[64];

/* The original initialisation port */
uint32_t hsmp_tcp_init_port=0;


/* hsmp_tcp_start_server() creates a TCP server and puts it in listening mode on the
 * given tcpport. If addr is given as an IPv4 address, the bind is performed on that
 * address. Otherwise a bind to INADDR_ANY is performed. */
void hsmp_tcp_start_server(const char *addr,uint32_t tcpport) {
  
  struct sockaddr_in receiver_sa;
  uint32_t from_port;
  uint32_t to_port;
  uint32_t i;

  /* int yes=1; */
  
#if defined(_WIN32)
  static int winTcpInitialised=0;
#endif
  
  /* Quick return if network has already been initialised */
#if IBM
  if(hsmp_tcp_server_socket!=INVALID_SOCKET) return;
#else
  if(hsmp_tcp_server_socket>=0 && hsmp_tcp_server_socket<=2147483647) return;
#endif

#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,">>> hsmp_tcp_start_server(%s:%d)",addr,tcpport);
  hsmp_net_log(str);
#endif

  if(tcpport) { from_port=tcpport; to_port=tcpport; }
  else { from_port=HSMP_DYN_TCP_PORT_START; to_port=HSMP_DYN_TCP_PORT_END; }
  
  /* Set local port for future initialisation */
  hsmp_tcp_init_port=tcpport;
  
  if(addr==NULL)
    sprintf(hsmp_tcp_init_addr,"0.0.0.0");
  else
  {
    /* Don't copy ourselves to ourselves */
    if(hsmp_tcp_init_addr!=addr) {
      strncpy(hsmp_tcp_init_addr,addr,63);
      hsmp_tcp_init_addr[63]='\0';
    }
  }

#if defined(_WIN32)
  if(!winTcpInitialised) {
    if(WSAStartup(0x202,&hsmp_wsa_tcp_data)!=0) {

#ifdef HSMPDEBUG
    sprintf(str,">>> hsmp_tcp_start_server() failed to initialise WSAStartup)");
    hsmp_net_log(str);
#endif
      return;
    }
  }
  winTcpInitialised=1;
#endif
  
  /* Only attempt to open socket once per second and not more */
  static time_t hsmp_tcp_server_last_init_attempt=0;
  time_t time_now=time(NULL);
  if(time_now==hsmp_tcp_server_last_init_attempt) return;
  hsmp_tcp_server_last_init_attempt=time_now;
  
  /* Create socket */
  hsmp_tcp_server_socket=socket(AF_INET,SOCK_STREAM,0);
#if IBM
  if(hsmp_tcp_server_socket==INVALID_SOCKET) {
#else
  if(hsmp_tcp_server_socket<0 || hsmp_tcp_server_socket>2147483647) {
#endif

#ifdef HSMPDEBUG
    sprintf(str,">>> hsmp_tcp_start_server() failed to create hsmp_tcp_server_socket)");
    hsmp_net_log(str);
#endif
    return;
  }
  /*
   setsockopt(hsmp_tcp_server_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(int));
   */
  
  struct in_addr a4;
  a4.s_addr=inet_addr(hsmp_tcp_init_addr);
  if(a4.s_addr!=INADDR_NONE)
    receiver_sa.sin_addr.s_addr=a4.s_addr;
  else
    receiver_sa.sin_addr.s_addr=INADDR_ANY;
  receiver_sa.sin_family=AF_INET;
  
  /* Try to bind to a port so that we can receive data on the socket */
  for(i=from_port;i<to_port+1;i++) {
    
    receiver_sa.sin_port=htons(i);
    if(!bind(hsmp_tcp_server_socket,(struct sockaddr *)&receiver_sa,sizeof(struct sockaddr))) break;
    
    /* If we couldn't bind, close the socket */
    if(i==to_port) {
#ifdef HSMPDEBUG
      sprintf(str,">>> hsmp_tcp_start_server() failed to bind to port %d",i);
      hsmp_net_log(str);
#endif
      close(hsmp_tcp_server_socket);
      hsmp_tcp_server_socket=INVALID_SOCKET;
    }
  }
  
  /* Configure listen */
  if(listen(hsmp_tcp_server_socket,1)) {
#ifdef HSMPDEBUG
    sprintf(str,">>> hsmp_tcp_start_server() failed to listen on tcp socket");
    hsmp_net_log(str);
#endif
    close(hsmp_tcp_server_socket);
    hsmp_tcp_server_socket=INVALID_SOCKET;
  }
  
  /* Clear up old address */
  memset(&hsmp_tcp_server_client_addr,0,sizeof(hsmp_tcp_server_client_addr));

  return;
  
}

/* hsmp_tcp_close_server() closes the tcp server socket */
void hsmp_tcp_close_server(void) {


#if IBM
  if(hsmp_tcp_server_socket!=INVALID_SOCKET) {
#else
  if(hsmp_tcp_server_socket>=0 && hsmp_tcp_server_socket<=2147483647) {
#endif

#ifdef HSMPDEBUG
    char str[256];
    sprintf(str,">>> hsmp_tcp_close_server()");
    hsmp_net_log(str);
#endif

#if defined(_WIN32)
    closesocket(hsmp_tcp_server_socket);
#else
    close(hsmp_tcp_server_socket);
#endif
    
    hsmp_tcp_server_socket=INVALID_SOCKET;
  }
}

/* hsmp_tcp_close_server_client() closes the tcp server client socket */
void hsmp_tcp_close_server_client(void) {

#if IBM
  if(hsmp_tcp_server_client_socket!=INVALID_SOCKET) {
#else
  if(hsmp_tcp_server_client_socket>=0 && hsmp_tcp_server_client_socket<=2147483647) {
#endif

#ifdef HSMPDEBUG
    char str[256];
    sprintf(str,">>> hsmp_tcp_close_server_client()");
    hsmp_net_log(str);
#endif

#if defined(_WIN32)
    closesocket(hsmp_tcp_server_client_socket);
#else
    close(hsmp_tcp_server_client_socket);
#endif
    
    hsmp_tcp_server_client_socket=INVALID_SOCKET;
    
    memset(&hsmp_tcp_server_client_addr,0,sizeof(hsmp_tcp_server_client_addr));
  }
}

/* hsmp_tcp_network_server_is_open checks if the network is open and returns 1 if so
 * and 0 otherwise. */
int hsmp_tcp_network_server_is_open(void) {
#if IBM
  if(hsmp_tcp_server_socket!=INVALID_SOCKET) return 1;
#else
  if(hsmp_tcp_server_socket>=0 && hsmp_tcp_server_socket<=2147483647) return 1;
#endif
  return 0;
}

/* hsmp_tcp_showtime() is a function that performs all TCP server pending tasks
 * in one cycle by calling all the appropriate functions. It is called from
 * hsmp_showtime() once every cycle. */
void hsmp_tcp_showtime(void) {
  
  /* Try to reinitialise network, just in case it has closed */
  hsmp_tcp_start_server(hsmp_tcp_init_addr,hsmp_tcp_init_port);
  
  /* Accept a new connection if one is pending */
  hsmp_tcp_server_accept_incoming();
  
  /* Read stuff and auto-close if gone */
  hsmp_tcp_server_read_data();
  
}

/* hsmp_tcp_server_accept_incoming() checks if there is a new connection to accept
 * and if so, accepts it. Only one connection is supported at a time, so if
 * one connection is open this function just returns and does nothing. */
int  hsmp_tcp_server_accept_incoming(void) {

#if IBM
  if(hsmp_tcp_server_client_socket!=INVALID_SOCKET) return hsmp_tcp_server_client_socket;
#else
  if(hsmp_tcp_server_client_socket>=0) return hsmp_tcp_server_client_socket;
#endif

#ifdef IBM
  if(hsmp_tcp_server_socket==INVALID_SOCKET) return -1;
#else
  if(hsmp_tcp_server_socket<0 || hsmp_tcp_server_socket>2147483647) return -1;
#endif

  if(!hsmp_tcp_descriptor_has_data_to_read(hsmp_tcp_server_socket)) return -1;


#ifdef HSMPDEBUG
  char str[256];
  sprintf(str,">>> hsmp_tcp_server_accept_incoming()");
  hsmp_net_log(str);
#endif

  socklen_t slen=(socklen_t)sizeof(hsmp_tcp_server_client_addr);
  hsmp_tcp_server_client_socket=accept(hsmp_tcp_server_socket,(struct sockaddr *)&hsmp_tcp_server_client_addr,&slen);

  return hsmp_tcp_server_client_socket;
  
}

/* Returns 1 if there is data to read and 0 if not */
int hsmp_tcp_descriptor_has_data_to_read(int descriptor) {

  fd_set readfds;
  struct timeval tv;
  
  /* See if there are any packets to be read */
  FD_ZERO(&readfds);
  FD_SET(descriptor,&readfds);
  tv.tv_sec=0;
  tv.tv_usec=0;
  if(select(descriptor+1,&readfds,NULL,NULL,&tv)==-1) return 0;
  
  /* If not, return here */
  if(!FD_ISSET(descriptor,&readfds)) return 0;

  return 1;
}

/* If a tcp server is active and has an active client, read data from it.
 * If the client is no longer there this function closes the client socket
 * thus releasing the server for further accepts. This function is
 * automatically called from showtime. It is non blocking i.e. returns
 * if there is nothing to read. */
void hsmp_tcp_server_read_data(void) {

#if IBM
  if(hsmp_tcp_server_client_socket==INVALID_SOCKET) return;
#else
  if(hsmp_tcp_server_client_socket<0) return;
#endif

#if IBM
  if(hsmp_tcp_server_socket==INVALID_SOCKET) return;
#else
  if(hsmp_tcp_server_socket<0 || hsmp_tcp_server_socket>2147483647) return;
#endif

  if(!hsmp_tcp_descriptor_has_data_to_read(hsmp_tcp_server_client_socket)) return;

  /* Read stuff */
  uint8_t buffer[HSMP_TCP_BUFFER_SIZE];
  ssize_t nobytes;
  if((nobytes=read(hsmp_tcp_server_client_socket,buffer,HSMP_TCP_BUFFER_SIZE))<1) {
    hsmp_tcp_close_server_client();
  } else {
    if(hsmp_tcp_server_data_received_callback!=NULL) {
      hsmp_tcp_server_data_received_callback(buffer,hsmp_tcp_server_client_addr,(uint64_t)nobytes);
    }
  }
}


/* Connects to and returns a new client socket ready to be read and writen
 * to. Returns < 0 if failed. The caller must call hsmp_tcp_close_client() to
 * close the socket when finished. */
int hsmp_tcp_client_connect(const char *addr,uint32_t tcpport) {
  
  struct sockaddr_in remote_sa;
  
  /* Create socket */
  int client=socket(AF_INET,SOCK_STREAM,0);
  if(client<0 || client>2147483647) return -1;
  
  remote_sa.sin_addr.s_addr=inet_addr(addr);
  remote_sa.sin_family=AF_INET;
  remote_sa.sin_port=htons(tcpport);
  
  /* Connect */
  if(!connect(client,(struct sockaddr *)&remote_sa,(socklen_t)sizeof(struct sockaddr_in))) {
    return client;
  }
  
  return -1;
}


/* hsmp_tcp_close_client() closes a client socket given one */
void hsmp_tcp_close_client(int *client) {

  if(*client>=0 && *client<=2147483647) {
    
#if defined(_WIN32)
    closesocket(*client);
#else
    close(*client);
#endif
    *client=-1;
  }
}
