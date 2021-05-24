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


rfFollow	equ	1b
rfJump		equ	10b
rfBackward	equ	100b
rfCondition	equ	1000b
rfTrue		equ	10000b
rfPrefix	equ	100000b
rfAddress32	equ	1000000b
rfOperand32	equ	10000000b

rf32		equ	(rfOperand32+rfAddress32)

;------------------------------------------------------------------------------
DumpReference	proc
		call	UnAsmCurLine
		cmp	MemoryFlag,0
		je	@@Exit
		call	CalcMemAddress
		push	si
		push	dx
		call	UpdateDumpWindow
  @@Exit:
		clc
		ret
		endp

DumpCurLine	proc
		push	CurLineIP
		push	Unasm_Seg
		call	UpdateDumpWindow
		endp

DumpESDI	proc
		push	CPURegs.xdi
		push	CPURegs.xes
		call	UpdateDumpWindow
		endp

DumpDSSI	proc
		push	CPURegs.xsi
		push	CPURegs.xds
		call	UpdateDumpWindow
		endp

DumpESBX	proc
		push	CPURegs.xbx
		push	CPURegs.xes
		call	UpdateDumpWindow
		endp

DumpSSBP	proc
		push	CPURegs.xbp
		push	CPURegs.xss
		call	UpdateDumpWindow
		endp

DumpDXAX	proc
		push	CPURegs.xax
		push	CPURegs.xdx
		call	UpdateDumpWindow
		endp

DumpDSDX	proc
		push	CPURegs.xdx
		push	CPURegs.xds
		call	UpdateDumpWindow
		endp

DumpESSI	proc
		push	CPURegs.xsi
		push	CPURegs.xes
		call	UpdateDumpWindow
		endp

DumpDSDI	proc
		push	CPURegs.xdi
		push	CPURegs.xds
		call	UpdateDumpWindow
		endp

DumpCSIP	proc
		push	CPURegs.xip
		push	CPURegs.xcs
		call	UpdateDumpWindow

UpdateDumpWindow:
		pop	DumpProc
		pop	NewDumpSeg
		pop	NewDumpOffs

		call	pushr

		mov	ax,DumpProc
		mov	bx,UpdateDumpWindow-DumpCSIP
		sub	ax,bx
		cmp	ax,offset DumpESDI
		jb	@@SetDump
		mov	DataWatchTemp,ax
		sub	ax,offset DumpESDI
		div	bl
		mov	bl,5
		mul	bl
		add	ax,offset DataWatchLabels
		mov	DataWatchLabel,ax

	@@SetDump:
		mov	ax,NewDumpOffs
		mov	dx,NewDumpSeg

;		cmp     DumpOffs,ax
;		jne	@@UpdateDump
;		cmp	DumpSeg,dx
;		je	@@Exit

	@@UpdateDump:
		mov	DumpOffs,ax
		mov	DumpSeg,dx
		call	UpdateDump

	@@Exit:
		call	popr
		clc
		ret
		endp

;------------------------------------------------------------------------------
SetCodeMark0	proc
		call	SetCodeMark
		endp

SetCodeMark1	proc
		call	SetCodeMark
		endp

SetCodeMark2	proc
		call	SetCodeMark
		endp

SetCodeMark3	proc
		call	SetCodeMark
		endp

SetCodeMark4	proc
		call	SetCodeMark
		endp

SetCodeMark5	proc
		call	SetCodeMark
		endp

SetCodeMark6	proc
		call	SetCodeMark
		endp

SetCodeMark7	proc
		call	SetCodeMark
		endp

SetCodeMark8	proc
		call	SetCodeMark
		endp

SetCodeMark9	proc
		call	SetCodeMark
		endp

SetCodeMark	proc
		pop	di
		mov	bx,offset SetCodeMark1
		call	FindCodeMark
		or	CodeMarkExist,ax
		call	CheckShift
		jnz	@@SetDataMark

		mov	ax,CurLine
		stosw
		mov	ax,CurIP
		stosw
		mov	ax,Unasm_Seg
		stosw
		mov	ax,CurLineIP
		stosw
		jmp	UpdateCommands

	@@SetDataMark:
		mov	ax,-1
		stosw
		mov	ax,DumpOffs
		stosw
		mov	ax,DumpSeg
		stosw
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
ClearCodeMark0	proc
		call	ClearCodeMark
		endp

ClearCodeMark1	proc
		call	ClearCodeMark
		endp

ClearCodeMark2	proc
		call	ClearCodeMark
		endp

ClearCodeMark3	proc
		call	ClearCodeMark
		endp

ClearCodeMark4	proc
		call	ClearCodeMark
		endp

ClearCodeMark5	proc
		call	ClearCodeMark
		endp

ClearCodeMark6	proc
		call	ClearCodeMark
		endp

ClearCodeMark7	proc
		call	ClearCodeMark
		endp

ClearCodeMark8	proc
		call	ClearCodeMark
		endp

ClearCodeMark9	proc
		call	ClearCodeMark
		endp

ClearCodeMark	proc
		pop	di
		mov	bx,offset ClearCodeMark1
		call	FindCodeMark
		mov	si,di
		jz	JumpBeepReturn
		not	ax
		and	CodeMarkExist,ax
		jmp	UpdateCommands
		endp

;------------------------------------------------------------------------------
CodeMark0	proc
		call	CodeMark
		endp

CodeMark1	proc
		call	CodeMark
		endp

CodeMark2	proc
		call	CodeMark
		endp

CodeMark3	proc
		call	CodeMark
		endp

CodeMark4	proc
		call	CodeMark
		endp

CodeMark5	proc
		call	CodeMark
		endp

CodeMark6	proc
		call	CodeMark
		endp

CodeMark7	proc
		call	CodeMark
		endp

CodeMark8	proc
		call	CodeMark
		endp

CodeMark9	proc
		call	CodeMark
		endp

CodeMark	proc
		pop	di
		mov	bx,offset CodeMark1
		call	FindCodeMark
		mov	si,di
JumpBeepReturn:
		jz	BeepReturn

		lodsw
		cmp	ax,0FFFFh
		je	@@SetDumpPos

		call	PushUnasmPos

		mov	CurLine,ax
		lodsw
		mov	CurIP,ax
		lodsw
		mov	Unasm_Seg,ax
		jmp	UpdateCommands

	@@SetDumpPos:
		lodsw
		mov	DumpOffs,ax
		lodsw
		mov	DumpSeg,ax
		jmp	UpdateDump

		endp

FindCodeMark	proc
		sub	di,bx
		xchg	ax,di
		mov	bl,3
		div	bl
		mov	di,ax
		shl	di,1
		shl	di,1
		shl	di,1
		add	di,offset CodeMarkBuff
		xchg	ax,cx
		mov	ax,1
		shl	ax,cl
		test	CodeMarkExist,ax
		cld
		ret
		endp


;----------------------------------------------------------------------------
Follow		proc
		test	RefFlags,rfFollow
		jz	BeepReturn

		call	PushUnasmPos

		mov	ax,FollowSeg
		mov	Unasm_Seg,ax
		mov	ax,FollowOffs
		mov	CurIP,ax
		mov	CurLine,0
		jmp	UpdateCommands
		endp

Previous	proc
		cld
		mov	si,FollowStackPtr
		cmp	si,FollowStack
		jae	BeepReturn
		lodsw
		mov	CurLine,ax
		lodsw
		mov	CurIP,ax
		lodsw
		mov	Unasm_Seg,ax
		mov	FollowStackPtr,si
		jmp	UpdateCommands

	BeepReturn:
		jmp	Beep
		endp

PushUnasmPos	proc
		push	ax
		push	si
		mov	si,FollowStackPtr
		cmp	si,offset FollowStackBtm
		jbe	@@StackFull
		sub	si,2
		mov	ax,Unasm_Seg
		mov	ds:[si],ax
		sub	si,2
		mov	ax,CurIP
		mov	ds:[si],ax
		sub	si,2
		mov	ax,CurLine
		mov	ds:[si],ax
		mov	FollowStackPtr,si
	@@Exit:
		pop	si
		pop	ax
		ret

	@@StackFull:
		call	Beep
		jmp	@@Exit
		endp

CheckSharedArea	proc
		push	si
		mov	si,BufPosMark
		mov	BufPosMark,offset WindowBuffer
		jmp	@@CheckCodeMark

_CheckSharedArea:
		push	si

	@@CheckCodeMark:
		cmp	si,offset CodeMarkBuff
		jbe	@@CheckStack
		mov	CodeMarkExist,0
		mov	si,offset CodeMarkMsg
		call	ErrorMessage
		jmp	@@Exit2

	@@CheckStack:
		cmp	si,FollowStackPtr
		jbe	@@Exit1
		mov	si,offset FollowStackMsg
		call	MessageBox

	@@Exit2:
		mov	FollowStackPtr,FollowStack

	@@Exit1:
		pop	si
		ret
		endp

UnAsmCurLine	proc
		push	si
		push	bp
		mov	si,CurIP
		xor	bp,bp
	@@NextCPUline:
		call	UnAssemble
		inc	bp
		cmp	bp,CurLine
		jbe	@@NextCPULine
		pop	bp
		pop	si
		ret
		endp

FindFirstByte	proc

		xor	cx,cx

	@@NextByte:
		lods	byte ptr es:[si]

		mov	dl,rfPrefix
		cmp	al,26h
		je	@@Next
		cmp	al,36h
		je	@@Next
		cmp	al,2Eh
		je	@@Next
		cmp	al,3Eh
		je	@@Next

		cmp	al,64h
		je	@@CheckCPU
		cmp	al,65h
		je	@@CheckCPU
		cmp	al,67h
		je	@@CheckCPU
		cmp	al,66h
		jne	@@NormalExit
		mov	dl,rfAddress32

	@@CheckCPU:
		cmp	CPUtype,a386
		jb	@@Exit
		or	RefFlags,dl

	@@Next:
		loop	@@NextByte
		stc
		ret

	@@NormalExit:
		clc
	@@Exit:
		ret
		endp

GetRefAddresses	proc
		call	pushr

		and	RefFlags,rfFollow
		mov	si,bx
		mov	bp,bx
		mov	es,Unasm_Seg
		mov	di,offset @@FollowTable - 2

		cld

	@@Command:
		call	FindFirstByte
		jc	@@JumpToErrorExit

		cmp	al,0FFh			;call/jmp mem/reg
		je	@@RefJump

		test	RefFlags,rfPrefix
		jnz	@@ErrorExit

		cmp	al,70h
		jb	@@Next
		cmp	al,7Fh
		jbe	@@ShortConditJump

	@@Next:
		scasw				;add	di,2
		cmp	byte ptr ds:[di],0
		je	@@ErrorExit
		cmp	byte ptr ds:[di],al
		jne	@@Next
		mov	bl,ds:[di+1]
		mov	bh,0
		lea	bx,@@FirstJump[bx]
		jmp	bx

	@@FirstJump:

	@@RefJump:
		test	RefFlags,rf32
		jnz	@@ErrorExit
		lods	byte ptr es:[si]
		mov	bl,al
		mov	cl,3
		shr	al,cl
		mov	ah,al
		and	al,111b
		cmp	al,010b

	@@JumpToErrorExit:
		jb	@@ErrorExit
		cmp	al,101b
		ja	@@ErrorExit
		cmp	al,011b
		ja	@@RefCall
		mov	di,offset @@LastJump

	@@RefCall:
		shr	ah,cl
		cmp	ah,11b
		je	@@RegRef
		call	CalcMemAddress
		mov	es,dx
		cmp	MemoryFlag,3
		je	@@FarJump
		lods	word ptr es:[si]
		jmp	@@SetCurSeg

	@@RegRef:
		and	bx,111b
		shl	bx,1
		mov	bx,word ptr RegOffsTable[bx]
		mov	ax,ds:[bx]
		jmp	@@SetCurSeg


	@@ConditJump:
		cmp	CPUtype,a386
		jb	@@ErrorExit
		lods	byte ptr es:[si]
		cmp	al,80h
		jb	@@ErrorExit
		cmp	al,8Fh
		jbe	@@NearConditJump

	@@ErrorExit:
		cmp	bp,CurLineIP
		jne	@@Exit
		and	RefFlags,not rfFollow

	@@Exit:
		call	popr
		ret

	@@ShortConditJump:
		call	@@SetCondition

	@@ShortJump:
		xor	ax,ax
		lods	byte ptr es:[si]
		cbw
		jmp	@@NearJump_

	@@NearReturn:
		mov	si,CPURegs.xsp
		mov	es,CPURegs.xss
		jmp	@@NearJump

	@@FarReturn:
		mov	si,CPURegs.xsp
		mov	es,CPURegs.xss
		jmp	@@FarJump

	@@Int3:
		mov	ax,3*4
		jmp	@@GetVector

	@@IntO:
		mov	ax,4*4
		jmp	@@GetVector

	@@Int:
		xor	ax,ax
		lods	byte ptr es:[si]
		shl	ax,1
		shl	ax,1

	@@GetVector:
		xchg	ax,si
		xor	ax,ax
		mov	es,ax
		jmp	@@FarJump

	@@NearConditJump:
		call	@@SetCondition

	@@NearJump:
		lods	word ptr es:[si]

	@@NearJump_:
		add	ax,si

	@@SetCurSeg:
		mov	dx,Unasm_Seg
		jmp	@@NormalExit

	@@FarJump:
		lods	word ptr es:[si]
		xchg	ax,dx
		lods	word ptr es:[si]
		xchg	ax,dx

	@@NormalExit:
		cmp	bp,CurLineIP
		jne	@@Skip
		mov	FollowSeg,dx
		mov	FollowOffs,ax
		or	RefFlags,rfFollow
	@@Skip:
		cmp	di,offset @@LastJump
		jae	@@Exit
		or	RefFlags,rfJump
		call	AdjustAddress
		xchg	ax,bx
		xchg	dx,cx
		mov	dx,Unasm_Seg
		mov	ax,bp
		call	AdjustAddress
		cmp	dx,cx
		jb	@@Exit
		ja	@@SetDir
		cmp	ax,bx
		jb	@@JumpToExit

	@@SetDir:
		or	RefFlags,rfBackward

	@@JumpToExit:
		jmp	@@Exit

	@@LoopJump:
		sub	al,0E0h-16
		mov	ah,0
		call	@@SetCondition_
		jmp	@@ShortJump

	@@SetCondition:
		and	ax,0Fh
	@@SetCondition_:
		or	RefFlags,rfCondition
		mov	dx,ax
		xchg	ax,bx
		mov	bl,@@ConditionTable[bx]
		mov	bh,0
		add	bx,offset @@jx

	@@CheckFlag:
		mov	cx,CPURegs.xflags
		and	cx,0000100011010101b
		push	cx
		popf
		call	bx
		mov	ax,dx
		ret

	@@SetResult:
		or	RefFlags,rfTrue

	@@Return:
		ret

	@@jx:
	@@loopne:
		mov	bx,offset @@jnz
		jmp	@@CheckCX

	@@loope:
		mov	bx,offset @@jz
		jmp	@@CheckCX

	@@loop:
		mov	bx,offset @@SetResult

	@@CheckCX:
		test	RefFlags,rfOperand32
		jnz	@@2
		cmp	CPURegs.xcx,1
		jmp	@@1

	@@2:
.386
		cmp	dword ptr CPURegs.xcx,1
.8086
	@@1:
		je	@@Return
		jmp	@@CheckFlag



	@@jcxz:
		test	RefFlags,rfOperand32
		jnz	@@4
		cmp	CPURegs.xcx,0
		jmp	@@3

	@@4:
.386
		cmp	dword ptr CPURegs.xcx,0
.8086
	@@3:
		je	@@SetResult
		ret

	@@jo:
		jo	@@SetResult
		ret

	@@jno:
		jno	@@SetResult
		ret

	@@jb:
		jb	@@SetResult
		ret

	@@jnb:
		jnb	@@SetResult
		ret

	@@jz:
		jz	@@SetResult
		ret

	@@jnz:
		jnz	@@SetResult
		ret

	@@jna:
		jna	@@SetResult
		ret

	@@ja:
		ja	@@SetResult
		ret

	@@js:
		js	@@SetResult
		ret

	@@jns:
		jns	@@SetResult
		ret

	@@jp:
		jp	@@SetResult
		ret

	@@jnp:
		jnp	@@SetResult
		ret

	@@jl:
		jl	@@SetResult
		ret

	@@jnl:
		jnl	@@SetResult
		ret

	@@jng:
		jng	@@SetResult
		ret

	@@jg:
		jg	@@SetResult
		ret


@@FollowTable	label	byte
		db	0Fh				;286+
		db	@@ConditJump- @@FirstJump

		db	0E0h				;loopne/loopnz
		db	@@LoopJump	- @@FirstJump

		db	0E1h				;loope/loopz
		db	@@LoopJump	- @@FirstJump

		db	0E2h				;loop
		db	@@LoopJump	- @@FirstJump

		db	0E3h				;jcxz
		db	@@LoopJump	- @@FirstJump

		db	0E9h				;jmp near
		db	@@NearJump	- @@FirstJump

		db	0EAh				;jmp far
		db	@@FarJump	- @@FirstJump

		db	0EBh				;jmp short
		db	@@ShortJump	- @@FirstJump

	@@LastJump	label	byte
		db	9Ah				;call far
		db	@@FarJump	- @@FirstJump

		db	0E8h				;call near
		db	@@NearJump	- @@FirstJump

		db	0C2h				;ret near
		db	@@NearReturn	- @@FirstJump

		db	0C3h				;ret near
		db	@@NearReturn	- @@FirstJump

		db	0CAh				;ret far
		db	@@FarReturn	- @@FirstJump

		db	0CBh				;ret far
		db	@@FarReturn	- @@FirstJump

		db	0CCh				;int 3
		db	@@Int3		- @@FirstJump

		db	0CDh				;int
		db	@@Int		- @@FirstJump

		db	0CEh				;into
		db	@@IntO		- @@FirstJump

		db	0CFh				;iret
		db	@@FarReturn	- @@FirstJump

		db	0

@@ConditionTable	label	byte
		db	@@jo	- @@jx		;jo
		db	@@jno	- @@jx		;jno
		db	@@jb	- @@jx		;jb/jnae
		db	@@jnb	- @@jx		;jae/jnb
		db	@@jz	- @@jx		;je/jz
		db	@@jnz	- @@jx		;jne/jnz
		db	@@jna	- @@jx		;jbe/jna
		db	@@ja	- @@jx		;ja/jnbe
		db	@@js	- @@jx		;js
		db	@@jns	- @@jx		;jns
		db	@@jp	- @@jx		;jp/jpe
		db	@@jnp	- @@jx		;jnp/jpo
		db	@@jl	- @@jx		;jl/jnge
		db	@@jnl	- @@jx		;jge/jnl
		db	@@jng	- @@jx		;jle/jng
		db	@@jg	- @@jx		;jg/jnle
		db	@@loopne- @@jx		;loopne
		db	@@loope	- @@jx		;loope
		db	@@loop	- @@jx		;loop
		db	@@jcxz	- @@jx		;jcxz

RegOffsTable	label	byte
		dw	CPURegs.xax
		dw	CPURegs.xcx
		dw	CPURegs.xdx
		dw	CPURegs.xbx
		dw	CPURegs.xsp
		dw	CPURegs.xbp
		dw	CPURegs.xsi
		dw	CPURegs.xdi
		endp

AdjustAddress	proc
		push	ax
		shr	ax,1
		shr	ax,1
		shr	ax,1
		shr	ax,1
		add	dx,ax
		pop	ax
		and	ax,000Fh
		ret
		endp

SetDirection	proc
		call	pushr
		test	RefFlags,rfJump
		jz	@@Exit

		cld
		mov	si,offset Buffer + BUF_SIZE

	@@NextChar:
		dec	si
		cmp	byte ptr ds:[si],' '
		je	@@NextChar
		mov	di,si
		push	ds
		pop	es
		scasw
		mov	al,18h
		test	RefFlags,rfBackward
		jnz	@@CheckCondition
		mov	al,19h

	@@CheckCondition:
		test	RefFlags,rfCondition
		jz	@@SetDirection
		test	RefFlags,rfTrue
		jnz	@@SetDirection
		add	al,1Eh-18h

	@@SetDirection:
		stosb

	@@Exit:
		call	popr
		ret

		endp

DisplayCodeMarks	proc
		call	pushr

		xchg	ax,bp
		mov	ah,al
		mov	al,CPUwidth-1
		call	GetAddr
		push	ax

		mov	cx,10
		mov	bp,offset CodeMarkBuff
		mov	di,1

	@@NextMark:
		test	CodeMarkExist,di
		jz	@@Next
		cmp	ss:[bp],0FFFFh
		je	@@Next
		mov	ax,ss:[bp+6]
		mov	dx,ss:[bp+4]
		call	AdjustAddress
		push	bx
		xchg	ax,bx
		mov	si,Unasm_seg
		xchg	dx,si
		call	AdjustAddress
		cmp	ax,bx
		pop	bx
		jne	@@Next
		mov	ax,atCodeMark*256+'0'+10
		sub	al,cl
		cmp	dx,si
		je	@@Write

	@@Next:
		shl	di,1
		add	bp,4*2
		loop	@@NextMark
		mov	ax,atCodeMark*256+' '

	@@Write:
		pop	di
		stosw
		call	popr
		ret
		endp
