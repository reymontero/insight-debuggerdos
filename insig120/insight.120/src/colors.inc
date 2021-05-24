;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; colors.inc
; Constants: colors.
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
; Main screen
;-----------------------------------------------------------------------------
atCPUborder		equ	BLACK*16+WHITE

atCmdNormal		equ	atNormInstr
atCmdSelected		equ	atSelInstr
atBP_and_Cur		equ	atBrkSelInstr
atBreakPoint		equ	atBrkInstr

atNormAddr		equ	BLACK*16+LIGHT_GRAY
atNormOpcode		equ	BLACK*16+GREEN
atNormInstr		equ	BLACK*16+LIGHT_GREEN
atNormNumber		equ	BLACK*16+CYAN
atNormArrow		equ	BLACK*16+WHITE
atNormDisable		equ	BLACK*16+LIGHT_GRAY
                	
atSelAddr		equ	LIGHT_GRAY*16
atSelOpcode		equ	LIGHT_GRAY*16
atSelInstr		equ	LIGHT_GRAY*16
atSelNumber		equ	LIGHT_GRAY*16
atSelArrow		equ	LIGHT_GRAY*16+LIGHT_CYAN
atSelDisable		equ	LIGHT_GRAY*16+DARK_GRAY

atBrkAddr		equ	RED*16+WHITE
atBrkOpcode		equ	RED*16+WHITE
atBrkInstr		equ	RED*16+WHITE
atBrkNumber		equ	RED*16+WHITE
atBrkArrow		equ	RED*16+WHITE
atBrkDisable		equ	RED*16+DARK_GRAY

atBrkDisAddr		equ	RED*16+DARK_GRAY
atBrkDisOpcode		equ	RED*16+DARK_GRAY
atBrkDisInstr		equ	RED*16+DARK_GRAY
atBrkDisNumber		equ	RED*16+DARK_GRAY
atBrkDisArrow		equ	RED*16+WHITE
atBrkDisDisable		equ	RED*16+DARK_GRAY

atBrkSelAddr		equ	MAGENTA*16+WHITE
atBrkSelOpcode		equ	MAGENTA*16+WHITE
atBrkSelInstr		equ	MAGENTA*16+WHITE
atBrkSelNumber		equ	MAGENTA*16+WHITE
atBrkSelArrow		equ	MAGENTA*16+WHITE
atBrkSelDisable		equ	MAGENTA*16+DARK_GRAY

atBrkDSAddr		equ	MAGENTA*16+DARK_GRAY
atBrkDSOpcode		equ	MAGENTA*16+DARK_GRAY
atBrkDSInstr		equ	MAGENTA*16+DARK_GRAY
atBrkDSNumber		equ	MAGENTA*16+DARK_GRAY
atBrkDSArrow		equ	MAGENTA*16+WHITE
atBrkDSDisable		equ	MAGENTA*16+DARK_GRAY

;atCmdNormal		equ	LIGHT_GREEN
;atCmdSelected		equ	LIGHT_GRAY*16
;atBreakPoint		equ	RED*16+WHITE
;atBP&Cur		equ	MAGENTA*16+WHITE

atAsmLine		equ	BLACK*16+WHITE

atTitle			equ	BLACK*16+YELLOW
atCPUName		equ	BLACK*16+CYAN

atCodeMark		equ	BLACK*16+LIGHT_RED

atRegName		equ	BLACK*16+LIGHT_GRAY
atRegValue		equ	BLACK*16+LIGHT_GREEN
atRegNewValue		equ	BLACK*16+LIGHT_CYAN

atFlagValue		equ	BLACK*16+LIGHT_GREEN
atFlagNewValue		equ	BLACK*16+LIGHT_CYAN

atDumpAddr		equ	BLACK*16+LIGHT_GRAY
atDumpValue		equ	BLACK*16+LIGHT_GREEN
;atDumpCursor		equ	BLUE*16+LIGHT_GREEN
atDumpCursor		equ	BLUE*16+WHITE

atStackName		equ	BLACK*16+LIGHT_GRAY
atStackValue		equ	BLACK*16+LIGHT_GREEN

atMemAddr		equ	BLACK*16+WHITE
atMemValue		equ	BLACK*16+YELLOW

atDataWatchMem		equ	BLACK*16+LIGHT_RED

atAssembler		equ	LIGHT_GRAY*16+WHITE
atsAssembler		equ	LIGHT_GRAY*16+WHITE
atBrkAssembler		equ	MAGENTA*16+LIGHT_CYAN
atsBrkAssembler		equ	MAGENTA*16+LIGHT_CYAN

;=============================================================================
; Menu.
;-----------------------------------------------------------------------------
MenuNormalAttr		equ	LIGHT_GRAY*16+BLACK
MenuNormalLetter	equ	LIGHT_GRAY*16+RED
MenuSelAttr		equ	GREEN*16+BLACK
MenuSelLetter		equ	GREEN*16+RED

atDisabled		equ	7*16+8
atSelDisabled		equ	GREEN*16+8

;=============================================================================
; Input line.
;-----------------------------------------------------------------------------
;~atReadWindow		equ	BLUE*16+WHITE	;LIGHT_GRAY*16+BLACK
atReadString		equ	BLUE*16+WHITE	;LIGHT_GRAY*16+WHITE
atsReadString		equ	GREEN*16+WHITE

;=============================================================================
; Message box / Error box.
;-----------------------------------------------------------------------------
atMessageBox		equ	BLUE*16+WHITE
atMessage		equ	BLUE*16+YELLOW
atErrorBox		equ	RED*16+WHITE
atErrorMsg		equ	RED*16+YELLOW

;=============================================================================
; Dialog windows.
;-----------------------------------------------------------------------------
atDialogWindow		equ	LIGHT_GRAY*16+WHITE	;BLUE*16+WHITE		; LIGHT_GRAY*16+WHITE

;=============================================================================
; Buttons.
;-----------------------------------------------------------------------------
atPushButton		equ	GREEN*16+0	;LIGHT_GRAY*16+BLACK	; GREEN*16+0
atlPushButton		equ	GREEN*16+YELLOW	;LIGHT_GRAY*16+YELLOW	; GREEN*16+YELLOW
atsPushButton		equ	GREEN*16+WHITE	;LIGHT_GRAY*16+WHITE	; GREEN*16+WHITE
atlsPushButton		equ	GREEN*16+YELLOW	;LIGHT_GRAY*16+YELLOW	; GREEN*16+YELLOW
atPushButtonDef		equ	GREEN*16+BLUE	;LIGHT_GRAY*16+BLUE	; GREEN*16+BLUE
atlPushButtonDef	equ	GREEN*16+YELLOW	;LIGHT_GRAY*16+YELLOW	; GREEN*16+YELLOW
atButtonShadow		equ	LIGHT_GRAY*16+BLACK	;BLUE*16+BLACK		; LIGHT_GRAY*16+BLACK

;=============================================================================
; Labels.
;-----------------------------------------------------------------------------
atLabel			equ	LIGHT_GRAY*16+BLACK	;BLUE*16+WHITE		; LIGHT_GRAY*16+BLACK
atlLabel		equ	LIGHT_GRAY*16+YELLOW	;BLUE*16+YELLOW		; LIGHT_GRAY*16+YELLOW
atsLabel		equ	LIGHT_GRAY*16+WHITE	;BLUE*16+WHITE		; LIGHT_GRAY*16+WHITE
atlsLabel		equ	LIGHT_GRAY*16+YELLOW	;BLUE*16+WHITE		; LIGHT_GRAY*16+YELLOW

;=============================================================================
; Input line, string list, check boxes, radio buttons.
;-----------------------------------------------------------------------------
atInputLine		equ	BLUE*16+WHITE	;LIGHT_GRAY*16+BLACK	; BLUE*16+WHITE
atsInputLine		equ	GREEN*16+WHITE	;GREEN*16+WHITE		; GREEN*16+WHITE
atListNormal		equ	CYAN*16+BLACK	;LIGHT_GRAY*16+BLACK	; CYAN*16+BLACK
atListSelected		equ	GREEN*16+WHITE	;GREEN*16+WHITE		; GREEN*16+WHITE
atCheckBoxes		equ	CYAN*16+BLACK
atsCheckBoxes		equ	CYAN*16+WHITE
atlCheckBoxes		equ	CYAN*16+YELLOW
atRadioButtons		equ	CYAN*16+BLACK
atsRadioButtons		equ	CYAN*16+WHITE
atlRadioButtons		equ	CYAN*16+YELLOW

;=============================================================================
; E0F
;=============================================================================

