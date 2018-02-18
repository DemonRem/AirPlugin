# -
# Copyright (c) 2013-2017 Haversine Ltd
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
#
#

INCLUDES=-I. -I.. -I./XPSDK/CHeaders/XPLM -I./XPSDK/CHeaders/Widgets -I./HSMP
CFLAGS=-DCPFXPL=1 -DXPLM200=1 -DXPLM210=1 -DXPLM211=1 -DXPL=1 -DCPFLIGHT=1 -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wimplicit-function-declaration # -DHSXPLDEBUG=3 # -DHSMPDEBUG=1 -fPIC -fno-stack-protector
LIBS=	# -lm
LFLAGS= # rdynamic
CC=cc

HDRS=./source/hsxpl.h ./HSMP/hsmpnet.h ./HSMP/hsmptcp.h ./HSMP/hsmpmsg.h ./HSMP/hsmpatp.h ./source/hsxplclist.h ./source/hsxplmcp.h ./source/hsxplcoms.h ./source/hsxplcpflight.h ./source/hsxplmisc.h ./source/hsxplnavdb.h ./source/hsxplatc.h ./source/hsxplairports.h ./source/hsxpldref-requests.h ./source/hsxpldefault738.h ./source/hsxplzibo738.h ./source/hsxplfmc.h ./source/hsxplsettings.h

SRCS=./source/hsxplclist.c ./source/hsxpl.c	./source/hsxplmcp.c ./source/hsxplcoms.c ./HSMP/hsmpnet.c ./HSMP/hsmptcp.c ./source/hsxplcpflight.c ./source/hsxplmisc.c ./source/hsxplnavdb.c ./source/hsxplatc.c ./source/hsxplairports.c ./source/hsxpldref-requests.c ./source/hsxpldefault738.c ./source/hsxplzibo738.c ./source/hsxplfmc.c ./source/hsxplsettings.c

##############################################################################
# Windows flags and libraries
##############################################################################

#CCWIN32=i686-w64-mingw32-gcc
CCWIN32=/usr/local/mingw-w32-bin_i686-darwin_20130516/bin/i686-w64-mingw32-gcc
#CCWIN64=x86_64-w64-mingw32-gcc
CCWIN64=/usr/local/mingw-w64-bin_i686-darwin_20130622/bin/x86_64-w64-mingw32-gcc

CFLAGSWIN64=-DIBM=1 -DAPL=0 -DLIN=0 -D_WIN32=1 -DCPFLIGHT=1
CFLAGSWIN32=-DIBM=1 -DAPL=0 -DLIN=0 -D_WIN32=1 

LFLAGSWIN32=-m32 -shared -L ./XPSDK/Libraries/Win # -ggdb -flat_namespace
LFLAGSWIN64=-m64 -shared -L ./XPSDK/Libraries/Win # -ggdb -flat_namespace

LIBSWIN32=-lXPLM -lXPWidgets -lws2_32	#-lwsock32
LIBSWIN64=-lXPLM_64 -lXPWidgets_64 -lws2_32	#-lwsock32

##############################################################################
# OSX flags and libraries
##############################################################################

CCOSX32=cc
CCOSX64=cc

#CFLAGSOSX32=-m32 -arch i386 -arch ppc -universal -DIBM=0 -DAPL=1 -DLIN=0
#CFLAGSOSX32=-m32 -arch i386 -arch ppc -universal -mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -DIBM=0 -DAPL=1 -DLIN=0
CFLAGSOSX32=-m32 -arch i386 -DIBM=0 -DAPL=1 -DLIN=0
CFLAGSOSX64=-m64 -arch x86_64 -DIBM=0 -DAPL=1 -DLIN=0 -DCPFXPL=1

#LFLAGSOSX32=-m32 -arch i386 -arch ppc -flat_namespace -undefined warning -dynamiclib -nodefaultlibs -ggdb -L./XPSDK/Libraries/Mac
#LFLAGSOSX32=-m32 -arch i386 -arch ppc -flat_namespace -undefined warning -syslibroot=/Developer/SDKs/MacOSX10.4u.sdk -dynamiclib -shared -nodefaultlibs -ggdb -L./XPSDK/Libraries/Mac
LFLAGSOSX32=-m32 -arch i386 -flat_namespace -undefined warning -dynamiclib -shared -L./XPSDK/Libraries/Mac
LFLAGSOSX64=-m64 -arch x86_64 -flat_namespace -undefined warning -dynamiclib -shared -L./XPSDK/Libraries/Mac

LIBSOSX32= # -framework XPWidgets -framework XPLM
LIBSOSX64= # -framework XPWidgets -framework XPLM

##############################################################################
# Linux flags and libraries
##############################################################################
CCLIN32=/usr/local/gcc-4.8.1-for-linux32/bin/i586-pc-linux-gcc
CCLIN64=/usr/local/gcc-4.8.1-for-linux64/bin/x86_64-pc-linux-gcc

CFLAGSLIN32=-fPIC -m32 -O0 -ansi -iquote. -DIBM=0 -DAPL=0 -DLIN=1 -D_BSD_SOURCE=1
CFLAGSLIN64=-fPIC -m64 -O0 -ansi -iquote. -DIBM=0 -DAPL=0 -DLIN=1 -D_BSD_SOURCE=1 -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wimplicit-function-declaration

LFLAGSLIN32=-m32 -shared
LFLAGSLIN64=-m64 -shared

LIBSLIN32=
LIBSLIN64=

##############################################################################
# Generic building section         
##############################################################################

help:
	@echo   ""
	@echo	Building Syntax:
	@echo   ""
	@echo	"\tmake osx64 - For Mac OS X 64 bit build"
	@echo	"\tmake osx32 - For Mac OS X 32 bit build"
	@echo	"\tmake lin64 - For Linux 64 bit build"
	@echo	"\tmake lin32 - For Linux 32 bit build"
	@echo	"\tmake win64 - For Windows 64 bit build"
	@echo	"\tmake win32 - For Windows 32 bit build"
	@echo	"\tmake all   - For all builds"
	@echo   ""
	@echo   ""

all:	osx32 osx64 win32 win64 lin32 lin64
osx64:	HaversineAir/64/mac.xpl
osx32:	HaversineAir/32/mac.xpl
lin64:	HaversineAir/64/lin.xpl
lin32:	HaversineAir/32/lin.xpl
win64:	HaversineAir/64/win.xpl
win32:	HaversineAir/32/win.xpl

##############################################################################
# Windows build files
##############################################################################

HaversineAir/32/win.xpl:  $(SRCS) $(HDRS)
	$(CCWIN32) -o HaversineAir/32/win.xpl $(SRCS) $(CFLAGS) $(CFLAGSWIN32) $(INCLUDES) $(LIBS) $(LFLAGS) $(LFLAGSWIN32) $(LIBSWIN32)

HaversineAir/64/win.xpl:  $(SRCS) $(HDRS)
	$(CCWIN64) -o HaversineAir/64/win.xpl $(SRCS) $(CFLAGS) $(CFLAGSWIN64) $(INCLUDES) $(LIBS) $(LFLAGS) $(LFLAGSWIN64) $(LIBSWIN64)

##############################################################################
# OSX build files
##############################################################################

HaversineAir/32/mac.xpl:  $(SRCS) $(HDRS)
	$(CCOSX32) -o HaversineAir/32/mac.xpl $(SRCS) $(CFLAGS) $(CFLAGSOSX32) $(INCLUDES) $(LIBS) $(LFLAGS) $(LFLAGSOSX32) $(LIBSOSX32)

HaversineAir/64/mac.xpl:  $(SRCS) $(HDRS)
	$(CCOSX64) -o HaversineAir/64/mac.xpl $(SRCS) $(CFLAGS) $(CFLAGSOSX64) $(INCLUDES) $(LIBS) $(LFLAGS) $(LFLAGSOSX64) $(LIBSOSX64)

##############################################################################
# Linux build files
##############################################################################

HaversineAir/32/lin.xpl:  $(SRCS) $(HDRS)
	$(CCLIN32) -o HaversineAir/32/lin.xpl $(SRCS) $(CFLAGS) $(CFLAGSLIN32) $(INCLUDES) $(LIBS) $(LFLAGS) $(LFLAGSLIN32) $(LIBSLIN32)

HaversineAir/64/lin.xpl:  $(SRCS) $(HDRS)
	$(CCLIN64) -o HaversineAir/64/lin.xpl $(SRCS) $(CFLAGS) $(CFLAGSLIN64) $(INCLUDES) $(LIBS) $(LFLAGS) $(LFLAGSLIN64) $(LIBSLIN64)

clean:
	rm -f *.o *.xpl *.exp *.lib HaversineAir/32/*.xpl HaversineAir/64/*.xpl

