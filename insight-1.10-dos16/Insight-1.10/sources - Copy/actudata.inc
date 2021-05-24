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


SaveCPURegs	Registers32	?
CPURegs		Registers32	?


BP0		BreakPoint	?
BP1		label	BreakPoint
		db 	MaxBreakPoints*size BreakPoint dup (?)


SaveInt01	dd	?
Int01set	db	?
SaveInt03	dd	?
Int03set	db	?
SaveInt09	dd	?
Int09set	db	?
SaveInt16	dd	?
SaveInt24	dd	?

SaveSP		dw	?

CPU		db	?

UserCursor	dw	?

SaveDTAseg	dw	?


CurX		dw	?
CurLine		dw	?
CurLineIP	dw	?
UserPID		dw	?
MemoryFlag	db	?
MemDispType	db	?
MemDispOfs	dw	?
MemPrefix	dw	?
CmdTextSize	dw	?
MyPort21	db	?
UserPort21	db	?

DlgWinA		dw	?
DlgWinB		dw	?
;DlgRunPtr	dw	?
DlgItems	dw	?
DlgActiveItem	dw	?
DlgItemsData	dw	MaxItems dup (?)

QuitFlag	db	?
ResidentMode	db	?
DumpEditMode	db	?
CodeMarkExist	dw	?
FileSpecified	db	?
CmdLineOptions	db	?
RegsMode	db	?
CurLineBrk	db	?
VideoType	db	?
DecimalCounter	db	?

Cur_IP		dw	?
Cur_cmd		db	?
; Second_byte	db	?
Cur_param	dw	?
Word_flag	db	?
DWord_flag	db	?
Print_flag	db	?
Cur_prefix	dw	?
Ignore_prefix	db	?
Unasm_seg	dw	?
Inst286		db	?
OpSizeFlag	db	?
AddrSizeFlag	db	?
StrFlag		db	?
CurRep		dw	?
JCNear		db	?
DBflag		db	?


;SavedByte	db	?
;TmpFlags	dw	?


FollowOffs	dw	?
FollowSeg	dw	?

DataWatchProc	dw	?
DataWatchTemp	dw	?
DataWatchLabel	dw	?

SearchOffs	dw	?
SearchSeg	dw	?
SearchLen	dw	?
SearchOptions	dw	?

DumpSeg		dw	?
DumpOffs	dw	?
NewDumpSeg	dw	?
NewDumpOffs	dw	?
DumpProc	dw	?

MinusFlag	db	?
MovFlag		db	?
LxxFlag		db	?
JumpFlag	db	?
ParamCount	db	?
RepPref		db	?
FarValue	dw	?

FromAddr	dd	?
ToAddr		dd	?
CountValue	dw	?

MagicOffs	dw	?

InstrTable	dw	CPUheight-1 dup (?)
AsmBuffer	db	20 dup (?)
Buffer		db	BUF_SIZE dup (?)
NameBuf		db	MaxLetter+2 dup (?)
MyCmdLine	db	80h dup (?)
CmdLineString	db	80h dup (?)
FileNameString	db	FileNameLen+1 dup (?)
SearchString	db	SearchFillLen+1 dup (?)
SearchBin	db	SearchFillLen+1 dup (?)
FName		db	128 dup (?)
AsmLine		db	MaxAsmSize+1 dup (?)

DumpAddrString	db	AddrStrLen+1 dup (?)
MainAddrString	db	AddrStrLen+1 dup (?)
SearchAddrStr	label	byte
FromString	db	AddrStrLen+1 dup (?)
ToString	db	AddrStrLen+1 dup (?)
CountString	db	AddrStrLen+1 dup (?)
FillString	db	SearchFillLen+1 dup (?)
FillBin		db	SearchFillLen+1 dup (?)

InterruptTable	db	1024 dup (?)
;UserIntTable	db	1024 dup (?)
UserScreen	db	4000 dup (?)

