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
 * Description:     Extension to support the CP Flight MCP Pro and EFIS
 *
 */

#ifdef CPFLIGHT

#include "hsxpl.h"
#include "hsxplcpflight.h"
#include "hsxplmcp.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <math.h>
#include <dirent.h>
#include <sys/time.h>

#if !defined(_WIN32)
#include <sys/select.h>
#endif

#if LIN /* can't seem to find roundf() in math.h / linux so need to declare it here */
float roundf(float x);
#endif

/* A structure to hold the hardware states so that we don't overwrite */
struct hsaircpf_hardware_s {

    uint32_t    crs1_dial;
    uint32_t    crs2_dial;
    uint32_t    hdg_dial;
    float       spd_dial;
    uint32_t    alt_dial;
    int32_t     vvm_dial;
    uint32_t    speed_is_mach;
    
    uint32_t    ma1_led;
    uint32_t    ma2_led;
    uint32_t    n1_led;
    uint32_t    spd_led;
    uint32_t    lchg_led;
    uint32_t    vnav_led;
    uint32_t    hdg_led;
    uint32_t    lnav_led;
    uint32_t    loc_led;
    uint32_t    app_led;
    uint32_t    alt_led;
    uint32_t    vs_led;
    uint32_t    cmda_led;
    uint32_t    cmdb_led;
    uint32_t    cwsa_led;
    uint32_t    cwsb_led;
    uint32_t    overspeed;
    
} hsaircpf_hardware;    /* And a global variable to hold the states */


/* The name of the serial port to open as defined in settings */
char hsaircpf_serial_port[256];

/* The file descriptor for the CP Flight MCP serial port */
#if IBM
HANDLE hsaircpf_fd=INVALID_HANDLE_VALUE;
#else
int32_t		hsaircpf_fd=-1;
#endif

/* Initialisation of the hardware once the serial port has been open */
void hsaircpf_initialise_hardware(void)
{
    
    char buffer[32];
    
    if(hsaircpf_fd<0) return;
    
    /* Initialise states to those given by the plugin */
    memset(&hsaircpf_hardware,0,sizeof(hsaircpf_hardware));
    
    /* Disable hardware */
    sprintf(buffer,"K999");
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    sleep(5);
    
    /* Turn on hardware */
    sprintf(buffer,"Q001");
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    sleep(8);
    
    /* Backlighting */
    sprintf(buffer,"L0198");
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set brighness if applicable */
    sprintf(buffer,"C0000");
    buffer[3]=10;
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set altitude dial */
    hsaircpf_hardware.alt_dial=hsairpl_mcp_get_alt_dial();
    sprintf(buffer,"V04%05d",hsaircpf_hardware.alt_dial);
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set hdg dial */
    hsaircpf_hardware.hdg_dial=hsairpl_mcp_get_hdg_dial();
    sprintf(buffer,"V03%03d",hsaircpf_hardware.hdg_dial);
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set CRS1 dial */
    hsaircpf_hardware.crs1_dial=hsairpl_mcp_get_crs1_dial();
    sprintf(buffer,"V01%03d",hsaircpf_hardware.crs1_dial);
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set CRS2 dial */
    hsaircpf_hardware.crs2_dial=hsairpl_mcp_get_crs2_dial();
    sprintf(buffer,"V06%03d",hsaircpf_hardware.crs2_dial);
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set SPD dial */
    hsaircpf_hardware.spd_dial=hsairpl_mcp_get_speed_dial();
    if(!hsairpl_mcp_get_speed_is_mach())
        sprintf(buffer,"V02%03.0f",hsaircpf_hardware.spd_dial);
    else {
        float f=hsaircpf_hardware.spd_dial*100.0;
        sprintf(buffer,"V02 .%2.0f",f);
        /* if(buffer[3]=='0') if(buffer[3]==' '); */
    }
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
    /* Set VS dial */
    hsaircpf_hardware.vvm_dial=hsairpl_mcp_get_vvm_dial();
    if(hsaircpf_hardware.vvm_dial<0) {
        sprintf(buffer,"V05-%04d",hsaircpf_hardware.vvm_dial);
        /* sprintf(buffer,"V05-%04d",hsaircpf_hardware.vvm_dial); */
    } else {
        sprintf(buffer,"V05+%04d",hsaircpf_hardware.vvm_dial);
    }
    
    hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    
}

/* This function opens the serial port and sets it to 38400 8N1
 * The success or failure of this action is determined by
 * ap_fd being >= 0 or not. */
void hsaircpf_open_serial_port(void)
{
    
#if IBM
    
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    
    if(hsaircpf_fd!=INVALID_HANDLE_VALUE) return;
    
    
    char serial_port[256];
    if(hsaircpf_serial_port[0]!='\0') {
        snprintf(serial_port,255,"\\\\.\\%s",hsaircpf_serial_port);
    } else {
        sprintf(serial_port,"\\\\.\\COM5");
    }
    
    hsaircpf_fd = CreateFile(
                        serial_port, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hsaircpf_fd == INVALID_HANDLE_VALUE) {
        return;
    }
    
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hsaircpf_fd, &dcbSerialParams) == 0)
    {
        CloseHandle(hsaircpf_fd);
        hsaircpf_fd=INVALID_HANDLE_VALUE;
        return;
    }
    
    dcbSerialParams.BaudRate = CBR_38400;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hsaircpf_fd, &dcbSerialParams) == 0)
    {
        CloseHandle(hsaircpf_fd);
        hsaircpf_fd=INVALID_HANDLE_VALUE;
        return;
        
    }
    
    /* Set COM port timeout settings */
    
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    
	/*
          timeouts.ReadIntervalTimeout = 50;
          timeouts.ReadTotalTimeoutConstant = 50;
          timeouts.ReadTotalTimeoutMultiplier = 10;
          timeouts.WriteTotalTimeoutConstant = 50;
          timeouts.WriteTotalTimeoutMultiplier = 10;
	*/
    if(SetCommTimeouts(hsaircpf_fd, &timeouts) == 0)
    {
        CloseHandle(hsaircpf_fd);
        hsaircpf_fd=INVALID_HANDLE_VALUE;
        return;
    }
    
#else

    if(hsaircpf_fd>=0) return;
    
    char fname[64];
    memset(fname,0,64);
    
    
    if(hsaircpf_serial_port[0]!='\0') {
        strncpy(fname,hsaircpf_serial_port,63);
    } else {
        strcpy(fname,"/dev/mcp");
#if !LIN
        /* Get file name */
        DIR *dirp = opendir("/dev");
        struct dirent *dp;
        while ((dp = readdir(dirp)) != NULL) {
            if (dp->d_namlen >strlen("CPF cu.usbserial-")  && !strncmp(dp->d_name, "cu.usbserial-",strlen("cu.usbserial-")))  {
                snprintf(fname,63,"/dev/%s",dp->d_name);
                fname[63]='\0';
                break;
            }
        }
        (void)closedir(dirp);
#endif
    }
    
    hsaircpf_fd=open(fname,O_RDWR | O_NOCTTY | O_NDELAY);
    
    if(hsaircpf_fd>=0) {
        
        struct termios options;
        tcgetattr(hsaircpf_fd, &options);
        cfsetispeed(&options, B38400);
        cfsetospeed(&options, B38400);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        tcsetattr(hsaircpf_fd, TCSANOW, &options);
        
    }
    
    

#endif
    
}

void hsaircpf_close_serial_port(void) {
    
#if IBM
    if(hsaircpf_fd!=INVALID_HANDLE_VALUE) {
        hsaircpf_write(hsaircpf_fd,"K999",5);
        CloseHandle(hsaircpf_fd);
        hsaircpf_fd=INVALID_HANDLE_VALUE;
    }
#else
    if(hsaircpf_fd>=0) {
        hsaircpf_write(hsaircpf_fd,"K999",5);
        close(hsaircpf_fd);
        hsaircpf_fd=-1;
    }
#endif
    
}

#if IBM
ssize_t hsaircpf_write(HANDLE fildes, const void *buf, size_t nbyte) {
    DWORD bytes_written = 0;
    if(WriteFile(hsaircpf_fd, buf, nbyte, &bytes_written, NULL)) {
        ssize_t b = (ssize_t)bytes_written;
        return b;
    };
    return -1;
}
#else
ssize_t hsaircpf_write(int fildes, const void *buf, size_t nbyte) {
    return write(fildes,buf,nbyte);
}
#endif

void hsaircpf_process_cmd(char *cmd)
{
    
    
    if(cmd==NULL) return;
    if(*cmd=='\0') return;
    
    /* CRS 1 dial */
    if(!strncmp(cmd,"V01",3)) {
        uint32_t nv=atoi(&cmd[3]);
        if(hsaircpf_hardware.crs1_dial!=nv) {
            hsaircpf_hardware.crs1_dial=nv;
            hsairpl_mcp_set_crs1_dial(nv);
        }
        return;
    }
    
    /* CRS 2 dial */
    if(!strncmp(cmd,"V06",3)) {
        uint32_t nv=atoi(&cmd[3]);
        if(hsaircpf_hardware.crs2_dial!=nv) {
            hsaircpf_hardware.crs2_dial=nv;
            hsairpl_mcp_set_crs2_dial(nv);
        }
        return;
    }

    /* Altitude dial */
    if(!strncmp(cmd,"V04",3)) {
        uint32_t nv=atoi(&cmd[3]);
        if(hsaircpf_hardware.alt_dial!=nv) {
            hsaircpf_hardware.alt_dial=nv;
            hsairpl_mcp_set_alt_dial(nv);
        }
        return;
    }
    
    /* HDG dial */
    if(!strncmp(cmd,"V03",3)) {
        uint32_t nv=atoi(&cmd[3]);
        if(hsaircpf_hardware.hdg_dial!=nv) {
            hsaircpf_hardware.hdg_dial=nv;
            hsairpl_mcp_set_hdg_dial(nv);
        }
        return;
    }

    /* VSpeed dial */
    if(!strncmp(cmd,"V05",3)) {
        int32_t nv;
        if(cmd[3]=='+') nv=atoi(&cmd[4]);
        if(cmd[3]=='-') nv=0-atoi(&cmd[4]);
        else nv=atoi(&cmd[3]);
        if(hsaircpf_hardware.vvm_dial!=nv) {
            hsaircpf_hardware.vvm_dial=nv;
            hsairpl_mcp_set_vvm_dial(nv);
        }
        return;
    }
    
    /* SPD dial */
    if(!strncmp(cmd,"V02",3)) {
        float nv=atof(&cmd[3]);

        if(strchr(cmd,'.')!=NULL)
            hsaircpf_hardware.speed_is_mach=1;
        else
            hsaircpf_hardware.speed_is_mach=0;
        
        if(hsairpl_mcp_get_speed_is_mach() != hsaircpf_hardware.speed_is_mach) {
            hsairpl_mcp_set_speed_is_mach(hsaircpf_hardware.speed_is_mach);
        }

        if(hsaircpf_hardware.spd_dial!=nv) {
            
            hsaircpf_hardware.spd_dial=nv;
            hsairpl_mcp_set_speed_dial(nv);
        }
        return;
    }
    
    /* CO toggle */
    if(!strncmp(cmd,"K023",4)) {
        hsairpl_mcp_press_co();
        return;
    }
    
    /* SPD intv */
    if(!strncmp(cmd,"K010",4)) {
        hsairpl_mcp_press_spdintv();
        return;
    }
    
    /* ALT intv */
    if(!strncmp(cmd,"K012",4)) {
        hsairpl_mcp_press_altintv();
        return;
    }

    /* ALT HLD toggle */
    if(!strncmp(cmd,"K030",4)) {
        hsairpl_mcp_press_alt();
        return;
    }
    
    /* HDG HLD toggle */
    if(!strncmp(cmd,"K025",4)) {
        hsairpl_mcp_press_hdg();
        return;
    }
    
    /* CMD A press */
    if(!strncmp(cmd,"K032",4)) {
        hsairpl_mcp_press_cmda();
        return;
    }

    /* CMD B press */
    if(!strncmp(cmd,"K033",4)) {
        hsairpl_mcp_press_cmdb();
        return;
    }

    /* CWS A press */
    if(!strncmp(cmd,"K034",4)) {
        hsairpl_mcp_press_cwsa();
        return;
    }

    /* CWS B press */
    if(!strncmp(cmd,"K035",4)) {
        hsairpl_mcp_press_cwsb();
        return;
    }

    /* AP DISENGAGE ON */
    if(!strncmp(cmd,"K041",4)) {
        hsairpl_mcp_set_apdisc(1);
        return;
    }

    /* AP DISENGAGE OFF */
    if(!strncmp(cmd,"K040",4)) {
        hsairpl_mcp_set_apdisc(0);
        return;
    }

    /* N1 PRESS */
    if(!strncmp(cmd,"K021",4)) {
        hsairpl_mcp_press_n1();
        return;
    }

    /* SPEED PRESS */
    if(!strncmp(cmd,"K022",4)) {
        hsairpl_mcp_press_spd();
        return;
    }

    /* LCHG PRESS */
    if(!strncmp(cmd,"K024",4)) {
        hsairpl_mcp_press_lchg();
        return;
    }

    /* VNAV PRESS */
    if(!strncmp(cmd,"K026",4)) {
        hsairpl_mcp_press_vnav();
        return;
    }

    /* LNAV PRESS */
    if(!strncmp(cmd,"K027",4)) {
        hsairpl_mcp_press_lnav();
        return;
    }

    /* VORLOC PRESS */
    if(!strncmp(cmd,"K028",4)) {
        hsairpl_mcp_press_loc();
        return;
    }

    /* APP PRESS */
    if(!strncmp(cmd,"K029",4)) {
        hsairpl_mcp_press_app();
        return;
    }

    /* VS PRESS */
    if(!strncmp(cmd,"K031",4)) {
        hsairpl_mcp_press_vs();
        return;
    }

    /* AT ARM ON */
    if(!strncmp(cmd,"K019",4)) {
        hsairpl_mcp_set_atarm(1);
        return;
    }
    
    /* AT ARM OFF */
    if(!strncmp(cmd,"K020",4)) {
        hsairpl_mcp_set_atarm(0);
        return;
    }

    /* FD1 ON/OFF */
    if(!strncmp(cmd,"K037",4)) {
        hsairpl_mcp_set_fd1(1);
        return;
    }
    if(!strncmp(cmd,"K038",4)) {
        hsairpl_mcp_set_fd1(0);
        return;
    }

    /* FD2 ON/OFF */
    if(!strncmp(cmd,"K017",4)) {
        hsairpl_mcp_set_fd2(1);
        return;
    }
    if(!strncmp(cmd,"K018",4)) {
        hsairpl_mcp_set_fd2(0);
        return;
    }

    /* EFIS range */
    if(!strncmp(cmd,"S000",4)) {
        hsairpl_efis1_set_range(5);
        return;
    }
    if(!strncmp(cmd,"S001",4)) {
        hsairpl_efis1_set_range(10);
        return;
    }
    if(!strncmp(cmd,"S002",4)) {
        hsairpl_efis1_set_range(20);
        return;
    }
    if(!strncmp(cmd,"S003",4)) {
        hsairpl_efis1_set_range(40);
        return;
    }
    if(!strncmp(cmd,"S004",4)) {
        hsairpl_efis1_set_range(80);
        return;
    }
    if(!strncmp(cmd,"S005",4)) {
        hsairpl_efis1_set_range(160);
        return;
    }
    if(!strncmp(cmd,"S006",4)) {
        hsairpl_efis1_set_range(320);
        return;
    }
    if(!strncmp(cmd,"S007",4)) {
        hsairpl_efis1_set_range(640);
        return;
    }

    /* EFIS mode */
    if(!strncmp(cmd,"M001",4)) {
        hsairpl_efis1_set_mode(0);
        return;
    }
    if(!strncmp(cmd,"M002",4)) {
        hsairpl_efis1_set_mode(1);
        return;
    }
    if(!strncmp(cmd,"M003",4)) {
        hsairpl_efis1_set_mode(2);
        return;

    }
    if(!strncmp(cmd,"M004",4)) {
        hsairpl_efis1_set_mode(4);
        return;
    }

    /* TFC mode */
    if(!strncmp(cmd,"S008",4)) {
        hsairpl_efis1_press_tfc();
        return;
    }
    
    /* CTR mode */
    if(!strncmp(cmd,"M005",4)) {
        hsairpl_efis1_press_ctr();
        return;
    }
    
    /* EFIS WXR */
    if(!strncmp(cmd,"B001",4)) {
        hsairpl_efis1_press_wxr();
        return;
    }
    
    /* EFIS STA */
    if(!strncmp(cmd,"B002",4)) {
        hsairpl_efis1_press_sta();
        return;
    }
    
    /* EFIS WPT*/
    if(!strncmp(cmd,"B003",4)) {
        hsairpl_efis1_press_wpt();
        return;
    }
    
    /* EFIS ARPT*/
    if(!strncmp(cmd,"B004",4)) {
        hsairpl_efis1_press_arpt();
        return;
    }

    /* EFIS DATA */
    if(!strncmp(cmd,"B005",4)) {
        hsairpl_efis1_press_data();
        return;
    }

    /* EFIS POS */
    if(!strncmp(cmd,"B006",4)) {
        hsairpl_efis1_press_pos();
        return;
    }

    /* EFIS TERR */
    if(!strncmp(cmd,"B007",4)) {
        hsairpl_efis1_press_terr();
        return;
    }


    /* EFIS VOR/ADF  SELECTORS */
    if(!strncmp(cmd,"K070",4)) {
        hsairpl_efis1_set_vas1(2);
        return;
    }
    if(!strncmp(cmd,"K071",4)) {
        hsairpl_efis1_set_vas1(0);
        return;
    }
    if(!strncmp(cmd,"K072",4)) {
        hsairpl_efis1_set_vas1(1);
        return;
    }
    if(!strncmp(cmd,"K073",4)) {
        hsairpl_efis1_set_vas2(2);
        return;
    }
    if(!strncmp(cmd,"K074",4)) {
        hsairpl_efis1_set_vas2(0);
        return;
    }
    if(!strncmp(cmd,"K075",4)) {
        hsairpl_efis1_set_vas2(1);
        return;
    }
    
    /* MINSINC/DEC */
    if(!strncmp(cmd,"V1010",5)) { /* INC */
        hsairpl_efis1_inc_mins(atof(&cmd[5]));
    }
    if(!strncmp(cmd,"V1000",5)) { /* DEC */
        hsairpl_efis1_dec_mins(atof(&cmd[5]));
    }
    
    /* EFIS BARO INC/DEC */
    if(!strncmp(cmd,"V1110",5)) {
        hsairpl_efis1_inc_baro(atof(&cmd[5]));
        return;
    }
    if(!strncmp(cmd,"V1100",5)) {
        hsairpl_efis1_dec_baro(atof(&cmd[5]));
        return;
    }

    /* EFIS BARO STD */
    if(!strncmp(cmd,"K049",4)) {
        hsairpl_efis1_press_std();
        return;
    }
    
    /* BARO units selector */
    if(!strncmp(cmd,"K062",4)) {
        hsairpl_efis1_select_inhg();
        return;
    }
    if(!strncmp(cmd,"K063",4)) {
        hsairpl_efis1_select_hpa();
        return;
    }
    
     if(!strncmp(cmd,"K064",4)) {
         hsairpl_efis1_mins_select_radio();
     }

    if(!strncmp(cmd,"K065",4)) {
        hsairpl_efis1_mins_select_baro();
    }

    if(!strncmp(cmd,"K048",4)) {
        hsairpl_efis1_mins_press_rst();
    }

    if(!strncmp(cmd,"K044",4)) {
        hsairpl_efis1_press_fpv();
        return;
    }
    
    /* MTRS push */
    if(!strncmp(cmd,"K045",4)) {
        hsairpl_efis1_press_mtrs();
        return;
    }
    

}

void hsaircpf_runloop(void) {
    
    char        buffer[512];
    uint32_t    x;
    int32_t     i;
    float       f;
    
#if IBM
    if(hsaircpf_fd==INVALID_HANDLE_VALUE) {
        hsaircpf_open_serial_port();
        if(hsaircpf_fd!=INVALID_HANDLE_VALUE) hsaircpf_initialise_hardware();
    }
    
    /* If failed, return here and try again later */
    if(hsaircpf_fd==INVALID_HANDLE_VALUE) return;
#else
    if(hsaircpf_fd<0) {
        hsaircpf_open_serial_port();
        if(hsaircpf_fd>=0) hsaircpf_initialise_hardware();
    }
    
    /* If failed, return here and try again later */
    if(hsaircpf_fd<0) return;
    
#endif
    
    /* PART 1 - CHECK IF SOMETHING CHANGED IN THE SIM */
    
    /* CRS1 */
    if((x=hsairpl_mcp_get_crs1_dial())!=hsaircpf_hardware.crs1_dial) {
        hsaircpf_hardware.crs1_dial=x;
        sprintf(buffer,"V01%03d",hsaircpf_hardware.crs1_dial);
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* CRS2 */
    if((x=hsairpl_mcp_get_crs2_dial())!=hsaircpf_hardware.crs2_dial) {
        hsaircpf_hardware.crs2_dial=x;
        sprintf(buffer,"V06%03d",hsaircpf_hardware.crs2_dial);
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* HDG */
    if((x=hsairpl_mcp_get_hdg_dial())!=hsaircpf_hardware.hdg_dial) {
        hsaircpf_hardware.hdg_dial=x;
        sprintf(buffer,"V03%03d",hsaircpf_hardware.hdg_dial);
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* ALT */
    if((x=hsairpl_mcp_get_alt_dial())!=hsaircpf_hardware.alt_dial) {
        hsaircpf_hardware.alt_dial=x;
        sprintf(buffer,"V04%05d",hsaircpf_hardware.alt_dial);
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* VS DIAL */
    if((i=hsairpl_mcp_get_vvm_dial())!=hsaircpf_hardware.vvm_dial) {
        hsaircpf_hardware.vvm_dial=i;
        if(hsaircpf_hardware.vvm_dial<0)
            sprintf(buffer,"V05-%04d",0-hsaircpf_hardware.vvm_dial);
        else
            sprintf(buffer,"V05+%04d",hsaircpf_hardware.vvm_dial);
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* Set SPD dial */
    f=hsairpl_mcp_get_speed_dial();
    f *= 100.0; f=roundf(f); f/=100.0;  /* Cut the crap */
    if(f!= hsaircpf_hardware.spd_dial ||
       (hsairpl_mcp_get_speed_is_mach() != hsaircpf_hardware.speed_is_mach)){

        hsaircpf_hardware.spd_dial=f;
        hsaircpf_hardware.speed_is_mach=hsairpl_mcp_get_speed_is_mach();
        
        if(!hsaircpf_hardware.speed_is_mach)
            sprintf(buffer,"V02 %03.0f",hsaircpf_hardware.spd_dial);
        else {
            float f=hsaircpf_hardware.spd_dial*100.0;
            sprintf(buffer,"V02 .%2.0f",f);
            /* if(buffer[3]=='0') if(buffer[3]==' '); */
        }

        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* MA1 LED*/
    if((x=hsairpl_mcp_get_ma1_led())!=hsaircpf_hardware.ma1_led) {
        hsaircpf_hardware.ma1_led=x;
        if(x)
            sprintf(buffer,"L0137");
        else
             sprintf(buffer,"L1137");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* MA2 LED */
    if((x=hsairpl_mcp_get_ma2_led())!=hsaircpf_hardware.ma2_led) {
        hsaircpf_hardware.ma2_led=x;
        if(x)
            sprintf(buffer,"L0117");
        else
            sprintf(buffer,"L1117");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* N1 LED */
    if((x=hsairpl_mcp_get_n1_led())!=hsaircpf_hardware.n1_led) {
        hsaircpf_hardware.n1_led=x;
        if(x)
             sprintf(buffer,"L0121");
        else
            sprintf(buffer,"L1121");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* SPD LED */
    if((x=hsairpl_mcp_get_spd_led())!=hsaircpf_hardware.spd_led) {
        hsaircpf_hardware.spd_led=x;
        if(x)
            sprintf(buffer,"L0122");
        else
            sprintf(buffer,"L1122");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* LCHG LED */
    if((x=hsairpl_mcp_get_lchg_led())!=hsaircpf_hardware.lchg_led) {
        hsaircpf_hardware.lchg_led=x;
        if(x)
            sprintf(buffer,"L0124");
        else
            sprintf(buffer,"L1124");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* VNAV LED */
    if((x=hsairpl_mcp_get_vnav_led())!=hsaircpf_hardware.vnav_led) {
        hsaircpf_hardware.vnav_led=x;
        if(x)
            sprintf(buffer,"L0126");
        else
            sprintf(buffer,"L1126");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* VS LED */
    if((x=hsairpl_mcp_get_vs_led())!=hsaircpf_hardware.vs_led) {
        hsaircpf_hardware.vs_led=x;
        if(x)
            sprintf(buffer,"X1105");
        else
            sprintf(buffer,"X1005");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* HDG LED */
    if((x=hsairpl_mcp_get_hdg_led())!=hsaircpf_hardware.hdg_led) {
        hsaircpf_hardware.hdg_led=x;
        if(x)
            sprintf(buffer,"L0125");
        else
            sprintf(buffer,"L1125");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* LNAV LED */
    if((x=hsairpl_mcp_get_lnav_led())!=hsaircpf_hardware.lnav_led) {
        hsaircpf_hardware.lnav_led=x;
        if(x)
            sprintf(buffer,"L0127");
        else
            sprintf(buffer,"L1127");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* LOC LED */
    if((x=hsairpl_mcp_get_loc_led())!=hsaircpf_hardware.loc_led) {
        hsaircpf_hardware.loc_led=x;
        if(x)
            sprintf(buffer,"L0128");
        else
            sprintf(buffer,"L1128");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* APP LED */
    if((x=hsairpl_mcp_get_app_led())!=hsaircpf_hardware.app_led) {
        hsaircpf_hardware.app_led=x;
        if(x)
            sprintf(buffer,"L0129");
        else
            sprintf(buffer,"L1129");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    /* ALT LED */
    if((x=hsairpl_mcp_get_alt_led())!=hsaircpf_hardware.alt_led) {
        hsaircpf_hardware.alt_led=x;
        if(x)
            sprintf(buffer,"L0130");
        else
            sprintf(buffer,"L1130");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    /* CMDA LED */
    if((x=hsairpl_mcp_get_cmda_led())!=hsaircpf_hardware.cmda_led) {
        hsaircpf_hardware.cmda_led=x;
        if(x)
            sprintf(buffer,"L0132");
        else
            sprintf(buffer,"L1132");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    /* CMDB LED */
    if((x=hsairpl_mcp_get_cmdb_led())!=hsaircpf_hardware.cmdb_led) {
        hsaircpf_hardware.cmdb_led=x;
        if(x)
            sprintf(buffer,"L0133");
        else
            sprintf(buffer,"L1133");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    /* CWSA LED */
    if((x=hsairpl_mcp_get_cwsa_led())!=hsaircpf_hardware.cwsa_led) {
        hsaircpf_hardware.cwsa_led=x;
        if(x)
            sprintf(buffer,"L0134");
        else
            sprintf(buffer,"L1134");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    /* CWSB LED */
    if((x=hsairpl_mcp_get_cwsb_led())!=hsaircpf_hardware.cwsb_led) {
        hsaircpf_hardware.cwsb_led=x;
        if(x)
            sprintf(buffer,"L0135");
        else
            sprintf(buffer,"L1135");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }
    
    if((x=hsairpl_mcp_get_overspeed())!=hsaircpf_hardware.overspeed) {
        hsaircpf_hardware.overspeed=x;
        if(x)
            sprintf(buffer,"L0196");
        else
            sprintf(buffer,"L1196");
        hsaircpf_write(hsaircpf_fd,buffer,strlen(buffer)+1);
    }

    
    
    /* PART II - Check if we received commands from the hardware */

    buffer[0]='\0';
    i=0;

#if IBM

    DWORD bytes_read = 0;
    do {
        
        if(i==511) break;
        
        if(ReadFile(hsaircpf_fd,&buffer[i], 1, &bytes_read, NULL)){
            if(buffer[i]==0 && i>0) {
                hsaircpf_process_cmd(buffer);
                i=-1;
            }
        } else {
            break;
        }
        i++;
    } while(bytes_read>0);
    
#else
    
    
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(hsaircpf_fd,&readfds);
    tv.tv_sec=0;
    tv.tv_usec=0;
    
    do {
        if(i==511) break;
        if(select(hsaircpf_fd+1,&readfds,NULL,NULL,&tv) == -1) break;
        
        if(FD_ISSET(hsaircpf_fd,&readfds)) {
            if(read(hsaircpf_fd,&buffer[i],1)==1) {
                if(buffer[i]==0 && i>0) {
                    hsaircpf_process_cmd(buffer);
                    i=-1;
                }
                i++;
            }
            
        } else {
            break;
        }
        
    } while(FD_ISSET(hsaircpf_fd,&readfds));
    
#endif
    
    
    
}



#endif /* CPFLIGHT */
