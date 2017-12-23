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
#include "hsairxplmisc.h"

#include <XPLMUtilities.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>


void IPONAVLatitudeCStringFor(double lat,char *latStr,int fmt)
{

  if(fmt==IPONAV_LATLON_FMT_DDD) {

    sprintf(latStr,"%f %c",lat,lat>=0?'N':'S');

  } else if(fmt==IPONAV_LATLON_FMT_DMM) {

    double clat=lat;
    char c='N';
    if(clat<0) { c='S'; clat=0-clat; }

    clat = round(clat * 60 * 100)/ 60 / 100;

    double degs=floor(clat);
    double mins=(clat-degs) * 60.0;
    double minsdec=mins-floor(mins);
    int d=(int)degs;
    int m=(int)mins;
    int dm=(int)(minsdec*100);

    sprintf(latStr,"%02d%c %02d.%02d' %c",d,161,m,dm,c);

  } else if(fmt==IPONAV_LATLON_FMT_DMS) {

    double clat=lat;
    char c='N';
    if(clat<0) { c='S'; clat=0-clat; }

    clat = round(clat * 60 * 100)/ 60 / 100;

    double degs=floor(clat);
    double mins=(clat-degs) * 60.0;
    double secs=mins-floor(mins);
    int d=(int)degs;
    int m=(int)mins;
    int s=(int)(secs*60);


    sprintf(latStr,"%02d%c %02d' %02d%c %c",d,161,m,s,'"',c);

  } else {
    latStr[0]='\0';
  }
}

void IPONAVLongitudeCStringFor(double lon,char *lonStr,int fmt)
{

  if(fmt==IPONAV_LATLON_FMT_DDD) {

    sprintf(lonStr,"%f %c",lon,lon<0?'W':'E');

  } else if(fmt==IPONAV_LATLON_FMT_DMM) {

    double clon=lon;
    char c='E';
    if(clon<0) { c='W'; clon=0-clon; }

    clon = round(clon * 60 * 100)/ 60 / 100;

    double degs=floor(clon);
    double mins=(clon-degs) * 60.0;
    double minsdec=mins-floor(mins);
    int d=(int)degs;
    int m=(int)mins;
    int dm=(int)(minsdec*100);
    sprintf(lonStr,"%03d%c %02d.%02d' %c",d,161,m,dm,c);

  } else if(fmt==IPONAV_LATLON_FMT_DMS) {

    double clon=lon;
    char c='E';
    if(clon<0) { c='W'; clon=0-clon; }

    clon = round(clon * 60 * 100)/ 60 / 100;

    double degs=floor(clon);
    double mins=(clon-degs) * 60.0;
    double secs=mins-floor(mins);
    int d=(int)degs;
    int m=(int)mins;
    int s=(int)(secs*60);

    sprintf(lonStr,"%03d%c %02d' %02d%c %c",d,161,m,s,'"',c);

  } else {
    lonStr[0]='\0';
  }
}

/* strqentry is like strentry but if it happens that a field is quoted between
 * quotechar characters, all the characters inbetween are considered even if
 * the match delim.
 *
 * In other words, if delim is ',' and there is "Something, something else"
 * and quotechar is '"' then the whole sentence is returned as an entry.
 *
 * If quotechar is \0 strqentry behaves like strentry.
 * As a matter of fact strentry calls strqentry with this character as quote.
 */
char *hsxpl_strqentry(int32_t element,char *str,char delim,char *entry,char quotechar) {

  char *pb;
  char d1,d2;
  char e1,e2,e3;
  int32_t i;

  /* Perform some validy tests */
  if(element<1) return NULL;
  if(str==NULL) return NULL;
  if(entry!=NULL) entry[0]='\0';

  /* Initialise EOL and delimiter characters */
  e1='\0'; e2='\n'; e3='\r';
  if(delim=='\r' || delim=='\n') { e2=e1; e3=e1; }

  if(delim=='\t' || delim==' ') { d1='\t'; d2=' '; }
  else if(delim=='\r' || delim=='\n') { d1='\r'; d2='\n'; }
  else { d1=delim; d2=delim; }
  /* If space / tab based, move to beginning of valid content */
  while(d1=='\t' && (*str==d1 || *str==d2) && *str!=e1 && *str!=e2  && *str!=e3)
    str++;

  /* If we are at the end, return here */
  if(*str==e1 || *str==e2 || *str==e3) {
    if(entry!=NULL) return entry;
    else return NULL;
  }

  /* Position str at the start of the nth element */
  for(i=0;i<element-1;i++) {

    /* Move to next delimiter */
    while(*str!=d1 && *str!=d2 && *str!=e1 && *str!=e2  && *str!=e3) {

      if(quotechar=='\0') str++;
      else {
        if((*str) != quotechar)
          str++;
        else {                                        str++;
          while(*str!=quotechar && *str!=e1 && *str!=e2  && *str!=e3) {
            str++;
          }
          if(*str==quotechar) str++;

        }
      }
    }

    /* If we reached the end, return here */
    if(*str==e1 || *str==e2 || *str==e3) {
      if(entry!=NULL)  return entry;
      else return NULL;
    }
    /* Move to beginning of next entry */
    if(d1=='\t') /* Tab / space based or not */
      while((*str==d1 || *str==d2) && *str!=e1 && *str!=e2  && *str!=e3)
        str++;
    else
      str++;

    /* If we reached the end, return here */
    if(*str==e1 || *str==e2 || *str==e3) {
      if(entry!=NULL)  return entry;
      else return NULL;
    }

  } /* end for() positioning */

  /* If no entry is passed, return current pointer */
  if(entry==NULL) return str;

  pb=entry;

  if(quotechar!='\0' && *str==quotechar) {
    d1=quotechar; d2=quotechar;
    str++;
  }

  /* Copy to entry */
  while((*str!=d1 && *str!=d2) && *str!=e1 && *str!=e2  && *str!=e3)
    *pb++ = *str++;

  *pb='\0';
  return entry;
}

/* strentry() returns the nth element of a string where elements are separated
 * by character <delim>.
 * If <delim> is a space, <delim> is expanded to both space or tab.
 * If <delim> is a CR or NL, <delim> is expanded to both CR or NL.
 * If <entry> is non-null (a valid string), strentry() copies the nth
 * element to entry and returns a pointer to it.
 * If <entry> is null, strentry() simply returns a pointer to the nth
 * element of the original string, including all subsequent elements.
 * strentry() returns NULL if str is NULL or element is invalid.
 *
 * strenty() is implemented using strqentry().
 */
char *hsxpl_strentry(int32_t element,char *str,char delim,char *entry) {
  
  return hsxpl_strqentry(element,str,delim,entry,'\0');
}

/* hsxpl_point_name_for_position() creates and returns a name for a
 * point based on its latitude and longitude. */
void hsxpl_point_name_for_position(double lat,double lon,char *pName)
{

  char c;
  int la,lo;

  pName[0]='\0';

  if(lat>90 || lat<-90) return;
  if(lon>180 || lon<-180) return;

  if(lat>=0 && lon>0) c='E';
  else if(lat>=0 && lon<=0) c='N';
  else if(lat<0 && lon>0) c='S';
  else c='W';

  la=(int)lat;
  lo=(int)lon;

  if(la<0) la=0-la;
  if(lo<0) lo=0-lo;

  if(lo>=100)
    sprintf(pName,"%02d%c%02d",la,c,lo%100);
  else
    sprintf(pName,"%02d%02d%c",la,lo%100,c);
  
  return;
}

void hsxpl_log(int level,char *logstr) {

#ifdef HSXPLDEBUG
  if(level<=HSXPLDEBUG) {
    char str[1024];
    memset(str,0,1024);
    sprintf(str,"HSAIRXPL: ");
    strncat(str,logstr,1000);
    strcat(str,"\n");
    XPLMDebugString(str);
  }
#endif

}

void hsxpl_log_str(char *logstr) {

#ifdef HSXPLDEBUG
  char str[1024];
  memset(str,0,1024);
  sprintf(str,"HSAIRXPL: ");
  strncat(str,logstr,1000);
  strcat(str,"\n");
  XPLMDebugString(str);
#endif
}

int hsxpl_path_is_dir(char *path) {

#if IBM
  struct _stat info;
  if( _stat( path, &info ) == 0 ) {
    char str[1024];
    sprintf(str,"stat on %s is %ld",path,info.st_mode);
    hsxpl_log(HSXPLDEBUG_ACTION,str);
    return S_ISDIR( info.st_mode );
  }
#else
  struct stat info;
  if( stat( path, &info ) == 0 ) {
    return S_ISDIR( info.st_mode );
  }
#endif
  return 0;
}

int hsxpl_path_is_reg(char *path) {

#if IBM
  struct _stat info;
  if( _stat( path, &info ) == 0 ) {
    char str[1024];
    sprintf(str,"stat on %s is %ld",path,info.st_mode);
    hsxpl_log(HSXPLDEBUG_ACTION,str);
    return S_ISREG( info.st_mode );
  }
#else
  struct stat info;
  if( stat( path, &info ) == 0 ) {
    return S_ISREG( info.st_mode );
  }
#endif
  return 0;
}
