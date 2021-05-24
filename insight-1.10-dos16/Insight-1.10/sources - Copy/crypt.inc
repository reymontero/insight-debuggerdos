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


CryptString	macro	StringName, StringText
  local	StrLen,Count,Value

  StringName label byte
	StrLen = 0
	Count  = 0

	irpc	Char,StringText
	  StrLen = StrLen + 1
	endm

	irpc	Char,StringText
	  Count = Count + 1

	  if '&Char&' eq '_'
		Value = ' ' xor _CryptByte
	  else
		Value = '&Char&' xor _CryptByte
	  endif

	  if 		_CryptCount eq (12+1)
		CryptValue1	equ	_CryptByte+Value*256
	  elseif	_CryptCount eq (12+12+1)
		CryptValue2	equ	_CryptByte+Value*256
	  elseif	_CryptCount eq (45+1)
		CryptValue3	equ	_CryptByte+Value*256
	  elseif	_CryptCount eq (12+45+1)
		CryptValue4	equ	_CryptByte+Value*256
	  elseif	_CryptCount eq (45+45+1)
		CryptValue5	equ	_CryptByte+Value*256
	  endif

	  db	Value

	  _CryptByte	=	Value
	  _CryptCount	=	_CryptCount + 1
	endm

	if 		_CryptCount eq (12+1)
		CryptValue1	equ	_CryptByte+_CryptByte*256
	elseif	_CryptCount eq (12+12+1)
		CryptValue2	equ	_CryptByte+_CryptByte*256
	elseif	_CryptCount eq (45+1)
		CryptValue3	equ	_CryptByte+_CryptByte*256
	elseif	_CryptCount eq (12+45+1)
		CryptValue4	equ	_CryptByte+_CryptByte*256
	elseif	_CryptCount eq (45+45+1)
		CryptValue5	equ	_CryptByte+_CryptByte*256
	endif

	db	_CryptByte
	_CryptCount	=	_CryptCount + 1
endm


_CryptByte	=	55h
_CryptCount	=	0

CryptText1	label	byte
  CryptString	AboutStr1,	<Original_idea_and_programming:>
  CryptString	AboutStr2,	<Victor_Gamayunov>
  CryptString	AboutStr3,	<Copyright_(c)_1993,_96,_97>
  CryptString	AboutStr4,	<Additional_ideas_and_modifications:>
  CryptString	AboutStr5,	<Sergey_Pimenov>
  CryptString	AboutStr6,	<Copyright_(c)_1996,_97>

ifdef PersonalInfo
  CryptString	AboutStr7,	<Special_edition_for_Igor_Tryndin>
else
  CryptString	AboutStr7,	<Also_thanks_to:>
  CryptString	AboutStr8,	<Sergey_Dyakov,_Igor_Tryndin,_Andrey_Ivanov>
  CryptString	AboutStr9,	<Igor_Merkulov,_Dmitriy_Dudkin,_Igor_Chorny>
endif

AboutStrLast	label	byte

CryptLen1	equ	$-CryptText1


CryptText2	label	byte
  CryptString	InfoStr1,	<Internal_version_number:____V06.S06>
  CryptString	InfoStr2,	<Compilation_date:__________08/05/97>
  CryptString	InfoStr3,	<Chip_&_Dale,__Rescue_Rangers>
  CryptString	InfoStr4,	<Copyright_(C)_1993,_96,_97>
CryptLen2	equ	$-CryptText2
