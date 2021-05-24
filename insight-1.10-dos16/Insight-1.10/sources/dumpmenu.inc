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


DumpMenuStruc	db	35, 14, 35+25, 14+7
		dw	5
		db	0
		db	23
		dw	DumpLocSub
		dw	DumpLocStr
DumpLocStr	db	'~D~ump position        ',0
		db	'Dump ~f~ollow          ',0
		db	'~B~lock                ',0
		db	Separator
		db	'~S~earch bytes... Ctrl+S',0
		db	'Search a~g~ain    Ctrl+L',0

DumpLocSub	dw	SubMenu, DumpPos
		dw	SubMenu, FollowMenuStruc
		dw	SubMenu, BlockMenuStruc
		dw	0, SearchBytes
		dw	0, SearchNext

FollowMenuStruc	db	46, 15, 46+15, 15+8
		dw	6
		db	0
		db	13
		dw	FollowLocSub
		dw	FollowLocStr
FollowLocStr	db	'~N~ear data',0
		db	'~F~ar data',0
		db	'~D~ata segment',0
		db	Separator
		db	'N~e~ar code',0
		db	'F~a~r code',0
		db	'~C~ode segment',0
FollowLocSub	dw	0, FollowNearData
		dw	0, FollowFarData
		dw	0, FollowDataSeg
		dw	0, FollowNearCode
		dw	0, FollowFarCode
		dw	0, FollowCodeSeg

BlockMenuStruc	db	46, 18, 46+15, 18+5
		dw	4
		db	0
		db	13
		dw	BlockLocSub
		dw	BlockLocStr
BlockLocStr	db	'~C~opy...',0
		db	'~F~ill...',0
		db	'~W~rite...',0
		db	'~R~ead...',0
BlockLocSub	dw	0, CopyMemBlock
		dw	0, FillMemBlock
		dw	0, WriteMemBlock
		dw	0, ReadMemBlock



