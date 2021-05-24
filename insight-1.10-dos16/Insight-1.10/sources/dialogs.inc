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


define _DIALOGS_

.errndef StringBufSize  ;! StringBuffer and StringBufSize must be defined
use _Window
use _ReadKey
use _ReadString
use _GetCursor
use _SetCursor

atDialogWindow	equ	LightGray*16+White	;Blue*16+White		; LightGray*16+White
atPushButton	equ	Green*16+0	;LightGray*16+Black	; Green*16+0
atlPushButton	equ	Green*16+Yellow	;LightGray*16+Yellow	; Green*16+Yellow
atsPushButton	equ	Green*16+White	;LightGray*16+White	; Green*16+White
atlsPushButton	equ	Green*16+Yellow	;LightGray*16+Yellow	; Green*16+Yellow
atPushButtonDef	equ	Green*16+Blue	;LightGray*16+Blue	; Green*16+Blue
atlPushButtonDef equ	Green*16+Yellow	;LightGray*16+Yellow	; Green*16+Yellow
atButtonShadow	equ	LightGray*16+Black	;Blue*16+Black		; LightGray*16+Black
atLabel		equ	LightGray*16+Black	;Blue*16+White		; LightGray*16+Black
atlLabel	equ	LightGray*16+Yellow	;Blue*16+Yellow		; LightGray*16+Yellow
atsLabel	equ	LightGray*16+White	;Blue*16+White		; LightGray*16+White
atlsLabel	equ	LightGray*16+Yellow	;Blue*16+White		; LightGray*16+Yellow
atInputLine	equ	Blue*16+White	;LightGray*16+Black	; Blue*16+White
atsInputLine	equ	Green*16+White	;Green*16+White		; Green*16+White
atListNormal	equ	Cyan*16+Black	;LightGray*16+Black	; Cyan*16+Black
atListSelected	equ	Green*16+White	;Green*16+White		; Green*16+White
atCheckBoxes	equ	Cyan*16+Black
atsCheckBoxes	equ	Cyan*16+White
atlCheckBoxes	equ	Cyan*16+Yellow
atRadioButtons	equ	Cyan*16+Black
atsRadioButtons	equ	Cyan*16+White
atlRadioButtons	equ	Cyan*16+Yellow

bfDefault	equ	1
cmCancel	equ	0
cmDefault	equ	1
cmOk		equ	1

;------------------------------------------------------------------------------
InitDialog	proc
; Expects: si=@struc
		call	pushr
		cld
		lodsw
		mov	DlgWinA,ax
		mov	bx,ax
		lodsw
		mov	DlgWinB,ax
		mov	dx,ax
		lodsw
		push	si
		mov	si,ax
		mov	byte ptr [si+1],atDialogWindow

ifdef __ACT__
		mov	ax,atDialogWindow shl 8
else
		mov	ah,atDialogWindow
endif

		call	OpenWindow
		pop	si
		mov	es,B800h
		lodsw
		mov	cx,ax
		mov	DlgItems,ax
		jcxz	@@Quit

		mov	DlgActiveItem,si
		lodsb

		mov	di,offset DlgItemsData
@@Next:
		lodsw
		add	si,3
		mov	[di],si
		inc	di
		inc	di
		call	ax
		loop	@@Next
@@Quit:
;		mov	DlgRunPtr,si
		call	popr
		ret
endp

;------------------------------------------------------------------------------
ExecDialog	proc
		mov	si,DlgActiveItem
		mov	byte ptr ds:[si],0

ExecDialog_:
		call	pushr
		call	GetCursor
		mov	@@curshape,cx
		mov	@@curpos,dx
		mov	cx,2020h
		call	SetCursor
		mov	es,B800h
;		mov	di,DlgRunPtr
		cld
		mov	cx,DlgItems
		jcxz	@@Quit

		mov	bx,DlgActiveItem
		mov	bl,ds:[bx]
		mov	bh,0

@@Next:
		push	cx
		mov	cx,2020h
		call	SetCursor
		pop	cx
		mov	di,bx
		add	di,di
		mov	si,DlgItemsData[di]
		call	word ptr [si-3]
		or	ax,ax
		jz	@@Quit
		cmp	ax,1
		jz	@@Enter
@@FindNext:
		cmp	ax,2
		je	@@Inc
		or	bx,bx
		jz	@@End
		dec	bx
		jmp	@@CheckStatic
@@End:
		mov	bx,cx
		dec	bx
		jmp	@@CheckStatic
@@Inc:
		inc	bx
		cmp	bx,cx
		jb	@@CheckStatic
@@Home:
		xor	bx,bx

@@CheckStatic:
		mov	di,bx
		add	di,di
		mov	si,DlgItemsData[di]
		cmp	word ptr [si-3],offset RunStaticText
		jne	@@Next
		jmp	@@FindNext

	@@Quit:
		xor	ax,ax		; cmCancel
		mov	si,DlgActiveItem
		mov	bl,ds:[si]

	@@Exit:
		mov	si,DlgActiveItem
		mov	ds:[si],bl

	@@RestoreCursor:
		mov	@@res,ax
		mov	cx,@@curshape
		call	SetCursor
		mov	dx,@@curpos
		call	GoToXY
		call	popr
		mov	ax,@@res
		or	ax,ax
		ret
@@Enter:
		mov	ax,cmDefault
		cmp	word ptr [si-3],offset RunPushButton
		jne	@@Exit
		mov	ax,[si+5]
		jmp	@@Exit
@@res		dw	0
@@curshape	dw	2020h
@@curpos	dw	0
endp

;------------------------------------------------------------------------------
ifndef __ACT__
DoneDialog	proc
		call	pushr
		mov	bx,DlgWinA
		mov	dx,DlgWinB
		call	CloseWindow
		call	popr
		ret
endp
endif


ifndef __ACT__
  MaxItems	equ	32
  DlgWinA	dw	0
  DlgWinB	dw	0
  ;DlgRunPtr	dw	0
  DlgItems	dw	0
  DlgItemsData	dw	MaxItems dup (0)
endif

;----------------------- Pushbutton -------------------------------------------
;------------------------------------------------------------------------------
InitPushButton	proc
		mov	bh,atPushButton
		mov	bl,atlPushButton
		test	byte ptr [si-1],bfDefault
		jz	@@1
		mov	bh,atPushButtonDef
		mov	bl,atlPushButtonDef
@@1:
		mov	dl,0
		call	DrawPushButton
		add	si,7
		ret
endp
DrawPushButton	proc
		call	pushr
		lodsw
		add	ax,DlgWinA
		call	GetAddr
;		mov	di,ax
;		mov	bp,ax
		lodsb
		cbw
		mov	cx,ax
		push	cx
		mov	al,' '
		or	dl,dl
		jz	@@2
		mov	ah,atDialogWindow
		stosw
@@2:
		mov	ah,bh
		rep	stosw
		mov	ah,atButtonShadow
		or	dl,dl
		jnz	@@3
		mov	al,'Ü'
		stosw
		mov	al,'ß'
@@3:
		mov	di,bp
		add	di,162
		pop	cx
		push	cx
		rep	stosw
		lodsw
		mov	si,ax
		call	PointLen
		pop	ax
;		dec	ax
		shr	ax,1
		shr	cx,1
		sub	ax,cx
		jns	@@1
		xor	ax,ax
@@1:
		add	al,dl
		mov	di,bp
		add	di,ax
		add	di,ax
		mov	ah,bh
		call	DrawText
		call	popr
		ret
endp

;------------------------------------------------------------------------------
DrawText	proc		; ah=main attr, bl=letter attr
@@Next:
		lodsb
		cmp	al,0
		je	@@Done
		cmp	al,'~'
		jne	@@Store
		xchg	ah,bl
		jmp	@@Next
@@Store:
		stosw
		jmp	@@Next
@@Done:
		ret
endp

;------------------------------------------------------------------------------
RunPushButton	proc
		push	bx cx
		mov	bh,atsPushButton
		mov	bl,atlsPushButton
		mov	dl,0
		call	DrawPushButton
@@Again:
		call	ReadKey
		call	CheckMoveKey
		jc	@@Again

		mov	bh,atPushButton
		mov	bl,atlPushButton
		cmp	ax,1
		jne	@@Quit
		mov	dl,1
		call	DrawPushButton
		xor	cx,cx
		loop	$
@@Quit:
		test	byte ptr [si-1],bfDefault
		jz	@@1
		mov	bh,atPushButtonDef
		mov	bl,atlPushButtonDef
@@1:
		mov	dl,0
		call	DrawPushButton
		pop	cx bx
		ret
endp


;------------------------ InputLine -------------------------------------------
;------------------------------------------------------------------------------
InitInputLine	proc
		call	DrawInputLine
		add	si,8
		ret
endp

DrawInputLine	proc
		call	pushr
		lodsw
		add	ax,DlgWinA
		call	GetAddr
;		mov	di,ax
		lodsw
		mov	dl,0
		call	DrawLabel
		mov	ah,atInputLine
		mov	al,' '
		stosw
		lodsw
		cbw
		mov	cx,ax
		lodsw
		mov	si,ax
		mov	ah,atInputLine
@@Next:
		lodsb
		cmp	al,0
		je	@@Done
		stosw
		loop	@@Next
@@Done:
		mov	al,' '
		inc	cx
		rep	stosw
		call	popr
		ret
endp

RunInputLine	proc
		push	ReadExitKeys
		push	bx cx si bp
		mov	ReadExitKeys,offset InpLineExitKeys
		lodsw
		add	ax,DlgWinA
		push	ax
		call	GetAddr
;		mov	di,ax
		lodsw
		mov	dl,1
		call	DrawLabel
		lodsw
		mov	cx,ax
		lodsw
		mov	di,ax
		pop	ax
		sub	ax,0102h
		mov	bl,atInputLine
		mov	bh,atsInputLine
		call	ReadLine
		call	CheckMoveKey
		pop	bp si cx bx
		call	DrawInputLine
		pop	ReadExitKeys
		ret
endp
InpLineExitKeys	dw	kbEnter, kbEsc, kbTab, kbShiftTab, 0

;------------------------------------------------------------------------------
DrawLabel	proc
		call	pushr
		sub	di,160
		mov	si,ax
		mov	ah,atLabel
		mov	bl,atlLabel
		or	dl,dl
		jz	@@1
		mov	ah,atsLabel
		mov	bl,atlsLabel
@@1:
		call	DrawText
		call	popr
		ret
endp


;---------------------------- String list -------------------------------------
;------------------------------------------------------------------------------
InitList	proc
		mov	ListChoice,-1
		mov	ListFirst,0
		mov	dl,0
		call	DrawList
		add	si,8
		ret
endp

DrawList	proc
		call	pushr
		lodsw
		add	ax,DlgWinA
		call	GetAddr
;		mov	bp,ax
;		mov	di,ax
		mov	ListStart,ax

		lodsw
		call	DrawLabel

		lodsw
		mov	ListHeight,ax
		lodsw
		mov	LinkString,ax
		mov	ax,ListFirst
		call	GetStringAddr
		mov	si,ax
;		mov	si,offset StringBuffer
		mov	cx,StringSize
		mov	dx,StringCount
		mov	bx,ListHeight
		cmp	dx,bx
		jbe	@@ns
		mov	dx,bx
@@ns:
		sub	bx,dx
		push	bx
		mov	bx,ListFirst
		or	dx,dx
		jz	@@Clear
@@NextLine:
		mov	ah,atListNormal
		cmp	bx,CurStr
		jne	@@1
		mov	ah,atListSelected
@@1:
		push	cx
		mov	di,bp
		mov	al,' '
		stosw
@@Next1:
		lodsb
		stosw
		loop	@@Next1
		pop	cx
		inc	bx
		add	bp,160
		dec	dx
		jnz	@@NextLine
@@Clear:
		pop	bx
		or	bx,bx
		jz	@@all
@@NextClear:
		mov	ah,atListNormal
		mov	di,bp
		push	cx
		inc	cx
		mov	al,' '
		rep	stosw
		pop	cx
		add	bp,160
		dec	bx
		jnz	@@NextClear
@@all:
		call	popr
		ret
@@r		dw	0
endp
;------------------------------------------------------------------------------
RunList		proc
		push	bx cx
		xor	bx,bx
		xor	cx,cx
		mov	dx,StringCount
@@Again:
		call	@@SetLink
		mov	CurStr,bx
		mov	ListFirst,cx
		push	dx
		mov	dl,1
		call	DrawList
		pop	dx
		call	ReadKey
		or	dx,dx
		jz	@@CMK
		cmp	ax,kbDown
		jne	@@1
		inc	bx
		cmp	bx,dx
		jb	@@locCorrect
@@End:
		mov	bx,dx
		dec	bx
@@locCorrect:
		jmp	@@Correct
@@1:
		cmp	ax,kbUp
		jne	@@2
		or	bx,bx
		jz	@@Again
		dec	bx
		jmp	@@Correct
@@2:
		cmp	ax,kbHome
		jne	@@3
@@Home:
		xor	bx,bx
		jmp	@@Correct
@@3:
		cmp	ax,kbEnd
		je	@@End
		cmp	ax,kbPgUp
		jne	@@4
		or	cx,cx
		jz	@@Home
		cmp	cx,ListHeight
		ja	@@BigUp
		xor	cx,cx
@@PgCorr:
		mov	bx,cx
		add	bx,ListHeight
		dec	bx
		jmp	@@Correct
@@BigUp:
		sub	cx,ListHeight
		jmp	@@PgCorr
@@4:
		cmp	ax,kbPgDn
		jne	@@CMK
		cmp	dx,ListHeight
		jbe	@@End
		push	cx
		add	cx,ListHeight
		cmp	cx,dx
		jne	@@5
		pop	cx
		jmp	@@End
@@5:
		add	cx,ListHeight
		cmp	cx,dx
		pop	cx
		jbe	@@BigDown
		mov	cx,dx
		sub	cx,ListHeight
		mov	bx,cx
		jmp	@@Correct
@@BigDown:
		add	cx,ListHeight
		mov	bx,cx
		jmp	@@Correct
@@CMK:
		call	CheckMoveKey
		jc	@@locAgain
		cmp	ax,1
		jne	@@Quit
		mov	ListChoice,bx
		call	@@SetLink
@@Quit:
		mov	CurStr,-1
		mov	dl,0
		call	DrawList
		pop	cx bx
		ret
@@Correct:
		cmp	bx,cx
		jb	@@Lower
		push	cx
		add	cx,ListHeight
		cmp	bx,cx
		pop	cx
		jb	@@locAgain
@@Upper:
		mov	cx,bx
		sub	cx,ListHeight
		inc	cx
		jmp	@@Again
@@Lower:
		mov	cx,bx
@@locAgain:
		jmp	@@Again
@@SetLink:
		cmp	LinkString,0
		je	@@SkipLink
		or	dx,dx
		jz	@@SkipLink
		push	ax cx si
		push	es
		push	cs
		pop	es
		mov	di,LinkString
		push	di
		mov	di,[di+6]
		mov	ax,bx
		call	GetStringAddr
		mov	si,ax
		mov	cx,StringSize
		push	cx
		rep	movsb
		pop	cx
		call	TrimSpaces
		pop	si
		pop	es
		call	DrawInputLine
		pop	si cx ax
@@SkipLink:
		ret
endp
TrimSpaces	proc
		push	di
		std
		dec	di
		mov	al,' '
		repe	scasb
		je	@@Emp
		inc	di
@@Emp:		inc	di
		mov	al,0
		cld
		stosb
		pop	di
		ret
endp

;------------------------------------------------------------------------------
InitStringList	proc
; Expects: ax=StringSize
		mov	StringSize,ax
		mov	StringCount,0
		mov	StringPtr,offset StringBuffer
		ret
endp

;------------------------------------------------------------------------------
AddString	proc
; Expects: si=@string
; Returns: CF=1 if string buffer is full
		mov	ax,offset StringBuffer+StringBufSize
		sub	ax,StringSize
		cmp	StringPtr,ax
		jb	@@1
		stc
		ret
@@1:
		call	pushr
		push	cs
		pop	es
		mov	di,StringPtr
		cld
		mov	cx,StringSize
@@Next:
		lodsb
		cmp	al,0
		je	@@Done
		stosb
		loop	@@Next
@@End:
		mov	StringPtr,di
		inc	StringCount
		call	popr
		clc
		ret
@@Done:
		mov	al,' '
		rep	stosb
		jmp	@@End
endp


;---------------------------- Static Text -------------------------------------
;------------------------------------------------------------------------------
InitStaticText	proc
		lodsw
		push	si di bp
		xchg	ax,si
	@@Next:
		lodsw
		or	ax,ax
		jz	@@Exit
		add	ax,DlgWinA
		call	GetAddr
;		mov	di,ax
		lodsw
		mov	dl,0
		call	DrawLabel
		jmp	@@Next
	@@Exit:
		pop	bp di si
		ret
		endp

RunStaticText	proc
		mov	ax,2
		ret
		endp



;---------------------------- Check Boxes -------------------------------------
;------------------------------------------------------------------------------
InitCheckBoxes	proc
		mov	dl,0
		mov	CBcur,-1
		call	DrawCheckBoxes
		mov	si,CBSI
		ret
endp

DrawCheckBoxes	proc
		call	pushr
		lodsw
		add	ax,DlgWinA
		call	GetAddr
;		mov	di,ax
;		mov	bp,ax
		lodsw
		call	DrawLabel
		lodsw
		mov	cx,ax
		sub	al,CBcur
		mov	CBcur,al
		lodsw
		mov	CBmax,ax
		lodsw
		xchg	ax,dx	; on/off

@@Next:
		lodsw
		push	si
		xchg	ax,si

		mov	ah,atCheckBoxes
		cmp	CBcur,cl
		jne	@@2
		mov	ah,atsCheckBoxes
@@2:
		mov	al,' '
		stosw
		mov	al,'['
		stosw
		mov	al,' '
		shr	dx,1
		jnc	@@1
		mov	al,'X'
@@1:
		stosw
		mov	al,']'
		stosw
		mov	al,' '
		stosw
		mov	bl,atlCheckBoxes
		push	cx
		mov	cx,di
		add	cx,CBmax
		add	cx,CBmax

		call	DrawText

		sub	cx,di
		shr	cx,1
		mov	al,' '
		rep	stosw
		pop	cx
		add	bp,160
		mov	di,bp

		pop	si

		loop	@@Next

		mov	CBSI,si
		call	popr
		ret
endp
CBmax		label	word
CBSI		dw	0
CBcur		db	-1

RunCheckBoxes	proc
		push	bx cx
		mov	cx,0607h
		call	SetCursor
		xor	cx,cx
@@Again:
		mov	dx,[si]
		add	dx,DlgWinA
		add	dh,cl
		inc	dx
		inc	dx
		call	GoToXY
		mov	dl,1
		mov	CBcur,cl
		call	DrawCheckBoxes

		call	ReadKey
		cmp	ax,kbDown
		jne	@@D
		inc	cx
		cmp	cx,[si+4]
		jb	@@Again
		xor	cx,cx
		jmp	@@Again
@@D:
		cmp	ax,kbUp
		jne	@@U
		dec	cx
		jns	@@Again
		mov	cx,[si+4]
		dec	cx
		jmp	@@Again
@@U:
		cmp	ax,kbSpace
		jne	@@S
		mov	ax,1
		shl	ax,cl
		xor	[si+8],ax
		jmp	@@Again
@@S:
		call	CheckMoveKey
		jc	@@Again
		mov	dl,0
		mov	CBcur,-1
		call	DrawCheckBoxes
		pop	cx bx
		ret
endp


ifndef __ACT__
;---------------------------- Radio Buttons -----------------------------------
;------------------------------------------------------------------------------
InitRadioButtons proc
		mov	dl,0
		call	DrawRadioButtons
		mov	si,CBSI
		ret
endp

DrawRadioButtons proc
		call	pushr
		lodsw
		add	ax,DlgWinA
		call	GetAddr
;		mov	di,ax
;		mov	bp,ax
		lodsw
		call	DrawLabel
		lodsw
		mov	cx,ax
;		sub	al,CBcur
;		mov	CBcur,al
		lodsw
		mov	CBmax,ax
		lodsw
		mov	bl,dl
		mov	dx,cx
		sub	dx,ax	; pos
		mov	dh,bl
@@Next:
		mov	ah,atRadioButtons
		mov	bh,' '
		cmp	dl,cl
		jne	@@2
		mov	bh,7
		or	dh,dh
		jz	@@2
		mov	ah,atsRadioButtons
@@2:
		mov	al,' '
		stosw
		mov	al,'('
		stosw
		mov	al,bh
		stosw
		mov	al,')'
		stosw
		mov	al,' '
		stosw
		mov	bl,atlRadioButtons
		push	cx
		mov	cx,di
		add	cx,CBmax
		add	cx,CBmax
		call	DrawText
		sub	cx,di
		shr	cx,1
		mov	al,' '
		rep	stosw
		pop	cx
		add	bp,160
		mov	di,bp
		loop	@@Next

		mov	CBSI,si
		call	popr
		ret
endp

RunRadioButtons	proc
		push	bx cx
		mov	cx,0607h
		call	SetCursor
		mov	cx,[si+8]
@@Again:
		mov	dx,[si]
		add	dx,DlgWinA
		add	dh,cl
		inc	dx
		inc	dx
		call	GoToXY
		mov	dl,1
		mov	[si+8],cx
		call	DrawRadioButtons

		call	ReadKey
		cmp	ax,kbDown
		jne	@@D
		inc	cx
		cmp	cx,[si+4]
		jb	@@Again
		xor	cx,cx
		jmp	@@Again
@@D:
		cmp	ax,kbUp
		jne	@@U
		dec	cx
		jns	@@Again
		mov	cx,[si+4]
		dec	cx
		jmp	@@Again
@@U:
		call	CheckMoveKey
		jc	@@Again
		mov	dl,0
		mov	CBcur,-1
		call	DrawRadioButtons
		pop	cx bx
		ret
endp

endif

;------------------------------------------------------------------------------
GetStringAddr	proc
; Expects: ax=number
; Returns: ax=@str
		push	dx
		mul	StringSize
		add	ax,offset StringBuffer
		pop	dx
		ret
endp

;------------------------------------------------------------------------------
PointLen	proc
		xor	cx,cx
		push	si
@@Next:
		lodsb
		cmp	al,0
		je	@@Done
		cmp	al,'~'
		je	@@Next
		inc	cx
		jmp	@@Next
@@Done:
		pop	si
		ret
endp

;------------------------------------------------------------------------------
CheckMoveKey	proc
		cmp	ax,kbEsc
		je	@@0
		cmp	ax,kbEnter
		je	@@1
		cmp	ax,kbTab
		je	@@2
		cmp	ax,kbShiftTab
		je	@@3
		stc
		ret
@@0:		xor	ax,ax
		ret
@@1:		mov	ax,1
		ret
@@2:		mov	ax,2
		ret
@@3:		mov	ax,3
		ret
endp

StringCount	dw	0
StringPtr	dw	0
StringSize	dw	64
ListStart	dw	0
ListHeight	dw	0
CurStr		dw	-1
LinkString	dw	0
ListChoice	dw	-1
ListFirst	dw	0
