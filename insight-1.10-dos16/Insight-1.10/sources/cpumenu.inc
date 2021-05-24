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



CPULocalStruc	db	35, 2, 35+25, 3+14
		dw	11
		db	0
		db	23
		dw	CPULocSub
		dw	CPULocStr
CPULocStr	db	'~A~ssembler      Shift+A',0
		db	'~N~ew CS:IP       Ctrl+N',0
RegsMenu1	db	'32-~b~it regs     Ctrl+R',0
		db	'Data ~w~atch      Ctrl+W',0
		db	Separator
		db	'~D~ump position        ',0
		db	'~C~ode position        ',0
		db	Separator
		db	'R~e~store CS:IP   Ctrl+C',0
		db	'~R~estore regs    Ctrl+A',0
		db	'Restore ~f~lags   Ctrl+T',0
		db	Separator
		db	'~S~earch bytes... Ctrl+S',0
		db	'Search a~g~ain    Ctrl+L',0
CPULocSub	dw	0, EditCommands
		dw	0, NewCS_IP
RegModeStatus1	dw	Disabled, SwitchRegMode
		dw	0, DataWatch
		dw	SubMenu, DumpPos
		dw	SubMenu, CodePos
		dw	0, RestoreCSIP
		dw	0, RestoreAll
		dw	0, RestoreFlags
		dw	0, SearchBytes
		dw	0, SearchNext


CodePos		db	40, 10, 40+21, 10+6
		dw	4
		db	0
		db	19
		dw	CodePosSub
		dw	CodePosStr
CodePosStr	db	'~A~ddress...  Ctrl+G',0
		db	Separator
		db	'~O~rigin      Ctrl+O',0
		db	'~F~ollow      Ctrl+F',0
		db	'~P~revious    Ctrl+P',0
CodePosSub	dw	0, UnasmGoTo
		dw	0, GoToOrigin
		dw	0, Follow
		dw	0, Previous

DumpPos		db	40, 9, 40+21, 9+14
		dw	12
		db	0
		db	19
		dw	DumpPosSub
		dw	DumpPosStr
DumpPosStr	db	'~A~ddress...  Ctrl+D',0
		db	'~R~eference    Alt+=',0
		db	Separator
		db	'Dump ~D~S:SI   Alt+D',0
		db	'Dump ~E~S:DI   Alt+E',0
		db	'Dump ~S~S:BP   Alt+S',0
		db	'Dump ~C~S:IP   Alt+C',0
		db	'Dump ES:~B~X   Alt+B',0
		db	'Dump ES:S~I~   Alt+I',0
		db	'Dump DS:DX   Alt+~F~',0
		db	'Dump DS:DI   Alt+~J~',0
		db	'Dump D~X~:AX   Alt+A',0
		db	'Dump code    Alt+~L~',0

DumpPosSub	dw	0, DumpGoTo
		dw	0, DumpReference
		dw	0, DumpDSSI
		dw	0, DumpESDI
		dw	0, DumpSSBP
		dw	0, DumpCSIP
		dw	0, DumpESBX
		dw	0, DumpESSI
		dw	0, DumpDSDX
		dw	0, DumpDSDI
		dw	0, DumpDXAX
		dw	0, DumpCurLine








