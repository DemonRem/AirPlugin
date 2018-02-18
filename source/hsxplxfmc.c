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
 * X-FMC: https://www.x-fmc.com
 *
 */

#include "hsxplxfmc.h"
#include "hsxpl.h"
#include "hsxplmisc.h"
#include "hsxpldatarefs.h"
#include "hsxplfmc.h"
#include "hsxplmcp.h"
#include "hsmpnet.h"

extern uint32_t hsxpl_fmc_type;
extern uint32_t hsxpl_plane_type;
extern hsxpl_fmc_t hsxpl_fmc;
extern hsxpl_xplane_datarefs_t hsxpl_xplane_datarefs;

hsxpl_xfmc_datarefs_t hsxpl_xfmc_datarefs;

void hsxpl_set_xfmc_datarefs(void) {

  if(XPLMFindDataRef("xfmc/Status") == NULL) {
    return;
  }
  if(XPLMGetDatai(XPLMFindDataRef("xfmc/Status"))==0) {
    return;
  }

  hsxpl_fmc_type = HSMP_FMC_TYPE_XP_XFMC;

  memset(&hsxpl_fmc,0,sizeof(struct hsxpl_fmc_s));
  memset(&hsxpl_xfmc_datarefs,0,sizeof(struct hsxpl_xfmc_datarefs_s));

  hsxpl_xfmc_datarefs.line[0]=XPLMFindDataRef("xfmc/Upper");
  hsxpl_xfmc_datarefs.line[2]=XPLMFindDataRef("xfmc/Panel_1");
  hsxpl_xfmc_datarefs.line[1]=XPLMFindDataRef("xfmc/Panel_2");
  hsxpl_xfmc_datarefs.line[4]=XPLMFindDataRef("xfmc/Panel_3");
  hsxpl_xfmc_datarefs.line[3]=XPLMFindDataRef("xfmc/Panel_4");
  hsxpl_xfmc_datarefs.line[6]=XPLMFindDataRef("xfmc/Panel_5");
  hsxpl_xfmc_datarefs.line[5]=XPLMFindDataRef("xfmc/Panel_6");
  hsxpl_xfmc_datarefs.line[8]=XPLMFindDataRef("xfmc/Panel_7");
  hsxpl_xfmc_datarefs.line[7]=XPLMFindDataRef("xfmc/Panel_8");
  hsxpl_xfmc_datarefs.line[10]=XPLMFindDataRef("xfmc/Panel_9");
  hsxpl_xfmc_datarefs.line[9]=XPLMFindDataRef("xfmc/Panel_10");
  hsxpl_xfmc_datarefs.line[12]=XPLMFindDataRef("xfmc/Panel_11");
  hsxpl_xfmc_datarefs.line[11]=XPLMFindDataRef("xfmc/Panel_12");
  hsxpl_xfmc_datarefs.line[13]=XPLMFindDataRef("xfmc/Scratch");

  hsxpl_xfmc_datarefs.status=XPLMFindDataRef("xfmc/Status");

  hsxpl_fmc.key_lk1=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk2=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk3=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk4=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk5=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_lk6=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_rk1=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk2=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk3=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk4=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk5=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rk6=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_0=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_1=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_2=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_3=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_4=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_5=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_6=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_7=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_8=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_9=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_dot=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_plusminus=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_a=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_b=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_c=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_d=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_e=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_f=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_g=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_h=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_i=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_j=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_k=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_l=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_m=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_n=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_o=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_p=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_q=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_r=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_s=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_t=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_u=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_v=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_w=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_x=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_y=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_z=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_space=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_del=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_slash=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_clr=XPLMFindDataRef("xfmc/Keypath");

  hsxpl_fmc.key_init=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_fix=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_menu=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_prevpage=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_rte=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_legs=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_radnav=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_nextpage=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_deparr=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_hold=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_ap=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_perf=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_vnav=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_prog=XPLMFindDataRef("xfmc/Keypath");
  hsxpl_fmc.key_exec=XPLMFindDataRef("xfmc/Keypath");
}

/* hsxpl_send_xfmc_data is used to send xfmc data */
void hsxpl_send_xfmc_data(void) {


  uint8_t i;
  uint8_t j;
  uint8_t si;

  /* Build screen, prepare matrix  */

  hsmp_fmc_screen_t screen;
  memset(&screen,0,sizeof(hsmp_fmc_screen_t));

  /* Initialise to spaces */
  for(i=0;i<HSMP_FMC_MAX_SCREEN_NOROWS;i++) {
    for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
      screen.matrix[i][j].row=i;
      screen.matrix[i][j].col=j;
      screen.matrix[i][j].colour=0xFFFFFFFF;
      screen.matrix[i][j].fsize=0;
      screen.matrix[i][j].chr=' ';
    }
  }


  unsigned char line[256];line[255]='\0';
  unsigned char *cp;

  for(i=0;i<14;i++) {
    if(hsxpl_xfmc_datarefs.line[i]!=NULL) {

      int j=1;
      unsigned char str[256];

      memset(line,0,256);
      XPLMGetDatab(hsxpl_xfmc_datarefs.line[i],line,0,255);
      cp=line; while(*cp!='\0') { if(*cp>127) *cp = *cp & 0x7F; cp++;  if(cp-line >80) break; }

      /* Move to the part after the slash */
      cp=line;
      while(*cp!='\0' && *cp!='/') cp++;
      if(*cp=='/') cp++;

      do {
        if(hsxpl_strentry(j,(char *)cp,';',(char *)str)==NULL) break;
        if(str[0]=='\0') break;

        else {

          char font_size[256];
          char pixel_offset[256];
          char text[256];

          if(hsxpl_strentry(1,(char *)str,',',(char *)font_size)==NULL) break;
          if(font_size[0]=='\0') break;
          if(hsxpl_strentry(2,(char *)str,',',(char *)pixel_offset)==NULL) break;
          if(pixel_offset[0]=='\0') break;
          if(hsxpl_strentry(3,(char *)str,',',(char *)text)==NULL) break;
          if(text[0]=='\0') break;

          uint8_t fsize=0;
          if(font_size[0]=='0' && font_size[1]=='\0') fsize=20;

          uint8_t scol=0xFF;
          float f=atof(pixel_offset);
          if(f>=0.0) {
            float k = f * 30.0 / 199.0;
            scol = (uint8_t) floor(k);
            if(scol>=HSMP_FMC_MAX_SCREEN_NOCOLS) scol=0xFF;
          }

          if(scol!=0xFF) {

            char *cx=text;
            while(*cx!='\0') {
              if(scol>=HSMP_FMC_MAX_SCREEN_NOCOLS) break;

              if(*cx == 31) screen.matrix[i][scol].chr='_'; /* Box symbol */
              else if(*cx == 30) screen.matrix[i][scol].chr=(char)161; /* Degree symbol */
              else
                screen.matrix[i][scol].chr = toupper(*cx);

              screen.matrix[i][scol].fsize = fsize;
              if(fsize) screen.matrix[i][scol].colour=0xCCE5FFFF;
              /* LIGHT BLUE */
              cx++;scol++;
            }
          }


        }
        j++;

      } while(1);

    }
  }

  for(si=0;si<HSMP_FMC_MAX_SCREEN_NOROWS;si+=3) {

    hsmp_pkt_t *pkt=(hsmp_pkt_t *)hsmp_net_make_packet();
    if(pkt!=NULL) {

      uint8_t i,j;

      uint32_t n=HSMP_FMC_TYPE_XP_XFMC;
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_TYPE,&n);



      char xstr[8];
      memset(xstr,0,8);
      sprintf(xstr,"LNAV");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSLU_TXT,xstr);
      sprintf(xstr,"VNAV");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSLD_TXT,xstr);
      sprintf(xstr,"ATHR");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSRU_TXT,xstr);
      sprintf(xstr,"MCP");
      hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_AN_VSRD_TXT,xstr);

      uint32_t an=0;
      if(hsxpl_xfmc_datarefs.status!=NULL) {
        uint32_t st=XPLMGetDatai(hsxpl_xfmc_datarefs.status);
        if(st&(1<<5))
          an |= HSMP_FMC_ANNUNC_EXEC;
        if(st&(1<<1))
          an |= HSMP_FMC_ANNUNC_VSLU;
        if(!(st&(1<<2)))
          an |= HSMP_FMC_ANNUNC_VSLD;
        if(st&(1<<3))
          an |= HSMP_FMC_ANNUNC_VSRU;
        if(st&(1<<4))
          an |= HSMP_FMC_ANNUNC_VSRD;
        hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_ANNUNCIATORS,&an);
      }

      for(i=0;i<3;i++) {

        if(si+i >= HSMP_FMC_MAX_SCREEN_NOROWS) break;

        for(j=0;j<HSMP_FMC_MAX_SCREEN_NOCOLS;j++) {
          hsmp_net_add_msg_to_pkt(pkt,HSMP_MID_FMC_SCREEN_C,&(screen.matrix[si+i][j]));
        }
      }
      hsmp_net_send_to_stream_peers(pkt,HSMP_PKT_NT_AIRFMC);
      free(pkt);
    }
  }
}
