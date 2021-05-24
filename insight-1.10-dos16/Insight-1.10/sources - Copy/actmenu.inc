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


MenuStructure	dw	6, HMenu, HSub
HMenu		db	'~F~ile',0
		db	'~E~dit',0
		db	'~R~un',0
		db	'~B~reakpoints',0
		db	'~O~ptions',0
;		db	'~T~ools',0
		db	'~W~indow',0
HSub		dw	SubMenu, FileSubMenu
		dw	SubMenu, EditSubMenu
		dw	SubMenu, RunSubMenu
		dw	SubMenu, BPSubMenu
		dw	SubMenu, OptionsSubMenu
;		dw	SubMenu, ToolsSubMenu
		dw	SubMenu, WindowSubMenu

;------------------------------------------------------------------------------
FileSubMenu	label	byte
		db	1, 1, 1+17, 9	; Coords
		dw	5		; Number
		db	0, 15		; Selected, Width
		dw	FileSub
		dw	FileStr
FileSub		dw	0, LoadFile
		dw	0, NewProgram
		dw	0, About
ResidentStatus	dw	0, Resident
ExitStatus	dw	0, QuitProc
FileStr		db	'~L~oad...     F3',0
		db	'~N~ew',0
		db	Separator
		db	'~A~bout...',0
		db	Separator
		db	'~R~esident',0
		db	'E~x~it     Alt+X',0

;------------------------------------------------------------------------------
EditSubMenu	label	byte
		db	7, 1, 7+21, 6	; Coords
		dw	4		; Number
		db	0, 19		; Selected, Width
		dw	EditSub
		dw	EditStr
EditSub		dw	0, EditCommands
		dw	0, EditDump
		dw	0, EditRegisters
		dw	0, EditFlags
EditStr		db	'~A~ssembler  Shift+A',0
		db	'~D~ump       Shift+D',0
		db	'~R~egisters  Shift+R',0
		db	'~F~lags      Shift+F',0

;------------------------------------------------------------------------------
RunSubMenu	label	byte
		db	13, 1, 13+25, 10; Coords
		dw	7		; Number
		db	0, 23		; Selected, Width
		dw	RunSub
		dw	RunStr
RunSub		dw	0, RunProgram
		dw	0, GoToCursor
		dw	0, SingleStep
		dw	0, ProcTrace
		dw	Disabled, TestProc
		dw	0, ReadCmdLine
		dw	0, ResetProgram
RunStr		db	'~R~un            Ctrl+F9',0
		db	'~G~o to cursor        F4',0
		db	'~T~race into          F7',0
		db	'~S~tep over           F8',0
		db	'A~n~imate...',0
		db	Separator
		db	'~A~rguments...',0
		db	'~P~rogram reset  Ctrl+F2',0

;------------------------------------------------------------------------------
BPSubMenu	label	byte
		db	18, 1, 18+20, 9	; Coords
		dw	6		; Number
		db	0, 18		; Selected, Width
		dw	BPSub
		dw	BPStr
BPSub		dw	0, ToggleBreakPoint
		dw	0, ToggleActivity
		dw	Disabled, TestProc
		dw	0, ClearAllBP
		dw	0, EnableAllBreaks
		dw	0, DisableAllBreaks
BPStr		db	'~S~et/Clear      F2',0
		db	'~A~ctivity   Alt+F2',0
		db	'~E~dit...          ',0
		db	Separator
		db	'~D~elete  all',0
		db	'E~n~able  all',0
		db	'D~i~sable all',0

;------------------------------------------------------------------------------
OptionsSubMenu	label	byte
		db	31, 1, 31+26, 5	; Coords
		dw	3		; Number
		db	0, 24		; Selected, Width
		dw	OptionsSub
		dw	OptionsStr
OptionsSub	dw	SubMenu, OptionsCPU
		dw	SubMenu, OptionsSwap
;		dw	Disabled, TestProc
;		dw	Disabled, TestProc
		dw	Disabled, TestProc
OptionsStr	db	'~C~PU for disassembler  ',0
		db	'~S~creen swapping       ',0
;		db	'~E~mulation...',0
;		db	'C~o~lors...',0
		db	'Co~n~figuration...',0

OptionsCPU	label	byte
		db	45, 3, 45+14, 10	; Coords
		dw	5			; Number
OptCPU		db	0, 12			; Selected, Width
		dw	OptionsCPUSub
		dw	OptionsCPUStr
OptionsCPUSub	dw	0, SetCPU88
		dw	0, SetCPU286
		dw	0, SetCPU386
		dw	0, SetCPU486
		dw	0, SetCPUAuto
OptionsCPUStr	db	'8~0~86/8088',0
		db	'80~2~86',0
		db	'80~3~86',0
		db	'80~4~86',0
		db	Separator
		db	'~A~uto detect',0

OptionsSwap	label	byte
		db	47, 4, 47+11, 8	; Coords
		dw	3		; Number
SwapMode	db	1, 9		; Selected, Width
		dw	OptionsSwapSub
		dw	OptionsSwapStr
OptionsSwapSub	dw	0, SetSwapOff
		dw	0, SetSwapSmart
		dw	0, SetSwapOn
OptionsSwapStr	db	'~N~one',0
		db	'~S~mart',0
		db	'~A~lways',0

;------------------------------------------------------------------------------
;ToolsSubMenu	label	byte
;		db	40, 1, 40+26, 8	; Coords
;		dw	6		; Number
;		db	0, 24		; Selected, Width
;		dw	ToolsSub
;		dw	ToolsStr
;ToolsSub	dw	Disabled, TestProc
;		dw	Disabled, TestProc
;		dw	Disabled, TestProc
;		dw	Disabled, TestProc
;		dw	Disabled, TestProc
;		dw	Disabled, TestProc
;ToolsStr	db	'~M~emory blocks list...',0
;		db	'~T~SR programs...',0
;		db	'~D~evice drivers...',0
;		db	'~H~ooked vectors...',0
;		db	'~I~/O',0
;		db	'~C~alculator...',0

;------------------------------------------------------------------------------
WindowSubMenu	label	byte
		db	40, 1, 40+25, 4	; Coords
		dw	2		; Number
		db	0, 23		; Selected, Width
		dw	WindowSub
		dw	WindowStr
WindowSub	dw	0, DrawScreen
		dw	0, ExecUserScreen
WindowStr	db	'~R~efresh display     F5',0
		db	'~U~ser screen     Alt+F5',0

TestProc:	ret