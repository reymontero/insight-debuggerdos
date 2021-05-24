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



;		Keyboard unit

define		_KEYBOARD_

ifdef _KeyLoop

use _ReadKey
use _FlushKey
use _Beep
KeyLoop		proc
;		Expects: Keys=@struc: dw KeyCode, Subroutine
;			 KeyCode=0 - end of list, Sub=0 - exit
;		Returns: none

@@Loop:
		push	ax bx si
		mov	si,Keys
		call	FlushKey
		call	ReadKey
		xchg	ax,bx
@@Next:
		lodsw
		or	ax,ax
		jz	@@NotFound
		cmp	ax,bx
		je	@@Found
		lodsw
		jmp	@@Next

	@@Found:
		lodsw
		or	ax,ax
		jz	@@Quit
		mov	@@Sub,ax
		pop	si bx ax
		call	@@Sub
		jmp	@@Loop

	@@NotFound:
		mov	CurTestCode,bx
		lodsw
		mov	@@Sub,ax
		or	ax,ax
		xchg	ax,bx
		pop	si bx
		jz	@@TestKey
		call	@@Sub
		jc	@@TestKey
		pop	ax
		jmp	@@Loop

	@@TestKey:
		pop	ax
		call	TestKey
		jnc	@@Quit1
		call	Beep
		jmp	@@Loop
@@Quit:
		mov	ax,bx
		pop	si
		pop	bx
		inc	sp
		inc	sp
@@Quit1:
		ret
@@Sub		dw	0
endp

Keys		dw	0
TestKey		dw	DefTestKey
CurTestCode	dw	0
DefTestKey	proc
		stc
		ret
endp

endif

ifdef	_ReadKey
ReadKey		proc
;		Expects: none
;		Returns: ax=code

		push	bx
		xor	bx,bx
@@Next:
		mov	ah,1
		int	16h
		jnz	@@Get
		mov	ah,2
		int	16h
		test	al,8
		jnz	@@Alt
		cmp	bl,2
		je	@@Ret
		mov	bl,1
		jmp	@@Next
@@Ret:
		mov	ax,kbAltAlone
		jmp	@@Quit
@@Get:
		mov	ah,0
		int	16h
@@Quit:
		pop	bx
		ret
@@Alt:
		or	bx,bx
		jz	@@Next
		mov	bl,2
		jmp	@@Next
		endp
endif

ifdef	_InKey
InKey		proc
;		Expects: none
;		Returns: ZF set if no key pressed, otherwise ax=code

		mov	ah,1
		int	16h
		ret
		endp
endif

ifdef	_FlushKey
FlushKey	proc
@@Next:
		mov	ah,1
		int	16h
		jz	@@Quit
		mov	ah,0
		int	16h
		jmp	@@Next
@@Quit:
		ret
endp
endif

CheckShift	proc
		push	ax
		push	ds
		xor	ax,ax
		mov	ds,ax
		test	byte ptr ds:[417h],11b
		pop	ds
		pop	ax
		ret
		endp

; Keyboard constants

kbF1		equ	3B00h
kbF2		equ	3C00h
kbF3		equ	3D00h
kbF4		equ	3E00h
kbF5		equ	3F00h
kbF6		equ	4000h
kbF7		equ	4100h
kbF8		equ	4200h
kbF9		equ	4300h
kbF10		equ	4400h

kbCtrlF1	equ	5E00h
kbCtrlF2	equ	5F00h
kbCtrlF3	equ	6000h
kbCtrlF4	equ	6100h
kbCtrlF5	equ	6200h
kbCtrlF6	equ	6300h
kbCtrlF7	equ	6400h
kbCtrlF8	equ	6500h
kbCtrlF9	equ	6600h
kbCtrlF10	equ	6700h

kbAltF1		equ	6800h
kbAltF2		equ	6900h
kbAltF3		equ	6A00h
kbAltF4		equ	6B00h
kbAltF5		equ	6C00h
kbAltF6		equ	6D00h
kbAltF7		equ	6E00h
kbAltF8		equ	6F00h
kbAltF9		equ	7000h
kbAltF10	equ	7100h

kbShiftF1	equ	5400h
kbShiftF2	equ	5500h
kbShiftF3	equ	5600h
kbShiftF4	equ	5700h
kbShiftF5	equ	5800h
kbShiftF6	equ	5800h
kbShiftF7	equ	5A00h
kbShiftF8	equ	5B00h
kbShiftF9	equ	5C00h
kbShiftF10	equ	5D00h

kbAltEqu	equ	8300h

kbAlt1		equ	7800h
kbAlt2		equ	7900h
kbAlt3		equ	7A00h
kbAlt4		equ	7B00h
kbAlt5		equ	7C00h
kbAlt6		equ	7D00h
kbAlt7		equ	7E00h
kbAlt8		equ	7F00h
kbAlt9		equ	8000h
kbAlt0		equ	8100h

kbAltQ		equ	1000h
kbAltW		equ	1100h
kbAltE		equ	1200h
kbAltR		equ	1300h
kbAltT		equ	1400h
kbAltY		equ	1500h
kbAltU		equ	1600h
kbAltI		equ	1700h
kbAltO		equ	1800h
kbAltP		equ	1900h

kbAltA		equ	1E00h
kbAltS		equ	1F00h
kbAltD		equ	2000h
kbAltF		equ	2100h
kbAltG		equ	2200h
kbAltH		equ	2300h
kbAltJ		equ	2400h
kbAltK		equ	2500h
kbAltL		equ	2600h

kbAltZ		equ	2C00h
kbAltX		equ	2D00h
kbAltC		equ	2E00h
kbAltV		equ	2F00h
kbAltB		equ	3000h
kbAltN		equ	3100h
kbAltM		equ	3200h

kbCtrlA		equ	1E01h
kbCtrlB		equ	3002h
kbCtrlC		equ	2E03h
kbCtrlD		equ	2004h
kbCtrlE		equ	1205h
kbCtrlF		equ	2106h
kbCtrlG		equ	2207h
kbCtrlH		equ	2308h
kbCtrlI		equ	1709h
kbCtrlJ		equ	240Ah
kbCtrlK		equ	250Bh
kbCtrlL		equ	260Ch
kbCtrlM		equ	320Dh
kbCtrlN		equ	310Eh
kbCtrlO		equ	180Fh
kbCtrlP		equ	1910h
kbCtrlQ		equ	1011h
kbCtrlR		equ	1312h
kbCtrlS		equ	1F13h
kbCtrlT		equ	1414h
kbCtrlU		equ	1615h
kbCtrlV		equ	2F16h
kbCtrlW		equ	1117h
kbCtrlX		equ	2D18h
kbCtrlY		equ	1519h
kbCtrlZ		equ	2C1Ah

kbHome		equ	4700h
kbUp		equ	4800h
kbPgUp		equ	4900h
kbLeft		equ	4B00h
kbRight		equ	4D00h
kbEnd		equ	4F00h
kbDown		equ	5000h
kbPgDn		equ	5100h

kbShiftHome	equ	4737h
kbShiftUp	equ	4838h
kbShiftPgUp	equ	4939h
kbShiftLeft	equ	4B34h
kbShiftRight	equ	4D36h
kbShiftEnd	equ	4F31h
kbShiftDown	equ	5032h
kbShiftPgDn	equ	5133h

kbIns		equ	5200h
kbDel		equ	5300h

kbEsc		equ	011Bh
kbEnter		equ	1C0Dh
kbTab		equ	0F09h
kbBackSpace	equ	0E08h
kbShiftTab	equ	0F00h
kbCtrlEnter	equ	1C0Ah
kbSpace		equ	3920h
kbGrayPlus	equ	4E2Bh
kbGrayMinus	equ	4A2Dh

kbCtrlLeft	equ	7300h
kbCtrlRight	equ	7400h

kbAltAlone	equ	0FFFEh
