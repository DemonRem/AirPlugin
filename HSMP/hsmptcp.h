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

#ifndef __HSMP_TCP_H__
#define __HSMP_TCP_H__

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

#if defined(_WIN32)
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int socklen_t;
#endif

/* The default port range to bind to */
#define HSMP_DYN_TCP_PORT_START             59000
#define HSMP_DYN_TCP_PORT_END               59999

#define HSMP_TCP_BUFFER_SIZE                65536
#define HSMP_TCP_INACTIVITY_TIMEOUT         5

/* hsmp_tcp_start_server() creates a TCP server and puts it in listening mode on the
 * given tcpport. If addr is given as an IPv4 address, the bind is performed on that
 * address. Otherwise a bind to INADDR_ANY is performed. */
void hsmp_tcp_start_server(const char *addr,uint32_t tcpport);

/* hsmp_tcp_close_server() closes the tcp server socket */
void hsmp_tcp_close_server(void);

/* hsmp_tcp_close_server_client() closes the tcp server client socket */
void hsmp_tcp_close_server_client(void);

/* hsmp_tcp_network_server_is_open checks if the network is open and returns 1 if so
 * and 0 otherwise. */
int hsmp_tcp_network_server_is_open(void);

/* hsmp_tcp_showtime() is a function that performs all TCP server pending tasks
 * in one cycle by calling all the appropriate functions. It is called from
 * hsmp_showtime() once every cycle. */
void hsmp_tcp_showtime(void);

/* hsmp_tcp_server_accept_incoming() checks if there is a new connection to accept
 * and if so, accepts it. Only one connection is supported at a time, so if
 * one connection is open this function just returns and does nothing. Returns
 * the client socket if accepted or -1 if not. */
int hsmp_tcp_server_accept_incoming(void);

/* Returns 1 if there is data to read and 0 if not */
int hsmp_tcp_descriptor_has_data_to_read(int descriptor);

/* If a tcp server is active and has an active client, read data from it.
 * If the client is no longer there this function closes the client socket
 * thus releasing the server for further accepts. This function is
 * automatically called from showtime. It is non blocking i.e. returns
 * if there is nothing to read. If there is somethign to read and the
 * hsmp_tcp_server_data_received_callback callback is set, this function is
 * called whenever data is read. */
void hsmp_tcp_server_read_data(void);

/* The callback executed whenever new server data arrives, if non null */
extern void (*hsmp_tcp_server_data_received_callback)(uint8_t *data,struct sockaddr_in from,uint64_t nbytes);

/* Connects to and returns a new client socket ready to be read and writen
 * to. Returns < 0 if failed. The caller must call hsmp_tcp_close_client() to
 * close the socket when finished. */
int hsmp_tcp_client_connect(const char *addr,uint32_t tcpport);

/* hsmp_tcp_close_client() closes a client socket given one */
void hsmp_tcp_close_client(int *client);

#endif /* __HSMP_TCP_H__ */
