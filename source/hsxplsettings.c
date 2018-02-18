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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "hsxplsettings.h"
#include "hsxpl.h"
#include "hsmpmsg.h"
#include "hsmpnet.h"

#ifdef CPFLIGHT
#include "hsxplcpflight.h"
#endif

#pragma mark Settings functions

#ifdef CPFLIGHT
extern char hsaircpf_serial_port[256];
#endif

extern struct sockaddr_in hsxpl_unicast_sa;
extern uint32_t hsxpl_cpflight_enabled;

void hsxpl_load_settings(void)
{

#ifdef CPFLIGHT
  memset(hsaircpf_serial_port,0,256);
#endif

#if !defined(_WIN32)
  FILE *fp=fopen("Resources/plugins/HaversineAir.cfg","r");
  if(fp==NULL) fp=fopen("Resources/plugins/HaversineAir/HaversineAir.cfg","r");
  if(fp==NULL) fp=fopen("Resources/plugins/hsairxpl.cfg","r");
  if(fp==NULL) fp=fopen("Resources/plugins/HaversineAir/hsairxpl.cfg","r");
#else
  FILE *fp=fopen("Resources\\plugins\\HaversineAir.cfg","r");
  if(fp==NULL) fp=fopen("Resources\\plugins\\HaversineAir\\HaversineAir\\HaversineAir.cfg","r");
  if(fp==NULL) fp=fopen("Resources\\plugins\\hsairxpl.cfg","r");
  if(fp==NULL) fp=fopen("Resources\\plugins\\HaversineAir\\hsairxpl.cfg","r");
#endif

  if(fp!=NULL) {
    char buffer[1024]; buffer[1023]='\0';
    while(fgets(buffer,1023,fp)!=NULL) {

      /* Remove leading \r\n */
      char *c=buffer;
      while(*c!='\0' && *c!='\r' && *c!='\n') c++;
      *c='\0';

#ifdef CPFLIGHT
      /* Test CPFLIGHT serial port setting */
      if(!memcmp(buffer,"CPFLIGHT-SERIAL-PORT=",strlen("CPFLIGHT-SERIAL-PORT="))) {

        hsxpl_cpflight_enabled=1;

        c=&buffer[strlen("CPFLIGHT-SERIAL-PORT=")];

        if(*c!='\0') {
          strncpy(hsaircpf_serial_port,c,255);
          if(!strcmp(hsaircpf_serial_port,"AUTO"))
            memset(hsaircpf_serial_port,0,256);
        }
      }
#endif

      /* Test broadcast mode */
      if(!memcmp(buffer,"DESTINATION-IP=",strlen("DESTINATION-IP="))) {

        c=&buffer[strlen("DESTINATION-IP=")];

        if(*c!='\0') {
          if(!memcmp(c,"AUTO",4)) {
            if(hsxpl_unicast_sa.sin_addr.s_addr) {
              hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
              hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
              hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
            }
            hsxpl_unicast_sa.sin_addr.s_addr=0;
          }
          else
          {
            struct in_addr a4;
            a4.s_addr=inet_addr(c);

            if(a4.s_addr==INADDR_NONE) {
              if(hsxpl_unicast_sa.sin_addr.s_addr) {
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
              }
              hsxpl_unicast_sa.sin_addr.s_addr=0;
              hsxpl_save_settings();
            } else {
              if(hsxpl_unicast_sa.sin_addr.s_addr) {
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
                hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
              }
              hsxpl_unicast_sa.sin_addr.s_addr=a4.s_addr;
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT,HSMP_PKT_NT_AIRTRACK|HSMP_PKT_PROTO_VER);
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT,HSMP_PKT_NT_AIRFMC|HSMP_PKT_PROTO_VER);
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT,HSMP_PKT_NT_AIREFB|HSMP_PKT_PROTO_VER);

              hsxpl_save_settings();

            }
          }
        }
      }
    }
    fclose(fp);
  }
}

void hsxpl_save_settings(void)
{

#if !defined(_WIN32)
  FILE *fp=fopen("Resources/plugins/HaversineAir.cfg","w");
#else
  FILE *fp=fopen("Resources\\plugins\\HaversineAir.cfg","w");
#endif
  if(fp!=NULL) {

    /* Save broadcast mode */
    if(hsxpl_unicast_sa.sin_addr.s_addr)
      fprintf(fp,"DESTINATION-IP=%s\r\n",inet_ntoa(hsxpl_unicast_sa.sin_addr));
    else
      fprintf(fp,"DESTINATION-IP=AUTO\r\n");

#ifdef CPFLIGHT
    if(hsaircpf_serial_port[0]!='\0') {
      fprintf(fp,"CPFLIGHT-SERIAL-PORT=%s\r\n",hsaircpf_serial_port);
    }
#endif

    fclose(fp);
  }
}

