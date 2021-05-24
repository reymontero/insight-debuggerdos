;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; keyboard.inc
; Procedures for working with keyboard.
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
; key_loop
;-----------------------------------------------------------------------------
; Expects: keys=@struc: dw KeyCode, Subroutine
;	 KeyCode=0 - end of list, Sub=0 - exit
; Returns: none
;
key_loop:

@@loop:
		push	ax
		push	bx
		push	si
		mov	si,[keys]
		call	flush_key
		call	read_key
		xchg	ax,bx
@@next:
		lodsw
		or	ax,ax
		jz	@@not_found
		cmp	ax,bx
		je	@@found
		lodsw
		jmp	@@next

@@found:
		lodsw
		or	ax,ax
		jz	@@quit
		mov	[key_loop_sub],ax
		pop	si
		pop	bx
		pop	ax
		call	word [key_loop_sub]
		jmp	@@loop

@@not_found:
		mov	[cur_test_code],bx
		lodsw
		mov	[key_loop_sub],ax
		or	ax,ax
		xchg	ax,bx
		pop	si
		pop	bx
		jz	@@test_key
		call	word [key_loop_sub]
		jc	@@test_key
		pop	ax
		jmp	@@loop

@@test_key:
		pop	ax
		call	[test_key]
		jnc	@@quit_1
		call	beep
		jmp	@@loop
@@quit:
		mov	ax,bx
		pop	si
		pop	bx
		inc	sp
		inc	sp
@@quit_1:
		ret
;=============================================================================
; def_test_key
;-----------------------------------------------------------------------------
def_test_key:
		stc
		ret
;=============================================================================
; read_key
;-----------------------------------------------------------------------------
; Expects: none
; Returns: ax=code
;
read_key:
		push	bx
		xor	bx,bx
@@next:
		mov	ah,1
		int	16h
		jnz	@@get
		mov	ah,2
		int	16h
		test	al,8
		jnz	@@alt
		cmp	bl,2
		je	@@ret
		mov	bl,1
		jmp	@@next
@@ret:
		mov	ax,kbAltAlone
		jmp	@@quit
@@get:
		mov	ah,0
		int	16h
@@quit:
		pop	bx
		ret
@@alt:
		or	bx,bx
		jz	@@next
		mov	bl,2
		jmp	@@next
;=============================================================================
; in_key
;-----------------------------------------------------------------------------
; Expects: none
; Returns: ZF set if no key pressed, otherwise ax=code
;
%if 0
in_key:

		mov	ah,1
		int	16h
		ret
%endif
;=============================================================================
; flush_key
;-----------------------------------------------------------------------------
flush_key:
@@next:
		mov	ah,1
		int	16h
		jz	@@quit
		mov	ah,0
		int	16h
		jmp	@@next
@@quit:
		ret
;=============================================================================
; check_shift
;-----------------------------------------------------------------------------
check_shift:
		push	ax
		push	ds
		xor	ax,ax
		mov	ds,ax
		test	byte [417h],11b
		pop	ds
		pop	ax
		ret
;=============================================================================

key_loop_sub	dw	0
keys		dw	0
test_key	dw	def_test_key
cur_test_code	dw	0

;=============================================================================
; E0F
;=============================================================================


