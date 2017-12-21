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
 * Description:     The X-Plane plugin for the Haversine Message Protocol
 *
 */

#ifndef __HS_HSAIRXPL_H__
#define __HS_HSAIRXPL_H__

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <io.h>
#else
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#endif

#if IBM
#include <windows.h>
#else
#include <termios.h>
#include <syslog.h>
#include <sys/select.h>
#endif

/* Some type definitions for windows */
#if defined(_WIN32)
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int socklen_t;
#endif

/* The X-Plane SDK include files */

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
#include <XPLMPlugin.h>

#define HSAIRXPL_VERSION    "4.7"

/* Plane type */
#define HSXPL_PLANE_XPLANE  0x00000000
#define HSXPL_PLANE_X737    0x00000001
#define HSXPL_PLANE_A320N   0x00000002
#define HSXPL_PLANE_FF757   0x00000004
#define HSXPL_PLANE_FF777   0x00000008
#define HSXPL_PLANE_PCRJ200 0x00000010
#define HSXPL_PLANE_A320Q   0x00000100
#define HSXPL_PLANE_A330J   0x00000200
#define HSXPL_PLANE_FF767   0x00000400
#define HSXPL_PLANE_IX733   0x00000800
#define HSXPL_PLANE_Z738    0x00001000

/* A320 colour IDs */
#define HSXPL_A320Q_COL_IDX_WHITE       0
#define HSXPL_A320Q_COL_IDX_GREEN       1
#define HSXPL_A320Q_COL_IDX_BLUE        2
#define HSXPL_A320Q_COL_IDX_AMBER       3
#define HSXPL_A320Q_COL_IDX_YELLOW      4
#define HSXPL_A320Q_COL_IDX_MAGENTA     5
#define HSXPL_A320Q_COL_IDX_S           6


/* A function that resets the datarefs */
void hsxpl_set_datarefs(void);
float hsxpl_runtime(float                inElapsedSinceLastCall,
                    float                inElapsedTimeSinceLastFlightLoop,
                    int                  inCounter,
                    void *               inRefcon);

void hsxpl_hsmp_message_callback(uint32_t mid,void *data,struct sockaddr_in *from);

void hsxpl_send_subsecond_data(void);
void hsxpl_send_second_data(void);

void hsxpl_set_datarefs(void);
void hsxpl_set_xplane_datarefs(void);
void hsxpl_set_x737_datarefs(void);
void hsxpl_set_ix733_datarefs(void);
void hsxpl_set_ufmc_datarefs(void);
void hsxpl_set_a320n_datarefs(void);
void hsxpl_set_a320q_datarefs(void);
void hsxpl_set_xsb_datarefs(void);
void hsxpl_set_p757_datarefs(void);
void hsxpl_set_p767_datarefs(void);
void hsxpl_set_p777_datarefs(void);
void hsxpl_set_pcrj200_datarefs(void);
void hsxpl_set_xfmc_datarefs(void);
void hsxpl_set_zibo737_datarefs(void);
void hsxpl_set_b738_keys(void);

void hsxpl_load_settings(void);
void hsxpl_save_settings(void);

void hsxpl_send_fmc_data(void);
void hsxpl_send_xfmc_data(void);
void hsxpl_send_ufmc_data(void);
void hsxpl_send_z738_data(void);
void hsxpl_send_a320n_fmc_data(void);
void hsxpl_send_a320q_fmc_data(void);
void hsxpl_send_p757_data(void);
void hsxpl_send_p777_data(void);
void hsxpl_send_pcrj200_data(void);
void hsxpl_send_ix733_data(void);

char *hsxpl_acf_icao(void);
char *hsxpl_acf_tailno(void);

void hsxpl_test_default_fmc_support(void);

void hsxpl_fmc_press_key(uint32_t mid,XPLMDataRef k);
void hsxpl_mcdu_toggle(void);

/* Widgets */
void hsxpl_create_settings_widget(int x, int y, int w, int h);
void hsxpl_select_menu_option(void *inMenuRef,void *inItemRef);
int hsxpl_settings_widget_handler(XPWidgetMessage inMessage,XPWidgetID inWidget,long inParam1,long inParam2);


#endif  /* __HS_HSAIRXPL_H__ */
