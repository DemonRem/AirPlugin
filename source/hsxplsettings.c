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
#include <time.h>
#include <ctype.h>

#include "hsxplsettings.h"
#include "hsxpl.h"
#include "hsxplmisc.h"
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
uint32_t hsxpl_appips[1024];
uint32_t hsxpl_no_appips=0;

void hsxpl_load_settings(void)
{

#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"hsxpl_load_settings()");
#endif

#ifdef CPFLIGHT
  memset(hsaircpf_serial_port,0,256);
#endif

  hsxpl_no_appips=0;
  memset(hsxpl_appips,0,sizeof(hsxpl_appips));

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
              hsxpl_unicast_sa.sin_addr.s_addr=a4.s_addr;
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT,HSMP_PKT_NT_AIRTRACK|HSMP_PKT_PROTO_VER);
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT,HSMP_PKT_NT_AIRFMC|HSMP_PKT_PROTO_VER);
              hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT,HSMP_PKT_NT_AIREFB|HSMP_PKT_PROTO_VER);

              hsxpl_save_settings();

            }
          }
        }
      }

      if(!memcmp(buffer,"APP-IP=",strlen("APP-IP="))) {
        c=&buffer[strlen("APP-IP=")];
        if(*c!='\0') {
          struct in_addr a4;
          a4.s_addr=inet_addr(c);
          if(a4.s_addr!=INADDR_NONE) {
            memcpy(&hsxpl_appips[hsxpl_no_appips],&a4.s_addr,4);
            hsxpl_no_appips++;
            hsmp_add_peer_target(inet_ntoa(a4),HSMP_AIRTRACK_PORT,HSMP_PKT_NT_AIRTRACK|HSMP_PKT_PROTO_VER);
            hsmp_add_peer_target(inet_ntoa(a4),HSMP_AIRFMC_PORT,HSMP_PKT_NT_AIRFMC|HSMP_PKT_PROTO_VER);
            hsmp_add_peer_target(inet_ntoa(a4),HSMP_AIREFB_PORT,HSMP_PKT_NT_AIREFB|HSMP_PKT_PROTO_VER);
          }
        }
      }
    }
    fclose(fp);
  }
}

void hsxpl_save_settings(void)
{

  uint32_t i;
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

    for(i=0;i<hsxpl_no_appips;i++) {
      struct in_addr a4;
      memcpy(&a4.s_addr,&hsxpl_appips[i],4);
      fprintf(fp,"APP-IP=%s\r\n",inet_ntoa(a4));
    }

    fclose(fp);
  }
}


#pragma mark Widget functions

/**** WIDGET RELATED PROTOTYPES AND GLOBAL VARS *****/
XPLMMenuID hsxpl_main_menu=NULL;
int hsxpl_settings_menu_item;
XPLMMenuID hsxpl_settingsmenu=NULL;
XPWidgetID hsxpl_settings_widget=NULL;
XPWidgetID hsxpl_ipsubwindow=NULL;
XPWidgetID hsxpl_iplabel=NULL;
XPWidgetID hsxpl_iptxinput=NULL;
XPWidgetID hsxpl_iplabel2=NULL;
XPWidgetID hsxpl_iplabel3=NULL;
XPWidgetID hsxpl_iplabel4=NULL;
XPWidgetID hsxpl_iplabel5=NULL;
XPWidgetID hsxpl_ipapply=NULL;

void hsxpl_settings_configure_menu(void) {
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPLMCreateMenu()");
#endif
  hsxpl_main_menu=XPLMCreateMenu("Haversine Air",NULL,0,hsxpl_select_menu_option,0);
#ifdef HSXPLDEBUG
  hsxpl_log(HSXPLDEBUG_ACTION,"XPLMAppendMenuItem()");
#endif
  hsxpl_settings_menu_item=XPLMAppendMenuItem(hsxpl_main_menu,"Settings",(void *)"Settings",1);
}
void hsxpl_select_menu_option(void *inMenuRef,void *inItemRef)
{

  if (!strcmp((char *) inItemRef, "Settings")) {

    if(hsxpl_settings_widget==NULL) {
      hsxpl_create_settings_widget(300,650,300,220);
      if(hsxpl_settings_widget==NULL) return;
    }

    if(!XPIsWidgetVisible(hsxpl_settings_widget))
      XPShowWidget(hsxpl_settings_widget);
  }
}

void hsxpl_create_settings_widget(int x, int y, int w, int h)
{


  /* AirTrack IP address part */

  char vstr[64];
  sprintf(vstr,"Haversine Air %s Settings",HSAIRXPL_VERSION);
  hsxpl_settings_widget = XPCreateWidget(x, y, x+w, y-h,1,vstr,1,NULL, xpWidgetClass_MainWindow);
  XPSetWidgetProperty(hsxpl_settings_widget, xpProperty_MainWindowHasCloseBoxes, 1);

  hsxpl_ipsubwindow = XPCreateWidget(x+15, y-30,x+w-15,y-200,1,"",0,hsxpl_settings_widget,xpWidgetClass_SubWindow);
  XPSetWidgetProperty(hsxpl_ipsubwindow, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);


  hsxpl_iplabel=XPCreateWidget(x+25,y-50,x+120,y-70,1,"AirApp IP Address: ",0,hsxpl_settings_widget, xpWidgetClass_Caption);

  if(hsxpl_unicast_sa.sin_addr.s_addr)
    hsxpl_iptxinput=XPCreateWidget(x+140, y-50, x+w-25, y-70,1, inet_ntoa(hsxpl_unicast_sa.sin_addr), 0, hsxpl_settings_widget,xpWidgetClass_TextField);
  else
    hsxpl_iptxinput=XPCreateWidget(x+140, y-50, x+w-25, y-70,1, "AUTO", 0, hsxpl_settings_widget,xpWidgetClass_TextField);
  XPSetWidgetProperty(hsxpl_iptxinput, xpProperty_TextFieldType, xpTextEntryField);

  hsxpl_iplabel2=XPCreateWidget(x+25,y-80,x+w-25,y-90,1,"This sets the IP address of the device",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_iplabel3=XPCreateWidget(x+25,y-100,x+w-25,y-110,1,"running an AirApp. You should leave this with",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_iplabel4=XPCreateWidget(x+25,y-120,x+w-25,y-130,1,"AUTO and set it only if you experience",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_iplabel5=XPCreateWidget(x+25,y-140,x+w-25,y-150,1,"connectivity problems.",0,hsxpl_settings_widget, xpWidgetClass_Caption);
  hsxpl_ipapply = XPCreateWidget(x+50,y-170,x+w-50,y-180,1,"Apply",0,hsxpl_settings_widget,xpWidgetClass_Button);

  XPAddWidgetCallback(hsxpl_settings_widget,(XPWidgetFunc_t)hsxpl_settings_widget_handler);
  XPAddWidgetCallback(hsxpl_ipapply,(XPWidgetFunc_t)hsxpl_settings_widget_handler);
}



int hsxpl_settings_widget_handler(XPWidgetMessage inMessage,XPWidgetID inWidget,long inParam1,long inParam2)
{


  /* Close button pressed, only hide the widget, rather than destropying it. */
  if (inMessage == xpMessage_CloseButtonPushed && inWidget==hsxpl_settings_widget)
  {
    if (hsxpl_settings_widget!=NULL)
    {
      XPHideWidget(hsxpl_settings_widget);
    }
    return 1;
  }

  /* IP addr apply button pressed */
  if (inMessage == xpMsg_PushButtonPressed && inWidget==hsxpl_ipapply)
  {

    char buffer[256];
    struct in_addr a4;

    /* Get the IP address from the text box */
    XPGetWidgetDescriptor(hsxpl_iptxinput, buffer, sizeof(buffer));
    if(!strncmp(buffer,"AUTO",4)) {
      if(hsxpl_unicast_sa.sin_addr.s_addr) {
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
      }
      hsxpl_unicast_sa.sin_addr.s_addr=0;
      hsxpl_save_settings();
      return 1;
    }


    a4.s_addr=inet_addr(buffer);
    if(a4.s_addr==INADDR_NONE) {
      if(hsxpl_unicast_sa.sin_addr.s_addr) {
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT);
        hsmp_remove_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT);
      }
      hsxpl_unicast_sa.sin_addr.s_addr=0;
      XPSetWidgetDescriptor(hsxpl_iptxinput, "AUTO");
      hsxpl_save_settings();
      return 1;
    }

    hsxpl_unicast_sa.sin_family=AF_INET;
    hsxpl_unicast_sa.sin_port=htons(HSMP_AIRTRACK_PORT);
    hsxpl_unicast_sa.sin_addr=a4;
    hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRTRACK_PORT,HSMP_PKT_NT_AIRTRACK|HSMP_PKT_PROTO_VER);
    hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIRFMC_PORT,HSMP_PKT_NT_AIRFMC|HSMP_PKT_PROTO_VER);
    hsmp_add_peer_target(inet_ntoa(hsxpl_unicast_sa.sin_addr),HSMP_AIREFB_PORT,HSMP_PKT_NT_AIREFB|HSMP_PKT_PROTO_VER);
    hsxpl_save_settings();
    return 1;

  }

  return 0;
}

