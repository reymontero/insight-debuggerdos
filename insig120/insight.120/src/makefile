## Makefile for building Insight 1.20
## Copyright (c) 2006, 2007 Oleg O. Chukaev <oleg.chukaev@mail.ru>
##
## Used programs:
##   Nasm 0.98.38
##   GNU Make 3.79.1
##   mkhelp, chksize, uclpack -- in `tools' directory


AS = ..\tools\nasm
AFLAGS = -t -s -O5 -w+orphan-labels -f bin


all:	insight.com

tools:
	cd ..\tools
	make
	cd ..\src

insight.com: insight.asm *.inc data\insight.xlp
	$(AS) $(AFLAGS) -o insight.com insight.asm

data\insight.xlp:	data\insight.hlp
	..\tools\mkhelp data\insight.hlp data\ins_help.tmp
	..\tools\chksize data\ins_help.tmp 4000
	..\tools\uclpack -R --10 --nrv2b data\ins_help.tmp data\insight.xlp

clean:
	del insight.com
	del data\insight.xlp
	del data\ins_help.tmp

realclean:	clean
	cd ..\tools
	make clean
	cd ..\src

