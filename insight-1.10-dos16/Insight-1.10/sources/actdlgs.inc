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


AboutDialog	label	byte
		db	15, 2, 65, 21
		dw	AboutTitle
		dw	2
		db	0

		dw	InitStaticText
		dw	RunStaticText
		db	0
		dw	AboutLabels

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	20, 17
		db	10
		dw	OkButton
		dw	cmOk

InfoDialog	label	byte
		db	16, 4, 62, 20
		dw	InfoTitle
		dw	2
		db	0

		dw	InitStaticText
		dw	RunStaticText
		db	0
		dw	InfoLabels

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	18, 14
		db	10
		dw	OkButton
		dw	cmOk

SearchDialog	label	byte
		db	14, 6, 65, 17
		dw	SearchTitle
		dw	5
SearchDlgItem	db	0

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 3
		dw	SearchForLabel
		db	44, SearchFillLen
		dw	SearchString

		dw	InitCheckBoxes
		dw	RunCheckBoxes
		db	0
		db	3, 6
		dw	OptionsLabel
		dw	4
		dw	15
SearchBits	dw	0
		dw	IgnoreCaseCheck
		dw	BackwardCheck
		dw	ExcludeHMACheck
		dw	ResultCodeCheck

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	25, 6
		dw	AddrNameLabel
		db	22, AddrStrLen
		dw	SearchAddrStr

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	25, 9			; Button1
		db	10
		dw	SearchButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	38, 9			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel

CopyDataDialog	label	byte
		db	23, 6, 57, 17
		dw	CopyTitle
		dw	5
CopyDlgItem	db	0

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 3
		dw	FromLabel
		db	AddrStrSize/2, AddrStrLen
		dw	FromString

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 6
		dw	ToLabel
		db	AddrStrSize/2, AddrStrLen
		dw	ToString

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 9
		dw	CountLabel
		db	AddrStrSize/2, AddrStrLen
		dw	CountString

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	21, 3			; Button1
		db	10
		dw	OkButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	21, 5			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel


FillDataDialog	label	byte
		db	22, 6, 58, 17
		dw	FillTitle
		dw	5
FillDlgItem	db	0


		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 3
		dw	FromLabel
		db	AddrStrSize/2, AddrStrLen
		dw	ToString

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	20, 3
		dw	CountLabel
		db	AddrStrSize/2, AddrStrLen
		dw	CountString

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 6
		dw	FillLabel
		db	29, SearchFillLen
		dw	FillString

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	11, 9			; Button1
		db	10
		dw	OkButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	23, 9			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel


WriteDataDialog	label	byte
ReadDataDialog	label	byte
		db	22, 6, 58, 17
BlockDlgTitle	dw	FillTitle
		dw	5
BlockDlgItem	db	0

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 3
		dw	FileNameLabel
		db	29, FileNameLen
		dw	FileNameString

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 6
BlockDlgLabel	dw	FromLabel
		db	AddrStrSize/2, AddrStrLen
		dw	FromString

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	20, 6
		dw	CountLabel
		db	AddrStrSize/2, AddrStrLen
		dw	CountString

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	11, 9			; Button1
		db	10
		dw	OkButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	23, 9			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel


QuestionDialog	label	byte
		db	18, 8, 63, 15
		dw	QuestionTitle
		dw	4
		db	0

		dw	InitStaticText
		dw	RunStaticText
		db	0
		dw	QuestionLabels

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	4, 5			; Button1
		db	10
		dw	AppendButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	17, 5			; Button1
		db	11
		dw	RewriteButton
		dw	cmOk+1

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	31, 5			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel


ArgDialog	label	byte
		db	21, 8, 59, 15
		dw	ArgTitle
		dw	3
		db	0

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 3
		dw	ArgNameLabel
		db	31, 80
		dw	CmdLineString

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	13, 5			; Button1
		db	10
		dw	OkButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	25, 5			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel

AddrDialog	label	byte
		db	20, 6, 51, 13
		dw	EnterAddrTitle
		dw	3
		db	0

		dw	InitInputLine
		dw	RunInputLine
		db	0
		db	3, 3
		dw	AddrNameLabel
		db	AddrStrSize, AddrStrLen
DAddrStr	dw	offset MainAddrString

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	3, 5			; Button1
		db	10
		dw	GoButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	18, 5			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel


LoadDialog	label	byte
		db	16, 4, 54, 19
		dw	LoadTitle
		dw	4
		db	0

		dw	InitInputLine		; String
		dw	RunInputLine
		db	0
ListString	db	5, 3
		dw	NameLabel
		db	27, FileNameLen
		dw	FName

		dw	InitList		; List
		dw	RunList
		db	0
		db	5, 6
		dw	FilesLabel
		dw	8
		dw	ListString

		dw	InitPushButton
		dw	RunPushButton
		db	0	; bfDefault
		db	24, 5			; Button1
		db	10
		dw	OkButton
		dw	cmOk

		dw	InitPushButton
		dw	RunPushButton
		db	0
		db	24, 8			; Button2
		db	10
		dw	CancelButton
		dw	cmCancel

InfoLabels	label	byte
		db	17, 4
		dw	MainTitle
		db	6, 7
		dw	InfoStr1
		db	6, 8
		dw	InfoStr2
		db	10, 11
		dw	InfoStr3
		db	11, 12
		dw	InfoStr4
		dw	0

AboutLabels	label	byte
		db	19, 4
		dw	MainTitle
		db	26-(AboutStr2-AboutStr1)/2, 6
		dw	AboutStr1
		db	26-(AboutStr3-AboutStr2)/2, 7
		dw	AboutStr2
		db	26-(AboutStr4-AboutStr3)/2, 8
		dw	AboutStr3
		db	26-(AboutStr5-AboutStr4)/2, 10
		dw	AboutStr4
		db	26-(AboutStr6-AboutStr5)/2, 11
		dw	AboutStr5
		db	26-(AboutStr7-AboutStr6)/2, 12
		dw	AboutStr6
ifdef PersonalInfo
		db	26-(AboutStrLast-AboutStr7)/2, 15
		dw	AboutStr7
else
		db	26-(AboutStr8-AboutStr7)/2, 14
		dw	AboutStr7
		db	26-(AboutStr9-AboutStr8)/2, 15
		dw	AboutStr8
		db	26-(AboutStrLast-AboutStr9)/2, 16
		dw	AboutStr9
endif
		dw	0

QuestionLabels	label	byte
		db	4, 3
		dw	QuestionStr1
		db	6, 4
		dw	QuestionStr2
		dw	0

NameLabel	db	'~N~ame',0
FileNameLabel	db	'File ~n~ame',0
FilesLabel	db	'~F~iles',0
ArgNameLabel	db	'~C~ommand line',0
AddrNameLabel	db	'~A~ddress',0
SearchForLabel	db	'Search ~f~or',0
OptionsLabel	db	'~O~ptions',0
FromLabel	db	'~F~rom',0
ToLabel		db	'~T~o',0
CountLabel	db	'~C~ount',0
FillLabel	db	'F~i~ll with',0

IgnoreCaseCheck	db	'~I~gnore case',0
BackwardCheck	db	'~B~ackward',0
ExcludeHMACheck	db	'~E~xclude HMA',0
ResultCodeCheck	db	'~R~esult in code',0

OkButton	db	'~O~k',0
AppendButton	db	'~A~ppend',0
RewriteButton	db	'~R~ewrite',0
CancelButton	db	'~C~ancel',0
GoButton	db	'~G~o',0
SearchButton	db	'~S~earch',0

QuestionStr1	db	'File with the same name already exist.',0
QuestionStr2	db	'What do you want to do with file?',0

AboutTitle	db	7,	1*16+15,	' About '
InfoTitle	db	13,	1*16+15,	' Information '
QuestionTitle	db	10,	1*16+15,	' Question '
SearchTitle	db	8,	1*16+15,	' Search '
CopyTitle	db	12,	1*16+15,	' Copy Block '
FillTitle	db	12,	1*16+15,	' Fill Block '
ReadTitle	db	12,	1*16+15,	' Read Block '
WriteTitle	db	13,	1*16+15,	' Write Block '
ArgTitle	db	11,	1*16+15,	' Arguments '
EnterAddrTitle	db	6,	0,		' Goto '
LoadTitle	db	13,	0,		' Load a File '


