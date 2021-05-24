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



include		init.inc
include		colors.inc
include		cmds.inc
include		insight.inc

Start:
		call	Install

PostInstall:



;------------------------------------------------------------------------------
CalcProgSize	proc	near
  ; cx = additional memory
  ;return cf = 1 on error, ax = program end, dx = paragraphs
		push	cx
		mov	ax,CodeEnd
		add	ax,cx
		jc	@@Exit
		test	ax,0Fh
		jz	@@CalcSize
		and	ax,0FFF0h
		add	ax,10h
		jc	@@Exit
	@@CalcSize:
		add	ax,StackSize
		jc	@@Exit
		cmp	ax,MaxProgramSize+1
		cmc
		jc	@@Exit
		mov	cl,4
		mov	dx,ax
		shr	dx,cl
		clc
	@@Exit:
		pop	cx
		ret
		endp

;------------------------------------------------------------------------------
About		proc
		mov	si,offset AboutDialog
_About:
		call	InitDialog
		call	ExecDialog
		jmp	DrawScreen
		endp

;------------------------------------------------------------------------------
AnyKey		proc
		call	CheckShift
		jz	@@CheckCheatCode
		mov	cx,4
		mov	bx,offset @@EditProcs
	@@Next:
		cmp	ds:[bx],ah
		jne	@@Skip
		call	ds:[bx+1]
		clc
		ret
	@@Skip:
		add	bx,3
		loop	@@Next

	@@Exit:
		stc
		ret

	@@CheckCheatCode:
		call	UpCase
		xor	al,45h
		mov	bx,MagicOffs
		cmp	ds:MagicCode[bx],al
		je	@@NextChar
		xor	bx,bx
		cmp	MagicCode,al
		jne	@@Exit

	@@NextChar:
		inc	bx
		mov	MagicOffs,bx
		cmp	bl,5
		jne	@@Exit
		mov	si,offset InfoDialog
		call	_About
		clc
		ret


@@EditProcs	label	byte
		db	kbAltR/256
		dw	EditRegisters
		db	kbAltA/256
		dw	EditCommands
		db	kbAltF/256
		dw	EditFlags
		db	kbAltD/256
		dw	EditDump
		endp

;------------------------------------------------------------------------------
DataWatch	proc
		cmp	DataWatchProc,0
		mov	DataWatchProc,0
		jne	@@UpdateWatch
		mov	ax,DataWatchTemp
		or	ax,ax
		jnz	@@On
		jmp	Beep

	@@On:
		mov	DataWatchProc,ax
		call	ax

	@@UpdateWatch:
		jmp	UpdateWatchLabel
		endp

;------------------------------------------------------------------------------
SwitchRegMode	proc
		call	pushr
		cmp	CPU,3
		jnc	@@Toggle
		jmp	Beep

	@@Toggle:
		mov	ax,'23'
		xor	RegsMode,1
		jz	@@Skip32
		mov	ax,'61'
	@@Skip32:
		mov	word ptr RegsMenu1,ax
		mov	word ptr RegsMenu2,ax
		call	GetCursor
		push	cx
		call	DrawScreen
		pop	cx
		call	SetCursor
		call	popr
		ret
		endp

;------------------------------------------------------------------------------
CurDown		proc
		cmp	CurLine,CPUheight-1
		jb	@@inc
		mov	si,CurIP
		call	UnAssemble
		mov	CurIP,si
		jmp	@@Quit
@@inc:
		inc	CurLine
@@Quit:
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
CurUp		proc
		cmp	CurLine,0
		ja	@@dec
		cmp	CurIP,0
		je	@@Quit
		mov	@@c,20
@@Next1:
		mov	si,CurIP
		sub	si,@@c
		jnc	@@Next
		xor	si,si
@@Next:
		mov	bp,si
		call	UnAssemble
		cmp	si,CurIP
		je	@@Found
		cmp	si,CurIP
		jb	@@Next
		dec	@@c
		jnz	@@Next1
		dec	CurIP
		jmp	@@Quit
@@Found:
		mov	CurIP,bp
		jmp	@@Quit
@@dec:
		dec	CurLine
@@Quit:
		jmp	UpdateCommands
@@c		dw	20
		endp

;------------------------------------------------------------------------------
PageDown	proc
		mov	si,CurIP
		mov	cx,CPUheight-1
@@Next:
		call	UnAssemble
		loop	@@Next
		mov	CurIP,si
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
PageUp		proc
		mov	si,CurIP
		or	si,si
		je	CurHome
		sub	si,5*CPUheight
		jnc	@@3
		xor	si,si
@@3:
		mov	cx,CPUheight-1
		xor	bx,bx
@@Next:
		mov	InstrTable[bx],si
		call	UnAssemble
		inc	bx
		inc	bx
		loop	@@Next
@@2:
		cmp	si,CurIP
		jae	@@Found
		mov	cx,CPUheight-2
		cld
		push	si
		mov	si,offset InstrTable
		mov	di,si
		lodsw
		rep	movsw
		pop	si
		mov	ax,si
		stosw
		call	UnAssemble
		jmp	@@2
@@Found:
		mov	si,InstrTable
		mov	CurIP,si
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
CurHome		proc
		mov	CurLine,0
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
CurEnd		proc
		mov	CurLine,CPUheight-1
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
CurRight	proc
		cmp	CurX,MaxCurX
		jae	@@Quit
		inc	CurX
		call	UpdateCommands
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
CurLeft		proc
		cmp	CurX,0
		je	@@Quit
		dec	CurX
		call	UpdateCommands
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
GoToOrigin	proc			; don't push
		mov	ax,CPURegs.xcs
		mov	Unasm_seg,ax
		mov	ax,CPURegs.xip
		mov	CurIP,ax
		mov	CurLine,0
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
NewCS_IP	proc
		mov	ax,CurLineIP
		mov	CPURegs.xip,ax
		mov	ax,Unasm_seg
		mov	CPURegs.xcs,ax
		jmp	UpdateRegsAndCmds
endp

;------------------------------------------------------------------------------
ByteUp		proc
		cmp	CurIP,0
		je	@@Quit
		dec	CurIP
		call	UpdateCommands
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
ByteDown	proc
		inc	CurIP
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
ExecUserScreen	proc
		call	RestoreScreen
		call	ReadKey
		call	SaveScreen
		jmp	DrawScreen
		endp

;------------------------------------------------------------------------------
RestoreAll	proc
		call	RestoreRegs
		jmp	UpdateRegsAndCmds
		endp

RestoreFlags	proc
		mov	di,offset CPURegs.xflags
		mov	ax,ds:[di+byte ptr SaveCPURegs-byte ptr CPURegs]
		mov	ds:[di],ax
		jmp	UpdateRegsAndCmds
		endp

RestoreCSIP	proc
		mov	di,offset CPURegs.xip
		mov	ax,ds:[di+byte ptr SaveCPURegs - byte ptr CPURegs]
		mov	ds:[di],ax
		mov	di,offset CPURegs.xcs
		mov	ax,ds:[di+byte ptr SaveCPURegs - byte ptr CPURegs]
		mov	ds:[di],ax
		jmp	UpdateRegsAndCmds
		endp

;------------------------------------------------------------------------------
UnasmGoTo	proc
		mov	ax,9*256+44
		mov	di,offset MainAddrString
		call	ReadAddress
		jc	@@Quit

		call	PushUnasmPos

		or	ch,ch
		jz	@@NoSeg
		mov	Unasm_seg,dx
		cmp	ch,2
		je	@@NoOfs
@@NoSeg:
		mov	CurIP,ax
@@NoOfs:
		mov	CurLine,0
		call	UpdateCommands
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
ReadAddress	proc
		push	si di bp es
		push	ds
		pop	es
		mov	DAddrStr,di
		mov	si,offset AddrDialog
		mov	[si],ax
		add	ax,31+7*256
		mov	[si+2],ax
		call	InitDialog

	@@Again:
		call	ExecDialog
		jz	@@ErrorExit

		mov	si,DAddrStr
		call	GetAddress
		jc	@@Again

		call	DrawScreen
		clc
		jmp	@@Exit

	@@ErrorExit:
		call	DrawScreen
		stc

	@@Exit:
		pop	es bp di si
		ret
		endp

;GetAddressCS	proc
;		push	CPURegs.xcs
;		jmp	_GetAddress
;		endp
;
;GetAddressDS	proc
;		push	CPURegs.xds
;
;	_GetAddress:
;		call	GetAddress
;		pop	bx

GetAddressDS	proc
		push	ds
		pop	es
		call	GetAddress
		jc	@@Exit

		mov	bx,CPURegs.xds
		cmp	ch,1
		je	@@Exit
		cmp	ch,0
		je	@@DefSeg
		xor	ax,ax
		jmp	@@Exit

	@@DefSeg:
		mov	dx,bx

	@@Exit:
		ret
		endp

GetAddress	proc
		call	GetAddress_
		jnc	@@Exit

		mov	si,offset AddrErrorMsg
		call	_ErrorMessage
		stc

	@@Exit:
		mov	si,offset AddrDialog
		ret
		endp

GetAddress_	proc
		xor	bp,bp
		xor	dx,dx
		xor	cx,cx
		mov	@@op,cl

	@@Next:
		lodsb
		cmp	al,' '
		je	@@Next
		dec	si
		cmp	al,0
		je	@@Done1
		call	GetRegValue
		je	@@DoOper
		call	GetHexValue
		jc	@@Error

	@@DoOper:
		cmp	@@op,'-'
		jne	@@Plus
		neg	bx

	@@Plus:
		add	bp,bx
		mov	@@op,0

	@@Skip:
		lodsb
		cmp	al,' '
		je	@@Skip
		cmp	al,0
		je	@@Done
		cmp	al,':'
		je	@@seg
		cmp	al,'+'
		je	@@p
		cmp	al,'-'
		jne	@@Error

	@@p:
		mov	@@op,al
		jmp	@@Next

	@@seg:
		or	cl,cl
		jne	@@Error
		inc	cx
		mov	dx,bp
		xor	bp,bp
		jmp	@@Next

	@@Done1:
		cmp	@@op,0
		jne	@@Error
		cmp	cx,1
		jne	@@Ok
		inc	cx

	@@Ok:
		clc
		jmp	@@Done

	@@Error:
	@@ErrorExit:
		stc

	@@Done:
		mov	ax,bp
		mov	ch,cl
		ret
@@op		db	0
		endp

GetRegValue	proc
		push	ax cx
		mov	di,offset RReg
		mov	ax,[si]
		xchg	al,ah
		call	UpCase
		xchg	al,ah
		call	UpCase
		mov	cx,16
		repne	scasw
		jne	@@Not
		cmp	ax,'SG'
		je	@@Check386
		cmp	ax,'SF'
		je	@@Check386

	@@GetValue:
		lodsw
		sub	di,offset RReg+2
		shl	di,1
		mov	bx,word ptr CPURegs[di]
		cmp	ax,ax

	@@Not:
		pop	cx ax
		ret

	@@Check386:
		cmp	CPU,3
		jb	@@Not
		jmp	@@GetValue
		endp

GetHexValue	proc
		push	ax cx
		mov	cl,4
		xor	bx,bx
		mov	al,[si]
		call	GetHex
		jc	@@Not
@@Set:
		test	bh,0F0h
		stc
		jnz	@@Not
		shl	bx,cl
		or	bl,al
		inc	si
		mov	al,[si]
		call	GetHex
		jnc	@@Set
		clc
@@Not:
		pop	cx ax
		ret
		endp

GetHex		proc
		call	UpCase
		cmp	al,'0'
		jb	@@Not
		cmp	al,'9'
		jbe	@@Digit
		cmp	al,'A'
		jb	@@Not
		cmp	al,'F'
		ja	@@Not
		sub	al,7
@@Digit:
		sub	al,'0'
		clc
		ret
@@Not:
		stc
		ret
		endp


GetCounter	proc
		mov	si,offset CountString
		call	GetAddress_
		jc	@@Error
		cmp	ch,0
		jne	@@Error
		mov	CountValue,ax
		clc
		ret

	@@Error:
		mov	si,offset CountErrorMsg
		call	_ErrorMessage
		stc
		ret
		endp

;------------------------------------------------------------------------------
SaveScreen	proc
		cmp	UserScreenMode,0
		je	@@Quit
		call	pushr
		cmp	VideoType,1
		jne	@@SkipPalette
		mov	di,PaletteBuffer
		call	GetPalette256

	@@SkipPalette:
		mov	UserScreenMode,0
		call	GetCursor
		mov	UserCursor,dx
		mov	UserCurShape,cx
		mov	ah,0Fh
		int	10h
		mov	UserMode,al
		cmp	al,2
		je	@@ModeOk
		cmp	al,3
		je	@@ModeOk
		mov	ax,83h
		int	10h
@@ModeOk:
		mov	ds,B800h
		xor	si,si
		mov	di,offset UserScreen
		mov	cx,2000
		cld
		rep	movsw
		xor	ax,ax
		mov	ds,ax
		and	byte ptr ds:[487h],7Fh
		call	popr
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
RestoreScreen	proc
		call	pushr
		mov	UserScreenMode,1
		mov	es,B800h
		xor	di,di
		mov	si,offset UserScreen
		mov	cx,2000
		cld
		rep	movsw
		mov	ah,0
		mov	al,UserMode
		cmp	al,2
		je	@@ModeOk
		cmp	al,3
		je	@@ModeOk
		or	al,80h
		int	10h
@@ModeOk:
		cmp	VideoType,1
		jne	@@SkipPalette
		mov	si,PaletteBuffer
		call	SetPalette256

	@@SkipPalette:
		mov	dx,UserCursor
		call	GoToXY
		mov	cx,UserCurShape
		call	SetCursor
		xor	ax,ax
		mov	ds,ax
		and	byte ptr ds:[487h],7Fh

		call	popr
		ret
		endp

;------------------------------------------------------------------------------
;SaveUserInts	proc
;		call	pushr
;		mov	di,offset UserIntTable
;		jmp	SaveInts
;endp

;------------------------------------------------------------------------------
SaveIntTable	proc
		call	pushr
		mov	di,offset InterruptTable
;SaveInts:
		cli
		xor	si,si
		mov	ds,si
		cld
		mov	cx,512
		rep	movsw
		sti
		call	popr
		ret
		endp

;------------------------------------------------------------------------------
;RestoreUserInts	proc
;		call	pushr
;		mov	si,offset UserIntTable
;		jmp	RestoreInts
;endp

;------------------------------------------------------------------------------
RestoreIntTable	proc
		call	pushr
		mov	si,offset InterruptTable
;RestoreInts:
		cli
		xor	di,di
		mov	es,di
		cld
		mov	cx,512
		rep	movsw
		sti
		call	popr
		ret
endp

;------------------------------------------------------------------------------
CreateProgram	proc
		call	SetMyPID
		call	MakeEnvironment
		mov	bp,es
		mov	ah,48h
		mov	bx,0FFFFh
		int	21h
		mov	ah,48h
		int	21h
		mov	UserPID,ax
		mov	SaveDTAseg,ax
		mov	CPURegs.xcs,ax
		mov	CPURegs.xds,ax
		mov	CPURegs.xes,ax
		mov	CPURegs.xss,ax
		mov	CPURegs.xip,100h
		mov	CPURegs.xsp,-2
		mov	es,ax
		push	ax			;
		mov	word ptr es:[0FFFEh],0
		dec	ax
		mov	es,ax
		inc	ax
		mov	es:[1],ax
		mov	si,ds:[2]
		mov	dx,ax
		mov	ah,55h
		int	21h
		pop	es			;
		mov	di,80h
		mov	si,offset MyCmdLine
		lodsb
		stosb
		mov	cl,al
		mov	ch,0
		rep	movsb
		movsb
		mov	cx,100h
		sub	cx,di
		mov	al,0
		rep	stosb
		mov	es:[2Ch],bp
		dec	bp
		mov	ax,es
		mov	es,bp
		mov	es:[1],ax
		push	cs
		pop	es
		call	InitRegisters
		mov	al,MyPort21
		mov	UserPort21,al
		call	SetUserPID
		mov	SaveDTAofs,80h
		jmp	RestoreDTA
		endp

;------------------------------------------------------------------------------
MakeEnvironment	proc
		mov	es,ds:[2Ch]
		xor	di,di
		mov	al,0
		cld
		mov	cx,-1
@@Again:
		repne	scasb
		jne	@@Error
		dec	cx
		scasb
		jne	@@Again
		not	cx
		mov	bx,cx
		add	bx,30
		shr	bx,1
		shr	bx,1
		shr	bx,1
		shr	bx,1
		mov	ah,48h
		int	21h
		jc	@@Error
		push	es
		pop	ds
		mov	es,ax
		xor	di,di
		xor	si,si
		rep	movsb
		mov	ax,1
		stosw
		mov	si,offset EnvName
		push	cs
		pop	ds
@@Next:
		lodsb
		stosb
		cmp	al,0
		jne	@@Next
	@@Error:
		ret
		endp


;------------------------------------------------------------------------------
LoadProgram	proc
		call	SetMyPID
		mov	CmdLineSeg,cs
		mov	FCB1seg,cs
		mov	FCB2seg,cs
		mov	dx,offset FName
		mov	bx,offset EPB
		mov	ax,4B01h
		int	21h
		jc	@@Quit
		mov	ax,UserSS
		mov	CPURegs.xss,ax
		mov	ax,UserSP
		mov	CPURegs.xsp,ax
		mov	ax,UserCS
		mov	CPURegs.xcs,ax
		mov	ax,UserIP
		mov	CPURegs.xip,ax

		mov	ah,51h
		int	21h
		mov	UserPID,bx
		mov	CPURegs.xds,bx
		mov	CPURegs.xes,bx
		mov	es,bx
		mov	al,es:[80h]
		mov	ah,0
		inc	ax
		inc	ax
		mov	di,80h
		add	di,ax
		mov	cx,80h
		sub	cx,ax
		mov	al,0
		cld
		rep	stosb
		push	cs
		pop	es
		mov	FileSpecified,1
		call	InitRegisters
		mov	al,MyPort21
		mov	UserPort21,al
	@@Quit:
		ret
		endp

EPB:
EnvSeg		dw	0
CmdLineOfs	dw	MyCmdLine
CmdLineSeg	dw	0
FCB1ofs		dw	MyFCB1
FCB1seg		dw	0
FCB2ofs		dw	MyFCB2
FCB2seg		dw	0

UserSP		dw	0
UserSS		dw	0
UserIP		dw	0
UserCS		dw	0

;MyCmdLine	db	0, 0Dh
MyFCB1:
MyFCB2:
		db	0, 11 dup (' ')

;------------------------------------------------------------------------------
InitRegisters	proc
		xor	ax,ax
		mov	CPURegs.xax,ax
		mov	CPURegs.xbx,ax
		mov	CPURegs.xcx,00FFh
		mov	ax,CPURegs.xds
		mov	CPURegs.xdx,ax
		mov	ax,CPURegs.xip
		mov	CPURegs.xsi,ax
		mov	ax,CPURegs.xsp
		mov	CPURegs.xdi,ax
		mov	CPURegs.xbp,91Ch
		mov	CPURegs.xflags,07202h
		jmp	SaveRegs
		endp

;------------------------------------------------------------------------------
ReadCmdLine	proc
		call	pushr
		cmp	FileSpecified,0
		jne	@@1
		mov	si,offset NoSpecifMsg
		call	_ErrorMessage
		jmp	@@Quit
@@1:
		mov	si,offset ArgDialog
		call	InitDialog
		call	ExecDialog
		call	DrawScreen
		or	ax,ax
		jz	@@Quit
		call	SetCommandLine
		call	popr
		jmp	ResetProgram
@@Quit:
		call	popr
		ret
endp

;------------------------------------------------------------------------------
SetCommandLine	proc
		call	pushr
		mov	si,offset CmdLineString
		call	StrLen
		mov	di,offset MyCmdLine
		mov	al,cl
		stosb
		rep	movsb
		mov	al,13
		stosb
		call	popr
		ret
		endp


;------------------------------------------------------------------------------
LoadFile	proc
		mov	dx,offset FMask
		call	InitFileList
		mov	si,offset LoadDialog
		call	InitDialog
		call	ExecDialog
		call	DrawScreen

		mov	si,StringPtr
		call	_CheckSharedArea

		or	ax,ax
		je	@@Quit
		call	FullProgName
		mov	FileSpecified,1
		jmp	ResetProgram

	@@Quit:
		ret
		endp

FullProgName	proc
		mov	si,offset FName
		mov	di,offset FName
		mov	ah,60h
		int	21h
		ret
		endp

InitFileList	proc
		call	pushr
		call	SaveDTA
		call	SetMyPID
		call	SetMyDTA
		mov	ax,16
		call	InitStringList
		mov	ah,4Eh
		mov	cx,27h
@@FindNext:
		int	21h
		jc	@@End
		mov	si,9Eh
		call	AddString
		mov	ah,4Fh
		jmp	@@FindNext
@@End:
		call	SetUserPID
		call	RestoreDTA
		call	popr
		ret
		endp

;------------------------------------------------------------------------------
ResetProgram	proc
		call	pushr
		cmp	FileSpecified,0
		je	@@NoSpec
		call	UnloadProgram
		call	LoadProgram
		jnc	@@Next
		mov	si,offset LoadErrorMsg
		call	_ErrorMessage
		call	CreateProgram
		mov	FileSpecified,0
@@Next:
ProgramCreated:
		call	SetUserPID
		call	SetTermHandler
		call	UpdateScreen
@@Quit:
		call	popr
		ret
@@NoSpec:
		mov	si,offset NoSpecifMsg
		call	_ErrorMessage
		jmp	@@Quit
		endp

;------------------------------------------------------------------------------
NewProgram	proc
		call	pushr
		mov	byte ptr CmdLineString,0
		mov	word ptr MyCmdLine,0D00h
		call	UnloadProgram
		call	ClearAllBP
		call	CreateProgram
		mov	ax,CPURegs.xip
		mov	CurIP,ax
		mov	ax,CPURegs.xcs
		mov	Unasm_seg,ax
		mov	CurLine,0
		mov	FileSpecified,0
		jmp	ProgramCreated
endp

;------------------------------------------------------------------------------
SetUserPID	proc
		mov	ah,50h
		mov	bx,UserPID
		int	21h
		ret
		endp

;------------------------------------------------------------------------------
SetMyPID	proc
		mov	ah,50h
		mov	bx,cs
		int	21h
		ret
		endp

;------------------------------------------------------------------------------
SaveDTA		proc
		push	bx es
		mov	ah,2Fh
		int	21h
		mov	SaveDTAseg,es
		mov	SaveDTAofs,bx
		pop	es bx
		ret
		endp

;------------------------------------------------------------------------------
SetMyDTA	proc
		push	dx
		push	cs
		pop	ds
		mov	dx,80h
		mov	ah,1Ah
		int	21h
		pop	dx
		ret
		endp

;------------------------------------------------------------------------------
RestoreDTA	proc
		push	ds
		mov	ds,SaveDTAseg
		mov	dx,SaveDTAofs
		int	21h
		pop	ds
		ret
endp

;------------------------------------------------------------------------------
UnloadProgram	proc
		call	SetUserPID
		mov	es,UserPID
		mov	es:[0Ch],cs
		mov	ax,offset @@1
		mov	es:[0Ah],ax
		pushf
		push	cs
		push	ax
		push	es ds bp di si dx cx bx ax
		mov	ds:[2Eh],sp
		mov	ds:[30h],ss
		mov	ax,4C00h
		int	21h
@@1:
		push	cs
		pop	ds
		push	cs
		pop	es
		ret
		endp

;------------------------------------------------------------------------------
SetTermHandler	proc
		mov	es,UserPID
		mov	es:[0Ch],cs
		mov	es:[0Ah],offset TermHandler

;		mov	ds:[2Eh],StackPtr-24
		xor	cx,cx
		call	CalcProgSize
		sub	ax,24
		mov	ds:[2Eh],ax

		mov	ds:[30h],ss
		push	cs
		pop	es
		ret
		endp

;------------------------------------------------------------------------------
TermHandler	proc
;		mov	sp,StackPtr

		mov	ax,cs
		mov	ds,ax
		mov	es,ax

		xor	cx,cx
		call	CalcProgSize
		mov	sp,ax

		call	Restore01vector
		call	Restore03vector
;		call	SaveUserInts
;		call	RestoreIntTable
		call	ClearBreakPoints
		call	SaveScreen
		call	CreateProgram
;		call	SetUserPID
		call	SetTermHandler
;		mov	CurLine,0
		call	DrawScreen
		mov	si,offset TerminateMsg
		call	_MsgBox
		jmp	MainKeyLoop
endp

;------------------------------------------------------------------------------
DrawCPUwindow	proc
		call	pushr
		mov	cx,2020h
		call	SetCursor
		mov	es,B800h
		xor	di,di
		cld
		mov	cx,2000
		xor	ax,ax
		rep	stosw

		mov	di,CPUstart
		mov	al,'Í'
		mov	ah,atCPUborder
		mov	cx,CPUwidth
		rep	stosw
		mov	al,'¸'
		stosw
		mov	al,'³'
		mov	cx,CPUheight
NextBordLine:
		add	di,158
		stosw
		loop	NextBordLine

		mov	di,CPUstart+(CPUheight+1)*160
		mov	al,'Ä'
		mov	cx,CPUwidth
		rep	stosw
		mov	al,'Á'
		stosw
		mov	cx,79-CPUwidth
		mov	al,'Ä'
		rep	stosw

		mov	bx,FLAG16start
		cmp	RegsMode,1
		jne	@@Skip32
		mov	bx,FLAG32start

	@@Skip32:
		lea	di,ds:[bx-160-2]

		mov	al,'Ã'
		stosw
		mov	cx,79-CPUwidth
		mov	al,'Ä'
		rep	stosw

		lea	di, ds:[bx+2*160-2]

		mov	al,'Ã'
		stosw
		mov	cx,79-CPUwidth
		mov	al,'Ä'
		rep	stosw


;flags names
		mov	ah,atRegName
		lea	di,ds:[bx+2]
		mov	si,offset FlagsStr
		call	FillStr

		mov	ax,40-TitleLen/2
		mov	bh,atTitle
		mov	si,offset MainTitle
		call	WriteString

		mov	ax,CPUstart+5
		mov	bh,atCPUName
		mov	si,CPUName
		call	WriteString

		cmp	RegsMode,1
		je	@@Regs32
		call	DrawRegisters16

	@@Exit:

		mov	si,offset FlagsMsg
		call	FillStr

		call	popr
		ret

	@@Regs32:
		call	DrawRegisters32
		jmp	@@Exit
		endp


DrawRegisters16	proc
		mov	di,REGstart
		mov	si,offset RegNames
		mov	ah,atRegName
		mov	cx,4

	@@NextRegLine:
		push	cx
		mov	cx,3

	@@NextRegCol:
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		add	di,12
		loop	@@NextRegCol

		add	di,160-18*3
		pop	cx
		loop	@@NextRegLine

		add	di,160
		mov	al,'I'
		stosw
		mov	al,'P'
		stosw
		mov	al,'='
		stosw
		add	di,24
		ret
		endp

DrawRegisters32	proc
		mov	di,REGstart
		mov	si,offset RReg
		mov	ah,atRegName
		mov	cx,6

	@@NextReg32:
		call	@@DisplayName32
		loop	@@NextReg32

		add	di,160
		call	@@DisplayName32
		call	@@DisplayName32

		mov	di,REGstart+2*18
		mov	cx,6

	@@NextReg16:
		call	@@DisplayName16
		loop	@@NextReg16

		add	di,160
		call	@@DisplayName16

		sub	di,3*2
		ret

	@@DisplayName16:
		push	di
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		jmp	@@DisplayExit

	@@DisplayName32:
		push	di
		mov	al,'E'
		stosw
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		add	di,4*2
		mov	al,':'
	@@DisplayExit:
		stosw
		pop	di
		add	di,160
		ret
		endp

;------------------------------------------------------------------------------
UpdateWatchLabel	proc
		call	pushr

		mov	es,B800h
		cld
		mov	di,WATCHstart

		cmp	DataWatchProc,0
		je	@@DrawLine

		mov	si,DataWatchLabel
		mov	cx,5
		mov	ax,atDataWatchMem*256 + ' '
		stosw

	@@Next2:
		lodsb
		stosw
		loop	@@Next2

		mov	al,' '
		stosw

	@@Exit:
		call	popr
		ret

	@@DrawLine:
		mov	ax,atCPUborder*256 + 'Ä'
		mov	cx,7
		rep	stosw
		jmp	@@Exit
		endp


;------------------------------------------------------------------------------
FillStr		proc
@@Next:
		lodsb
		cmp	al,0
		je	@@Quit
		stosw
		jmp	@@Next
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
UpdateRegisters	proc
		call	pushr
		mov	es,B800h

		mov	ax,offset @@DisplayRegs16
		cmp	RegsMode,1
		jne	@@Skip32
		mov	ax,offset @@DisplayRegs32

	@@Skip32:
		call	ax

		mov	bx,CPURegs.xflags
		mov	bp,SaveCPURegs.xflags
		mov	si,offset FlagStruc
		mov	cx,NumFlags

	@@NextFlag:
		push	cx

		lodsb
		mov	cl,al
		xor	dx,dx
		shl	bp,cl
		adc	dl,0
		mov	ax,(atFlagValue shl 8) + '0'
		shl	bx,cl
		adc	al,0
		xor	dx,ax
		test	dl,1
		jz	@@DisplayFlag
		mov	ah,atFlagNewValue

	@@DisplayFlag:
		stosw
		add	di,4
		pop	cx
		loop	@@NextFlag

		call	popr
		ret

	@@DisplayRegs16:
		mov	si,offset Reg16PosTable
		mov	cx,Num16Registers

	@@NextReg16:
		call	@@DisplayReg
		loop	@@NextReg16

		mov	di,FLAG16start+160+4
		ret

	@@DisplayRegs32:
		mov	si,offset Reg32PosTable
		mov	cx,Num32Registers

	@@NextReg32:
		call	@@DisplayReg
		loop	@@NextReg32

		mov	di,FLAG32start+160+4
		ret


	@@DisplayReg:
		xor	dx,dx
		mov	dl,[si].x
		shl	dx,1
		mov	al,[si].y
		mov	bl,160
		mul	bl
		add	dx,ax
		mov	di,[si].RegLink

		mov	ax,ds:[di]

		mov	bh,atRegNewValue
		cmp	ds:[di+byte ptr SaveCPURegs - byte ptr CPURegs],ax
		jne	@@Fill
		mov	bh,atRegValue
		cmp	RegsMode,1
		jne	@@Fill
		call	AlignReg
		push	ax
.386
		mov	eax,ds:[di]
		cmp	dword ptr ds:[di+byte ptr SaveCPURegs - byte ptr CPURegs],eax
.8086
		pop	ax
		je	@@Fill
		mov	bh,atRegNewValue


	@@Fill:
		mov	di,dx
		call	FillWord
		add	si,size RegPosition
		ret
		endp

AlignReg	proc
		lea	di,ds:[di-(CPURegs-Start+100h)]
		and	di,11111100b
		lea	di,ds:[di+offset CPURegs]
		ret
		endp

;------------------------------------------------------------------------------
UpdateCommands	proc
		call	pushr
		mov	CurLineBrk,0
		mov	CurLineIP,0
		mov	CurIPline,-1
		mov	si,CurIP
		mov	di,CMDstart
		mov	es,B800h
		mov	cx,CPUheight
		xor	bp,bp

	NextCPUline:
		xor	dx,dx
		mov	bx,Unasm_seg
		cmp	bx,CPURegs.xcs
		jne	@@1
		cmp	si,CPURegs.xip
		jne	@@1
		inc	dx
		mov	CurIPline,bp
	@@1:
		call	CheckBP
		jc	@@8
		mov	dh,1
		je	@@8
		mov	dh,2
	@@8:
		mov	bx,si
		call	UnAssemble
		mov	ax,offset CmdNormal
		cmp	dh,1
		jb	@@A
		mov	ax,offset CmdBreakpoint
		jz	@@A
		mov	ax,offset CmdBrkDisabled
	@@A:
		or	dl,dl
		jz	@@2
		mov	word ptr Buffer+4,1011h
	@@2:
		cmp	bp,CurLine
		jne	@@Display
		cmp	Focus,1
		jne	@@E
		cmp	ax,offset CmdBreakpoint
		mov	ax,offset CmdSelected
		jb	@@E
		mov	CurLineBrk,1
		mov	ax,offset CmdBrkSelected
		je	@@E
		mov	ax,offset CmdBrkDisSel
	@@E:
		mov	CurLineIP,bx
		call	UpdateCmdMem

	@@Display:
		call	GetRefAddresses
		call	SetDirection
		call	DisplayCPUline


		add	di,160
		inc	bp

		call	DisplayCodeMarks

		loop	NextCPUline

		mov	ah,LightCyan
		mov	bh,ah
		add	di,10
		mov	al,' '
		stosw
		mov	ax,Unasm_seg
		call	FillWord
		mov	ah,bh
		mov	al,':'
		stosw
		mov	ax,CurLineIP
		call	FillWord
		mov	ah,bh
		mov	al,' '
		stosw

		call	popr
		ret
		endp

DisplayCPUline	proc
		call	pushr
		xchg	ax,bp
		mov	si,offset Buffer
		mov	dx,CurX
		mov	cx,CPUwidth-1

		mov	ah,ss:[bp]
		mov	bx,CPUAddrSize
		call	@@Display

		mov	ah,ss:[bp+1]
		mov	bx,CPUOpcodeSize
		call	@@Display

		call	@@DisplayCommand

		call	popr
		ret

	@@Display:
		xchg	bx,cx
	@@NextChar:
		lodsb
		or	dx,dx
		jz	@@WriteChar
		dec	dx
		jmp	@@UpdateCX

	@@WriteChar:
		stosw
		dec	bx

	@@UpdateCX:
		loop	@@NextChar
		xchg	bx,cx
		ret

	@@DisplayCommand:
		lodsb

		mov	ah,ss:[bp+4]
		cmp	al,18h
		je	@@Write
		cmp	al,19h
		je	@@Write

		mov	ah,ss:[bp+5]
		cmp	al,1Eh
		je	@@DisabledArrow
		cmp	al,1Fh
		je	@@DisabledArrow

		mov	ah,ss:[bp+2]
		cmp	al,'0'
		jb	@@Write
		cmp	al,'9'
		jbe	@@Digit
		cmp	al,'A'
		jb	@@Write
		cmp	al,'F'
		ja	@@Write
	@@Digit:
		mov	ah,ss:[bp+3]

	@@Write:
		or	dx,dx
		jz	@@Stosw
		dec	dx
		jmp	@@DisplayCommand
	@@Stosw:
		stosw
		loop	@@DisplayCommand
		ret

	@@DisabledArrow:
		sub	al,1Eh-18h
		jmp	@@Write

		endp

UpdateCmdMem	proc
		call	pushr
		mov	es,B800h
		mov	di,MEMstart
		mov	al,'Ä'
		mov	ah,atCPUborder
		mov	cx,21
		rep	stosw
		cmp	MemoryFlag,0
		je	@@Exit
		call	CalcMemAddress
		call	DispMemAddr
		mov	bl,MemoryFlag
		mov	bh,0
		dec	bx
		cmp	bx,2
		ja	@@Exit
		mov	ds,dx
		add	bx,bx
		call	cs:DisplayMemDisp[bx]
@@Exit:
		call	popr
		ret

CalcMemDisp	dw	Calc_BXSI
		dw	Calc_BXDI
		dw	Calc_BPSI
		dw	Calc_BPDI
		dw	Calc_SI
		dw	Calc_DI
		dw	Calc_BP
		dw	Calc_BX
		dw	Calc_Mem

Calc_BXSI:	mov	ax,CPURegs.xbx
		add	ax,CPURegs.xsi
		ret

Calc_BXDI:	mov	ax,CPURegs.xbx
		add	ax,CPURegs.xdi
		ret

Calc_BPSI:	mov	ax,CPURegs.xbp
		add	ax,CPURegs.xsi
@@SS:		mov	dx,CPURegs.xss
		mov	cl,'S'
		ret

Calc_BPDI:	mov	ax,CPURegs.xbp
		add	ax,CPURegs.xdi
		jmp	@@SS

Calc_SI:	mov	ax,CPURegs.xsi
		ret

Calc_DI:	mov	ax,CPURegs.xdi
		ret

Calc_BX:	mov	ax,CPURegs.xbx
		ret

Calc_BP:	mov	ax,CPURegs.xbp
		jmp	@@SS

Calc_mem:	xor	ax,ax
		ret

CalcMemAddress	proc
		mov	bl,MemDispType
		mov	bh,0
		shl	bx,1
		mov	dx,CPURegs.xds
		mov	cl,'D'
		call	CalcMemDisp[bx]
		add	ax,MemDispOfs
		mov	si,ax
		mov	ax,MemPrefix
		or	ax,ax
		jz	@@Exit

		cmp	ax,offset pfs
		jb	@@SetPrefix
		cmp	CPU,3
		jb	@@Exit
		cmp	CPUtype,a386
		jb	@@Exit

	@@SetPrefix:
		mov     bx,ax
		mov	cl,ds:[bx]
		sub	cl,'a'-'A'
		add	bx,CPURegs.xcs - pcs
		mov	dx,ds:[bx]
	@@Exit:
		ret
		endp

DisplayMemDisp	dw	Disp_Byte
		dw	Disp_Word
		dw	Disp_DWord
Disp_Byte:
		mov	bh,ah
		lodsb
		call	FillByte
		jmp	@@Rest
Disp_Word:
		mov	bh,ah
		cmp	si,-1
		jne	@@lodsw
		lodsb
		mov	ah,al
		lodsb
@@lodsw:
		lodsw
		call	FillWord
		jmp	@@Rest
Disp_DWord:
		mov	bh,ah
		lodsw
		xchg	ax,cx
		lodsw
		call	FillWord
		mov	ah,bh
		mov	al,':'
		stosw
		xchg	ax,cx
		call	FillWord
@@Rest:
		mov	ah,bh
		mov	al,' '
		stosb
		ret
endp

DispMemAddr	proc
		mov	es,B800h
		mov	di,MEMstart
		mov	ah,atMemAddr
		mov	al,' '
		stosw
		mov	al,cl
		stosw
		mov	al,'S'
		stosw
		mov	al,':'
		stosw
		mov	bh,ah
		mov	ax,si
		call	FillWord
		mov	ah,bh
		mov	al,' '
		stosw
		mov	al,'='
		stosw
		mov	al,' '
		stosw
		mov	ah,atMemValue
		ret
endp

;------------------------------------------------------------------------------
_UpdateDump	proc
		cmp	DataWatchProc,0
		je	UpdateDump
		jmp	word ptr DataWatchProc

UpdateDump:
		call	pushr
		mov	si,DumpOffs
		mov	es,B800h

		mov	ax,si
		mov	ah,atDumpAddr
		mov	di,DUMPstart-160+24
		mov	cx,16
@@Next:
		and	al,0Fh
		call	FillHexDigit
		scasw
		scasw
		inc	ax
		loop	@@Next

		mov	ds,DumpSeg
		mov	di,DUMPstart
		mov	cx,DUMPheight
		sub	dl,16
		neg	dl

		push	dx

@@NextRow:
		mov	bh,atDumpAddr
		mov	ax,ds
		call	FillWord
		mov	al,':'
		mov	ah,bh
		stosw
		mov	ax,si
		call	FillWord
		scasw
		scasw
		mov	bl,atDumpValue
		mov	ah,atDumpCursor
;		mov	ah,bh
		push	cx
		mov	cx,16
@@NextCol:
		mov	bh,bl
		cmp	cs:DumpEditMode,0
		je	@@NotCur1
		cmp	dh,0
		jne	@@NotCur1
		cmp	dl,cl
		jne	@@NotCur1
		mov	bh,ah
@@NotCur1:
		lodsb
		call	FillByte
		scasw
		loop	@@NextCol

		pop	cx
		dec	dh
		add	di,42
		loop	@@NextRow

		pop	dx
		mov	bh,atDumpValue
		mov	bl,atDumpCursor

		mov	si,cs:DumpOffs
		mov	di,DUMPstart+122
		mov	cx,DUMPheight
@@NextRow1:
		push	cx
		mov	cx,16
@@NextCol1:
		mov	ah,bh
		cmp	cs:DumpEditMode,0
		je	@@NotCur2
		cmp	dh,0
		jne	@@NotCur2
		cmp	dl,cl
		jne	@@NotCur2
		mov	ah,bl
@@NotCur2:
		lodsb
;		cmp	al,0FFh
;		je	@@2
		cmp	al,0
		jne	@@1
@@2:
		mov	al,'.'
@@1:
		stosw
		loop	@@NextCol1
		jmp	@@3
		jmp	@@1
@@3:
		pop	cx
		dec	dh
		add	di,128
		loop	@@NextRow1

		call	popr
		ret
		endp

;------------------------------------------------------------------------------
UpdateStack	proc
		call	pushr

		mov	ax,STK16x+STK16y*256
		mov	cx,STK16len
		mov	dx,STK16x+(STK16y+STK16len-1)*256
		mov	di,STK16start
		cmp	RegsMode,1
		jne	@@SkipStack32
		mov	ax,STK32x+STK32y*256
		mov	cx,STK32len
		mov	dx,STK32x+(STK32y+STK32len-1)*256
		mov	di,STK32start

	@@SkipStack32:

		mov	si,offset StackMsg
		mov	bh,atStackName
		call	WriteString
		mov	ax,dx
		call	WriteString
		mov	es,B800h
		mov	si,CPURegs.xsp
		mov	ds,CPURegs.xss

@@Next:
		mov	ah,atStackName
		mov	al,'S'
		stosw
		stosw
		mov	al,':'
		stosw
		mov	bh,ah
		mov	ax,si
		call	FillWord
		scasw
		scasw
		mov	bh,atStackValue
		lodsw
		call	FillWord
		sub	di,186
		loop	@@Next
		call	popr
		ret
		endp


UpdateRegsColor	proc
		cmp	RegsSaved,1
		je	@@Exit
		inc	RegsSaved
		call	SaveRegs
		call	UpdateRegisters

	@@Exit:
		ret
		endp

DisplayFocus	proc
		inc	Focus
		jmp	UpdateCommands
		endp

HideFocus	proc
		dec	Focus
		jmp	UpdateCommands
		endp

EditRegisters	proc
;		push
		call	HideFocus
		call	UpdateRegsColor
		mov	cx,000Dh
		call	SetCursor

	@@StartEdit:
		mov	si,offset Reg16PosTable
		cmp	RegsMode,1
		jne	@@NextZ
		mov	si,offset Reg32PosTable
	@@NextZ:
		xor	dx,dx

	@@Next:
		push	dx
		add	dx,[si]
		call	GoToXY
		pop	dx

	@@ReadKey:
		call	ReadKey

		mov	di,offset @@Actions - 3
		call	ActionShortJump
		jc	@@Other
		add	di,offset @@FirstAction
		jmp	di

	@@FirstAction:
	@@Other:
		cmp	al,'0'
		jb	@@Next

		cmp	al,'9'
		jbe	@@Digit
		call	UpCase
		cmp	al,'A'
		jb	@@Next
		cmp	al,'F'
		ja	@@Next
		sub	al,7
	@@Digit:
		sub	al,'0'
		call	SetHalfByte
		inc	dl
		cmp	dl,4
		jae	@@Move

	@@Update:
		call	UpdateRegsAndCmds
		jmp	@@Next

	@@Move:
		mov	si,[si].LRight
		xor	dx,dx
		jmp	@@Update

	@@MoveBack:
		mov	si,[si].LLeft
		mov	dl,0
		jmp	@@Next

	@@Right:
		inc	dl
		cmp	dl,3
		ja	@@Move
		jmp	@@Next

	@@Left:
		or	dl,dl
		jnz	@@Dec
		mov	si,[si].LLeft
		mov	dl,3
		jmp	@@Next

	@@Dec:
		dec	dl
		jmp	@@Next

	@@Up:
		mov	si,[si].LUp
		jmp	@@Next

	@@Down:
		mov	si,[si].LDown
		jmp	@@Next

	@@LocalMenu:
		push	si
		mov	si,offset RegsLocalStruc
		call	_LocalMenu
		pop	si
		jc	@@Next
		jmp	ax

	_SwitchRegMode:
		call	SwitchRegMode
		jmp	@@StartEdit

	_RestoreAll:
		push	si
		call	RestoreAll
		pop	si
		jmp	@@Next

	_RestoreFlags:
		call	RestoreFlags
		jmp	@@Next

	_RestoreCSIP:
		call	RestoreFlags
		jmp	@@Next

	IncReg:
		mov	di,[si].RegLink
		cmp	RegsMode,1
		je	@@Inc32
	@@Inc16:
		inc	word ptr ds:[di]
		jmp	@@Update
	@@Inc32:
		cmp	di,offset CPURegs.xcs
		jae	@@Inc16
		call	AlignReg
.386
		inc	dword ptr ds:[di]
.8086
		jmp	@@Update

	DecReg:
		mov	di,[si].RegLink
		cmp	RegsMode,1
		je	@@Dec32
	@@Dec16:
		dec	word ptr ds:[di]
		jmp	@@Update
	@@Dec32:
		cmp	di,offset CPURegs.xcs
		jae	@@Dec16
		call	AlignReg
.386
		dec	dword ptr ds:[di]
.8086
		jmp	@@Update

	ZeroReg:
		mov	di,[si].RegLink
		call	AlignReg
		mov	word ptr ds:[di],0
		mov	word ptr ds:[di+2],0
		jmp	@@Update

	RestoreReg:
		mov	di,[si].RegLink
		call	AlignReg
		mov	ax,ds:[di+byte ptr SaveCPURegs - byte ptr CPURegs]
		mov	ds:[di],ax
		mov	ax,ds:[di+2+byte ptr SaveCPURegs - byte ptr CPURegs]
		mov	ds:[di+2],ax
		jmp	@@Update

	@@Quit:
		mov	cx,2020h
		call	SetCursor
		call	UpdateScreen
		jmp	DisplayFocus
;		pop

@@Actions	dw	kbEsc
		db	@@Quit		- @@FirstAction
		dw	kbEnter
		db	@@Quit		- @@FirstAction
		dw	kbTab
		db	@@Move		- @@FirstAction
		dw	kbShiftTab
		db	@@MoveBack	- @@FirstAction
		dw	kbRight
		db	@@Right		- @@FirstAction
		dw	kbLeft
		db	@@Left		- @@FirstAction
		dw	kbUp
		db	@@Up		- @@FirstAction
		dw	kbDown
		db	@@Down		- @@FirstAction
		dw	kbCtrlA
		db	_RestoreAll	- @@FirstAction
		dw	kbCtrlO
		db	RestoreReg	- @@FirstAction
		dw	kbCtrlT
		db	_RestoreFlags	- @@FirstAction
		dw	kbCtrlC
		db	_RestoreCSIP	- @@FirstAction
		dw	kbCtrlR
		db	_SwitchRegMode	- @@FirstAction
		dw	kbCtrlI
		db	IncReg		- @@FirstAction
		dw	kbCtrlD
		db	DecReg		- @@FirstAction
		dw	kbCtrlZ
		db	ZeroReg		- @@FirstAction
		dw	kbAltF10
		db	@@LocalMenu	- @@FirstAction
		dw	0
endp

SetHalfByte	proc
		mov	di,[si].RegLink
		mov	bx,0FFF0h
		mov	cl,3
		sub	cl,dl
		shl	cl,1
		shl	cl,1
		rol	bx,cl
		and	bx,[di]
		mov	ah,0
		shl	ax,cl
		or	ax,bx
		mov	[di],ax
		ret
endp

;------------------------------------------------------------------------------
EditFlags	proc
		call	HideFocus
		call	UpdateRegsColor
		mov	cx,000Dh
		call	SetCursor
		xor	bx,bx
@@Next:

		mov	dx,FLAG16x+2+(FLAG16y+1)*256
		cmp	RegsMode,1
		jne	@@Skip32
		mov	dx,FLAG32x+2+(FLAG32y+1)*256
	@@Skip32:
		add	dl,bl
		add	dl,bl
		add	dl,bl
		call	GoToXY
		call	ReadKey

		mov	di,offset @@Actions - 3
		call	ActionShortJump
		jc	@@Next
		add	di,offset @@FirstAction
		jmp	di

	@@FirstAction:
	@@Clear:
		mov	cl,RegShift[bx]
		mov	ax,0FFFEh
		rol	ax,cl
		and	CPURegs.xflags,ax
		jmp	@@Update

	@@Set:
		mov	cl,RegShift[bx]
		mov	ax,1
		shl	ax,cl
		or	CPURegs.xflags,ax
		jmp	@@Update

	@@LocalMenu:
		mov	si,offset FlagsLocalStruc
		call	_LocalMenu
		jc	@@Next
		call	ax
		jmp	@@Update

	ToggleFlag:
		jmp	@@ToggleFlag

	@@Toggle:
		call	@@ToggleFlag
	@@Update:
		call	UpdateRegsAndCmds
		jmp	@@Next

	@@ToggleFlag:
		mov	cl,RegShift[bx]
		mov	ax,1
		shl	ax,cl
		xor	CPURegs.xflags,ax
		ret


	@@Right:
		inc	bl
		cmp	bl,NumFlags
		jb	@@Next
		mov	bl,0
		jmp	@@Next

	@@Left:
		cmp	bl,0
		ja	@@Dec
		mov	bl,NumFlags-1
		jmp	@@Next

	@@Dec:
		dec	bx
		jmp	@@Next

	@@RestoreFlags:
		call	RestoreFlags
		jmp	@@Next

	@@Quit:
		mov	cx,2020h
		call	SetCursor
		jmp	DisplayFocus

@@Actions	dw	kbEsc
		db	@@Quit		- @@FirstAction
		dw	kbEnter
		db	@@Quit		- @@FirstAction
		dw	kbRight
		db	@@Right		- @@FirstAction
		dw	kbLeft
		db	@@Left		- @@FirstAction
		dw	0B00h+'0'
		db	@@Clear		- @@FirstAction
		dw	0200h+'1'
		db	@@Set		- @@FirstAction
		dw	3900h+' '
		db	@@Toggle	- @@FirstAction
		dw	kbCtrlT
		db	@@RestoreFlags	- @@FirstAction
		dw	kbAltF10
		db	@@LocalMenu	- @@FirstAction
		dw	0

endp

;------------------------------------------------------------------------------
EditDump	proc
		mov	DumpEditMode,1
		call	HideFocus

		push	es

	@@NextR:
		mov	si,DumpOffs
		mov	es,DumpSeg
		xor	bx,bx
		xor	dx,dx
		xor	bp,bp

		call	UpdateDump

	@@Next:
		mov	cx,000Dh
		call	SetCursor
		push	dx
		or	bp,bp
		jz	@@HexCur
		add	dl,DUMPx+11+48+2
		jmp	@@SetCur

	@@HexCur:
		mov	al,dl
		add	dl,al
		add	dl,al
		add	dl,bl
		add	dl,DUMPx+11
	@@SetCur:
		add	dh,DUMPy
		call	GoToXY
		pop	dx

	@@ReadKey:
		call	ReadKey

		mov	di,offset @@Actions - 3
		call	ActionShortJump
		jc	@@NotFound
		add	di,offset @@FirstAction
		mov	ax,offset DumpCSIP
		jmp	di

	@@NotFound:
		or	bp,bp
		jnz	@@FillChar
;		call	Beep
		call	UpCase
		cmp	al,'0'
		jb	@@Next
		cmp	al,'9'
		jbe	@@Digit
		cmp	al,'A'
		jb	@@Next
		cmp	al,'F'
		ja	@@Next
		sub	al,7

	@@Digit:
		sub	al,'0'
		mov	cl,bl
		xor	cl,1
		shl	cl,1
		shl	cl,1
		mov	ah,0F0h
		rol	ah,cl
		and	ah,es:[si]
		shl	al,cl
		or	ah,al
		mov	es:[si],ah
		cmp	es:[si],ah
		je	@@Right

	@@NotRAM:
		push	si
		mov	si,offset DumpNotInRAM
		call	_ErrorMessage
		pop	si
		jmp	@@Update

	@@FirstAction:
	@@DumpFollow:
		push	si
		mov	si,offset FollowMenuStruc
		jmp	@@ExecMenu

	@@Block:
		push	si
		mov	si,offset BlockMenuStruc
		jmp	@@ExecMenu

	@@LocalMenu:
		push	si
		mov	si,offset DumpMenuStruc

	@@ExecMenu:
		call	_LocalMenu
		pop	si
		jc	@@JumpToNext

	@@CallAction:
		call	pushr
		call	ax
		call	popr
		jmp	@@SelectNextJump

	@@DumpGoTo:
		call	DumpGoTo

	@@SelectNextJump:
		jc	@@JumpToNext

	@@JumpToNextR:
		jmp	@@NextR

	@@FillChar:
		cmp	al,0
		je	@@JumpToNext
		mov	es:[si],al
		cmp	es:[si],al
		jne	@@NotRAM
	@@Right:
		or	bp,bp
		jnz	@@CM1
		inc	bl
		cmp	bl,1
		jbe	@@Update
		mov	bl,0
	@@CM1:
		inc	si
		inc	dl
		cmp	dl,15
		ja	@@NextLine
	@@Update:
		call	UpdateDump
		call	UpdateCommands
		call	UpdateStack

	@@JumpToNext:
		jmp	@@Next

	@@DumpReference:
		mov	ax,offset DumpReference
		jmp	@@CallAction

	@@DumpCurLine:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpESDI:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpDSSI:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpESBX:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpSSBP:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpDXAX:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpDSDX:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpESSI:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpDSDI:
		sub	ax,UpdateDumpWindow-DumpCSIP

	@@DumpCSIP:
		jmp	@@CallAction

	@@Search:
		mov	ax,offset DataSearch
		jmp	@@CallAction

	@@SearchNext:
		mov	ax,offset DataSearchNext
		jmp	@@CallAction

	@@NextLine:
		mov	dl,0
	@@Down1:
		inc	dh
		cmp	dh,4
		jbe	@@Update
		add	DumpOffs,16
		mov	dh,4
		jmp	@@Update
	@@Down:
		add	si,16
		jmp	@@Down1
	@@Left:
		or	bp,bp
		jnz	@@CM2
		dec	bl
		jz	@@Update
		mov	bl,1
	@@CM2:
		dec	si
		dec	dl			;dl always >=0 before dec
		jns	@@Update
		mov	dl,15
	@@Up1:
		dec	dh
		jns	@@Update		;dh always >=0 before dec
		mov	dh,0
		sub	DumpOffs,16
		jmp	@@Update
	@@Up:
		sub	si,16
		jmp	@@Up1
	@@PageUp:
		sub	DumpOffs,16*5
		sub	si,16*5
		jmp	@@Update
	@@PageDown:
		add	DumpOffs,16*5
		add	si,16*5
		jmp	@@Update

	@@Quit:
		mov	cx,2020h
		call	SetCursor
		mov	DumpEditMode,0
		call	UpdateDump
		call	DisplayFocus
		pop	es
		ret

	@@ByteRight:
		inc	DumpOffs
		inc	si
		jmp	@@Update

	@@ByteLeft:
		dec	DumpOffs
		dec	si
		jmp	@@Update

	@@Switch:
		or	bp,bp
		jz	@@Hex2Char
		xor	bx,bx
		xor	bp,bp
		jmp	@@Next

	@@Hex2Char:
		inc	bp
		jmp	@@Next

	FollowDataSeg:
		mov	ax,es:[si]
		mov	DumpSeg,ax
		xor	ax,ax
		jmp	@@FollowData

	FollowFarData:
		mov	ax,es:[si+2]
		mov	DumpSeg,ax

	FollowNearData:
		mov	ax,es:[si]
	@@FollowData:
		mov	DumpOffs,ax
		call	UpdateDump
		clc
		ret

	FollowCodeSeg:
		mov	ax,es:[si]
		mov	Unasm_seg,ax
		xor	ax,ax
		jmp	@@FollowCode

	FollowFarCode:
		mov	ax,es:[si+2]
		mov	Unasm_seg,ax

	FollowNearCode:
		mov	ax,es:[si]
	@@FollowCode:
		mov	CurIP,ax
		mov	CurLine,0
		call	UpdateCommands
		stc
		ret

	DataSearch:
		call	SearchBytes
		clc
		ret

	DataSearchNext:
		call	SearchNext
		clc
		ret

@@Actions	dw	kbEsc
		db	@@Quit		- @@FirstAction
		dw	kbEnter
		db	@@Quit		- @@FirstAction
		dw	kbRight
		db	@@Right		- @@FirstAction
		dw	kbLeft
		db	@@Left		- @@FirstAction
		dw	kbUp
		db	@@Up		- @@FirstAction
		dw	kbDown
		db	@@Down		- @@FirstAction
		dw	kbPgUp
		db	@@PageUp	- @@FirstAction
		dw	kbPgDn
		db	@@PageDown	- @@FirstAction
		dw	kbCtrlLeft
		db	@@ByteLeft	- @@FirstAction
		dw	kbCtrlRight
		db	@@ByteRight	- @@FirstAction
		dw	kbTab
		db	@@Switch	- @@FirstAction
		dw	kbCtrlD
		db	@@DumpGoTo	- @@FirstAction
		dw	kbAltF10
		db	@@LocalMenu	- @@FirstAction
		dw	kbAltEqu
		db	@@DumpReference	- @@FirstAction
		dw	kbAltE
		db	@@DumpESDI	- @@FirstAction
		dw	kbAltD
		db	@@DumpDSSI	- @@FirstAction
		dw	kbAltB
		db	@@DumpESBX	- @@FirstAction
		dw	kbAltS
		db	@@DumpSSBP	- @@FirstAction
		dw	kbAltA
		db	@@DumpDXAX	- @@FirstAction
		dw	kbAltF
		db	@@DumpDSDX	- @@FirstAction
		dw	kbAltI
		db	@@DumpESSI	- @@FirstAction
		dw	kbAltJ
		db	@@DumpDSDI	- @@FirstAction
		dw	kbAltC
		db	@@DumpCSIP	- @@FirstAction
		dw	kbAltL
		db	@@DumpCurLine	- @@FirstAction
		dw	kbCtrlF
		db	@@DumpFollow	- @@FirstAction
		dw	kbCtrlB
		db	@@Block		- @@FirstAction
		dw	kbCtrlS
		db	@@Search	- @@FirstAction
		dw	kbCtrlL
		db	@@SearchNext	- @@FirstAction

		dw	0			;end of table
		endp

;------------------------------------------------------------------------------
BlockMenu	proc
		mov	si,offset BlockMenuStruc
		call	_LocalMenu
		jc	@@Exit
		call	ax
	@@Exit:
		ret
		endp

CopyMemBlock	proc
		mov	si,offset CopyDataDialog
		call	InitDialog

	@@Again_:
		mov	CopyDlgItem,0

	@@Again:
		call	ExecDialog_
		jz	@@Exit

		mov	si,offset FromString
		call	GetAddressDS
		jc	@@Again_

		call	AdjustAddress

		mov	word ptr FromAddr,ax
		mov	word ptr FromAddr+2,dx

		mov	si,offset ToString
		call	GetAddressDS
		mov	CopyDlgItem,1
		jc	@@Again

		call	AdjustAddress

		mov	word ptr ToAddr,ax
		mov	word ptr ToAddr+2,dx

		call	GetCounter
		mov	CopyDlgItem,2
		jc	@@Again

		xchg	ax,cx
		cld
		les	di,ToAddr
		lds	si,FromAddr
		mov	ax,ds
		mov	bx,es
		cmp	ax,bx
		ja	@@Copy
		jb	@@ChangeDir
		cmp	si,di
		ja	@@Copy

	@@ChangeDir:
		add	si,cx
		add	di,cx
		dec	si
		dec	di
		std

	@@Copy:
		rep	movsb
		push	cs
		pop	ds

	@@Exit:
		call	DrawScreen
		stc
		ret
		endp

FillMemBlock	proc
		mov	si,offset FillDataDialog
		call	InitDialog

	@@Again_:
		mov	FillDlgItem,0

	@@Again:
		call	ExecDialog_
		jz	@@Exit

		mov	si,offset ToString
		call	GetAddressDS
		jc	@@Again_

		call	AdjustAddress

		mov	word ptr ToAddr,ax
		mov	word ptr ToAddr+2,dx

		call	GetCounter
		mov	FillDlgItem,1
		jc	@@Again

		mov	CountValue,ax

		mov	si,offset FillString
		mov	di,offset FillBin
		call	CreateBin
		jc	@@Error
		sub	di,offset FillBin
		jz	@@Exit

		xchg	ax,di
		mov	cx,CountValue
		les	di,ToAddr
		cld

	@@Next:
		push	cx
		mov	cx,ax
		mov	si,offset FillBin
		rep	movsb
		pop	cx
		loop	@@Next

	@@Exit:
		call	DrawScreen
		stc
		ret

	@@Error:
		mov	si,offset StringErrorMsg
		call	_ErrorMessage
		mov	FillDlgItem,2
		jmp	@@Again
		endp

WriteMemBlock	proc
		mov	BlockDlgTitle,offset WriteTitle
		mov	BlockDlgLabel,offset FromLabel

		mov	si,offset ReadDataDialog
		call	InitDialog

	@@Again_:
		mov	BlockDlgItem,0

	@@Again:
		call	ExecDialog_
		jz	RWBExit

		mov	si,offset FromString
		mov	BlockDlgItem,1
		call	GetAddressDS
		jc	@@Again

		mov	word ptr FromAddr,ax
		mov	word ptr FromAddr+2,dx

		inc	BlockDlgItem
		call	GetCounter
		jc	@@Again

		call	DrawScreen

		call	Set24vector

		mov	dx,offset FileNameString
		mov	ax,3D02h
		int	21h
		jc	@@Create

		xchg	ax,bx
		mov	si,offset QuestionDialog
		call	InitDialog
		call	ExecDialog
		call	DrawScreen
		cmp	al,cmOk
		jb	RWBClose
		jne	@@Write
		cwd
		mov	ax,4202h
		xor	cx,cx
		int	21h
		jmp	@@Write

	@@Create:
		mov	ah,3Ch
		xor	cx,cx
		int	21h
		xchg	ax,bx
		jc	@@CreateError

	@@Write:
		mov	cx,CountValue
		push	ds
		lds	dx,FromAddr
		mov	ah,40h
		int	21h
		pop	ds
		jc	RWBError
		xor	cx,cx
		mov	ah,40h
		int	21h
		mov	si,offset WriteErrorMsg

ReadWriteBlockExit:
		jnc	RWBClose

	RWBError:
		call	_ErrorMessage

	RWBClose:
		mov	ah,3Eh
		int	21h

	RWBRestore:
		call	Restore24vector

	RWBExit:
		call	DrawScreen
		stc
		ret

	@@CreateError:
		mov	si,offset CreateErrorMsg
		call	_ErrorMessage
		jmp	RWBRestore

		endp

ReadMemBlock	proc
		mov	BlockDlgTitle,offset ReadTitle
		mov	BlockDlgLabel,offset ToLabel

		mov	si,offset ReadDataDialog
		call	InitDialog

	@@Again_:
		mov	BlockDlgItem,0

	@@Again:
		call	ExecDialog_
		jz	RWBExit

		mov	si,offset FromString
		call	GetAddressDS
		mov	BlockDlgItem,1
		jc	@@Again

		mov	word ptr FromAddr,ax
		mov	word ptr FromAddr+2,dx

		call	GetCounter
		mov	BlockDlgItem,2
		jc	@@Again

		call	DrawScreen

		call	Set24vector

		mov	dx,offset FileNameString
		mov	ax,3D00h
		int	21h
		mov	si,offset OpenErrorMsg
		jc	RWBError

		xchg	ax,bx
		mov	cx,CountValue

		push	ds
		lds	dx,FromAddr
		mov	ah,3Fh
		int	21h
		pop	ds
		mov	si,offset ReadErrorMsg
		jc	RWBError
		cmp	ax,cx
		jmp	ReadWriteBlockExit
		endp


;------------------------------------------------------------------------------
FillHexDigit	proc
		push	ax
		add	al,'0'
		cmp	al,'9'
		jbe	@@1
		add	al,7
@@1:
		stosw
		pop	ax
		ret
endp

;------------------------------------------------------------------------------
DumpGoTo	proc
		mov	ax,16*256+40
		mov	di,offset DumpAddrString
		call	ReadAddress
		jc	@@Quit
		or	ch,ch
		jz	@@NoSeg
		mov	DumpSeg,dx
		cmp	ch,2
		je	@@NoOfs

	@@NoSeg:
		mov	DumpOffs,ax

	@@NoOfs:
		call	UpdateDump
		clc
	@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
EditCommands	proc
		cmp	CurX,0
		je	@@Skip
		mov	CurX,0
		call	UpdateCommands
@@Skip:
		push	ReadExitKeys
		mov	ReadExitKeys,offset AsmExitKeys
		push	cs
		pop	es
@@NextLine:
		mov	si,CurLineIP
		call	UnAssemble
		mov	di,offset Buffer+CMD_POS+MaxAsmSize-2
		mov	cx,MaxAsmSize-2
		std
		mov	al,' '
		repe	scasb
		cld
		scasw
		mov	al,0
		stosb
		add	cx,3
		mov	si,offset Buffer+CMD_POS
		mov	di,offset AsmLine
		rep	movsb
;		mov	AsmLine+MaxAsmSize-1,al
@@Again:
		mov	di,offset AsmLine
		mov	ah,byte ptr CurLine
;		add	ah,CPUy
		mov	al,CMD_POS-3
		mov	cx,MaxAsmSize*256+AsmLineSize-1
		mov	bx,atsAssembler*256+atAssembler
		cmp	CurLineBrk,1
		jne	@@ReadLine
		mov	bx,atsBrkAssembler*256+atBrkAssembler

	@@ReadLine:
		call	ReadLine
		cmp	ax,kbEsc
		je	@@Quit
		cmp	ax,kbDown
		jne	@@1
		call	CurDown
		jmp	@@NextLine
@@1:		cmp	ax,kbUp
		jne	@@2
		call	CurUp
		jmp	@@NextLine
@@2:		cmp	ax,kbPgDn
		jne	@@3
		call	PageDown
		jmp	@@NextLine
@@3:		cmp	ax,kbPgUp
		jne	@@4
		call	PageUp
		jmp	@@NextLine
@@4:
		mov	si,offset AsmLine
		mov	di,CurLineIP
		push	es
		mov	es,Unasm_seg
		mov	al,es:[di]
		inc	byte ptr es:[di]
		inc	al
		cmp	al,es:[di]
		jne	@@ROMError
		dec	byte ptr es:[di]
		mov	bx,di
		call	Assemble
		pop	es
		jnc	@@Ok
		cmp	ax,erEmpty
		je	@@Ok1
		dec	ax
		dec	ax
		cmp	ax,9-2
		ja	@@UnkErr
		mov	bx,ax
		shl	bx,1
		mov	si,AsmErrors[bx]
		jmp	@@ErrMsg
@@UnkErr:
		mov	si,offset AsmErrorMsg
@@ErrMsg:
		call	_ErrorMessage
		jmp	@@Again
@@Ok:
		call	UpdateDump
@@Ok1:
		call	CurDown
		jmp	@@NextLine
@@ROMError:
		pop	es
		mov	dx,offset CodeNotInRAM
		call	_ErrorMessage
@@Quit:
		call	UpdateCommands
		pop	ReadExitKeys
		ret
		endp

;------------------------------------------------------------------------------
ToggleBreakPoint proc
		mov	si,CurLineIP
		call	CheckBP
		jc	@@Set
		mov	[bx].BPactive,bpUnused
		jmp	@@Ok
@@Set:
		mov	bx,offset BP1
		mov	cx,MaxBreakPoints
@@NextBP:
		cmp	[bx].BPactive,bpUnused
		jne	@@Skip
		mov	ax,Unasm_seg
		mov	[bx].BPseg,ax
		mov	ax,CurLineIP
		mov	[bx].BPofs,ax
		mov	[bx].BPset,0
		call	TestBreakPoint
		jc	@@Not
		mov	[bx].BPactive,bpEnabled
		jmp	@@Ok
@@Not:
		mov	si,offset BPnotInRAM
		call	_ErrorMessage
		jmp	@@Ok
@@Skip:
		add	bx,size BreakPoint
		loop	@@NextBP
		mov	si,offset ManyBPMsg
		call	_ErrorMessage
@@Ok:
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
ToggleActivity	proc
		mov	si,CurLineIP
		call	CheckBP
		jnc	@@Toggle
		jmp	Beep

	@@Toggle:
		cmp	[bx].BPactive,bpEnabled
		mov	[bx].BPactive,bpDisabled
		je	@@Exit

		mov	[bx].BPactive,bpEnabled

	@@Exit:
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
SetAllBreakPoints	proc
		mov	bx,offset BP1
		mov	cx,MaxBreakPoints

	@@NextBP:
		cmp	[bx].BPactive,bpUnused
		je	@@Skip
		mov	[bx].BPactive,al

	@@Skip:
		add	bx,size BreakPoint
		loop	@@NextBP
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
ClearAllBP	proc
		mov	al,bpUnused
		jmp	SetAllBreakPoints
		endp

;------------------------------------------------------------------------------
EnableAllBreaks	proc
		mov	al,bpEnabled
		jmp	SetAllBreakPoints
		endp

;------------------------------------------------------------------------------
DisableAllBreaks	proc
		mov	al,bpDisabled
		jmp	SetAllBreakPoints
		endp

;------------------------------------------------------------------------------
TestBreakPoint	proc
		push	ax si es
		cli
		mov	es,[bx].BPseg
		mov	si,[bx].BPofs
		mov	al,es:[si]
		mov	byte ptr es:[si],0CCh
		cmp	byte ptr es:[si],0CCh
		je	@@Ok
		stc
@@Ok:
		mov	es:[si],al
		sti
		pop	es si ax
		ret
		endp

;------------------------------------------------------------------------------
CheckBP		proc
		push	ax cx
		mov	bx,offset BP1
		mov	cx,MaxBreakPoints
@@NextBP:
		cmp	[bx].BPactive,bpUnused
		je	@@Skip
		cmp	si,[bx].BPofs
		jne	@@Skip
		mov	ax,Unasm_seg
		cmp	ax,[bx].BPseg
		jne	@@Skip
		cmp	[bx].BPactive,bpEnabled
		jmp	@@Exit
@@Skip:
		add	bx,size BreakPoint
		loop	@@NextBP
		stc
@@Exit:
		pop	cx ax
		ret
		endp

;------------------------------------------------------------------------------
ExecuteMenu	proc
		call	HideFocus
		mov	si,offset MenuStructure
		call	ExecMenu
		call	DisplayFocus
		call	CheckSharedArea
		mov	al,CPUtype
		sub	al,a86
		mov	OptCPU,al
		cmp	QuitFlag,1
		jne	@@1
		jmp	QuitACT
@@1:
		ret
		endp

QuitProc	proc
		mov	QuitFlag,1
		ret
		endp

SetCPU88	proc
;		mov	OptCPU,0
		mov	CPUtype,a86
		jmp	UpdateCommands
		endp

SetCPU286	proc
;		mov	OptCPU,1
		mov	CPUtype,a286
		jmp	UpdateCommands
		endp

SetCPU386	proc
;		mov	OptCPU,2
		mov	CPUtype,a386
		jmp	UpdateCommands
		endp

SetCPU486	proc
;		mov	OptCPU,3
		mov	CPUtype,a486
		jmp	UpdateCommands
		endp

SetCPUAuto	proc
		mov	al,CPU
		sub	al,1
		adc	al,0
		mov	OptCPU,al
		add	al,a86
		mov	CPUtype,al
		jmp	UpdateCommands
		endp

SetSwapOff	proc
		mov	SwapMode,0
		ret
		endp

SetSwapSmart	proc
		mov	SwapMode,1
		ret
		endp

SetSwapOn	proc
		mov	SwapMode,2
		ret
		endp

;------------------------------------------------------------------------------
ActionShortJump	proc
	@@Next:
		add	di,3
		cmp	word ptr ds:[di],0
		je	@@Exit
		cmp	word ptr ds:[di],ax
		jne	@@Next
		push	ax
		xor	ax,ax
		mov	al,ds:[di+2]
		xchg	ax,di
		pop	ax
		clc
		ret

	@@Exit:
		stc
		ret
		endp

;------------------------------------------------------------------------------
ActionJump	proc
	@@Next:
		add	di,4
		cmp	word ptr ds:[di],0
		je	@@Exit
		cmp	word ptr ds:[di],ax
		jne	@@Next
;		push	ax
;		xor	ax,ax
		mov	di,ds:[di+2]
;		xchg	ax,di
;		pop	ax
		clc
		ret

	@@Exit:
		stc
		ret
		endp

;------------------------------------------------------------------------------
CPULocalMenu	proc
		mov	si,offset CPULocalStruc
		call	_LocalMenu
		jc	@@Exit
		jmp	ax
	@@Exit:
		ret
		endp


;------------------------------------------------------------------------------
ContinueInit:
		call	Init

		cmp	Loaded,1
		je	@@CheckResident
		mov	si,offset LoadErrorMsg
		call	_ErrorMessage

	@@CheckResident:
		mov	si,offset CryptText2
		mov	cx,CryptLen2
		call	Decrypt
		test	CmdLineOptions,cmdResident
		jz	StartKeyLoop
		call	_Resident

	StartKeyLoop:

		call	DrawScreen
	MainKeyLoop:
		mov	Keys,offset MainLoop
		call	KeyLoop
		cmp	ResidentMode,0
		jne	MainKeyLoop

;----- Exit from ACT ----------------------------------------------------------
QuitACT:
		call	UnloadProgram
		call	SetMyPid
		call	RestoreIntTable
		call	RestoreScreen


		mov	al,MyPort21
		out	21h,al
		mov	ax,4C00h
		int	21h


_ErrorMessage	proc
		call	ErrorMessage
		jmp	CheckSharedArea
		endp

_MsgBox		proc
		call	MessageBox
		jmp	CheckSharedArea
		endp

_LocalMenu	proc
		call	LocalMenu
		pushf
		call	CheckSharedArea
		popf
		ret
		endp

DrawScreen	proc
		call	DrawCPUwindow
UpdateScreen:
		call	UpdateStack
		call	UpdateWatchLabel

UpdateRegsAndCmds:
		call	_UpdateDump
		call	UpdateRegisters
		jmp	UpdateCommands
		endp



define __ACT__
use _Window
use _KeyLoop
use _GetAddr
use _WriteString
use _UpCase
use _FillHex
use _UnAssemble
use _GetCursor
use _SetCursor
use _GoToXY
use _Beep
use _WriteMessage
use _MessageBox
use _ReadString
use _ExecMenu

include		pushpop.inc
include		keyboard.inc
include		windows.inc
include		dialogs.inc
include		video.inc

include		crypt.inc

include		inst.inc
include		unasm.inc
include		asm.inc
include		trace.inc

include		actmenu.inc
include		cpumenu.inc
include		regmenu.inc
include		dumpmenu.inc
include		flagmenu.inc




include		follow.inc
include		search.inc
include		resident.inc
include		actdlgs.inc



include		actdata.inc


;------------------------------------------------------------------------------
FollowStackBtm	label	byte
SharedDataArea	label	byte
StringBuffer	label	byte
WindowBuffer	label	byte


include		cpu.inc
include		cmdline.inc
include		install.inc

SharedDataSize	equ	$-SharedDataArea

FileNameBuff	label	byte
IniFileReadBuff	equ	(offset FileNameBuff + MaxFileName)

UDataStart	label	byte

		db	WindowBufSize - SharedDataSize dup (?)
CodeMarkBuff	dw	10 * 4 dup (?)

include		actudata.inc

UDataEnd	label	byte
ProgramEnd	label	byte

		end
