;_____________________________________________________________________________
;
; Insight, real-mode debugger for MS/PC-DOS
;
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002
; 
;_____________________________________________________________________________
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
; 02111-1307, USA.
;_____________________________________________________________________________


if ??VERSION lt (4*256)
  cpuid		equ	db	00Fh, 0A2h
endif

SerialNumber		equ	'Insight 1.10'


WindowBufSize		equ	2500
StringBufSize		equ	(WindowBufSize - 256 * 3)
PaletteBuffer		equ	(offset WindowBuffer + StringBufSize)
FollowStack		equ	PaletteBuffer


SearchFillLen		equ	50
FileNameLen		equ	80

StackSize		equ	800h
MaxProgramSize		equ	65000

MaxIniFileBuff		equ	(200+2)

Num16Registers		equ	14
Num32Registers		equ	24

MaxBreakPoints		equ	10

MaxItems		equ	32
                	
RPTlength		equ	14

NameStrSize		equ	64
NameStrLen		equ	100
MaxFileName		equ	150
AddrStrLen		equ	24

                	
CPUAddrSize		equ	6
CPUOpcodeSize		equ     15
                	
CPUx			equ	0
CPUy			equ	0
CPUstart		equ	CPUy*160+CPUx*2
CMDstart		equ	(CPUy+1)*160+CPUx*2
CPUwidth		equ	54
CPUheight		equ	17
REGx			equ	CPUx+CPUwidth+1
REGy			equ	0
REGstart		equ	REGy*160+REGx*2

DUMPheight		equ	5
DUMPx			equ	0
DUMPy			equ	25-DUMPheight
DUMPstart		equ	DUMPy*160+DUMPx*2

STK16x			equ	REGx+1
STK16y			equ	10
STK16len		equ	8
STK16start		equ	(STK16y+STK16len-1)*160+(STK16x+7)*2

STK32x			equ	REGx+1
STK32y			equ	13
STK32len		equ	5
STK32start		equ	(STK32y+STK32len-1)*160+(STK32x+7)*2

FLAG16x			equ	REGx
FLAG16y			equ	(STK16y-3)
FLAG16start		equ	FLAG16y*160+FLAG16x*2

FLAG32x			equ	REGx
FLAG32y			equ	(STK32y-3)
FLAG32start		equ	FLAG32y*160+FLAG32x*2
                	
MEMstart		equ	(CPUy+CPUheight+1)*160+40
WATCHstart		equ	(CPUy+CPUheight+1)*160+40+42+8
                	
MaxCurX			equ	80-CPUwidth
AsmLineSize		equ	CPUwidth-1-CMD_POS
MaxAsmSize		equ	80-CMD_POS
AddrStrSize		equ	24
ScanCode		equ	1
ShiftCode		equ	4

                	
atCPUborder		equ	White

atCmdNormal		equ	atNormInstr
atCmdSelected		equ	atSelInstr
atBP&Cur		equ	atBrkSelInstr
atBreakPoint		equ	atBrkInstr

atNormAddr		equ	LightGray
atNormOpcode		equ	Green
atNormInstr		equ	LightGreen
atNormNumber		equ	Cyan
atNormArrow		equ	White
atNormDisable		equ	LightGray
                	
atSelAddr		equ	LightGray*16
atSelOpcode		equ	LightGray*16
atSelInstr		equ	LightGray*16
atSelNumber		equ	LightGray*16
atSelArrow		equ	LightGray*16+LightCyan
atSelDisable		equ	LightGray*16+DarkGray

atBrkAddr		equ	Red*16+White
atBrkOpcode		equ	Red*16+White
atBrkInstr		equ	Red*16+White
atBrkNumber		equ	Red*16+White
atBrkArrow		equ	Red*16+White
atBrkDisable		equ	Red*16+DarkGray

atBrkDisAddr		equ	Red*16+DarkGray
atBrkDisOpcode		equ	Red*16+DarkGray
atBrkDisInstr		equ	Red*16+DarkGray
atBrkDisNumber		equ	Red*16+DarkGray
atBrkDisArrow		equ	Red*16+White
atBrkDisDisable		equ	Red*16+DarkGray

atBrkSelAddr		equ	Magenta*16+White
atBrkSelOpcode		equ	Magenta*16+White
atBrkSelInstr		equ	Magenta*16+White
atBrkSelNumber		equ	Magenta*16+White
atBrkSelArrow		equ	Magenta*16+White
atBrkSelDisable		equ	Magenta*16+DarkGray

atBrkDSAddr		equ	Magenta*16+DarkGray
atBrkDSOpcode		equ	Magenta*16+DarkGray
atBrkDSInstr		equ	Magenta*16+DarkGray
atBrkDSNumber		equ	Magenta*16+DarkGray
atBrkDSArrow		equ	Magenta*16+White
atBrkDSDisable		equ	Magenta*16+DarkGray

;atCmdNormal		equ	LightGreen
;atCmdSelected		equ	LightGray*16
;atBreakPoint		equ	Red*16+White
;atBP&Cur		equ	Magenta*16+White

atAsmLine		equ	White

atTitle			equ	Black*16 + Yellow
atCPUName		equ	Cyan

atCodeMark		equ	LightRed

atRegName		equ	LightGray
atRegValue		equ	LightGreen
atRegNewValue		equ	LightCyan


atFlagValue		equ	LightGreen
atFlagNewValue		equ	LightCyan

atDumpAddr		equ	LightGray
atDumpValue		equ	LightGreen
;atDumpCursor		equ	Blue*16+LightGreen
atDumpCursor		equ	Blue*16+White

atStackName		equ	LightGray
atStackValue		equ	LightGreen

atMemAddr		equ	White
atMemValue		equ	Yellow

atDataWatchMem		equ	LightRed

atAssembler		equ	LightGray*16+White
atsAssembler		equ	LightGray*16+White
atBrkAssembler		equ	Magenta*16+LightCyan
atsBrkAssembler		equ	Magenta*16+LightCyan

bpUnused		equ	0
bpEnabled		equ	1
bpDisabled		equ	2

MagicRequestCode	equ	0C0DEh
DeveloperID 		equ	0FACEh
ApplicationID		equ	1
DebuggerVersion		equ	0103h
MagicAnswerCode		equ	0FADEh



Registers32	struc
  xax		label	word
		dd      ?
  xbx		label	word
		dd      ?
  xcx		label	word
		dd      ?
  xdx		label	word
		dd      ?
  xsi		label	word
		dd      ?
  xdi		label	word
		dd      ?
  xbp		label	word
		dd      ?
  xsp		label	word
		dd      ?
  xcs		label	word
		dd      ?
  xds		label	word
		dd      ?
  xes		label	word
		dd      ?
  xss		label	word
		dd      ?
  xfs		label	word
		dd      ?
  xgs		label	word
		dd      ?
  xip		label	word
		dd      ?
  xflags	label	word
		dd      ?
Registers32	ends


RegPosition	struc
  x		db	?
  y		db	?
  RegLink	dw	?
  LLeft		dw	?
  LRight	dw	?
  LUp		dw	?
  LDown		dw	?
ends

BreakPoint	struc
  BPseg		dw	?
  BPofs		dw	?
  BPbyte	db	?
  BPset		db	?
  BPactive	db	?
ends

AsmParam	struc
  RegNumber	db	-1
  Value		dw	0
  MemFlag	db	0
  BXflag	db	0
  BPflag	db	0
  SIflag	db	0
  DIflag	db	0
  WordFlag	db	0
ends

