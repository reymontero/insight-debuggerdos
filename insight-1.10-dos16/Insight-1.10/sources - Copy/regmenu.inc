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


RegsLocalStruc	db	47, 10, 47+25, 10+11
		dw	8
		db	0
		db	23
		dw	RegsLocSub
		dw	RegsLocStr
RegsLocStr	db	'~I~ncrement       Ctrl+I',0
		db	'~D~ecrement       Ctrl+D',0
		db	'~Z~ero            Ctrl+Z',0
		db	Separator
		db	'Rest~o~re         Ctrl+O',0
		db	'~R~estore all     Ctrl+A',0
		db	'R~e~store CS:IP   Ctrl+C',0
		db	'Restore ~f~lags   Ctrl+T',0
		db	Separator
RegsMenu2	db	'32-~b~it regs     Ctrl+R',0
RegsLocSub	dw	0, IncReg
		dw	0, DecReg
		dw	0, ZeroReg
		dw	0, RestoreReg
		dw	0, _RestoreAll
		dw	0, _RestoreCSIP
		dw	0, _RestoreFlags
RegModeStatus2	dw	Disabled, _SwitchRegMode

