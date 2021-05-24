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


Resident	proc
		call	IsResident
		jne	_Resident
		mov	si,offset AlreadyResident
		jmp	_ErrorMessage

	_Resident:
		call	UnloadProgram
		call	SetMyPid
		call	RestoreIntTable
		call	RestoreScreen

		mov	al,MyPort21
		out	21h,al
		cli
		call	SetBreakPoints
		mov	dx,offset BPEntry
		call	Set03vector
		call	Set09vector
		call	Set16vector
		mov	ResidentMode,1
		mov	ResidentStatus,Disabled
		mov	ExitStatus,Disabled
		mov	SwapMode,1
		SaveACTregs
		sti
		mov	dx,offset ResidentMsg
		call	WriteMessage

		xor	cx,cx
		call	CalcProgSize

		mov	ax,3100h
		int	21h
		endp


Set16vector	proc
		mov	si,16h*4
		mov	di,offset SaveInt16
		mov	dx,offset KeyboardService
		jmp	SetVector
		endp

Restore16vector	proc
		mov	si,offset SaveInt16
		mov	di,16h*4
		jmp	RestoreVector
		endp

Set24vector	proc
		mov	si,24h*4
		mov	di,offset SaveInt24
		mov	dx,offset CriticalHandler
		jmp	SetVector
		endp

Restore24vector	proc
		mov	si,offset SaveInt24
		mov	di,24h*4
		jmp	RestoreVector
		endp

;------------------------------------------------------------------------------
SetVector	proc
		push	ds
		push	cs
		pop	es
		xor	ax,ax
		mov	ds,ax
		cld
		movsw
		movsw
		mov	[si-4],dx
		mov	[si-2],cs
		pop	ds
		ret
endp


RestoreVector	proc
		push	es
		xor	ax,ax
		mov	es,ax
		cld
		movsw
		movsw
		pop	es
		ret
endp


CheckVector	proc
		push	ds
		xor	ax,ax
		mov	ds,ax
		cmp	ds:[si],di
		jne	@@Exit
		mov	ax,es
		cmp	ds:[si+2],ax
	@@Exit:
		pop	ds
		ret
		endp


;------------------------------------------------------------------------------
KeyboardService	proc
		cmp	ax,MagicRequestCode
		jne	@@IntExit
		cmp	bx,DeveloperID
		jne	@@IntExit
		cmp	cx,ApplicationID
		jne	@@IntExit
		mov	ax,MagicAnswerCode
		mov	bx,DebuggerVersion
		push	cs
		pop	es
		iret
	@@IntExit:
		jmp	dword ptr cs:SaveInt16
		endp

CriticalHandler	proc
		mov	al,3
		iret
		endp

IsResident	proc
		mov	ax,MagicRequestCode
		mov	bx,DeveloperID
		mov	cx,ApplicationID
		int	16h
		mov	dx,offset NotInstalledMsg
		cmp	ax,MagicAnswerCode
		jne	@@Exit
		mov	dx,offset VersionMsg
		cmp	bx,DebuggerVersion
	@@Exit:
		ret
		endp
