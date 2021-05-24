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



$sw7		db	00000001b
$sw78		equ	00000101b
$sw7m		equ	00000010b
$sw8		equ	00000100b
$jmp		equ	00001000b
$em		equ	00010000b
;$em8		equ	00100000b
;$em78		equ	00110000b
$prefix		equ	01000000b
$grp		equ	10000000b

;------------------------------------------------------------------------------
CheckSwap	proc
	; Expects: dl=0 (Single Step), dl=1 (Proc Trace), dl=2 (Other)
		push	ax bx si es

		cmp	SwapMode,0
		je	@@Not
		cmp	SwapMode,2
		je	@@Swap
		cmp	dl,2
		jae	@@Swap

		mov	si,CPURegs.xip
		mov	es,CPURegs.xcs
		call	GetInstDesc

		test	al,$sw7
		jnz	@@Swap
		test	al,$sw8
		jnz	@@Swap8
		test	al,$sw7m
		jz	@@Not
		lods	byte ptr es:[si]
		and	al,11000000b
		cmp	al,11000000b
		jne	@@Swap
@@Not:
		mov	SwapFlag,0
@@Quit:
		pop	es si bx ax
		ret
@@Swap8:
		or	dl,dl
		jz	@@Not
@@Swap:
		mov	SwapFlag,1
		jmp	@@Quit
endp
SwapFlag	db	0

;------------------------------------------------------------------------------
CheckJump	proc
		push	ax bx si es
		mov	si,CPURegs.xip
		mov	es,CPURegs.xcs
		call	GetInstDesc
		test	al,$jmp
		jz	@@Quit
		stc
@@Quit:
		pop	es si bx ax
		ret
endp

comment #
;------------------------------------------------------------------------------
CheckEmul	proc
	; Expects: dl=0 (Single Step), dl=1 (Proc Trace)
	; Returns: CF set if emulation needed
		push	ax bx si es
		mov	si,_IP
		mov	es,_CS
		call	GetInstDesc
		test	al,$em78
		jz	@@Quit
		or	dl,dl
		jz	@@SS
		test	al,$em8
		jz	@@Quit
@@Emul:
		stc
@@Quit:
		pop	es si bx ax
		ret
@@SS:
		test	al,$em7
		jz	@@Quit
		jmp	@@Emul
endp
#
;------------------------------------------------------------------------------
GetInstDesc	proc
		mov	bx,offset InstDesc
		cld
@@Pref:
		lods	byte ptr es:[si]
		xlat
		test	al,$prefix
		jnz	@@Pref
		test	al,$grp
		jz	@@Quit
@@Grp:
		mov	al,es:[si]
		mov	bx,offset $grjump
		mov	ah,al
		shr	al,1
		shr	al,1
		shr	al,1
		and	al,0111b
		xlat
@@Quit:
		ret
endp
;------------------------------------------------------------------------------
InstDesc	label	byte

		db	$sw7m		; 00
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	$sw7m		; 08
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	0
		db	$jmp	;$second

		db	$sw7m		; 10
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	$sw7m		; 18
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	$sw7m		; 20
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	$prefix
		db	0

		db	$sw7m		; 28
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	$prefix
		db	0

		db	$sw7m		; 30
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	$prefix
		db	0

		db	$sw7m		; 38
		db	$sw7m
		db	0
		db	0
		db	0
		db	0
		db	$prefix
		db	0

		db	0		; 40
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 48
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 50
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 58
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 60
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 68
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	$jmp		; 70
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp

		db	$jmp		; 78
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp
		db	$jmp

		db	$sw7m		; 80
		db	$sw7m
		db	$sw7m
		db	$sw7m
		db	0
		db	0
		db	$sw7m
		db	$sw7m

		db	$sw7m		; 88
		db	$sw7m
		db	0
		db	0
		db	$sw7m
		db	0
		db	0
		db	$sw7m

		db	0		; 90
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 98
		db	0
		db	$sw8
		db	0
		db	$em
		db	$em
		db	0
		db	0

		db	0		; A0
		db	0
		db	$sw78
		db	$sw78
		db	$sw78
		db	$sw78
		db	0
		db	0

		db	0		; A8
		db	0
		db	$sw78
		db	$sw78
		db	0
		db	0
		db	0
		db	0

		db	0		; B0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; B8
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; C0
		db	0
		db	$jmp
		db	$jmp
		db	0
		db	0
		db	$sw7m
		db	$sw7m

		db	0		; C8
		db	0
		db	$jmp
		db	$jmp
		db	0
		db	$em+$sw8
		db	$em+$sw8
		db	$em+$jmp

		db	$sw7m		; D0
		db	$sw7m
		db	$sw7m
		db	$sw7m
		db	0
		db	0
		db	0
		db	0

		db	0		; D8
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	$sw8		; E0
		db	$sw8
		db	$sw8
		db	$jmp
		db	0
		db	0
		db	0
		db	0

		db	$sw8		; E8
		db	$jmp
		db	$jmp
		db	$jmp
		db	0
		db	0
		db	0
		db	0

		db	$prefix		; F0
		db	$prefix
		db	$prefix
		db	$prefix
		db	0
		db	0
		db	0
		db	0

		db	0		; F8
		db	0
		db	0
		db	0
		db	0
		db	0
		db	$grp
		db	$grp


$grjump		label	byte
		db	$sw7m, $sw7m, $sw8, $sw8, $jmp, $jmp, 0, 0
