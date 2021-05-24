;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; system.inc
; Misc. system procedures.
;-----------------------------------------------------------------------------
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
;=============================================================================


;=============================================================================
; pushr
;-----------------------------------------------------------------------------
pushr:
		pop	word [cs:keep_ip]
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	bp
		push	ds
		push	es
		jmp	word [cs:keep_ip]
;=============================================================================
; popr
;-----------------------------------------------------------------------------
popr:
		pop	word [cs:keep_ip]
		pop	es
		pop	ds
		pop	bp
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		jmp	word [cs:keep_ip]
;=============================================================================
; calc_prog_size
;-----------------------------------------------------------------------------
; cx = additional memory
; return cf = 1 on error, ax = program end, dx = paragraphs
;
calc_prog_size:
		push	cx
		mov	ax,[code_end]
		add	ax,cx
		jc	@@exit
		test	ax,0fh
		jz	@@calc_size
		and	ax,0fff0h
		add	ax,10h
		jc	@@exit
@@calc_size:
		add	ax,STACK_SIZE
		jc	@@exit
		cmp	ax,MAX_PROGRAM_SIZE+1
		cmc
		jc	@@exit
		mov	cl,4
		mov	dx,ax
		shr	dx,cl
		clc
@@exit:
		pop	cx
		ret
;=============================================================================
; action_short_jump
;-----------------------------------------------------------------------------
action_short_jump:
@@next:
		add	di,3
		cmp	word [di],0
		je	@@exit
		cmp	word [di],ax
		jne	@@next
		push	ax
		xor	ax,ax
		mov	al,[di+2]
		xchg	ax,di
		pop	ax
		clc
		ret
@@exit:
		stc
		ret
;=============================================================================

keep_ip		dw	0

;=============================================================================
; E0F
;=============================================================================


