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


SearchBytes	proc
		push	ds
		pop	es

		mov	ax,SearchOptions
		mov	SearchBits,ax
		mov	si,offset SearchDialog
		call	InitDialog

	@@Again_:
		mov	SearchDlgItem,0

	@@Again:
		call	ExecDialog_
		or	ax,ax
		jz	@@ErrorExit

		mov	si,offset SearchString
		mov	di,offset SearchBin
		call	CreateBin
		jc	@@StringError

		sub	di,offset SearchBin
		jz	@@ErrorExit

	@@CheckAddress:
		push	di
		mov	si,offset SearchAddrStr
		call	GetAddress
		pop	bx
		mov	SearchDlgItem,2
		jc	@@Again

		mov	SearchLen,bx

		mov	bx,DumpSeg
		test	SearchOptions,1000b
		jz	@@DefSeg
		mov	bx,Unasm_seg

	@@DefSeg:
		cmp	ch,0
		je	@@Search
		cmp	ch,1
		je	@@SetSeg
		xor	ax,ax

	@@SetSeg:
		mov	bx,dx

	@@Search:
		mov	dx,bx
		call	LinearAddress
		test	SearchOptions,100b
		jz	@@SetSearchPos
		and	dl,0Fh

	@@SetSearchPos:
		mov	SearchOffs,ax
		mov	SearchSeg,dx

		mov	ax,SearchBits
		mov	SearchOptions,ax

		mov	ax,offset SearchNext
		push	ax

	@@ErrorExit:
		call	DrawScreen

	@@Exit:
		ret

	@@StringError:
		mov	si,offset StringErrorMsg
		call	_ErrorMessage
		jmp	@@Again_
		endp

SearchNext	proc
		mov	cx,SearchLen
		jcxz	@@NotFoundMsg

		mov	ExecMsgType,1
		mov	si,offset SearchMsg
		call	MessageBox

		xor	ax,ax
		cwd
		test	SearchOptions,10b
		jnz	@@StartSearch
		dec	dx
		sub	ax,cx
		test	SearchOptions,100b
		jz	@@StartSearch
		mov	dx,0F000h

	@@StartSearch:
		call	LinearAddress
		xchg	SearchOffs,ax
		xchg	SearchSeg,dx
		xchg	ax,di
		cld

	@@SearchByte:
		mov	si,offset SearchBin
		lodsb

		call	@@Compare
		jne	@@Next
		call	@@CompareBytes
		jne	@@Next

		push	dx
		mov	ax,di
		call	PhisicalAddress
		mov	bx,ax
		mov	si,dx
		mov	ax,offset SearchBin
		mov	dx,cs
		call	AdjustAddress
		cmp	dx,si
		pop	dx
		jne	@@Found
		cmp	ax,bx
		jne	@@Found

	@@Next:
		call	@@NextByte
		jnc	@@SearchByte

	@@NotFound:
		call	@@CloseWindow
		mov	SearchLen,0

	@@NotFoundMsg:
		mov	si,offset NotFoundMsg
		jmp	_MsgBox

	@@CloseWindow:
		mov	ExecMsgType,2
		call	_MsgBox
		mov	ExecMsgType,0
		ret

	@@Found:
		call	@@CloseWindow
		push	di
		push	dx

		call	@@NextByte
		jnc	@@SetNewPos
		mov	SearchLen,0

	@@SetNewPos:
		mov	SearchOffs,di
		mov	SearchSeg,dx

		pop	dx
		pop	ax

		call	PhisicalAddress

		test	SearchOptions,1000b
		jz	@@SetDump
		mov	CurLine,0
		mov	CurIP,ax
		mov	Unasm_seg,dx
		jmp	UpdateCommands

	@@SetDump:
		mov	DumpOffs,ax
		mov	DumpSeg,dx
		jmp	UpdateDump

	@@Compare:
		call	pushr
		call	@@UpCase
		xchg	ax,bx
		xchg	ax,di
		call	PhisicalAddress
		xchg	ax,di
		mov	es,dx
		mov	al,es:[di]
		call	@@UpCase
		cmp	al,bl
		call	popr
		ret

	@@CompareBytes:
		call	pushr
		dec	cx
		jz	@@Exit1

	@@NextChar:
		lodsb
		call	@@NextByte
		call	@@Compare
		loope	@@NextChar
	@@Exit1:
		call	popr
		ret

	@@UpCase:
		test	SearchOptions,1b
		jz	@@Exit
		jmp	UpCase

	@@NextByte:
		test	SearchOptions,10b
		jnz	@@Backward
		add	di,1
		adc	dx,0
		mov	bx,SearchOffs
		mov	bp,SearchSeg
		sub	bx,di
		sbb	bp,dx

	@@Exit:
		ret

	@@Backward:
		sub	di,1
		sbb	dx,0
		ret
		endp

LinearAddress	proc
		push	bx cx
		mov	cl,4
		mov	bx,dx
		shl	bx,cl
		rol	dx,cl
		and	dx,000Fh
		add	ax,bx
		adc	dl,0
		pop	cx bx
		ret
		endp

PhisicalAddress	proc
		cmp	dx,10h
		jb	@@SkipHMA
		sub	ax,0FFF0h
		mov	dx,0FFFFh
		ret
	@@SkipHMA:
		push	bx cx
		mov	cl,4
		mov	bx,ax
		shr	bx,cl
		ror	dx,cl
		add	dx,bx
		and	ax,000Fh
		pop	cx bx
		ret
		endp

CreateBin	proc
		mov	bp,di
		cld

	@@Next2:
		lodsb

		cmp	al,' '
		je	@@Next2

		cmp	al,';'
		je	@@Next2

		or	al,al
		jnz	@@Convert

	@@EOL:
		clc

	@@Exit:
		ret

	@@Convert:
		call	ConvertNumber
		jnc	@@Next4

		call	ConvertString
		jnc	@@Next4

		call	ConvertCommand
		jc	@@Exit

	@@Next4:
		lodsb
		cmp	al,';'
		je	@@Next2
		or	al,al
		je	@@EOL
		cmp	al,' '
		je	@@Next2
		stc
		ret
		endp

ConvertString	proc
		cmp	al,'"'
		je	@@Convert
		cmp	al,"'"
		jne	@@ErrorExit

	@@Convert:
		mov	dl,al

	@@Next:
		lodsb
		or	al,al
		jz	@@ErrorExit
		cmp	al,dl
		je	@@Exit
		stosb
		jmp	@@Next

	@@ErrorExit:
		stc

	@@Exit:
		ret
		endp

ConvertNumber	proc
		mov	bx,si
		mov	cx,di
		dec	bx
		call	@@IsHexNumber
		jc	@@ErrorExit
		xchg	ax,dx
		lodsb
		call	@@IsHexNumber
		jnc	@@Add
		xchg	ax,dx
		jmp	@@Save

	@@Add:
		shl	dx,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		add	ax,dx

	@@Save:
		stosb
		mov	al,ds:[si]
		cmp	al,' '
		je	@@Ok
		cmp	al,';'
		je	@@Ok
		cmp	al,0
		jne	@@ErrorExit

	@@Ok:
		clc
		ret

	@@IsHexNumber:
		call	UpCase
		sub	al,'0'
		jb	@@ErrorExit
		cmp	al,9
		jbe	@@Ok
		sub	al,'A'-'0'
		jb	@@ErrorExit
		cmp	al,5
		ja	@@ErrorExit
		add	al,10
		clc
		ret

	@@ErrorExit:
		mov	si,bx
		mov	di,cx
		lodsb
		stc
		ret
		endp

ConvertCommand	proc
		dec	si
		mov	bx,si

	@@Next:
		lodsb
		cmp	al,';'
		je	@@EOA
		or	al,al
		jne	@@Next

	@@EOA:
		dec	si
		push	ds:[si]
		push	si

		mov	byte ptr ds:[si],0
		mov	si,bx
		mov	bx,di
		sub	bx,bp
		add	bx,CPURegs.xip

		call	Assemble

		pop	si
		pop	ds:[si]
		ret
		endp

