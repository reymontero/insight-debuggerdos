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


;
;			Windows unit
;	WindowBuffer must be at the end of program (or resident part)
;	Requires:  WindowBuffer (label), WindowBufSize (const)
;

define		_WINDOWS_

ifdef _ExecMenu

use _Window
use _ReadKey
use _VertMenu
SubMenu		equ	1

;Menu structure:	dw	HorCnt, @HorStr, @HorSub
ExecMenu	proc
	; Expects: si=@Menu
	; Returns: ax=ExitCode

		xor	bx,bx
		mov	dx,79
		call	SaveWindow

		cld
		lodsw
		mov	cx,ax
		mov	bx,HPos
@@Next:
		call	UpdateHMenu
@@Read:
		call	ReadKey
		cmp	ax,kbEsc
		je	@@Esc
		cmp	ax,kbAltAlone
		je	@@Esc
		cmp	ax,kbEnter
		je	@@Enter
		cmp	ax,kbDown
		je	@@Enter
		cmp	ax,kbRight
		je	@@Right
		cmp	ax,kbLeft
		je	@@Left
		push	si
		mov	si,[si]
		call	FindMenuLetter
		pop	si
		jc	@@Read
		mov	bx,ax
		call	UpdateHMenu
		jmp	@@Enter
@@Right:
		inc	bx
		cmp	bx,cx
		jb	@@Next
		xor	bx,bx
		jmp	@@Next
@@Left:
		dec	bx
		jns	@@Next
		mov	bx,cx
		dec	bx
		jmp	@@Next
@@Enter:
		push	si
		mov	si,[si+2]
		call	ExecSubMenu
		pop	si
		cmp	ax,2
		je	@@LeftExp
		cmp	ax,3
		je	@@RightExp
		cmp	ax,100h
		jae	@@Quit
@@Esc:
		xor	ax,ax
@@Quit:
		mov	PointOptions,0
		mov	HPos,bx
		xor	bx,bx
		mov	dx,79
		call	RestoreWindow
		or	ax,ax
		jz	@@ret
		call	ax
@@ret:
		ret
@@LeftExp:
		dec	bx
		jns	@@Exp
		mov	bx,cx
		dec	bx
		jmp	@@Exp
@@RightExp:
		inc	bx
		cmp	bx,cx
		jb	@@Exp
		xor	bx,bx
@@Exp:
		call	UpdateHMenu
		jmp	@@Enter
endp

ExecSubMenu	proc
  ; bx = menu item # (zero based), si = offset menu actions table
		push	bx cx dx si
		push	SubStruc
		push	SubCoord
		shl	bx,1
		shl	bx,1
		mov	ax,[bx+si]
		test	al,Disabled
		jz	@@Enabled
		mov	ax,1
		jmp	@@Quit2
@@Enabled:
		and	ax,not Disabled
		or	ax,ax
		jnz	@@SubMenu
		mov	ax,[bx+si+2]
;		call	word ptr [bx+si+2]
;		xor	ax,ax
		jmp	@@Quit2
@@SubMenu:
		mov	si,[bx+si+2]
		lodsw
		mov	SubCoord,ax
		mov	bx,ax
		lodsw
		mov	dx,ax

ifdef __ACT__
		mov	ax,(MenuNormalAttr shl 8) or 1
else
		mov	ah,MenuNormalAttr
endif

		push	bx dx
		push	si
		mov	si,offset EmpTitle
		mov	cx,SingleBorder
		call	OpenWindowBord
		pop	si

		mov	SubStruc,si
@@NextMenu:
		lodsw
		mov	cx,ax
		lodsw
		mov	dx,ax
		lodsw
		mov	bp,ax
		mov	PointOptions,ax
		lodsw
		mov	si,ax
		mov	ax,SubCoord
		add	ax,0101h
		mov	bl,MenuNormalAttr
		mov	bh,MenuSelAttr
		call	VertMenu
		mov	si,SubStruc
		mov	[si+2],ah
		cmp	al,-3
		jae	@@QuitA

		push	si
		mov	bl,al
		mov	bh,0
		mov	si,bp
		call	ExecSubMenu
		pop	si
		cmp	ax,1
		je	@@NextMenu

@@Quit:
		pop	dx bx
		call	CloseWindow
@@Quit2:
		pop	SubCoord
		pop	SubStruc
		pop	si dx cx bx
		ret
@@QuitA:
		neg	al
		mov	ah,0
		jmp	@@Quit
endp

UpdateHMenu	proc
		call	pushr
		mov	si,[si]
		mov	es,B800h
		xor	di,di
		xor	bp,bp
		mov	ah,MenuNormalAttr
		mov	al,' '
		stosw
@@NextPoint:
		mov	ah,MenuNormalAttr
		mov	dh,MenuNormalLetter
		cmp	bx,bp
		jne	@@1
		mov	ah,MenuSelAttr
		mov	dh,MenuSelLetter
@@1:
		mov	al,' '
		stosw
@@Next:
		lodsb
		cmp	al,0
		je	@@End
		cmp	al,'~'
		je	@@Sw
		stosw
		jmp	@@Next
@@Sw:
		xchg	ah,dh
		jmp	@@Next
@@End:
		mov	al,' '
		stosw
		inc	bp
		cmp	bp,cx
		jb	@@NextPoint

		mov	cx,160
		sub	cx,di
		shr	cx,1
		mov	al,' '
		mov	ah,MenuNormalAttr
		rep	stosw

		call	popr
		ret
endp

LocalMenu	proc
; Expects: si=@struc
		call	pushr

		call	GetCursor
		push	cx
		mov	cx,2020h
		call	SetCursor
		xor	bx,bx
		mov	@@FakeTable[2],si
		mov	si,offset @@FakeTable
		call	ExecSubMenu
		mov	@@FakeTable[2],ax
		pop	cx
		call	SetCursor

		call	popr
		mov	ax,@@FakeTable[2]
		cmp	ax,100h
		ret

@@FakeTable	dw	SubMenu, ?

endp

HPos		dw	0
SubStruc	dw	0
SubCoord	dw	0
MenuNormalAttr	equ	LightGray*16+Black
MenuNormalLetter equ	LightGray*16+Red
MenuSelAttr	equ	Green*16+Black
MenuSelLetter	equ	Green*16+Red

endif

ifdef _ReadString

use _SetCursor
use _GetCursor
use _Window
use _ReadKey
use _StrLen
use _GoToXY
atReadWindow	equ	Blue*16+White	;LightGray*16+Black
atReadString	equ	Blue*16+White	;LightGray*16+White
atsReadString	equ	Green*16+White
;------------------------------------------------------------------------------
ifndef __ACT__
ReadString	proc
	; Expects: al=x, ah=y, di=@buffer, cl=width, ch=maxlen, si=@title
		push	bx dx
		mov	bx,ax
		mov	dx,ax
		add	dl,cl
		add	dl,5
		add	dh,2
		push	bx dx
		push	ax
		mov	ah,atReadWindow
		call	OpenWindow
		pop	ax
		mov	bl,atReadString
		mov	bh,atsReadString
		call	ReadLine
		pop	dx bx
		call	CloseWindow
		pop	dx bx
		ret
endp
endif

;------------------------------------------------------------------------------
ReadLine	proc
		call	pushr
		push	cs
		pop	es
		mov	StringChanged,1
		mov	word ptr ReadStringAttr,bx
		add	ax,0102h
		mov	StrStart,ax
		push	di
		call	GetAddr
		pop	di
;		mov	bp,ax
		mov	byte ptr StrLength,cl
		mov	byte ptr MaxStrLength,ch
		mov	StrBuffer,di
		mov	si,di
		add	si,MaxStrLength
		mov	byte ptr [si],0
		push	cx
		call	GetCursor
		mov	@@Cur,cx
		mov	@@CurC,dx
		mov	cx,0607h
		call	SetCursor
		pop	cx
;		mov	ch,0

		mov	si,di
		call	StrLen

		xor	bx,bx
		xor	dx,dx
		jmp	@@Next1
@@Next:
		mov	StringChanged,0
@@Next1:
		call	UpdateString
@@Read:
		call	ReadKey
		call	CheckExitRead
		jc	@@1
		jmp	@@Ignore
@@1:		cmp	ax,kbRight
		je	@@Right
		cmp	ax,kbLeft
		je	@@Left
		cmp	ax,kbHome
		jne	@@5
		jmp	@@Home
@@5:		cmp	ax,kbEnd
		jne	@@4
		jmp	@@End
@@4:		cmp	ax,kbDel
		je	@@Delete
		cmp	ax,kbBackSpace
		je	@@BackSpace
		cmp	bl,byte ptr MaxStrLength
		jae	@@Next
		call	TestChar
		jc	@@Read
		cmp	StringChanged,1
		je	@@Clear
@@Cleared:
		push	cx
		mov	cx,MaxStrLength
		mov	di,StrBuffer
		add	di,cx
		dec	di
		lea	si,[di-1]
		sub	cx,bx
		jz	@@3
		dec	cx
		std
		rep	movsb
@@3:
		cld
		mov	si,StrBuffer
;		add	si,bx
		mov	[si+bx],al
		pop	cx
		cmp	cx,MaxStrLength
		jae	@@Right
		inc	cx
@@Right:
		cmp	bl,cl
		jne	@@2
@@locRead:
		jmp	@@Read
@@2:		inc	bx
		mov	ax,bx
		sub	ax,dx
		cmp	ax,StrLength
		jbe	@@locNext
		inc	dx
		jmp	@@Next
@@Clear:
		mov	si,StrBuffer
		mov	byte ptr [si],0
		xor	cx,cx
		xor	bx,bx
		jmp	@@Cleared

@@Left:
		or	bl,bl
		je	@@locNext
		dec	bx
		cmp	bx,dx
		jae	@@locNext
		mov	dx,bx
@@locNext:
		jmp	@@Next
@@BackSpace:
		or	bx,bx
		je	@@locRead
		dec	bx
@@Delete:
		or	cx,cx
		je	@@locRead
		cmp	bx,cx
		jae	@@locRead
		mov	di,StrBuffer
		add	di,bx
		lea	si,[di+1]
		push	cx
		mov	cx,MaxStrLength
		sub	cx,bx
		cld
		rep	movsb
		mov	al,0
		stosb
		pop	cx
		dec	cx
		or	dx,dx
		jz	@@locNext
		mov	ax,cx
		sub	ax,dx
		cmp	ax,StrLength
		jae	@@locNext
		dec	dx
		jmp	@@Next
@@Home:
		xor	bx,bx
		xor	dx,dx
		jmp	@@Next
@@End:
		mov	bx,cx
		cmp	bx,StrLength
		jbe	@@locNext
		mov	dx,bx
		sub	dx,StrLength
		jmp	@@Next

@@Ignore:
;		mov	@@Result,0
;		jmp	@@Quit
@@Enter:
		mov	@@Result,ax
@@Quit:
		mov	cx,@@Cur
		call	SetCursor
		mov	dx,@@CurC
		call	GoToXY
		call	popr
		mov	ax,@@Result
		ret
@@Result	dw	0
StringChanged	db	1
@@Cur		dw	0607h
@@CurC		dw	0
StrLength	dw	0
MaxStrLength	dw	0
StrBuffer	dw	0
StrStart	dw	0
endp

ReadStringAttr	db	1*16+15
ReadStringSelAttr db	2*16+15

UpdateString	proc
		call	pushr
		mov	ax,dx
		mov	dx,StrStart
		add	dl,bl
		sub	dl,al
		inc	dl
		call	GoToXY
		mov	dx,ax
		mov	di,bp
		mov	es,B800h
		mov	si,StrBuffer
		add	si,ax
		mov	ah,ReadStringAttr
		xor	dx,dx
		mov	al,' '
		stosw
		cmp	StringChanged,0
		je	@@NotSel1
		mov	ah,ReadStringSelAttr
@@NotSel1:
		jcxz	@@Skip
		cmp	cx,StrLength
		jbe	@@Next
		mov	cx,StrLength
@@Next:
		lodsb
		stosw
		inc	dx
		loop	@@Next
@@Skip:
		mov	ah,ReadStringAttr
		sub	dx,StrLength
		ja	@@Quit
		mov	cx,dx
		neg	cx
		inc	cx
		mov	al,' '
		rep	stosw
@@Quit:
		call	popr
		ret
endp


TestChar	dw	DefaultTestChar
DefaultTestChar	proc
;		cmp	al,0
;		je	@@1
;		clc
;		ret
;@@1:
;		stc
;		ret

		cmp	al,' '
		jb	@@Exit
		cmp	ax,kbAltAlone
		cmc
  @@Exit:
		ret
endp

CheckExitRead	proc
		push	ax bx si
		mov	bx,ax
		mov	si,ReadExitKeys
@@Next:
		lodsw
		cmp	ax,bx
		je	@@Quit
		or	ax,ax
		jnz	@@Next
@@End:
		stc
@@Quit:
		pop	si bx ax
		ret
endp
ReadExitKeys	dw	DefaultExitKeys
DefaultExitKeys	dw	kbEsc, kbEnter, 0
endif


ifdef _VertMenu

.errndef	_KEYBOARD_
use _Window
use _KeyLoop
use _UpCase
;use _WritePoint
Separator	equ	0
Disabled	equ	2
atDisabled	equ	7*16+8
atSelDisabled	equ	Green*16+8

;------------------------------------------------------------------------------
VertMenu	proc
;		Expects: al=x1, ah=y1, cx=count, bl=normal attr, bh=sel attr,
;			 dl=selected point, dh = width, si=@items
;			Items: 'First', 0, 'Second', 0, ...
;		Returns: al=point (-1 if Esc pressed, -2 if Left, -3 if Right)

		call	pushr
		push	Keys
		push	TestKey
		call	GetAddr
		mov	es,B800h
;		mov	bp,ax
		call	WriteItems
		mov	Keys,offset MenuKeys
		mov	TestKey,offset VMTestKey
		call	KeyLoop
		cmp	ax,kbEsc
		je	@@Cancel
		cmp	ax,kbLeft
		je	@@Left
		cmp	ax,kbRight
		je	@@Right
		mov	al,dl
		jmp	@@Quit
@@Cancel:
		mov	al,-1
@@Quit:
		mov	byte ptr @@Result,al
		mov	byte ptr @@Result+1,dl
		pop	TestKey
		pop	Keys
		call	popr
		mov	ax,@@Result
		ret
@@Left:
		mov	al,-2
		jmp	@@Quit
@@Right:
		mov	al,-3
		jmp	@@Quit
@@Result	dw	00FFh

@@Down:
		inc	dl
		cmp	dl,cl
		jb	@@ret
@@Home:
		mov	dl,0
@@ret:
		jmp	WriteItems

@@Up:
		dec	dl
		cmp	dl,-1
		jne	@@ret
@@End:
		mov	dl,cl
		dec	dl
		jmp	@@ret

VMTestKey	proc
		mov	al,byte ptr CurTestCode
		call	FindMenuLetter
		jc	@@Quit
		mov	dl,al
		call	WriteItems
		mov	ax,kbEnter
@@Quit:
		ret
endp

MenuKeys	dw	kbEsc,   0
		dw	kbEnter, 0
		dw	kbLeft,  0
		dw	kbRight, 0
		dw	kbUp,   @@Up
		dw	kbDown, @@Down
		dw	kbHome, @@Home
		dw	kbEnd,  @@End
		dw	kbPgUp, @@Home
		dw	kbPgDn, @@End
		dw	0,	0
endp

MenuItems	dw	0
PointOptions	dw	0

WriteItems	proc
		push	ax cx si bp
		mov	al,0
@@Next:
		push	cx
@@NextStr:
		mov	di,bp
		mov	ah,bl
		cmp	byte ptr [si],0
		je	@@Separator
		cmp	PointOptions,0
		je	@@NoOpt
		push	bx
		mov	bl,al
		mov	bh,0
		add	bx,bx
		add	bx,bx
		add	bx,PointOptions
		test	byte ptr [bx],Disabled
		pop	bx
		jz	@@NoOpt
		mov	ch,atDisabled
		mov	ah,atDisabled
		cmp	al,dl
		jne	@@Write
		mov	ch,atSelDisabled
		mov	ah,atSelDisabled
		jmp	@@Write
@@NoOpt:
		mov	ch,NormalLetter
		cmp	al,dl
		jne	@@Write
		mov	ah,bh
		mov	ch,AccentLetter
@@Write:
		call	WritePoint
		add	bp,160
		inc	ax
		pop	cx
		loop	@@Next
		pop	bp si cx ax
		ret
@@Separator:
		push	ax
		inc	si
		sub	di,2
		mov	al,'√'
		stosw
		mov	al,'ƒ'
		mov	cl,dh
		mov	ch,0
		inc	cx
		rep	stosw
		mov	al,'¥'
		stosw
		add	bp,160
		pop	ax
		jmp	@@NextStr
endp


;------------------------------------------------------------------------------
WritePoint	proc
;		Expects: si=@str, ah=attr, es:di=@scr, dh=width
;			 dh=0 if don't care

		push	ax bx cx
		cld
		mov	bl,ch
		mov	cl,dh
		mov	ch,0
		mov	al,' '
		stosw
@@Next:
		lodsb
		cmp	al,0
		je	@@Adjust
		cmp	al,'~'
		je	@@Sw
		stosw
		dec	cx
		jmp	@@Next
@@Sw:
		xchg	ah,bl
		jmp	@@Next
@@Adjust:
		cmp	cx,0
		jle	@@Quit
		mov	al,' '
		rep	stosw
@@Quit:
		pop	cx bx ax
		ret
endp

NormalLetter	db	7*16+Red
AccentLetter	db	Green*16+Red

;------------------------------------------------------------------------------
FindMenuLetter	proc
		push	bx si
		call	UpCase
		mov	ah,al
		xor	bx,bx
@@Next:
		lodsb
		cmp	al,0
		je	@@EndPoint
		cmp	al,'~'
		jne	@@Next
		lodsb
		inc	si
		call	UpCase
		cmp	al,ah
		jne	@@Next
		mov	ax,bx
		jmp	@@Quit
@@EndPoint:
		cmp	byte ptr [si],0
		jne	@@1
		inc	si
@@1:
		inc	bx
		cmp	bx,cx
		jb	@@Next
		stc
@@Quit:
		pop	si bx
		ret
endp



endif

ifdef _MessageBox
use _Window
use _WriteString
use _StrLen
use _ReadKey
use _FlushKey
use _SetCursor
use _GetCursor
use _Beep

atMessageBox	equ	Blue*16+White
atMessage	equ	Blue*16+Yellow
atErrorBox	equ	Red*16+White
atErrorMsg	equ	Red*16+Yellow
;------------------------------------------------------------------------------
ErrorMessage	proc
		mov	MsgTitle,offset ErrorTitle
		mov	MsgBoxAttr,atErrorBox
		mov	MessageAttr,atErrorMsg
		jmp	ExecMessage
endp
;------------------------------------------------------------------------------
MessageBox	proc
		mov	MsgTitle,offset MessageTitle
		mov	MsgBoxAttr,atMessageBox
		mov	MessageAttr,atMessage
; !		jmp	ExecMessage
endp
;------------------------------------------------------------------------------
ExecMessage	proc
	; Expects: si=@Message
		call	Beep
		call	pushr

		cmp	ExecMsgType,1
		ja	@@SkipOpen

		call	GetCursor
		mov	@@C,cx
		mov	cx,2020h
		call	SetCursor
		mov	bh,10
		mov	dh,12
		call	StrLen
		shr	cx,1
		mov	al,39
		sub	al,cl		; al=message X
		cmp	cl,10
		jae	@@1
		mov	cl,10

	@@1:
		add	cl,2
		mov	bl,39
		sub	bl,cl
		mov	dl,39
		add	dl,cl

		mov	ah,MsgBoxAttr

		mov	bp,si
		mov	si,MsgTitle

		mov	@@SaveBX,bx
		mov	@@SaveDX,dx

ifdef __ACT__
		push	ax
		mov	al,1
endif

		call	OpenWindow

ifdef __ACT__
		pop	ax
endif

		mov	ah,11
		mov	bh,MessageAttr
		mov	si,bp
		call	WriteString

	@@SkipOpen:
		cmp	ExecMsgType,1
		je	@@SkipClose

		cmp	ExecMsgType,2
		je	@@SkipReadKey

		call	FlushKey

		call	ReadKey

	@@SkipReadKey:
		mov	bx,@@SaveBX
		mov	dx,@@SaveDX

		call	CloseWindow
		mov	cx,@@c
		call	SetCursor

	@@SkipClose:
		call	popr
		ret

@@c		dw	2020h
@@SaveBX	dw	?
@@SaveDX	dw	?
		endp

MsgTitle	dw	MessageTitle
MessageTitle	db	9, Blue*16+LightCyan, ' Message '
ErrorTitle	db	7, Red*16+Yellow, ' Error '
MsgBoxAttr	db	atMessageBox
MessageAttr	db	atMessage
ExecMsgType	db	0

endif

ifdef _StrLen
StrLen		proc
	; Expects: si=@string
	; Returns: cx=Length
		push	ax si di es
		push	cs
		pop	es
		mov	di,si
		mov	cx,-1
		mov	al,0
		repne	scasb
		not	cx
		dec	cx
		pop	es di si ax
		ret
endp
endif

ifdef _UpCase
UpCase		proc
	; Expects: al=char
	; Returns: al=upper case
		cmp	al,'a'
		jb	@@Quit
		cmp	al,'z'
		ja	@@Quit
		sub	al,20h
@@Quit:
		ret
		endp
endif

ifdef _Window
.errndef	WindowBufSize
use _GetAddr

DoubleBorderStruc db	'…Õª∫∫»Õº'
SingleBorderStruc db	'⁄ƒø≥≥¿ƒŸ'

ifndef __ACT__
ThickBorderStruc  db	'€ﬂ€€€€‹€'
EmptyBorderStruc  db	'        '
endif

DoubleBorder	equ	offset DoubleBorderStruc
SingleBorder	equ	offset SingleBorderStruc

ifndef __ACT__
ThickBorder	equ	offset ThickBorderStruc
EmptyBorder	equ	offset EmptyBorderStruc
endif

;------------------------------------------------------------------------------
OpenWindow	proc
;	Expects: bl=x1, bh=y1, dl=x2, dh=y2, ah=attr, si=@title
;	Returns: None
		call	pushr
		mov	cx,DoubleBorder
		jmp	OpenWindow2
endp
OpenWindowBord	proc
;	Expects: bl=x1, bh=y1, dl=x2, dh=y2, ah=attr, si=@title, cx=@Border
;		 ACT: al = SaveWindow (1 = save)
;	Returns: None

		call	pushr
OpenWindow2:
		mov	bp,cx
		push	ax

ifdef __ACT__
		or	al,al
		jz	@@SkipSaveWindow
endif

		push	dx
		add	dx,0102h
		call	SaveWindow
		pop	dx
	@@SkipSaveWindow:
		mov	ax,bx
		push	bp
		call	GetAddr
		pop	bp
;		mov	di,ax
		mov	@@bp,ax
		mov	es,B800h
		pop	ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0
		dec	cx
		mov	al,ds:[bp]		;'…'
		stosw

		push	cx
		sub	cl,ds:[si]
		shr	cx,1
		mov	bl,cl
		mov	al,ds:[bp+1]	;'Õ'
		rep	stosw

		push	ax
		lodsw
		add	bl,al
		mov	cl,al
		jcxz	@@Skip2
@@Next2:
		lodsb
		stosw
		loop	@@Next2
@@Skip2:
		pop	ax
		pop	cx
		push	cx
		sub	cl,bl
		mov	al,ds:[bp+1]	;'Õ'
		rep	stosw
		pop	cx
		mov	al,ds:[bp+2]	;'ª'
		stosw
		inc	bh
@@Next:
		cmp	bh,dh
		je	@@EndLoop
		inc	bh
		add	@@bp,160
		mov	di,@@bp
		mov	al,ds:[bp+3]	;'∫'
		stosw
		push	cx
		mov	al,' '
		rep	stosw
		pop	cx
		mov	al,ds:[bp+4]	;'∫'
		stosw
		mov	al,7
		inc	di
		stosb
		inc	di
		stosb

		jmp	@@Next
@@EndLoop:
		add	@@bp,160
		mov	di,@@bp
		mov	al,ds:[bp+5]	;'»'
		stosw
		mov	al,ds:[bp+6]	;'Õ'
		push	cx
		rep	stosw
		pop	cx
		mov	al,ds:[bp+7]	;'º'
		stosw
		mov	al,7
		inc	di
		stosb
		inc	di
		stosb

		add	@@bp,165
		mov	di,@@bp
		mov	al,7
		inc	cx
		inc	cx
@@Next1:
		stosb
		inc	di
		loop	@@Next1

		call	popr
		ret
@@bp		dw	0
EmpTitle	db	0, 0, 0
endp

;------------------------------------------------------------------------------
CloseWindow	proc
;		Expects: bl=x1, bh=y1, dl=x2, dh=y2
;		Returns: None
		add	dx,0102h
		jmp	RestoreWindow
		endp

;------------------------------------------------------------------------------
SaveWindow	proc
		call	pushr
		mov	ax,bx
		call	GetAddr
;		mov	bp,ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0		; cx=width
		inc	cx
		push	cs
		pop	es
		mov	di,BufPos
		mov	ds,B800h
;		inc	dh
@@Next:
		mov	si,bp
		push	cx
		rep	movsw
		pop	cx
		add	bp,160
		cmp	bh,dh
		jae	@@1
		inc	bh
		jmp	@@Next
@@1:
		mov	cs:BufPos,di
		mov	cs:BufPosMark,di
		call	popr
		ret
endp

;------------------------------------------------------------------------------
RestoreWindow	proc
		call	pushr
;		inc	dh
		mov	ax,dx
		call	GetAddr
;		mov	bp,ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0		; cx=width
		inc	cx
		mov	si,BufPos
		mov	es,B800h
		std
		lodsw
@@Next:
		mov	di,bp
		push	cx
		rep	movsw
		pop	cx
		sub	bp,160
		cmp	bh,dh
		jae	@@1
		inc	bh
		jmp	@@Next
@@1:
		cld
		lodsw
		mov	BufPos,si
		call	popr
		ret
endp

endif

ifdef _WriteString

;------------------------------------------------------------------------------
WriteString	proc
		push	di bp es
		call	GetAddr
		mov	es,B800h
;		mov	di,ax
		mov	ah,bh
@@Next:
		lodsb
		cmp	al,0
		je	@@Quit
		stosw
		jmp	@@Next
@@Quit:
		pop	es bp di
		ret
endp

endif

ifdef _FillHex

FillWord	proc
		xchg	al,ah
		call	FillByte
		xchg	al,ah
		jmp	FillByte
		endp

FillByte	proc
		push	ax cx
		mov	ch,al
		shr	al,1
		shr	al,1
		shr	al,1
		shr	al,1
		add	al,'0'
		cmp	al,'9'
		jbe	@@1
		add	al,7
@@1:
		mov	ah,bh
		stosw
		mov	al,ch
		and	al,0Fh
		add	al,'0'
		cmp	al,'9'
		jbe	@@2
		add	al,7
@@2:
		stosw
		pop	cx ax
		ret
endp

endif

ifdef _GetAddr
;------------------------------------------------------------------------------
GetAddr		proc
		push	bx
		mov	bx,ax
		mov	al,160
		mul	bh
		mov	bh,0
		add	ax,bx
		add	ax,bx
		pop	bx
		mov	di,ax
		mov	bp,ax
		cld
		ret
endp
endif

ifdef _GoToXY
GoToXY		proc
;	Expects: dl=x, dh=y
		push	ax bx
		mov	ah,2
		mov	bh,0
		int	10h
		pop	bx ax
		ret
endp
endif

ifdef _GetCursor
GetCursor	proc
;	Returns: dx=coord, cx=shape
		push	ax bx
		mov	ah,3
		mov	bh,0
		int	10h
		pop	bx ax
		ret
endp
endif

ifdef _SetCursor
SetCursor	proc
;	Expects: cx=shape
		push	ax
		mov	ah,1
		int	10h
		pop	ax
		ret
endp
endif

ifdef _Beep
Beep		proc
		push	ax cx
		in	al,61h
		or	al,3
		out	61h,al
		mov	al,10110110b
		out	43h,al
		mov	ax,700
		out	42h,al
		mov	al,ah
		out	42h,al

		mov	cx,0A000h
		loop	$

		in	al,61h
		and	al,11111100b
		out	61h,al
		pop	cx ax
		ret
endp
endif

ifdef _WriteMessage
WriteMessage	proc
		mov	ah,9
		int	21h
		ret
		endp
endif

B800h		dw	0B800h

ifdef _Window
BufPos		dw	WindowBuffer
endif

ifdef __ACT__
BufPosMark	dw	WindowBuffer
endif

;WindowBuffer	label	byte

