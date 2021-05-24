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


Assemble	proc
  ;ds:si -> assembling line
  ;es:di -> result buffer
  ;bx    =  jump/call offset origin

		cld
		mov	AsmSeg,es
		mov	AsmOfs,di
		mov	JumpOriginOfs,bx
		mov	AsmSP,sp
		push	cs
		pop	es
		mov	RepPref,0
@@FindNext:
		call	FindName

		cmp	bx,offset RepPrefixes
		jb	NotRep
		mov	al,[bx+5]
		mov	RepPref,al
		jmp	@@FindNext
NotRep:
		call	UpLine

		cmp	RepPref,0
		je	@@NoRep
		cmp	byte ptr [bx+6],-1
		je	@@NoRep
		jmp	RepError
@@NoRep:
		mov	JumpFlag,0
		cmp	bx,offset Jumps&calls
		jb	NotJumps
		mov	JumpFlag,1
NotJumps:
		mov	ParamCount,0
		mov	CurrPrefix,-1
		mov	di,offset Parameter1
		call	GetParameter
		jc	@@NoParams
		inc	ParamCount
		mov	di,offset Parameter2
		call	GetParameter
		jc	OneParam
		inc	ParamCount
		mov	al,Parameter1.WordFlag
		mov	ah,Parameter2.WordFlag
;		or	ax,ax
;		jz	TypeWarning
		cmp	bx,offset DiffTypes
		jae	@@1
		cmp	al,0
		je	@@1
		cmp	ah,0
		je	@@1
		cmp	al,ah
		jne	RegError
@@1:
		call	SkipSpaces
		jz	OneParam
		jmp	ExtraError
OneParam:
;		cmp	Parameter1.WordFlag,0
;		jz	TypeWarning
@@NoParams:
		mov	di,offset AsmBuffer
		mov	cl,[bx]
		lea	si,[bx+3]
		mov	ch,0
NextProc:
		push	cx
		mov	bl,Parameter1.MemFlag
		mov	bh,Parameter2.MemFlag
		mov	cl,Parameter1.RegNumber
		mov	ch,Parameter2.RegNumber
		lodsw
		xchg	ax,dx
		lodsw
		call	dx
		pop	cx
		loopne	NextProc
		jne	WrongParam

		mov	es,AsmSeg
		mov	di,AsmOfs
		mov	si,offset AsmBuffer
		lodsb
		mov	cl,al
		mov	ch,0
		cmp	RepPref,0
		jz	@@ChPr
		mov	al,RepPref
		stosb
@@ChPr:
		cmp	CurrPrefix,-1
		je	@@Copy
;		cmp	Parameter1.MemFlag,0
;		jne	@@Mem1
;		cmp	Parameter2.MemFlag,0
;		je	@@Copy
;@@Mem1:
		mov	al,CurrPrefix
		mov	bx,offset PrefCodes
		xlat
		stosb
@@Copy:
		rep	movsb

		clc
		ret

RepError:
		mov	ax,erRepError
		jmp	AsmError
;TypeWarning:
;		mov	ax,erParamType
;		jmp	AsmError
RegError:
		mov	ax,erRegBits
		jmp	AsmError
WrongParam:
		mov	ax,erInvalidParam
		jmp	AsmError
MnemError:
		mov	ax,erMnemonic
AsmError:				; SI=error place
		mov	sp,AsmSP	; AX=error code
		stc
		ret
endp
AsmSeg		dw	0
AsmOfs		dw	0
AsmSP		dw	0
JumpOriginOfs	dw	0

erEmpty		equ	1
erMnemonic	equ	2
erInvalidParam	equ	3
erMemParam	equ	4
erExtra		equ	5
erBigNumber	equ	6
erRegBits	equ	7
erParamType	equ	8
erRepError	equ	8

regAL		equ	0
regCL		equ	1
regDL		equ	2
regBL		equ	3
regAH		equ	4
regCH		equ	5
regDH		equ	6
regBH		equ	7
regAX		equ	8
regCX		equ	9
regDX		equ	10
regBX		equ	11
regSP		equ	12
regBP		equ	13
regSI		equ	14
regDI		equ	15

MaxLetter	equ	7
;------------------------------------------------------------------------------
FindName	proc
		call	SkipSpaces
		jz	NoName
		mov	di,offset NameBuf
		mov	cx,MaxLetter
@@NextLetter:
		lodsb
		call	TestSpace
		jz	EndName
		call	LowCase
		stosb
		loop	@@NextLetter
		jmp	MnemError
EndName:
		dec	si
		mov	al,0
		stosb
		push	si
		mov	cx,di
		mov	bx,offset AsmTable
		mov	si,offset NameBuf
		sub	cx,si
NextMnem:
		mov	di,[bx+1]
		push	cx si
		repe	cmpsb
		pop	si cx
		je	@@Found
		mov	al,[bx]
		cbw
		shl	ax,1
		shl	ax,1
		add	bx,ax
		add	bx,3
		or	ax,ax
		jne	NextMnem
		jmp	MnemError
@@Found:
;		mov	CurCmd,bx
		pop	si
		ret
NoName:
		mov	ax,erEmpty
		jmp	AsmError
endp

;------------------------------------------------------------------------------
GetParameter	proc
		call	SkipSpaces
		jnz	@@1
		stc
		jmp	@@Quit
@@1:
		xor	ax,ax
		mov	[di].MemFlag,al
		mov	[di].WordFlag,al
		mov	[di].Value,ax
		mov	[di].RegNumber,-1
		mov	MinusFlag,al
@@Next:
		mov	ax,[si]
		call	GetXPtr
		jz	@@Next
		cmp	al,'$'
		je	@@J$
		cmp	al,'['
		jne	@@Skip
		jmp	@@MemStart		;jump out of range
@@Skip:
		call	FindSegReg
		jz	SegRegParam
		call	FindReg
		je	RegParam		;jump out of range
		cmp	al,'-'
		jne	@@NM
		mov	MinusFlag,1
		inc	si
@@NextVal:
		mov	ax,[si]
@@NM:
		call	TestHex
		jc	JumpToWrongParam1		;jump out of range
		call	GetValue
@@AddOfs:
		cmp	MinusFlag,0
		je	@@PL
		neg	dx
		mov	MinusFlag,0
@@PL:
		add	[di].Value,dx
		lodsb
		cmp	al,'+'
		je	@@NextVal
		cmp	al,'-'
		jne	@@EMP
		mov	MinusFlag,1
		jmp	@@NextVal

@@J$:

;		mov	dx,AsmOfs
		mov	dx,JumpOriginOfs

		inc	si
		jmp	@@AddOfs

SegRegParam:
		cmp	byte ptr [si+2],':'
		je	@@Prefix
		mov	dx,word ptr [bx+1]
		cmp	dx,offset cmov
		je	@@okp
		cmp	dx,offset cpush
		je	@@okp
		cmp	dx,offset cpop
		je	@@okp
JumpToWrongParam1:
		jmp	WrongParam

RegParam:
		mov	[di].RegNumber,al
		mov	dl,1
		test	al,1000b
		jz	ByteReg
		inc	dl
ByteReg:
		mov	[di].WordFlag,dl
@@CheckExtra:
		inc	si
		inc	si
		jmp	CheckExtra

@@EMP:
		dec	si
		cmp	JumpFlag,0
		je	CheckExtra
		lodsb
		cmp	al,':'
		je	FarJump
		dec	si
CheckExtra:
		call	SkipSpaces
		jz	@@JumpToOkQuit		;jump out of range
		cmp	al,','
		jnz	ExtraError		;jump out of range
		inc	si
@@JumpToOkQuit:
		jmp	@@OkQuit

ExtraError:
		mov	ax,erExtra
		jmp	AsmError

FarJump:
		mov	al,[si]
		call	TestHex
		jc	CheckExtra
		call	GetValue
		mov	FarValue,dx
		mov	JumpFlag,2
		jmp	CheckExtra

@@okp:
		add	al,10h
		mov	[di].RegNumber,al
		mov	[di].WordFlag,2
		jmp	@@CheckExtra
@@Prefix:
		cmp	CurrPrefix,-1
		jne	JumpToWrongParam1		;jump out of range
		mov	CurrPrefix,al
		add	si,3
		call	SkipSpaces
		jmp	@@Next

@@MemStart:
		mov	[di].MemFlag,1
		xor	ax,ax
		mov	word ptr [di].BXflag,ax
		mov	word ptr [di].SIflag,ax
		inc	si
@@Plus:
NextMemParam:
		call	SkipSpaces
		jz	JumpToBadMemParam		;jump out of range

		mov	ax,[si]
		cmp	al,'$'
		je	@@$
		call	FindReg
		je	MemReg
@@GetValue:
		call	TestHex
		jc	JumpToBadMemParam		;jump out of range
		call	GetValue
		cmp	MinusFlag,0
		je	@@2
		neg	dx
		mov	MinusFlag,0
@@2:
		add	[di].Value,dx
		jmp	@@3
@@$:

;		mov	dx,AsmOfs
		mov	dx,JumpOriginOfs

		inc	si
		jmp	@@2
MemNext:
		inc	si
		inc	si
@@3:
		call	SkipSpaces
		jz	JumpToBadMemParam

		lodsb
		cmp	al,']'
		je	MemEnd
		cmp	al,'+'
		je	@@Plus
		cmp	al,'-'
		je	@@Minus
JumpToBadMemParam:
		jmp	BadMemParam
@@Minus:
		mov	MinusFlag,1
		call	SkipSpaces
		jnz	@@GetValue
		jmp	BadMemParam
MemReg:
		cmp	al,regBX
		jne	@@BP
		cmp	word ptr [di].BXflag,0
		jne	BadMemParam
		inc	[di].BXflag
		jmp	MemNext
@@BP:
		cmp	al,regBP
		jne	@@SI
		cmp	word ptr [di].BXflag,0
		jne	BadMemParam
		inc	[di].BPflag
		jmp	MemNext
@@SI:
		cmp	al,regSI
		jne	@@DI
		cmp	word ptr [di].SIflag,0
		jne	BadMemParam
		inc	[di].SIflag
		jmp	MemNext
@@DI:
		cmp	al,regDI
		jne	BadMemParam
		cmp	word ptr [di].SIflag,0
		jne	BadMemParam
		inc	[di].DIflag
		jmp	MemNext

MemEnd:
		jmp	CheckExtra
@@OkQuit:
		clc
@@Quit:
		ret
BadMemParam:
		mov	ax,erMemParam
		jmp	AsmError
endp

;------------------------------------------------------------------------------
GetXPtr		proc
		cmp	[di].WordFlag,0
		jne	@@4
		cmp	al,'W'
		je	@@CheckWord
		cmp	JumpFlag,1
		je	JumpXPtr
		cmp	al,'B'
		je	@@CheckByte
@@4:
		ret
JumpXPtr:
		cmp	ax,'AF'
		je	@@CheckFar
		cmp	ax,'WD'
		je	@@CheckDW
		cmp	ax,'HS'
		je	@@CheckShort
		ret
@@CheckShort:
		lodsw
		lodsw
		cmp	ax,'RO'
		je	@@CS1
		ret
@@CS1:
		lodsb
		cmp	al,'T'
		je	@@CheckPtr
		ret
@@CheckFar:
		lodsw
		lodsb
		mov	dl,3
		cmp	al,'R'
		je	@@CheckPtr
		ret
@@CheckDW:
		lodsw
		lodsw
		mov	dl,3
		cmp	ax,'RO'
		je	@@CD2
		ret
@@CD2:
		lodsb
		cmp	al,'D'
		je	@@CheckPtr
@@CheckByte:
		mov	dl,1
		cmp	ah,'/'
		je	@@Ptr1
		cmp	ah,'Y'
		je	@@CheckByte1
		ret
@@CheckWord:
		mov	dl,2
		cmp	ah,'/'
		je	@@Ptr1
		cmp	ah,'O'
		je	@@CheckWord1
		ret
@@Ptr1:
		mov	[di].WordFlag,dl
		inc	si
		inc	si
@@Quit:
		call	SkipSpaces
		jz	JumpToWrongParam2
		cmp	ax,ax
		ret
@@CheckByte1:
		lodsw
		lodsw
		cmp	ax,'ET'
		je	@@CheckPtr
		ret
@@CheckWord1:
		lodsw
		lodsw
		cmp	ax,'DR'
		je	@@CheckPtr
		ret
@@CheckPtr:
		call	SkipSpaces
		jnz	@@NotSpc
JumpToWrongParam2:
		jmp	WrongParam
@@NotSpc:
		cmp	al,'['
		je	@@Ok
		cmp	al,'$'
		je	@@Ok
		call	TestHex
		jnc	@@Ok
		lodsw
		cmp	ax,'TP'
		jne	@@Quit2
		lodsb
		cmp	al,'R'
		je	@@Ok
@@Quit2:
		ret
@@Ok:
		mov	[di].WordFlag,dl
		jmp	@@Quit
endp

;------------------------------------------------------------------------------
FindSegReg	proc
		push	di
		mov	di,offset AsmSegRegs
		mov	cx,4
		repne	scasw
		jne	@@Quit
		lea	ax,[di-(AsmSegRegs-@Startup+102h)]
		shr	ax,1
		cmp	ax,ax
@@Quit:
		pop	di
		ret
endp
AsmSegRegs	db	'ESCSSSDS'

;------------------------------------------------------------------------------
FindReg		proc
		push	di
		mov	di,offset AsmRegs
		mov	cx,16
		repne	scasw
		jne	@@Quit
		lea	ax,[di-(AsmRegs-@Startup+102h)]
		shr	ax,1
		cmp	ax,ax
@@Quit:
		pop	di
		ret
endp
AsmRegs		db	'ALCLDLBLAHCHDHBH'
		db	'AXCXDXBXSPBPSIDI'

;------------------------------------------------------------------------------
TestHex		proc
		cmp	al,'0'
		jb	@@not
		cmp	al,'9'
		jbe	@@Hex
		cmp	al,'A'
		jb	@@not
		cmp	al,'F'
		ja	@@not
@@Hex:
		clc
		ret
@@not:
		stc
		ret
endp

;------------------------------------------------------------------------------
GetValue	proc
		xor	dx,dx
@@Next:
		lodsb
		sub	al,'0'
		jb	@@end
		cmp	al,9
		jbe	@@Digit
		cmp	al,'A'-'0'
		jb	@@end
		cmp	al,'F'-'0'
		ja	@@end
		sub	al,7
@@Digit:
		test	dh,0F0h
		jnz	BigNumError
		shl	dx,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		or	dl,al
		jmp	@@Next
@@end:
		dec	si
		ret
BigNumError:
		mov	ax,erBigNumber
		jmp	AsmError
endp

;------------------------------------------------------------------------------
SkipSpaces	proc
@@Next:
		lodsb
		cmp	al,' '
		je	@@Next
		cmp	al,9
		je	@@Next
		dec	si
		cmp	al,0
		je	@@Ok
		cmp	al,';'
@@Ok:		ret
endp

;------------------------------------------------------------------------------
TestSpace	proc
		cmp	al,' '
		je	@@Ok
		cmp	al,9
		je	@@Ok
		cmp	al,0
		je	@@Ok
		cmp	al,';'
@@Ok:
		ret
endp

;------------------------------------------------------------------------------
UpLine		proc
		push	si di
		mov	di,si
@@2:
		lodsb
		cmp	al,0
		je	@@1
		call	UpCase
		stosb
		jmp	@@2
@@1:
		pop	di si
		ret
endp

;------------------------------------------------------------------------------
LowCase		proc
		cmp	al,'A'
		jb	@@1
		cmp	al,'Z'
		ja	@@1
		add	al,20h
@@1:
		ret
		endp

ifndef __ACT__
;------------------------------------------------------------------------------
UpCase		proc
		cmp	al,'a'
		jb	@@1
		cmp	al,'z'
		ja	@@1
		sub	al,20h
@@1:
		ret
		endp
endif

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
AsmTable	label	byte
		db	3
		dw	cadd
		dw	_r_rm
		db	0, 0
		dw	_a_im
		db	4, 0
		dw	_rm_im
		db	80h, 0

		db	3
		dw	cor
		dw	_r_rm
		db	8, 0
		dw	_a_im
		db	0Ch, 0
		dw	_rm_im
		db	80h, 1

		db	3
		dw	cadc
		dw	_r_rm
		db	10h, 0
		dw	_a_im
		db	14h, 0
		dw	_rm_im
		db	80h, 2

		db	3
		dw	csbb
		dw	_r_rm
		db	18h, 0
		dw	_a_im
		db	1Ch, 0
		dw	_rm_im
		db	80h, 3

		db	3
		dw	cand
		dw	_r_rm
		db	20h, 0
		dw	_a_im
		db	24h, 0
		dw	_rm_im
		db	80h, 4

		db	3
		dw	csub
		dw	_r_rm
		db	28h, 0
		dw	_a_im
		db	2Ch, 0
		dw	_rm_im
		db	80h, 5

		db	3
		dw	cxor
		dw	_r_rm
		db	30h, 0
		dw	_a_im
		db	34h, 0
		dw	_rm_im
		db	80h, 6

		db	3
		dw	ccmp
		dw	_r_rm
		db	38h, 0
		dw	_a_im
		db	3Ch, 0
		dw	_rm_im
		db	80h, 7

		db	3
		dw	ctest
		dw	_r_rm
		db	84h, 0
		dw	_a_im
		db	0A8h, 0
		dw	_rm_im
		db	0F6h, 0

		db	5
		dw	cmov
		dw	_sr_rm
		db	8Ch, 0
		dw	_r_im
		db	0B0h, 0
		dw	_a_mem
		db	0A0h, 0
		dw	_r_rm
		db	88h, 0
		dw	_rm_im1
		db	0C6h, 0

		db	3
		dw	cpush
		dw	_sreg
		db	6, 0
		dw	_reg16
		db	50h,0
		dw	_rm
		db	0FFh, 6

		db	3
		dw	cpop
		dw	_sreg
		db	7, 0
		dw	_reg16
		db	58h,0
		dw	_rm
		db	08Fh, 6

		db	1
		dw	clea
		dw	_r_m16
		db	8Dh, 0

		db	1
		dw	cles
		dw	_r_m16
		db	0C4h, 0

		db	1
		dw	clds
		dw	_r_m16
		db	0C5h, 0

		db	2
		dw	cinc
		dw	_reg16
		db	40h,0
		dw	_rm
		db	0FEh, 0

		db	2
		dw	cdec
		dw	_reg16
		db	48h,0
		dw	_rm
		db	0FEh, 1

		db	1
		dw	cmul
		dw	_rm
		db	0F6h, 100b

		db	1
		dw	cimul
		dw	_rm
		db	0F6h, 101b

		db	1
		dw	cdiv
		dw	_rm
		db	0F6h, 110b

		db	1
		dw	cidiv
		dw	_rm
		db	0F6h, 111b

		db	1
		dw	cneg
		dw	_rm
		db	0F6h, 011b

		db	1
		dw	cnot
		dw	_rm
		db	0F6h, 010b

		db	2
		dw	cxchg
		dw	_a_reg
		db	90h, 0
		dw	_r_rm
		db	86h, 0

		db	1
		dw	cesc
		dw	_esc
		db	0D8h, 0

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

		db	1
		dw	cdb
		dw	_db
		db	0, 0

		db	1
		dw	cdw
		dw	_dw
		db	0, 0

		db	2
		dw	caam
		dw	_im8
		db	0D4h, 0
		dw	_imn
		db	0D4h, 0Ah

		db	2
		dw	caad
		dw	_im8
		db	0D5h, 0
		dw	_imn
		db	0D5h, 0Ah

		db	1
		dw	cdaa
		dw	_noparam
		db	27h,0

		db	1
		dw	cdas
		dw	_noparam
		db	2Fh,0

		db	1
		dw	caaa
		dw	_noparam
		db	37h,0

		db	1
		dw	caas
		dw	_noparam
		db	3Fh,0

		db	1
		dw	cnop
		dw	_noparam
		db	90h,0

		db	1
		dw	ccbw
		dw	_noparam
		db	98h,0

		db	1
		dw	ccwd
		dw	_noparam
		db	99h,0

		db	1
		dw	cwait
		dw	_noparam
		db	9Bh,0

		db	1
		dw	cpushf
		dw	_noparam
		db	9Ch,0

		db	1
		dw	cpopf
		dw	_noparam
		db	9Dh,0

		db	1
		dw	csahf
		dw	_noparam
		db	9Eh,0

		db	1
		dw	clahf
		dw	_noparam
		db	9Fh,0

		db	1
		dw	cmovsb
		dw	_noparam
		db	0A4h,-1

		db	1
		dw	cmovsw
		dw	_noparam
		db	0A5h,-1

		db	1
		dw	ccmpsb
		dw	_noparam
		db	0A6h,-1

		db	1
		dw	ccmpsw
		dw	_noparam
		db	0A7h,-1

		db	1
		dw	cstosb
		dw	_noparam
		db	0AAh,-1

		db	1
		dw	cstosw
		dw	_noparam
		db	0ABh,-1

		db	1
		dw	clodsb
		dw	_noparam
		db	0ACh,-1

		db	1
		dw	clodsw
		dw	_noparam
		db	0ADh,-1

		db	1
		dw	cscasb
		dw	_noparam
		db	0AEh,-1

		db	1
		dw	cscasw
		dw	_noparam
		db	0AFh,-1

		db	2
		dw	cret
		dw	_im16
		db	0C2h, 0
		dw	_noparam
		db	0C3h,0

		db	2
		dw	cretn
		dw	_im16
		db	0C2h, 0
		dw	_noparam
		db	0C3h,0

		db	2
		dw	cretf
		dw	_im16
		db	0CAh, 0
		dw	_noparam
		db	0CBh,0

		db	1
		dw	cint3
		dw	_noparam
		db	0CCh,0

		db	1
		dw	cint
		dw	_im8ch3
		db	0CDh, 0

		db	1
		dw	cinto
		dw	_noparam
		db	0CEh,0

		db	1
		dw	ciret
		dw	_noparam
		db	0CFh,0

		db	1
		dw	cxlat
		dw	_noparam
		db	0D7h,0

		db	1
		dw	plock
		dw	_noparam
		db	0F0h,0

		db	1
		dw	chlt
		dw	_noparam
		db	0F4h,0

		db	1
		dw	ccmc
		dw	_noparam
		db	0F5h,0

		db	1
		dw	cclc
		dw	_noparam
		db	0F8h,0

		db	1
		dw	cstc
		dw	_noparam
		db	0F9h,0

		db	1
		dw	ccli
		dw	_noparam
		db	0FAh,0

		db	1
		dw	csti
		dw	_noparam
		db	0FBh,0

		db	1
		dw	ccld
		dw	_noparam
		db	0FCh,0

		db	1
		dw	cstd
		dw	_noparam
		db	0FDh,0

DiffTypes:

		db	1
		dw	cshl
		dw	_rm_cl_1
		db	0D0h, 100b

		db	1
		dw	csal
		dw	_rm_cl_1
		db	0D0h, 100b

		db	1
		dw	cshr
		dw	_rm_cl_1
		db	0D0h, 101b

		db	1
		dw	csar
		dw	_rm_cl_1
		db	0D0h, 111b

		db	1
		dw	crol
		dw	_rm_cl_1
		db	0D0h, 000b

		db	1
		dw	cror
		dw	_rm_cl_1
		db	0D0h, 001b

		db	1
		dw	crcl
		dw	_rm_cl_1
		db	0D0h, 010b

		db	1
		dw	crcr
		dw	_rm_cl_1
		db	0D0h, 011b


		db	2
		dw	cin
		dw	_a_dx
		db	0ECh, 0
		dw	_a_im8
		db	0E4h, 0

		db	2
		dw	cout
		dw	_dx_a
		db	0EEh, 0
		dw	_im8_a
		db	0E6h, 0

Jumps&calls:
		db	4
		dw	cjmp
		dw	_short
		db	0EBh, 0
		dw	_near
		db	0E9h, 0
		dw	_far
		db	0EAh, 0
		dw	_rmj
		db	0FFh, 100b

		db	3
		dw	ccall
		dw	_near
		db	0E8h, 0
		dw	_far
		db	9Ah, 0
		dw	_rmj
		db	0FFh, 010b

		db	1
		dw	cjo
		dw	_short
		db	070h, 0

		db	1
		dw	cjno
		dw	_short
		db	071h, 0

		db	1
		dw	cjb
		dw	_short
		db	072h, 0

		db	1
		dw	cjc
		dw	_short
		db	072h, 0

		db	1
		dw	cjnae
		dw	_short
		db	072h, 0

		db	1
		dw	cjnb
		dw	_short
		db	073h, 0

		db	1
		dw	cjnc
		dw	_short
		db	073h, 0

		db	1
		dw	cjae
		dw	_short
		db	073h, 0

		db	1
		dw	cje
		dw	_short
		db	074h, 0

		db	1
		dw	cjz
		dw	_short
		db	074h, 0


		db	1
		dw	cjne
		dw	_short
		db	075h, 0

		db	1
		dw	cjnz
		dw	_short
		db	075h, 0

		db	1
		dw	cjbe
		dw	_short
		db	076h, 0

		db	1
		dw	cjna
		dw	_short
		db	076h, 0

		db	1
		dw	cjnbe
		dw	_short
		db	077h, 0

		db	1
		dw	cja
		dw	_short
		db	077h, 0


		db	1
		dw	cjs
		dw	_short
		db	078h, 0

		db	1
		dw	cjns
		dw	_short
		db	079h, 0

		db	1
		dw	cjp
		dw	_short
		db	07Ah, 0

		db	1
		dw	cjpe
		dw	_short
		db	07Ah, 0

		db	1
		dw	cjnp
		dw	_short
		db	07Bh, 0

		db	1
		dw	cjpo
		dw	_short
		db	07Bh, 0

		db	1
		dw	cjl
		dw	_short
		db	07Ch, 0

		db	1
		dw	cjnge
		dw	_short
		db	07Ch, 0

		db	1
		dw	cjnl
		dw	_short
		db	07Dh, 0

		db	1
		dw	cjge
		dw	_short
		db	07Dh, 0

		db	1
		dw	cjle
		dw	_short
		db	07Eh, 0

		db	1
		dw	cjng
		dw	_short
		db	07Eh, 0

		db	1
		dw	cjnle
		dw	_short
		db	07Fh, 0

		db	1
		dw	cjg
		dw	_short
		db	07Fh, 0

		db	1
		dw	cloop
		dw	_short
		db	0E2h, 0

		db	1
		dw	cloopne
		dw	_short
		db	0E0h, 0

		db	1
		dw	cloopnz
		dw	_short
		db	0E0h, 0

		db	1
		dw	cloope
		dw	_short
		db	0E1h, 0

		db	1
		dw	cloopz
		dw	_short
		db	0E1h, 0

		db	1
		dw	cjcxz
		dw	_short
		db	0E3h, 0

RepPrefixes:
		db	1
		dw	crep
		dw	0
		db	0F3h, 0

		db	1
		dw	crepe
		dw	0
		db	0F3h, 0

		db	1
		dw	crepz
		dw	0
		db	0F3h, 0

		db	1
		dw	crepne
		dw	0
		db	0F2h, 0

		db	1
		dw	crepnz
		dw	0
		db	0F2h, 0

comment #
Asm286table	label	byte
		db	1
		dw	cpusha
		dw	_noparam
		db	60h, 0

		db	1
		dw	cpopa
		dw	_noparam
		db	61h, 0

		db	1
		dw	cbound
		dw	_r_m16
		db	62h, 0

		db	1
		dw	carpl
		dw	_rm_r16
#
		db	0		; End of table


;------------------------------------------------------------------------------
_r_m16		proc
		cmp	bx,0100h
		jne	@@Quit
		cmp	cl,-1
		je	@@Quit
		test	cl,1000b
		jz	@@Quit
		mov	LxxFlag,1
		jmp	_r_rm2
@@Quit:
		or	sp,sp
		ret
endp
;------------------------------------------------------------------------------
_r_rm		proc
		mov	LxxFlag,0
_r_rm2:
		cmp	ParamCount,2
		jne	@@Q			;jump out of range
		cmp	bx,0001h
		je	@@M_R
		cmp	bx,0100h
		je	@@R_M
		or	bx,bx
		jne	@@Q
@@R_R:
		cmp	cl,-1
		je	@@Q
		cmp	ch,-1
		je	@@Q
		mov	byte ptr [di],2
		cmp	LxxFlag,1
		je	@@L1
		call	AddWordFlag
@@L1:
		mov	[di+1],al
		call	MakeModrm
;;		mov	al,cl
;		and	al,0111b
;		or	al,11000000b
		and	ch,0111b
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		jmp	@@Ok

@@Q:
ExitZFclear:
		or	sp,sp
		ret
@@Ok:
		cmp	ax,ax
		ret

@@M_R:
		cmp	ch,-1
		je	@@Q
@@MakeMODRM:
		cmp	LxxFlag,1
		je	@@L2
		call	AddWordFlag
@@L2:
		mov	[di+1],al

		call	MakeModrm

		mov	dx,Parameter1.Value
		add	ah,2
		mov	[di],ah
@@SetReg:
		mov	[di+3],dx
		mov	ah,Parameter2.RegNumber
		and	ah,0111b
		shl	ah,1
		shl	ah,1
		shl	ah,1
		or	al,ah
		mov	[di+2],al
		jmp	@@Ok

@@R_M:
		cmp	cl,-1
		je	@@Q
		cmp	LxxFlag,1
		je	@@7
		or	al,2
@@7:
		call	SwapParameters
		xchg	cl,ch
		jmp	@@MakeMODRM

endp

;------------------------------------------------------------------------------
_a_im		proc
		cmp	ParamCount,2
		jne	@@Quit
		cmp	cl,0FFh
		je	ExitZFclear
		and	cl,0111b
		cmp	cl,regAL
		jne	@@Quit
		cmp	bh,0
		jne	@@Quit
		call	AddWordFlag
		mov	dx,Parameter2.Value
		or	ah,ah
		jnz	@@1
		call	CheckByte
		jz	@@1
		or	dh,dh
		jnz	JumpToBigNumError		;jump out of range
@@1:
		add	ah,2
		mov	[di],ah
		mov	[di+1],al
		mov	[di+2],dx
		cmp	ax,ax
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_rm_im1		proc
		mov	MovFlag,1
		jmp	_rm_im2
endp

;------------------------------------------------------------------------------
_db		proc
		cmp	ParamCount,1
		jne	@@Quit
		or	bl,bl
		jnz	@@Quit
		cmp	cl,-1
		jne	@@Quit
		mov	ax,Parameter1.Value
		or	ah,ah
		jnz	JumpToBigNumError		;jump out of range
		mov	byte ptr [di],1
		mov	[di+1],al
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_rm_im		proc
		mov	MovFlag,0
_rm_im2:
		cmp	ParamCount,2
		jne	@@Quit
		cmp	ch,0FFh
		jne	@@Quit
		cmp	bx,0001h
		je	@@M_I
		or	bx,bx
		jnz	@@Quit
@@R_I:
		mov	ch,ah
		call	AddWordFlag
		jmp	@@Make
@@Ok:
		cmp	ax,ax
@@Quit:
		ret
@@M_I:
		mov	ch,ah
;		or	al,1		;; WordFlag:=1
@@Make:
		mov	dx,Parameter2.Value
		mov	bl,Parameter1.WordFlag
		mov	cl,4
		call	CheckByte
		jz	@@ByteImm
		or	dh,dh
		jz	@@ByteImm2
		or	al,1
		cmp	bl,1
		jne	@@WordImm
JumpToBigNumError:
		jmp	BigNumError
@@ByteImm2:
		dec	cl
		cmp	bl,2
		jb	@@Byte
		inc	cl
		or	al,1
		jmp	@@WordImm
@@ByteImm:
		dec	cl
		cmp	bl,2
		jb	@@Byte
		or	al,3
		cmp	MovFlag,0
		je	@@Byte
		inc	cl
@@Byte:
@@WordImm:
		mov	[di+1],al
		call	MakeModrm	; al=md000r/m, ah=size of offset(0,1,2)
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		add	cl,ah
		mov	[di],cl
		mov	[di+2],al
		mov	bl,ah
		mov	bh,0
		mov	cx,Parameter1.Value
		mov	[di+3],cx
		mov	[di+bx+3],dx
		jmp	@@Ok
endp

;------------------------------------------------------------------------------
_esc		proc
		cmp	ParamCount,2
		jne	@@Quit
		or	bl,bl
		jnz	@@Quit
		cmp	cl,-1
		jne	@@Quit
		mov	bl,ch
		cmp	bx,00FFh
		je	@@Quit
		xchg	cl,ch
		call	SwapParameters
		call	MakeModrm
		add	ah,2
		mov	[di],ah
		mov	dx,Parameter2.Value
		cmp	dx,64
		jae	JumpToBigNumError		;jump out of range
		mov	dh,dl
		and	dh,00111000b
		or	al,dh
		mov	[di+2],al
		mov	al,dl
		and	al,0111b
		or	al,0D8h
		mov	[di+1],al
		mov	dx,Parameter1.Value
		mov	[di+3],dx
		cmp	ax,ax
		ret
@@Quit:
		or	sp,sp
		ret
endp

;------------------------------------------------------------------------------
_a_mem		proc
		cmp	ParamCount,2
		jne	@@Quit
		cmp	bx,0001h
		je	@@M_A
		cmp	bx,0100h
		jne	@@Quit
		and	cl,0111b
		cmp	cl,regAL
		jne	@@Quit
		cmp	word ptr Parameter2.BXflag,0
		jne	@@Quit
		cmp	word ptr Parameter2.SIflag,0
		jne	@@Quit
		mov	dx,Parameter2.Value
@@Make:
		call	AddWordFlag
		mov	[di+1],al
		mov	byte ptr [di],3
		mov	[di+2],dx
		cmp	ax,ax
		ret
@@M_A:
		and	ch,0111b
		cmp	ch,regAL
		jne	@@Quit
		cmp	word ptr Parameter1.BXflag,0
		jne	@@Quit
		cmp	word ptr Parameter1.SIflag,0
		jne	@@Quit
		or	al,0010b
		mov	dx,Parameter1.Value
		jmp	@@Make
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_r_im		proc
		cmp	ParamCount,2
		jne	@@Quit
		or	bx,bx
		jnz	@@not
		cmp	cl,-1
		jz	@@not
		cmp	ch,-1
		jnz	@@not
		or	al,cl
		mov	ah,3
		mov	dx,Parameter2.Value
		test	al,1000b
		jnz	@@Word
		dec	ah
		call	CheckByte
		jz	@@Word
		or	dh,dh
		jz	@@Word
		jmp	BigNumError
@@Word:
		mov	[di],ah
		mov	[di+1],al
		mov	[di+2],dx
		cmp	ax,ax
		ret
@@not:
		or	sp,sp
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_sr_rm		proc
		cmp	ParamCount,2
		jne	@@Quit
		cmp	bx,0001h
		je	@@M_R
		cmp	bx,0100h
		je	@@R_M
		or	bx,bx
		jnz	@@Quit
@@R_R:
		cmp	cl,-1
		je	@@Quit
		cmp	ch,-1
		je	@@Quit
		cmp	cl,10h
		jae	@@5
		cmp	ch,10h
		jae	@@M_S
		jmp	@@Quit
@@5:
		cmp	ch,10h
		jb	@@S_M
		jmp	WrongParam
@@M_R:
		cmp	ch,-1
		je	@@Quit
		cmp	ch,10h
		jb	@@Quit
@@M_S:
		mov	[di+1],al
		call	MakeModrm
		add	ah,2
		mov	[di],ah
		and	ch,0111b
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		mov	dx,Parameter1.Value
		mov	[di+3],dx

		cmp	ax,ax
		ret

@@R_M:
		cmp	cl,-1
		je	@@Quit
		cmp	cl,10h
		jb	@@Quit
@@S_M:
		or	al,2
		xchg	cl,ch
		call	SwapParameters
		jmp	@@M_S
@@Quit:
		or	sp,sp
		ret
endp

;------------------------------------------------------------------------------
_rm		proc
		cmp	ParamCount,1
		jne	@@Quit
		cmp	bl,1
		je	@@M
		cmp	cl,-1
		je	@@Quit
@@M:
		mov	ch,ah
		call	AddWordFlag
		mov	[di+1],al
		call	MakeModrm
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		add	ah,2
		mov	[di],ah
		mov	ax,Parameter1.Value
		mov	[di+3],ax
		cmp	ax,ax
		ret
@@Quit:
		or	sp,sp
		ret
endp

;------------------------------------------------------------------------------
_a_reg		proc
		cmp	ParamCount,2
		jne	@@Quit
		or	bx,bx
		jnz	@@Quit
		cmp	cl,regAX
		je	@@A_R
		cmp	ch,regAX
		je	@@R_A
@@Quit:
		call	SwapParameters	;; Only for XCHG
		or	sp,sp
		ret
@@A_R:
		cmp	ch,-1
		je	@@Quit
		jmp	@@AX_R
@@R_A:
		cmp	cl,-1
		je	@@Quit
		xchg	cl,ch
@@AX_R:
		and	ch,0111b
		or	al,ch
		mov	byte ptr [di],1
		mov	[di+1],al
		cmp	ax,ax
		ret
endp

;------------------------------------------------------------------------------
_reg16		proc
		cmp	ParamCount,1
		jne	@@Quit
		cmp	cl,-1
		je	@@Quit
		test	cl,1000b
		jz	@@Quit
		and	cl,0111b
		or	al,cl
		mov	byte ptr [di],1
		mov	[di+1],al
		cmp	sp,sp
		ret
@@Quit:
		or	sp,sp
		ret
endp

;------------------------------------------------------------------------------
_sreg		proc
		cmp	ParamCount,1
		jne	@@Quit
		cmp	cl,-1
		je	@@Quit
		cmp	cl,10h
		jb	@@Quit
		and	cl,0111b
		shl	cl,1
		shl	cl,1
		shl	cl,1
		or	al,cl
		mov	byte ptr [di],1
		mov	[di+1],al
		cmp	ax,ax
		ret
@@Quit:
		or	sp,sp
		ret
endp

;------------------------------------------------------------------------------
_rm_cl_1	proc
		cmp	bh,1
		je	@@Quit
		cmp	ch,-1
		je	@@Check1
		cmp	ch,regCL
		jne	@@Quit
		mov	cl,10b
		jmp	@@Make
@@Check1:
		cmp	Parameter2.Value,1
		jne	@@Quit
		mov	cl,0
@@Make:
		mov	ch,ah
		call	AddWordFlag
		or	al,cl
		mov	[di+1],al
		call	MakeModrm
		add	ah,2
		mov	[di],ah
		and	ch,0111b
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		mov	dx,Parameter1.Value
		mov	[di+3],dx
		cmp	ax,ax
		ret
@@Quit:
		or	sp,sp
		ret
endp

;------------------------------------------------------------------------------
_im8ch3		proc
		call	_im8
		jnz	@@Quit
		cmp	byte ptr [di+2],3
		jne	@@Quit1
		mov	word ptr [di],0CC01h
@@Quit1:
		cmp	ax,ax
@@Quit:
		ret
endp
;------------------------------------------------------------------------------
_im8		proc
		cmp	ParamCount,1
		jne	@@Quit
		cmp	bl,0
		jne	@@Quit
		cmp	cl,-1
		jne	@@Quit
		mov	dx,Parameter1.Value
		cmp	dh,0
		jnz	@@Quit
		mov	byte ptr [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_im16		proc
		cmp	ParamCount,1
		jne	@@Quit
		cmp	bl,0
		jne	@@Quit
		cmp	cl,-1
		jne	@@Quit
		mov	dx,Parameter1.Value
		mov	byte ptr [di],3
		mov	[di+1],al
		mov	[di+2],dx
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_imn		proc
		cmp	ParamCount,0
		jne	@@Quit
		mov	byte ptr [di],2
		mov	[di+1],ax
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_a_dx		proc
		cmp	ParamCount,2
		jne	@@Quit
		test	cl,1000b
		jz	@@1
		or	al,1
@@1:
		and	cl,0111b
		cmp	cx,regAL + (regDX shl 8)
		jne	@@Quit
		mov	byte ptr [di],1
		mov	[di+1],al
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_dx_a		proc
		cmp	ParamCount,2
		jne	@@Quit
		test	ch,1000b
		jz	@@1
		or	al,1
@@1:
		and	ch,0111b
		cmp	cx,regDX + (regAL shl 8)
		jne	@@Quit
		mov	byte ptr [di],1
		mov	[di+1],al
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_a_im8		proc
		cmp	ParamCount,2
		jne	@@Quit
		or	bx,bx
		jnz	@@Quit
		cmp	ch,-1
		jne	@@Quit
		test	cl,1000b
		jz	@@1
		or	al,1
@@1:
		and	cl,0111b
		cmp	cl,regAL
		jne	@@Quit
		mov	dx,Parameter2.Value
		or	dh,dh
		jnz	@@Quit
		mov	byte ptr [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_im8_a		proc
		cmp	ParamCount,2
		jne	@@Quit
		or	bx,bx
		jnz	@@Quit
		cmp	cl,-1
		jne	@@Quit
		test	ch,1000b
		jz	@@1
		or	al,1
@@1:
		and	ch,0111b
		cmp	ch,regAL
		jne	@@Quit
		mov	dx,Parameter1.Value
		or	dh,dh
		jnz	@@Quit
		mov	byte ptr [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_short		proc
		cmp	JumpFlag,1
		jne	@@Quit
		cmp	ParamCount,1
		jne	@@Quit
		or	bl,bl
		jnz	@@Quit
		cmp	cl,-1
		jnz	@@Quit
		mov	dx,Parameter1.Value

;		sub	dx,AsmOfs
		sub	dx,JumpOriginOfs

		sub	dx,2
		call	CheckByte
		jnz	@@Quit
		mov	byte ptr [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_near		proc
		cmp	JumpFlag,1
		jne	@@Quit
		cmp	ParamCount,1
		jne	@@Quit
		or	bl,bl
		jnz	@@Quit
		cmp	cl,-1
		jnz	@@Quit
		mov	dx,Parameter1.Value

;		sub	dx,AsmOfs
		sub	dx,JumpOriginOfs

		sub	dx,3
		mov	byte ptr [di],3
		mov	[di+1],al
		mov	[di+2],dx
		cmp	ax,ax
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_far		proc
		cmp	ParamCount,1
		jne	@@Quit
		cmp	JumpFlag,2
		jne	@@Quit
		mov	dx,Parameter1.Value
		mov	cx,FarValue
		mov	byte ptr [di],5
		mov	[di+1],al
		mov	[di+2],cx
		mov	[di+4],dx
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_rmj		proc
		cmp	Parameter1.WordFlag,3
		jne	loc_rm
		inc	ah
loc_rm:
		jmp	_rm
endp


;------------------------------------------------------------------------------
_dw		proc
		cmp	ParamCount,1
		jne	@@Quit
		or	bl,bl
		jnz	@@Quit
		cmp	cl,-1
		jne	@@Quit
		mov	ax,Parameter1.Value
		mov	byte ptr [di],2
		mov	[di+1],ax
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
_noparam	proc
		cmp	ParamCount,0
		jne	@@Quit
		mov	byte ptr [di],1
		mov	[di+1],al
@@Quit:
		ret
endp

;------------------------------------------------------------------------------
AddWordFlag	proc
		mov	ah,Parameter1.WordFlag
		or	ah,ah
		jz	@@2
@@1:
		dec	ah
		or	al,ah
		ret
@@2:
		cmp	ParamCount,1
		jbe	@@3
		mov	ah,Parameter2.WordFlag
		or	ah,ah
		jnz	@@1
@@3:
		ret
endp

;------------------------------------------------------------------------------
SwapParameters	proc
		push	ax cx si di
		mov	si,offset Parameter1
		mov	di,offset Parameter2
		mov	cx,size AsmParam
@@Next:
		lodsb
		xchg	al,es:[di]
		mov	[si-1],al
		inc	di
		loop	@@Next
		pop	di si cx ax
		ret
endp

;------------------------------------------------------------------------------
CheckByte	proc
		push	ax
		mov	al,dl
		cbw
		cmp	ax,dx
		pop	ax
		ret
endp

;------------------------------------------------------------------------------
MakeModrm	proc
		; Makes MODRM byte for 1st parameter
		; Returns: al=md000r/m, ah=offset size (0, 1 or 2)
		push	bx cx dx
		mov	al,Parameter1.RegNumber
		cmp	al,-1
		je	@@Mem
		and	al,0111b
		or	al,11000000b
;;		and	ch,0111b
;		shl	ch,1
;		shl	ch,1
;		shl	ch,1
;		or	al,ch
		mov	ah,0
		jmp	@@Ok
@@Mem:
		cmp	Parameter1.MemFlag,0
		je	@@Error
@@MakeMODRM:
		mov	bx,word ptr Parameter1.BXflag
		mov	cx,word ptr Parameter1.SIflag
		mov	dx,Parameter1.Value
		push	bx
		or	bx,cx
		pop	bx
		jz	@@NoBaseIndex
		or	bx,bx
		jz	@@NoBase
		or	cx,cx
		jz	@@NoIndex
		mov	al,bh
		add	al,bh
		add	al,ch
		jmp	@@MakeMOD
@@NoBase:
		mov	al,4
		add	al,ch
		jmp	@@MakeMOD
@@NoIndex:
		mov	al,6
		add	al,bl
		cmp	bh,1
		jne	@@MakeMOD
		mov	bl,2
		jmp	@@ChByte
@@NoBaseIndex:
		mov	al,0110b
		mov	ah,2
		jmp	@@Ok
@@MakeMOD:
		mov	bl,2
		or	dx,dx
		jz	@@NoOfs
@@ChByte:
		call	CheckByte
		jz	@@ByteOfs
@@WordOfs:
		inc	bl
@@ByteOfs:
		inc	bl
@@NoOfs:
		mov	[di],bl
		sub	bl,2
		mov	ah,bl
		ror	bl,1
		ror	bl,1
		or	al,bl
@@Ok:
		clc
@@Quit:
		pop	dx cx bx
		ret
@@Error:
		stc
		jmp	@@Quit
endp

PrefCodes	label	byte
		db	26h, 2Eh, 36h, 3Eh

cretn		db	'retn',0
csal		db	'sal',0
cjnae		db	'jnae',0
cjc		db	'jc',0
cjnc		db	'jnc',0
cjae		db	'jae',0
cje		db	'je',0
cjne		db	'jne',0
cjna		db	'jna',0
cjnbe		db	'jnbe',0
cjpe		db	'jpe',0
cjpo		db	'jpo',0
cjnge		db	'jnge',0
cjnl		db	'jnl',0
cjng		db	'jng',0
cjnle		db	'jnle',0
cloopz		db	'loopz',0
cloopnz		db	'loopnz',0
crepz		db	'repz',0
crepnz		db	'repnz',0
crepe		db	'repe',0
crepne		db	'repne',0
crep		db	'rep',0
cdw		db	'dw',0

CurrPrefix	db	-1

Parameter1	AsmParam	<>
Parameter2	AsmParam	<>

