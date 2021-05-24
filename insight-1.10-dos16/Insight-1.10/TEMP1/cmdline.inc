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


GetCmdLine	proc
		mov	si,81h
		cld
		call	ProcessOptions

		mov	ah,' '
		mov	di,offset FName
		call	ProcessParam

		mov	ah,13
		mov	di,offset CmdLineString
		jmp	ProcessParam
		endp

ProcessOptions	proc
	@@NextChar:
		lodsb
		cmp	al,' '
		je	@@NextChar
		cmp	al,9
		je	@@NextChar
		cmp	al,'/'
		je	@@Option
		dec	si
		ret

	@@Option:
		lodsb
		call	UpCase
		mov	di,offset AvaibleOptions

	@@NextOption:
		scasb
		mov	ah,ds:[di + (OptionsBits - AvaibleOptions) - 1]
		je	@@SetOption
		cmp	byte ptr ds:[di-1],0
		jne	@@NextOption
		mov	ah,cmdError

	@@SetOption:
		mov	CmdLineOptions,ah
		jmp	@@NextChar
		endp

ProcessParam	proc
	@@NextChar:
		lodsb
		cmp	al,' '
		je	@@NextChar
		cmp	al,9
		je	@@NextChar

	@@SaveChar:
		cmp	al,13
		je	@@Exit
		cmp	al,ah
		je	@@Exit
;		call	UpCase
		stosb
		lodsb
		jmp	@@SaveChar

	@@Exit:
		mov	al,0
		stosb
		dec	si
		ret
		endp

