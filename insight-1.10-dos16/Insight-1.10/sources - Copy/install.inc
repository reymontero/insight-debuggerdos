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


Install		proc

		mov	dx,offset Copyright
		call	WriteMessage

		xor	ax,ax
		mov	cx,UDataEnd - UDataStart
		mov	di,offset UDataStart
		cld
		rep	stosb

		call	GetVideoType
		mov	VideoType,al

		mov	ax,word ptr CryptText1+45+12
		add	ax,(HMAExist - Start + 100h) - CryptValue4
		call	ax
		jnc	@@CommandLine
		or	SearchOptions,100b

	@@CommandLine:
		mov	word ptr MyCmdLine,0D00h
		mov	CmdLineString,0
		mov	FName,0

		mov	ax,word ptr CryptText1+45+45
		add	ax,(GetCmdLine - Start + 100h) - CryptValue5
		call	ax

		call	FullProgName

		mov	dx,offset HelpMsg
		test	CmdLineOptions,cmdHelp
		jnz	@@DisplayError

		mov	dx,offset ErrorHelpMsg
		test	CmdLineOptions,cmdError
		jnz	DisplayError

		test	CmdLineOptions,cmdUnload
		jnz	@@Unload

	@@CheckMemory:
		xor	cx,cx
		call	CalcProgSize
		pop	si			;return address
		add	si,(ContinueInit - Start + 100h) - (PostInstall - Start + 100h) - CryptValue1
		add	si,word ptr CryptText1+12
		mov	sp,ax
		mov	bx,dx
		mov	ah,4Ah
		int	21h

	@@MemoryError:
		mov	dx,offset FatalMemoryMsg
		jc	DisplayError
		jmp	si

	@@IniError:
		mov	ah,3Eh
		int	21h

	@@DisplayError:
		mov	BeepChar,'$'

	DisplayError:
		call	WriteMessage
		mov	dx,offset EOL
		call	WriteMessage

	@@ErrorQuit:
		mov	ax,4C01h
		int	21h

	@@Unload:
		call	IsResident
		jne	DisplayError

		mov	dx,offset UnloadErrorMsg

		mov	si,3*4
		mov	di,offset BPEntry
		call	CheckVector
		jne     DisplayError

		mov	si,9*4
		mov	di,offset Int09handler
		call	CheckVector
		jne     DisplayError

		mov	si,16h*4
		mov	di,offset KeyboardService
		call	CheckVector
		jne     DisplayError

		push	es
		pop	ds
		call	Restore03vector
		call	Restore09vector
		call	Restore16vector

		mov	es,ds:[2Ch]
		mov	ah,49h
		int	21h
		jc	@@SkipFree
		push	ds
		pop	es
		mov	ah,49h
		int	21h

	@@SkipFree:
		push	cs
		pop	ds
		mov	dx,offset MemoryFreeMsg
		jc	DisplayError

		mov	dx,offset UnloadedMsg
		call	WriteMessage
		jmp	@@ErrorQuit
		endp

Init		proc
		call	SaveIntTable

		test	CmdLineOptions,cmdResident
		jz	@@CheckProgName
		call	IsResident
		jne	@@CreateProgram
		mov	dx,offset AlredyTSRMsg
		jmp	DisplayError

	@@CheckProgName:
		cmp	FName,0
		je	@@CreateProgram
		call	SetCommandLine
		call	LoadProgram
		jnc	@@Loaded
		mov	Loaded,0

	@@CreateProgram:
		mov	ax,word ptr CryptText1+45
		add	ax,(CreateProgram - Start + 100h) - CryptValue3
		call	ax

	@@Loaded:
		call	SetUserPID
		call	SetTermHandler
		call	SaveScreen
		in	al,21h
		and	al,11111100b
		mov	MyPort21,al
		mov	UserPort21,al

		call	GetCPUType
		cmp	al,7Fh
		jne	@@SetCPUType

if ??VERSION ge (4*256)
  .586
else
  .386
endif

		xor	eax,eax
		cpuid
		mov	dword ptr cpuAdvancedName[0],ebx
		mov	dword ptr cpuAdvancedName[4],edx
		mov	dword ptr cpuAdvancedName[8],ecx
		xor	eax,eax
		inc	ax
		cpuid
.8086
		and	ah,0Fh
		add	cpuAdvancedID,ah
		mov	ax,4
		mov	CPUNames[8],offset cpuAdvanced

	@@SetCPUType:
		mov	CPU,al
		cmp	al,3
		jb	@@SkipEnable
		mov	RegModeStatus1,0
		mov	RegModeStatus2,0

	@@SkipEnable:
		mov	bx,ax
		or	ax,ax
		jz	@@1
		dec	ax
	@@1:
		mov	OptCPU,al
		add	al,a86
		mov	CPUType,al
		shl	bx,1
		mov	ax,CPUNames[bx]
		mov	CPUName,ax

		mov	ax,CPURegs.xcs
		mov	Unasm_seg,ax
		mov	ax,CPURegs.xip
		mov	CurIP,ax
		mov	ax,CPURegs.xds
		mov	DumpSeg,ax

		xor	ax,ax
		mov	byte ptr MainAddrString,al
		mov	byte ptr DumpAddrString,al
		mov	byte ptr AsmLine,al
		mov	CurLine,ax
		mov	DumpOffs,ax

		mov	ax,word ptr CryptText1+12+12
		add	ax,(SetPrefixes - Start + 100h) - CryptValue2
		call	ax

		mov	si,offset CryptText1
		mov	cx,CryptLen1
		jmp	Decrypt
		endp

GetVideoType	proc
		mov	ah,12h
		mov	bl,10h
		int	10h
		cmp	bl,10h
		je	@@OldVideo
		mov	ax,1A00h
		int	10h
		cmp	al,1Ah
		jne	@@OldVideo
		cmp	bl,7
		jbe	@@OldVideo
		mov	al,1
		ret

	@@OldVideo:
		xor	ax,ax
		ret
		endp

HMAExist	proc
		mov	ax,4300h
		int	2Fh
		cmp	al,80h
		jne	@@ErrorExit

		push	es
		mov	ax,4310h
		int	2Fh

		push	cs
		mov	ax,offset @@RetPoint
		push	ax
		mov	ah,0
		push	es
		push	bx
		retf

	@@RetPoint:
		pop	es
		or	ax,ax
		jz	@@ErrorExit
		or	dx,dx
		jz	@@ErrorExit
		clc
		ret

	@@ErrorExit:
		stc
		ret
		endp

Decrypt		proc
		mov	ah,@@CryptByte
		cld
		mov	di,si

	@@Next:
		lodsb
		xchg	al,ah
		xor	al,ah
		stosb
		loop	@@Next

		mov	@@CryptByte,ah
		ret

@@CryptByte	db	55h
		endp

SetPrefixes	proc
		cmp	CPU,3
		jb	@@Exit
		mov	cx,@@PrefixTableEnd - @@PrefixTable
		mov	si,offset @@PrefixTable
		xor	bx,bx
	@@Next:
		mov	bl,ds:[si]
		lea	di,FirstPrefix[bx]
		mov	byte ptr ds:[di],066h
		inc	si
		loop	@@Next
	@@Exit:
		ret

@@PrefixTable		label	byte
		db	Prefix01 - FirstPrefix
		db	Prefix02 - FirstPrefix
		db	Prefix03 - FirstPrefix
		db	Prefix04 - FirstPrefix
		db	Prefix05 - FirstPrefix
		db	Prefix06 - FirstPrefix
		db	Prefix07 - FirstPrefix
		db	Prefix08 - FirstPrefix
		db	Prefix09 - FirstPrefix
		db	Prefix10 - FirstPrefix
		db	Prefix11 - FirstPrefix
		db	Prefix12 - FirstPrefix
		db	Prefix13 - FirstPrefix
		db	Prefix14 - FirstPrefix
		db	Prefix15 - FirstPrefix
		db	Prefix16 - FirstPrefix
		db	Prefix17 - FirstPrefix
		db	Prefix18 - FirstPrefix
		db	Prefix19 - FirstPrefix
@@PrefixTableEnd	label	byte

		endp


AvaibleOptions	db	'RUH?',0

OptionsBits	db	cmdResident,cmdUnload,cmdHelp,cmdHelp


Copyright	db	SerialNumber,', Copyright (C) 1993, 96, 97 by Victor Gamayunov, Sergey Pimenov', 13, 10, 13, 10
			db	'Insight comes with ABSOLUTELY NO WARRANTY. This is free software covered by', 13, 10
			db	'GNU GENERAL PUBLIC LICENSE 2, and you are welcome to redistribute it under', 13, 10
			db	'certain conditions. See http://www.gnu.org/ or LICENSE.ENG file for details.', 13, 10, '$'

ErrorHelpMsg	db	'Error in command line parameters.',13,10
HelpMsg		db	'  Usage: INSIGHT [options] [filename [parameters]]', 13, 10
		db	'  Options:',13,10
		db	'	/h, /? 		- display this help screen', 13, 10

		db	'	/r		- terminate and stay resident in memory',13,10
		db	'	/u		- unload resident debugger$'


FatalMemoryMsg		db	'Memory allocation error$'

MemoryFreeMsg	db	"Can't release debugger memory$"
NotInstalledMsg	db	'Debugger not found in memory$'
VersionMsg	db	'Incorrect debugger version$'
UnloadErrorMsg	db	"Can't unload debugger$"
UnloadedMsg	db	'Debugger unloaded successfully.',13,10,'$'
AlredyTSRMsg	db	'Debugger already installed resident.',13,10
		db	'  Press Ctrl+Esc to activate$'
EOL		db	'.', 13, 10
BeepChar	db	7, '$'

