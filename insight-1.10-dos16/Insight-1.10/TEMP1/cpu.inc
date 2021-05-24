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



i8086		equ	0
i8088		equ	1
i80286		equ	2
i80386		equ	3
i80486		equ	4

 GetCPUType	proc	near
;	Returns: ax=type:
;		0  =  8086
;		1  =  8088
;		2  = 80286
;		3  = 80386
;		4  = 80486
;		7F = Advanced (with CPUID)
.486
		pushf
		push	bx
		push	cx
		pushf
		pop	ax
		and	ax,0FFFh
		push	ax
		popf
		pushf
		pop	ax
		and	ax,0F000h
		cmp	ax,0F000h
		jne	cpu1
		xor	cx,cx
		mov	di,offset IncCX
		mov	al,90h
		cli
		mov	[di],al
		nop
		nop
		nop
 IncCX:		inc	cx
		sti
		mov	ax,1
		sub	ax,cx
		jmp	@@Quit
 cpu1:
		or	ax,0F000h
		push	ax
		popf
		pushf
		pop	ax
		and	ax,0F000h
		jnz	cpu2
		mov	al,2
		jmp	@@Quit
 cpu2:
		mov	bx,sp
		and	sp,not 3
		pushfd
		pop	eax
		mov	ecx,eax
		xor	eax,40000h
		push	eax
		popfd
		pushfd
		pop	eax
		mov	sp,bx
		xor	eax,ecx
		mov	al,3
		jz	@@Quit
		mov	ebx,200000h
		mov	eax,ecx
		xor	eax,ebx
		push	eax
		popfd
		pushfd
		pop	eax
		push	ecx
		popfd
		and	eax,ebx
		and	ecx,ebx
		cmp	eax,ecx
		mov	al,4
		jz	@@Quit
		mov	al,7Fh
 @@Quit:
		cbw
		pop	cx
		pop	bx
		popf
		ret
.8086
		endp

