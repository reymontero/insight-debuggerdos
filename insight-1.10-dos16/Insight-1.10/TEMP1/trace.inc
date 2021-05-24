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


SaveACTregs	macro
		mov	SaveSP,sp
		endm


RestoreACTregs	macro
		mov	ax,cs
		mov	ds,ax
		mov	es,ax
		mov	ss,ax
		mov	sp,SaveSP
		endm


SaveUserRegs	macro
		cmp	cs:CPU,3
		jb	@@SkipESP
		db	66h
	@@SkipESP:
		mov	cs:CPURegs.xsp,sp
		call	_SaveUserRegs
		endm


RestoreUserRegs	macro
		call	_RestoreUserRegs
		endm


;------------------------------------------------------------------------------
_SaveUserRegs	proc
	FirstPrefix:
	Prefix01:
		nop
		mov	cs:CPURegs.xax,ax

		mov	cs:CPURegs.xds,ds
		mov	ax,cs
		mov	ds,ax
		pop	KeepIP

	Prefix02:
		nop
		mov	CPURegs.xbx,bx

	Prefix03:
		nop
		mov	CPURegs.xcx,cx

	Prefix04:
		nop
		mov	CPURegs.xdx,dx

	Prefix05:
		nop
		mov	CPURegs.xsi,si

	Prefix06:
		nop
		mov	CPURegs.xdi,di

	Prefix07:
		nop
		mov	CPURegs.xbp,bp

		cmp	CPU,3
		jb	@@Skip
.386
		mov	CPURegs.xgs,gs
		mov	CPURegs.xfs,fs
.8086
	@@Skip:
		mov	CPURegs.xes,es
		mov	CPURegs.xss,ss
		mov	es,ax
		mov	ss,ax

	Prefix16:
		nop
		mov	ax,CPURegs.xip

	Prefix17:
		nop
		mov	SaveCPURegs.xip,ax

	Prefix18:
		nop
		mov	ax,CPURegs.xcs

	Prefix19:
		nop
		mov	SaveCPURegs.xcs,ax

		mov	RegsSaved,0
		mov	sp,SaveSP
		jmp     KeepIP
		endp


_RestoreUserRegs	proc
		pop	KeepIP
		call	SaveRegs

	Prefix08:
		nop
		mov	ax,CPURegs.xax

	Prefix09:
		nop
		mov	bx,CPURegs.xbx

	Prefix10:
		nop
		mov	cx,CPURegs.xcx

	Prefix11:
		nop
		mov	dx,CPURegs.xdx

	Prefix12:
		nop
		mov	si,CPURegs.xsi

	Prefix13:
		nop
		mov	di,CPURegs.xdi

	Prefix14:
		nop
		mov	sp,CPURegs.xsp

	Prefix15:
		nop
		mov	bp,CPURegs.xbp
		cmp	CPU,3
		jb	@@Skip
.386
		mov	gs,CPURegs.xgs
		mov	fs,CPURegs.xfs
.8086
	@@Skip:
		mov	es,CPURegs.xes
		mov	ss,CPURegs.xss
		mov	ds,CPURegs.xds
		jmp     cs:KeepIP
		endp

SaveRegs	proc
		push	ds
		pop	es
		mov	si,offset CPURegs
		mov	di,offset SaveCPURegs
		mov	cx,size Registers32
		cld
		rep	movsb
		ret
		endp

RestoreRegs	proc
		push	ds
		pop	es
		push	CPURegs.xflags
		push	CPURegs.xcs
		push	CPURegs.xip
		mov	di,offset CPURegs
		mov	si,offset SaveCPURegs
		mov	cx,size Registers32
		cld
		rep	movsb
		pop	CPURegs.xip
		pop	CPURegs.xcs
		pop	CPURegs.xflags
		ret
		endp


;------------------------------------------------------------------------------
SingleStep	proc
		mov	dl,0
		call	Emulate
		jnc	@@1
		call	SaveRegs
		jmp	TraceUpdate
@@1:		call	CheckSwap
		cmp	SwapFlag,0
		je	@@N1
		call	RestoreScreen
@@N1:
		cli
;		call	Set09vector
		mov	dx,offset @@Int01entry
		call	Set01vector
		mov	al,UserPort21
		or	al,3
		out	21h,al
		SaveACTregs
		RestoreUserRegs
		or	cs:CPURegs.xflags,100h
		push	cs:CPURegs.xflags
		push	cs:CPURegs.xcs
		push	cs:CPURegs.xip
		iret
@@Int01entry:
		pop	cs:CPURegs.xip
		pop	cs:CPURegs.xcs
		pop	cs:CPURegs.xflags
		SaveUserRegs
		in	al,21h
		and	al,not 3
		and	UserPort21,3
		or	UserPort21,al
		mov	al,MyPort21
		out	21h,al
		and	CPURegs.xflags,not 100h
		call	Restore01vector
		call	ClearBreakPoints
		sti
		cmp	UserScreenMode,0
		je	TraceUpdate
		call	SaveScreen
		call	DrawCPUWindow
TraceUpdate:
		call	UpdateScreen
		cmp	CurIPline,-1
		jne	@@2
		jmp	GoToOrigin
@@2:
		mov	ax,CurIPline
		cmp	ax,CPUheight-4
		ja	@@Adjust
@@Ok:
		mov	CurLine,ax
		jmp	UpdateCommands
@@Adjust:
		mov	si,CurIP
		sub	ax,6
		mov	cx,ax
@@Next:
		call	UnAssemble
		loop	@@Next
		mov	CurIP,si
		mov	ax,6
		jmp	@@Ok
		endp


;------------------------------------------------------------------------------
ProcTrace	proc
		call	CheckJump
		jnc	@@3
		jmp	SingleStep
@@3:
		xor	bp,bp
		mov	ax,CPURegs.xcs
		mov	Unasm_seg,ax
		mov	si,CPURegs.xip
		call	CheckBP
		jc	@@A
		mov	bp,bx
@@A:
		cli
		mov	es,ax
		call	UnAssemble
		call	CheckBP
		jnc	@@Ok
		mov	bx,offset BP0
		call	SetBreakPoint
		jnc	@@Ok
		mov	si,offset CodeNotInRAM
		call	ErrorMessage
		call	ClearBreakPoint
		jmp	TraceUpdate
@@Ok:
		mov	dl,1
		call	CheckSwap
		cmp	SwapFlag,0
		je	@@1
		call	RestoreScreen
@@1:
		call	Set09vector
		mov	dx,offset @@Int03entry
		call	Set03vector
		call	SetBreakPoints
		or	bp,bp
		jz	@@B
		mov	bx,bp
		call	ClearBreakPoint
@@B:
		mov	al,UserPort21
		and	al,11111110b
		out	21h,al
		SaveACTregs
		RestoreUserRegs
		push	cs:CPURegs.xflags
		push	cs:CPURegs.xcs
		push	cs:CPURegs.xip
		iret

@@Int03entry:
		pop	cs:CPURegs.xip
		pop	cs:CPURegs.xcs
		pop	cs:CPURegs.xflags
		SaveUserRegs
		mov	ax,CPURegs.xip
		dec	ax
		mov	CPURegs.xip,ax
		mov	SaveCPURegs.xip,ax
		call	ClearBreakPoints
		in	al,21h
		mov	UserPort21,al
		mov	al,MyPort21
		out	21h,al
		sti
@@Skip:
		cmp	UserScreenMode,0
		je	@@2
		call	SaveScreen
		call	DrawCPUwindow
@@2:
		jmp	TraceUpdate
		endp


;------------------------------------------------------------------------------
Set01vector	proc
		mov	si,1*4
		mov	di,offset SaveInt01
		call	SetVector
		mov	Int01set,1
		ret
		endp

Restore01vector	proc
		cmp	Int01set,0
		je	@@Quit
		mov	si,offset SaveInt01
		mov	di,1*4
		call	RestoreVector
		mov	Int01set,0
@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
Set03vector	proc
		mov	si,3*4
		mov	di,offset SaveInt03
		call	SetVector
		mov	Int03set,1
		ret
		endp

Restore03vector	proc
		cmp	Int03set,0
		je	@@Quit
		mov	si,offset SaveInt03
		mov	di,3*4
		call	RestoreVector
		mov	Int03set,0
@@Quit:
		ret
		endp


;------------------------------------------------------------------------------
Set09vector	proc
		mov	si,9*4
		mov	di,offset SaveInt09
		mov	dx,offset Int09handler
		call	SetVector
		mov	Int09set,1
		ret
		endp

Restore09vector	proc
		cmp	Int09set,0
		je	@@Quit
		mov	si,offset SaveInt09
		mov	di,9*4
		call	RestoreVector
		mov	Int09set,0
@@Quit:
		ret
		endp


;------------------------------------------------------------------------------
RunProgram	proc
		mov	ax,CPURegs.xcs
		mov	Unasm_seg,ax
		mov	si,CPURegs.xip
		call	CheckBP
		jc	@@A
		call	SingleStep
@@A:
		cmp	SwapMode,0
		je	@@N
		call	RestoreScreen
@@N:
		cli
;		call	RestoreUserInts
		call	SetBreakPoints
		call	Set09vector
		mov	dx,offset BPentry
		call	Set03vector
		mov	al,UserPort21
		and	al,11111110b
		out	21h,al
		SaveACTregs
		RestoreUserRegs
		push	cs:CPURegs.xflags
		push	cs:CPURegs.xcs
		push	cs:CPURegs.xip
		iret
		endp


;------------------------------------------------------------------------------
GoToCursor	proc
		mov	bx,offset BP0
		mov	ax,Unasm_seg
		mov	[bx].BPseg,ax
		mov	ax,CurLineIP
		mov	[bx].BPofs,ax
		call	TestBreakPoint
		jnc	@@Ok
		mov	si,offset CodeNotInRAM
		jmp	ErrorMessage
@@Ok:
		push	Unasm_seg
		push	CurLineIP
		mov	ax,CPURegs.xcs
		cmp	ax,Unasm_seg
		jne	@@A
		mov	ax,CurLineIP
		cmp	ax,CPURegs.xip
		jne	@@A
		call	SingleStep
		jmp	@@B
@@A:
		mov	ax,CPURegs.xcs
		mov	Unasm_seg,ax
		mov	si,CPURegs.xip
		call	CheckBP
		jc	@@B
		call	SingleStep
@@B:
		cmp	SwapMode,0
		je	@@N1
		call	RestoreScreen
@@N1:
		cli
;		call	RestoreUserInts
		call	Set09vector
		mov	dx,offset @@Int03entry
		call	Set03vector
		pop	si
		pop	es
		call	SetBreakPoints
		mov	es,BP0.BPseg
		mov	si,BP0.BPofs
		mov	bx,offset BP0
		call	SetBreakPoint
		mov	al,UserPort21
		and	al,11111110b
		out	21h,al
		SaveACTregs
		RestoreUserRegs
		push	cs:CPURegs.xflags
		push	cs:CPURegs.xcs
		push	cs:CPURegs.xip
		iret
@@Int03entry:
BPentry:
		pop	cs:CPURegs.xip
		pop	cs:CPURegs.xcs
		pop	cs:CPURegs.xflags
		SaveUserRegs
;		RestoreACTregs
		dec	CPURegs.xip
ACTpopup:
		call	ClearBreakPoints
		in	al,21h
		mov	UserPort21,al
		mov	al,MyPort21
		out	21h,al
		sti
		cmp	UserScreenMode,0
		je	@@N2
		call	SaveScreen
		call	DrawCPUWindow
@@N2:
		call	UpdateScreen
		mov	ax,CurIPline
		cmp	ax,-1
		jne	@@1
		jmp	GoToOrigin
@@1:
		cmp	ax,CurLine
		je	@@Quit
		mov	CurLine,ax
		call	UpdateCommands
@@Quit:
		ret
		endp

;------------------------------------------------------------------------------
Int09handler	proc
		push	ax
		in	al,60h
		cmp	al,ScanCode
		jne	@@Quit
		push	ds
		xor	ax,ax
		mov	ds,ax
		mov	al,ds:[417h]
		test	al,ShiftCode
		jnz	Int09execute
		pop	ds
@@Quit:
		pop	ax
		jmp	cs:SaveInt09
Int09execute:
		in	al,61h
		or	al,80h
		out	61h,al
		and	al,7Fh
		out	61h,al
		mov	al,20h
		out	20h,al
		pop	ds
		pop	ax
		pop	cs:CPURegs.xip
		pop	cs:CPURegs.xcs
		pop	cs:CPURegs.xflags
		SaveUserRegs
;		RestoreACTregs
		jmp	ACTpopup
		endp


;------------------------------------------------------------------------------
SetBreakPoint	proc
		mov	[bx].BPseg,es
		mov	[bx].BPofs,si
		mov	[bx].BPset,1
		mov	al,es:[si]
		mov	[bx].BPbyte,al
		mov	byte ptr es:[si],0CCh
		cmp	byte ptr es:[si],0CCh
		je	@@Quit
		stc
@@Quit:
		ret
		endp


;------------------------------------------------------------------------------
ClearBreakPoint	proc
		cmp	[bx].BPset,0
		je	@@Quit
		push	ax si es
		mov	es,[bx].BPseg
		mov	si,[bx].BPofs
		mov	al,[bx].BPbyte
		mov	es:[si],al
		mov	[bx].BPset,0
		pop	es si ax
@@Quit:
		ret
		endp


;------------------------------------------------------------------------------
ClearBreakPoints proc
		call	Restore03vector
		call	Restore09vector
		mov	bx,offset BP0
		mov	cx,MaxBreakPoints+1

	@@NextBP:
		call	ClearBreakPoint
		add	bx,size BreakPoint
		loop	@@NextBP
		ret
		endp


;------------------------------------------------------------------------------
SetBreakPoints	proc
		push	bx cx si es
		mov	bx,offset BP1
		mov	cx,MaxBreakPoints
@@NextBP:
		cmp	[bx].BPactive,bpUnused
		je	@@Skip
		cmp	[bx].BPactive,bpDisabled
		je	@@Skip
		mov	es,[bx].BPseg
		mov	si,[bx].BPofs
		call	SetBreakPoint
@@Skip:
		add	bx,size BreakPoint
		loop	@@NextBP
		pop	es si cx bx
		ret
		endp


;------------------------------------------------------------------------------
Emulate		proc
		cld
		push	ds es
		mov	si,CPURegs.xip
		mov	es,CPURegs.xcs
		mov	bx,offset InstDesc
@@NextByte:
		lods	byte ptr es:[si]
		xlat
		test	al,$prefix
		jnz	@@NextByte
		test	al,$em
		jnz	@@Emulate
@@Quit:
		pop	es ds
		ret
@@Emulate:
		mov	al,es:[si-1]
		cmp	al,0CDh
		je	@@INT
		cmp	al,0CEh
		je	@@INTO
		cmp	al,9Ch
		je	@@PUSHF
		cmp	al,9Dh
		je	@@POPF
		cmp	al,0CFh
		je	@@IRET
		clc
		jmp	@@Quit
@@PUSHF:
		mov	ax,CPURegs.xflags
		and	ax,not 100h
		call	UserPUSH
		jmp	@@Skip
@@POPF:
		call	UserPOP
		and	ax,not 100h
		mov	CPURegs.xflags,ax
		jmp	@@Skip
@@IRET:
		call	UserPOP
		mov	CPURegs.xip,ax
		call	UserPOP
		mov	CPURegs.xcs,ax
		call	UserPOP
		mov	CPURegs.xflags,ax
		stc
		jmp	@@Quit
@@INTO:
		test	byte ptr CPURegs.xflags+1,00001000b
		jz	@@Skip
		mov	al,4
		jmp	@@INTn
@@INT:
		lods	byte ptr es:[si]
@@INTn:
		push	ax
		mov	ax,CPURegs.xflags
		and	ax,not 100h
		call	UserPUSH
		and	ax,not 200h
		mov	CPURegs.xflags,ax
		mov	ax,es
		call	UserPush
		mov	ax,si
		call	UserPUSH
		pop	ax
		mov	ah,0
		shl	ax,1
		shl	ax,1
		mov	si,ax
		xor	ax,ax
		mov	ds,ax
		lodsw
		mov	cs:CPURegs.xip,ax
		lodsw
		mov	cs:CPURegs.xcs,ax
		stc
		jmp	@@Quit
@@Skip:
		mov	CPURegs.xip,si
		stc
		jmp	@@Quit
		endp


;------------------------------------------------------------------------------
UserPUSH	proc
		push	bx es
		mov	es,CPURegs.xss
		mov	bx,CPURegs.xsp
		dec	bx
		dec	bx
		mov	es:[bx],ax
		mov	CPURegs.xsp,bx
		pop	es bx
		ret
		endp


;------------------------------------------------------------------------------
UserPOP		proc
		push	bx es
		mov	es,CPURegs.xss
		mov	bx,CPURegs.xsp
		mov	ax,es:[bx]
		inc	bx
		inc	bx
		mov	CPURegs.xsp,bx
		pop	es bx
		ret
		endp
