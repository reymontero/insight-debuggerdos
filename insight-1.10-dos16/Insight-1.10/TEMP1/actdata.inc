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


MainLoop	dw	kbAltX,		0
		dw	kbF10,		ExecuteMenu
		dw	kbAltAlone,	ExecuteMenu
		dw	kbAltF10,	CPULocalMenu

		dw	kbDown,		CurDown
		dw	kbUp,		CurUp
		dw	kbPgDn,		PageDown
		dw	kbPgUp,		PageUp
		dw	kbHome,		CurHome
		dw	kbEnd,		CurEnd
		dw	kbRight,	CurRight
		dw	kbLeft,		CurLeft
		dw	kbCtrlLeft,	ByteUp
		dw	kbCtrlRight,	ByteDown

		dw	kbCtrlEnter,	GoToOrigin
		dw	kbCtrlO,	GoToOrigin
		dw	kbCtrlN,	NewCS_IP
		dw	kbCtrlG,	UnasmGoTo
		dw	kbCtrlD,	DumpGoTo
		dw	kbCtrlB,	BlockMenu
		dw	kbCtrlF,	Follow
		dw	kbCtrlP,	Previous

		dw	kbCtrlA,	RestoreAll
		dw	kbCtrlT,	RestoreFlags
		dw	kbCtrlC,	RestoreCSIP
		dw	kbCtrlR,	SwitchRegMode

		dw	kbCtrlS,	SearchBytes
		dw	kbCtrlL,	SearchNext

		dw	kbAltF5,	ExecUserScreen
		dw	kbF5,		DrawScreen

		dw	kbF7,		SingleStep
		dw	kbF8,		ProcTrace
		dw	kbCtrlF9,	RunProgram
		dw	kbF4,		GoToCursor
		dw	kbF2,		ToggleBreakPoint
		dw	kbAltF2,	ToggleActivity

		dw	kbCtrlF2,	ResetProgram
		dw	kbF3,		LoadFile

		dw	kbCtrlW,	DataWatch

		dw	kbAltE,		DumpESDI
		dw	kbAltD,		DumpDSSI
		dw	kbAltB,		DumpESBX
		dw	kbAltS,		DumpSSBP
		dw	kbAltC,		DumpCSIP
		dw	kbAltA,		DumpDXAX
		dw	kbAltF,		DumpDSDX
		dw	kbAltI,		DumpESSI
		dw	kbAltJ,		DumpDSDI
		dw	kbAltL,		DumpCurLine
		dw	kbAltEqu,	DumpReference

		dw	kbAlt1,		SetCodeMark1
		dw	kbAlt2,		SetCodeMark2
		dw	kbAlt3,		SetCodeMark3
		dw	kbAlt4,		SetCodeMark4
		dw	kbAlt5,		SetCodeMark5
		dw	kbAlt6,		SetCodeMark6
		dw	kbAlt7,		SetCodeMark7
		dw	kbAlt8,		SetCodeMark8
		dw	kbAlt9,		SetCodeMark9
		dw	kbAlt0,		SetCodeMark0

		dw	0200h + '1',	CodeMark1
		dw	0300h + '2',	CodeMark2
		dw	0400h + '3',	CodeMark3
		dw	0500h + '4',	CodeMark4
		dw	0600h + '5',	CodeMark5
		dw	0700h + '6',	CodeMark6
		dw	0800h + '7',	CodeMark7
		dw	0900h + '8',	CodeMark8
		dw	0A00h + '9',	CodeMark9
		dw	0B00h + '0',	CodeMark0

		dw	0200h + '!',	ClearCodeMark1
		dw	0300h + '@',	ClearCodeMark2
		dw	0400h + '#',	ClearCodeMark3
		dw	0500h + '$',	ClearCodeMark4
		dw	0600h + '%',	ClearCodeMark5
		dw	0700h + '^',	ClearCodeMark6
		dw	0800h + '&',	ClearCodeMark7
		dw	0900h + '*',	ClearCodeMark8
		dw	0A00h + '(',	ClearCodeMark9
		dw	0B00h + ')',	ClearCodeMark0

		dw	0,		AnyKey

MagicCode	db	'I' xor 45h
		db	'D' xor 45h
		db	'D' xor 45h
		db	'Q' xor 45h
		db	'D' xor 45h

Reg16PosTable	label	word
AXpos:
		db	REGx+3,REGy
		dw	CPURegs.xax
		dw	FlagsPos, SIpos, IPpos, BXpos
BXpos:
		db	REGx+3,REGy+1
		dw	CPURegs.xbx
		dw	CSpos, DIpos, AXpos, CXpos
CXpos:
		db	REGx+3,REGy+2
		dw	CPURegs.xcx
		dw	DSpos, BPpos, BXpos, DXpos
DXpos:
		db	REGx+3,REGy+3
		dw	CPURegs.xdx
		dw	ESpos, SPpos, CXpos, IPpos

SIpos:
		db	REGx+12,REGy
		dw	CPURegs.xsi
		dw	AXpos, CSpos, SPpos, DIpos
DIpos:
		db	REGx+12,REGy+1
		dw	CPURegs.xdi
		dw	BXpos, DSpos, SIpos, BPpos
BPpos:
		db	REGx+12,REGy+2
		dw	CPURegs.xbp
		dw	CXpos, ESpos, DIpos, SPpos
SPpos:
		db	REGx+12,REGy+3
		dw	CPURegs.xsp
		dw	DXpos, SSpos, BPpos, IPpos

CSpos:
		db	REGx+21,REGy
		dw	CPURegs.xcs
		dw	SIpos, BXpos, FlagsPos, DSpos
DSpos:
		db	REGx+21,REGy+1
		dw	CPURegs.xds
		dw	DIpos, CXpos, CSpos, ESpos
ESpos:
		db	REGx+21,REGy+2
		dw	CPURegs.xes
		dw	BPpos, DXpos, DSpos, SSpos
SSpos:
		db	REGx+21,REGy+3
		dw	CPURegs.xss
		dw	SPpos, IPpos, ESpos, FlagsPos

IPpos:
		db	REGx+3, REGy+5
		dw	CPURegs.xip
		dw	SSpos, FlagsPos, DXpos, AXpos

FlagsPos:
		db	REGx+21, REGy+5
		dw	CPURegs.xflags
		dw	IPpos, AXpos, SSpos, CSpos

Reg32PosTable	label	word
EAXhi:
		db	REGx+4,REGy
		dw	CPURegs.xax+2
		dw	EFlagsPos, EAXlo, ESPhi, EBXhi
EAXlo:
		db	REGx+9,REGy
		dw	CPURegs.xax
		dw	EAXhi, ECSpos, ESPlo, EBXlo
EBXhi:
		db	REGx+4,REGy+1
		dw	CPURegs.xbx+2
		dw	ECSpos, EBXlo, EAXhi, ECXhi
EBXlo:
		db	REGx+9,REGy+1
		dw	CPURegs.xbx
		dw	EBXhi, EDSpos, EAXlo, ECXlo
ECXhi:
		db	REGx+4,REGy+2
		dw	CPURegs.xcx+2
		dw	EDSpos, ECXlo, EBXhi, EDXhi
ECXlo:
		db	REGx+9,REGy+2
		dw	CPURegs.xcx
		dw	ECXhi, EESpos, EBXlo, EDXlo
EDXhi:
		db	REGx+4,REGy+3
		dw	CPURegs.xdx+2
		dw	EESpos, EDXlo, ECXhi, ESIhi
EDXlo:
		db	REGx+9,REGy+3
		dw	CPURegs.xdx
		dw	EDXhi, ESSpos, ECXlo, ESIlo
ESIhi:
		db	REGx+4,REGy+4
		dw	CPURegs.xsi+2
		dw	ESSpos, ESIlo, EDXhi, EDIhi
ESIlo:
		db	REGx+9,REGy+4
		dw	CPURegs.xsi
		dw	ESIhi, EFSpos, EDXlo, EDIlo
EDIhi:
		db	REGx+4,REGy+5
		dw	CPURegs.xdi+2
		dw	EFSpos, EDIlo, ESIhi, EBPhi
EDIlo:
		db	REGx+9,REGy+5
		dw	CPURegs.xdi
		dw	EDIhi, EGSpos, ESIlo, EBPlo

EBPhi:
		db	REGx+4,REGy+7
		dw	CPURegs.xbp+2
		dw	EGSpos, EBPlo, EDIhi, ESPhi
EBPlo:
		db	REGx+9,REGy+7
		dw	CPURegs.xbp
		dw	EBPhi, EIPpos, EDIlo, ESPlo
ESPhi:
		db	REGx+4,REGy+8
		dw	CPURegs.xsp+2
		dw	EIPpos, ESPlo, EBPhi, EAXhi
ESPlo:
		db	REGx+9,REGy+8
		dw	CPURegs.xsp
		dw	ESPhi, EFlagsPos, EBPlo, EAXlo

ECSpos:
		db	REGx+21,REGy
		dw	CPURegs.xcs
		dw	EAXlo, EBXhi, EFlagsPos, EDSpos
EDSpos:
		db	REGx+21,REGy+1
		dw	CPURegs.xds
		dw	EBXlo, ECXhi, ECSpos, EESpos
EESpos:
		db	REGx+21,REGy+2
		dw	CPURegs.xes
		dw	ECXlo, EDXhi, EDSpos, ESSpos
ESSpos:
		db	REGx+21,REGy+3
		dw	CPURegs.xss
		dw	EDXlo, ESIhi, EESpos, EFSPos
EFSpos:
		db	REGx+21,REGy+4
		dw	CPURegs.xfs
		dw	ESIlo, EDIhi, ESSpos, EGSPos
EGSpos:
		db	REGx+21,REGy+5
		dw	CPURegs.xgs
		dw	EDIlo, EBPhi, EFSpos, EIPPos

EIPpos:
		db	REGx+21, REGy+7
		dw	CPURegs.xip
		dw	EBPlo, ESPhi, EGSpos, EFlagsPos

EFlagsPos:
		db	REGx+21, REGy+8
		dw	CPURegs.xflags
		dw	ESPlo, EAXhi, EIPpos, ECSpos

;			address/opcode/instruction/number/arrow/disabled

CmdNormal	db	atNormAddr,	atNormOpcode,	atNormInstr
		db	atNormNumber,	atNormArrow,	atNormDisable

CmdSelected	db	atSelAddr,	atSelOpcode,	atSelInstr
		db	atSelNumber,	atSelArrow,	atSelDisable

CmdBreakpoint	db	atBrkAddr,	atBrkOpcode,	atBrkInstr
		db	atBrkNumber,	atBrkArrow,	atBrkDisable

CmdBrkDisabled	db	atBrkDisAddr,	atBrkDisOpcode,	atBrkDisInstr
		db	atBrkDisNumber,	atBrkDisArrow,	atBrkDisDisable

CmdBrkSelected	db	atBrkSelAddr,	atBrkSelOpcode,	atBrkSelInstr
		db	atBrkSelNumber,	atBrkSelArrow,	atBrkSelDisable

CmdBrkDisSel	db	atBrkDSAddr,	atBrkDSOpcode,	atBrkDSInstr
		db	atBrkDSNumber,	atBrkDSArrow,	atBrkDSDisable


AsmExitKeys	dw	kbEnter, kbEsc, kbDown, kbUp, kbPgDn, kbPgUp, 0

AsmErrors	label	word
		dw	msMnemonic	;	2
		dw	msInvalidParam	;	3
		dw	msMemParam	;	4
		dw	msExtra		;	5
		dw	msBigNumber	;	6
		dw	msRegBits	;	7
		dw	msParamType	;	8
		dw	msRepError	;	9
msMnemonic	db	'Unknown mnemonic',0
msInvalidParam	db	'Invalid parameter(s)',0
msMemParam	db	'Invalid register addressing',0
msExtra		equ	msInvalidParam			;db	'Too many parameters',0
msBigNumber	db	'Number too big',0
msRegBits	equ	msInvalidParam			;	'',0
msParamType	equ	msInvalidParam
msRepError	db	'Incorrect prefix use',0

MainTitle	db	' ',SerialNumber,' ',0
TitleLen	equ	$-MainTitle

ResidentMsg	db	'Debugger has been installed resident.', 13, 10
		db	'  Press Ctrl+Esc to activate.', 13, 10, '$'
AlreadyResident	db	'Debugger already resident',0
AddrErrorMsg	db	'Invalid address value',0
CountErrorMsg	db	'Invalid count value',0
WriteErrorMsg	db	"Can't write block on disk",0
ReadErrorMsg	db	"Can't read block from disk",0
OpenErrorMsg	db	"Can't open file",0
CreateErrorMsg	db	"Can't create file",0
NoSpecifMsg	db	'No program loaded',0
LoadErrorMsg	db	"Can't load program",0
TerminateMsg	db	'Program terminated',0
DumpNotInRAM	db	'This address is not in RAM',0
ManyBPMsg	db	'Too many breakpoints',0
BPnotInRAM	db	'Breakpoint not in RAM',0
CodeNotInRAM	db	'Code not in RAM',0
AsmErrorMsg	db	'Invalid assembler instruction',0
CodeMarkMsg	db	'Code marks and '
FollowStackMsg	db	'Follow stack cleared.',0
StringErrorMsg	db	'Invalid string.',0
NotFoundMsg	db	'Bytes not found.',0
SearchMsg	db	'Searching bytes...',0



EnvName		db	'TEMP.COM',0
RReg		db 	'AXBXCXDXSIDIBPSP'
RRegSegs	db	'CSDSESSSFSGSIPFL'
RegNames	db	'AXSICSBXDIDSCXBPESDXSPSS'
FlagsMsg	db	'Flags=',0
FlagsStr	db	'OF DF IF SF ZF AF PF CF',0
StackMsg	db	'Stack:',0
SSSPMsg		db	'SS:SP ',16, 0
SSMsg		db	'SS:',0

DataWatchStr	db	' watch = ',0

DataWatchLabels	label	byte
		db	'ES:DI'
		db	'DS:SI'
		db	'ES:BX'
		db	'SS:BP'
		db	'DX:AX'
		db	'DS:DX'
		db	'ES:SI'
		db	'DS:DI'
		db	'CS:IP'

FMask		db	'*.*',0

CPUName		dw	cpu8086

CPUNames	label	word
		dw	cpu8086
		dw	cpu8088
		dw	cpu80286
		dw	cpu80386
		dw	cpu80486

cpu8086		db	' 8086 ',0
cpu8088		db	' 8088 ',0
cpu80286	db	' 80286 ',0
cpu80386	db	' 80386 ',0
cpu80486	db	' 80486 ',0
cpuAdvanced	db	' '
cpuAdvancedName	db	'???????????? '
cpuAdvancedID	db	'086 ',0


RegShift	db	11, 10, 9, 7, 6, 4, 2, 0

FlagStruc	db	5, 1, 1, 2, 1, 2, 2, 2
NumFlags	equ	8


FollowStackPtr	dw	FollowStack

CodeEnd		dw	ProgramEnd


UserCurShape	dw	0607h
UserMode	db	3


SaveDTAofs	dw	80h


CurIP		dw	100h
CurIPline	dw	-1
UserScreenMode	db	1

Loaded		label	byte
RefFlags	db	1
Focus		db	1
RegsSaved	db	0

