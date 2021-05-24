## Makefile for building mkhelp.com and chksize.com
## Copyright (c) 2007 Oleg O. Chukaev <oleg.chukaev@mail.ru>
##
## Used programs:
##   GNU Make 3.79.1
##   Open Watcom C 1.5

CC = wcl
CFLAGS = -zq -mt -0 -bcl=com -wx

all:	mkhelp.com chksize.com

mkhelp.com:	mkhelp.c mkhelp.h
	$(CC) $(CFLAGS) mkhelp.c

chksize.com:	chksize.c
	$(CC) $(CFLAGS) chksize.c

clean:
	del *.obj
	del mkhelp.com
	del chksize.com


