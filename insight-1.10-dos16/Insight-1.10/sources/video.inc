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


InitPalette	proc
		mov	ax,cs
		mov	ds,ax
		mov	es,ax
		push	dx
		mov  	cx,256
		mov  	dx,3DAh

	@@wait1:
		in   	al,dx
		test 	al,8
		jne  	@@wait1

	@@wait2:
		in   	al,dx
		test 	al,8
		je   	@@wait2
		pop	dx
		mov  	al,0
		out  	dx,al
		inc	dx
;		nop
;		nop
;		nop
		cld
		ret
		endp

GetPalette256	proc
		mov  	dx,3C7h
		call	InitPalette
		inc	dx

	  @@NextOut:
		in	al,dx
		stosb
		in	al,dx
		stosb
		in	al,dx
		stosb
		loop	@@NextOut

		ret
		endp

SetPalette256	proc
		mov  	dx,3C8h
		call	InitPalette

	  @@NextOut:
		lodsb
		out	dx,al
		lodsb
		out	dx,al
		lodsb
		out	dx,al
		loop	@@NextOut

		ret
		endp

