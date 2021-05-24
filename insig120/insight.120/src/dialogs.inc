;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; dialogs.inc
; Procedures for creating dialog windows.
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
; init_dialog
;-----------------------------------------------------------------------------
; Expects: si=@struc
init_dialog:
		call	pushr
		cld
		lodsw
		mov	[dlg_win_a],ax
		mov	bx,ax
		lodsw
		mov	[dlg_win_b],ax
		mov	dx,ax
		lodsw
		push	si
		mov	si,ax
		mov	byte [si+1],atDialogWindow

		mov	ax,atDialogWindow << 8

		call	open_window
		pop	si
		mov	es,[video_seg]
		lodsw
		mov	cx,ax
		mov	[dlg_items],ax
		jcxz	@@quit

		mov	[dlg_active_item],si
		lodsb

		mov	di,dlg_items_data
@@next:
		lodsw
		add	si,3
		mov	[di],si
		inc	di
		inc	di
		call	ax
		loop	@@next
@@quit:
;		mov	DlgRunPtr,si
		call	popr
		ret
;=============================================================================
; exec_dialog
;-----------------------------------------------------------------------------
exec_dialog:
		mov	si,[dlg_active_item]
		mov	byte [si],0

exec_dialog_:
		call	pushr
		call	get_cursor
		mov	[@@curshape],cx
		mov	[@@curpos],dx
		call	no_cursor
		mov	es,[video_seg]
;		mov	di,DlgRunPtr
		cld
		mov	cx,[dlg_items]
		jcxz	@@quit

		mov	bx,[dlg_active_item]
		mov	bl,[bx]
		mov	bh,0

@@next:
		push	cx
		call	no_cursor
		pop	cx
		mov	di,bx
		add	di,di
		mov	si,[dlg_items_data+di]
		call	word [si-3]
		or	ax,ax
		jz	@@quit
		cmp	ax,1
		jz	@@enter
@@find_next:
		cmp	ax,2
		je	@@inc
		or	bx,bx
		jz	@@end
		dec	bx
		jmp	@@check_static
@@end:
		mov	bx,cx
		dec	bx
		jmp	@@check_static
@@inc:
		inc	bx
		cmp	bx,cx
		jb	@@check_static
@@home:
		xor	bx,bx

@@check_static:
		mov	di,bx
		add	di,di
		mov	si,[dlg_items_data+di]
		cmp	word [si-3],run_static_text
		jne	@@next
		jmp	@@find_next

@@quit:
		xor	ax,ax		; CM_CANCEL
		mov	si,[dlg_active_item]
		mov	bl,[si]

@@exit:
		mov	si,[dlg_active_item]
		mov	[si],bl

@@restore_cursor:
		mov	[@@res],ax
		mov	cx,[@@curshape]
		call	set_cursor
		mov	dx,[@@curpos]
		call	gotoxy
		call	popr
		mov	ax,[@@res]
		or	ax,ax
		ret
@@enter:
		mov	ax,CM_DEFAULT
		cmp	word [si-3],run_push_button
		jne	@@exit
		mov	ax,[si+5]
		jmp	@@exit

@@res		dw	0
@@curshape	dw	2020h
@@curpos	dw	0
;=============================================================================
; 
;-----------------------------------------------------------------------------
;~ifndef __ACT__
;~DoneDialog	proc
;~		call	pushr
;~		mov	bx,DlgWinA
;~		mov	dx,DlgWinB
;~		call	CloseWindow
;~		call	popr
;~		ret
;~endp
;~endif


;~ifndef __ACT__
;~  MaxItems	equ	32
;~  DlgWinA	dw	0
;~  DlgWinB	dw	0
;~  ;DlgRunPtr	dw	0
;~  DlgItems	dw	0
;~  DlgItemsData	dw	MaxItems dup (0)
;~endif

;=============================================================================
; init_push_button
;-----------------------------------------------------------------------------
init_push_button:
		mov	bh,atPushButton
		mov	bl,atlPushButton
		test	byte [si-1],BF_DEFAULT
		jz	@@1
		mov	bh,atPushButtonDef
		mov	bl,atlPushButtonDef
@@1:
		mov	dl,0
		call	draw_push_button
		add	si,7
		ret
;=============================================================================
; draw_push_button
;-----------------------------------------------------------------------------
draw_push_button:
		call	pushr
		lodsw
		add	ax,[dlg_win_a]
		call	get_addr
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
		call	point_len
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
		call	draw_text
		call	popr
		ret
;=============================================================================
; draw_text
;-----------------------------------------------------------------------------
; ah=main attr, bl=letter attr
draw_text:
@@next:
		lodsb
		cmp	al,0
		je	@@done
		cmp	al,'~'
		jne	@@store
		xchg	ah,bl
		jmp	@@next
@@store:
		stosw
		jmp	@@next
@@done:
		ret
;=============================================================================
; run_push_button
;-----------------------------------------------------------------------------
run_push_button:
		push	bx
		push	cx
		mov	bh,atsPushButton
		mov	bl,atlsPushButton
		mov	dl,0
		call	draw_push_button
@@again:
		call	read_key
		call	check_move_key
		jc	@@again

		mov	bh,atPushButton
		mov	bl,atlPushButton
		cmp	ax,1
		jne	@@quit
		mov	dl,1
		call	draw_push_button
		xor	cx,cx
		loop	$
@@quit:
		test	byte [si-1],BF_DEFAULT
		jz	@@1
		mov	bh,atPushButtonDef
		mov	bl,atlPushButtonDef
@@1:
		mov	dl,0
		call	draw_push_button
		pop	cx
		pop	bx
		ret
;=============================================================================
; init_input_line
;-----------------------------------------------------------------------------
init_input_line:
		call	draw_input_line
		add	si,8
		ret
;=============================================================================
; draw_input_line
;-----------------------------------------------------------------------------
draw_input_line:
		call	pushr
		lodsw
		add	ax,[dlg_win_a]
		call	get_addr
;		mov	di,ax
		lodsw
		mov	dl,0
		call	draw_label
		mov	ah,atInputLine
		mov	al,' '
		stosw
		lodsw
		cbw
		mov	cx,ax
		lodsw
		mov	si,ax
		mov	ah,atInputLine
@@next:
		lodsb
		cmp	al,0
		je	@@done
		stosw
		loop	@@next
@@done:
		mov	al,' '
		inc	cx
	rep	stosw
		call	popr
		ret
;=============================================================================
; run_input_line
;-----------------------------------------------------------------------------
run_input_line:
		push	word [read_exit_keys]
		push	bx
		push	cx
		push	si
		push	bp
		mov	word [read_exit_keys],inp_line_exit_keys
		lodsw
		add	ax,[dlg_win_a]
		push	ax
		call	get_addr
;		mov	di,ax
		lodsw
		mov	dl,1
		call	draw_label
		lodsw
		mov	cx,ax
		lodsw
		mov	di,ax
		pop	ax
		sub	ax,0102h
		mov	bl,atInputLine
		mov	bh,atsInputLine
		call	read_line
		call	check_move_key
		pop	bp
		pop	si
		pop	cx
		pop	bx
		call	draw_input_line
		pop	word [read_exit_keys]
		ret

inp_line_exit_keys:
		dw	kbEnter, kbEsc, kbTab, kbShiftTab, 0
;=============================================================================
; draw_label
;-----------------------------------------------------------------------------
draw_label:
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
		call	draw_text
		call	popr
		ret
;=============================================================================
; init_list
;-----------------------------------------------------------------------------
init_list:
		mov	word [list_choice],-1
		mov	word [list_first],0
		mov	dl,0
		call	draw_list
		add	si,8
		ret
;=============================================================================
; draw_list
;-----------------------------------------------------------------------------
draw_list:
		call	pushr
		lodsw
		add	ax,[dlg_win_a]
		call	get_addr
;		mov	bp,ax
;		mov	di,ax
		mov	[list_start],ax

		lodsw
		call	draw_label

		lodsw
		mov	[list_height],ax
		lodsw
		mov	[link_string],ax
		mov	ax,[list_first]
		call	get_string_addr
		mov	si,ax
;		mov	si,offset StringBuffer
		mov	cx,[string_size]
		mov	dx,[string_count]
		mov	bx,[list_height]
		cmp	dx,bx
		jbe	@@ns
		mov	dx,bx
@@ns:
		sub	bx,dx
		push	bx
		mov	bx,[list_first]
		or	dx,dx
		jz	@@clear
@@next_line:
		mov	ah,atListNormal
		cmp	bx,[cur_str]
		jne	@@1
		mov	ah,atListSelected
@@1:
		push	cx
		mov	di,bp
		mov	al,' '
		stosw
@@next1:
		lodsb
		stosw
		loop	@@next1

		pop	cx
		inc	bx
		add	bp,160
		dec	dx
		jnz	@@next_line
@@clear:
		pop	bx
		or	bx,bx
		jz	@@all
@@next_clear:
		mov	ah,atListNormal
		mov	di,bp
		push	cx
		inc	cx
		mov	al,' '
	rep	stosw
		pop	cx
		add	bp,160
		dec	bx
		jnz	@@next_clear
@@all:
		call	popr
		ret

@@r		dw	0
;=============================================================================
; run_list
;-----------------------------------------------------------------------------
run_list:
		push	bx
		push	cx
		xor	bx,bx
		xor	cx,cx
		mov	dx,[string_count]
@@again:
		call	@@set_link
		mov	[cur_str],bx
		mov	[list_first],cx
		push	dx
		mov	dl,1
		call	draw_list
		pop	dx
		call	read_key
		or	dx,dx
		jz	@@cmk
		cmp	ax,kbDown
		jne	@@1
		inc	bx
		cmp	bx,dx
		jb	@@loc_correct
@@end:
		mov	bx,dx
		dec	bx
@@loc_correct:
		jmp	@@correct
@@1:
		cmp	ax,kbUp
		jne	@@2
		or	bx,bx
		jz	@@again
		dec	bx
;nop	;!!!
		jmp	@@correct
@@2:
		cmp	ax,kbHome
		jne	@@3
@@home:
		xor	bx,bx
		jmp	@@correct
@@3:
		cmp	ax,kbEnd
		je	@@end
		cmp	ax,kbPgUp
		jne	@@4
		or	cx,cx
		jz	@@home
		cmp	cx,[list_height]
		ja	@@big_up
		xor	cx,cx
@@pg_corr:
		mov	bx,cx
		add	bx,[list_height]
		dec	bx
		jmp	@@correct
@@big_up:
		sub	cx,[list_height]
		jmp	@@pg_corr
@@4:
		cmp	ax,kbPgDn
		jne	@@cmk
		cmp	dx,[list_height]
		jbe	@@end
		push	cx
		add	cx,[list_height]
		cmp	cx,dx
		jne	@@5
		pop	cx
		jmp	@@end
@@5:
		add	cx,[list_height]
		cmp	cx,dx
		pop	cx
		jbe	@@big_down
		mov	cx,dx
		sub	cx,[list_height]
		mov	bx,cx
		jmp	@@correct
@@big_down:
		add	cx,[list_height]
		mov	bx,cx
		jmp	@@correct
@@cmk:
		call	check_move_key
		jc	@@loc_again
		cmp	ax,1
		jne	@@quit
		mov	[list_choice],bx
		call	@@set_link
@@quit:
		mov	word [cur_str],-1
		mov	dl,0
		call	draw_list
		pop	cx
		pop	bx
		ret
@@correct:
		cmp	bx,cx
		jb	@@lower
		push	cx
		add	cx,[list_height]
		cmp	bx,cx
		pop	cx
		jb	@@loc_again
@@upper:
		mov	cx,bx
		sub	cx,[list_height]
		inc	cx
		jmp	@@again
@@lower:
		mov	cx,bx
@@loc_again:
		jmp	@@again
@@set_link:
		cmp	word [link_string],0
		je	@@skip_link
		or	dx,dx
		jz	@@skip_link
		push	ax
		push	cx
		push	si
		push	es
		push	cs
		pop	es
		mov	di,[link_string]
		push	di
		mov	di,[di+6]
		mov	ax,bx
		call	get_string_addr
		mov	si,ax
		mov	cx,[string_size]
		push	cx
	rep	movsb
		pop	cx
		call	trim_spaces
		pop	si
		pop	es
		call	draw_input_line
		pop	si
		pop	cx
		pop	ax
@@skip_link:
		ret
;=============================================================================
; trim_spaces
;-----------------------------------------------------------------------------
trim_spaces:
		push	di
		std
		dec	di
		mov	al,' '
	repe	scasb
		je	@@emp
		inc	di
@@emp:		inc	di
		mov	al,0
		cld
		stosb
		pop	di
		ret
;=============================================================================
; init_string_list
;-----------------------------------------------------------------------------
; Expects: ax=StringSize
init_string_list:
		mov	[string_size],ax
		mov	word [string_count],0
		mov	word [string_ptr],string_buffer
		ret
;=============================================================================
; add_string
;-----------------------------------------------------------------------------
; Expects: si=@string
; Returns: CF=1 if string buffer is full
add_string:
		mov	ax,string_buffer+STRING_BUF_SIZE
		sub	ax,[string_size]
		cmp	[string_ptr],ax
		jb	@@1
		stc
		ret
@@1:
		call	pushr
		push	cs
		pop	es
		mov	di,[string_ptr]
		cld
		mov	cx,[string_size]
@@next:
		lodsb
		cmp	al,0
		je	@@done
		stosb
		loop	@@next
@@end:
		mov	[string_ptr],di
		inc	word [string_count]
		call	popr
		clc
		ret
@@done:
		mov	al,' '
	rep	stosb
		jmp	@@end
;=============================================================================
; init_static_text
;-----------------------------------------------------------------------------
init_static_text:
		lodsw
		push	si
		push	di
		push	bp
		xchg	ax,si
@@next:
		lodsw
		or	ax,ax
		jz	@@exit
		add	ax,[dlg_win_a]
		call	get_addr
;		mov	di,ax
		lodsw
		mov	dl,0
		call	draw_label
		jmp	@@next
@@exit:
		pop	bp
		pop	di
		pop	si
		ret
;=============================================================================
; run_static_text
;-----------------------------------------------------------------------------
run_static_text:
		mov	ax,2
		ret
;=============================================================================
; init_check_boxes
;-----------------------------------------------------------------------------
init_check_boxes:
		mov	dl,0
		mov	byte [cb_cur],-1
		call	draw_check_boxes
		mov	si,[cb_si]
		ret
;=============================================================================
; draw_check_boxes
;-----------------------------------------------------------------------------
draw_check_boxes:
		call	pushr
		lodsw
		add	ax,[dlg_win_a]
		call	get_addr
;		mov	di,ax
;		mov	bp,ax
		lodsw
		call	draw_label
		lodsw
		mov	cx,ax
		sub	al,[cb_cur]
		mov	[cb_cur],al
		lodsw
		mov	[cb_max],ax
		lodsw
		xchg	ax,dx	; on/off

@@next:
		lodsw
		push	si
		xchg	ax,si

		mov	ah,atCheckBoxes
		cmp	[cb_cur],cl
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
		add	cx,[cb_max]
		add	cx,[cb_max]

		call	draw_text

		sub	cx,di
		shr	cx,1
		mov	al,' '
	rep	stosw
		pop	cx
		add	bp,160
		mov	di,bp

		pop	si

		loop	@@next

		mov	[cb_si],si
		call	popr
		ret
;=============================================================================
cb_max:
cb_si		dw	0
cb_cur		db	-1
;=============================================================================
; run_check_boxes
;-----------------------------------------------------------------------------
run_check_boxes:
		push	bx
		push	cx
		call	small_cursor
		xor	cx,cx
@@again:
		mov	dx,[si]
		add	dx,[dlg_win_a]
		add	dh,cl
		inc	dx
		inc	dx
		call	gotoxy
		mov	dl,1
		mov	[cb_cur],cl
		call	draw_check_boxes

		call	read_key
		cmp	ax,kbDown
		jne	@@d
		inc	cx
		cmp	cx,[si+4]
		jb	@@again
		xor	cx,cx
		jmp	@@again
@@d:
		cmp	ax,kbUp
		jne	@@u
		dec	cx
		jns	@@again
		mov	cx,[si+4]
		dec	cx
		jmp	@@again
@@u:
		cmp	ax,kbSpace
		jne	@@s
		mov	ax,1
		shl	ax,cl
		xor	[si+8],ax
		jmp	@@again
@@s:
		call	check_move_key
		jc	@@again
		mov	dl,0
		mov	byte [cb_cur],-1
		call	draw_check_boxes
		pop	cx
		pop	bx
		ret
;=============================================================================

;~ifndef __ACT__
;~;---------------------------- Radio Buttons -----------------------------------
;~;------------------------------------------------------------------------------
;~InitRadioButtons proc
;~		mov	dl,0
;~		call	DrawRadioButtons
;~		mov	si,CBSI
;~		ret
;~endp
;~
;~DrawRadioButtons proc
;~		call	pushr
;~		lodsw
;~		add	ax,DlgWinA
;~		call	get_addr
;~;		mov	di,ax
;~;		mov	bp,ax
;~		lodsw
;~		call	DrawLabel
;~		lodsw
;~		mov	cx,ax
;~;		sub	al,CBcur
;~;		mov	CBcur,al
;~		lodsw
;~		mov	CBmax,ax
;~		lodsw
;~		mov	bl,dl
;~		mov	dx,cx
;~		sub	dx,ax	; pos
;~		mov	dh,bl
;~@@Next:
;~		mov	ah,atRadioButtons
;~		mov	bh,' '
;~		cmp	dl,cl
;~		jne	@@2
;~		mov	bh,7
;~		or	dh,dh
;~		jz	@@2
;~		mov	ah,atsRadioButtons
;~@@2:
;~		mov	al,' '
;~		stosw
;~		mov	al,'('
;~		stosw
;~		mov	al,bh
;~		stosw
;~		mov	al,')'
;~		stosw
;~		mov	al,' '
;~		stosw
;~		mov	bl,atlRadioButtons
;~		push	cx
;~		mov	cx,di
;~		add	cx,CBmax
;~		add	cx,CBmax
;~		call	DrawText
;~		sub	cx,di
;~		shr	cx,1
;~		mov	al,' '
;~		rep	stosw
;~		pop	cx
;~		add	bp,160
;~		mov	di,bp
;~		loop	@@Next
;~
;~		mov	CBSI,si
;~		call	popr
;~		ret
;~endp
;~
;~RunRadioButtons	proc
;~		push	bx cx
;~		mov	cx,0607h
;~		call	set_cursor
;~		mov	cx,[si+8]
;~@@Again:
;~		mov	dx,[si]
;~		add	dx,DlgWinA
;~		add	dh,cl
;~		inc	dx
;~		inc	dx
;~		call	gotoxy
;~		mov	dl,1
;~		mov	[si+8],cx
;~		call	DrawRadioButtons
;~
;~		call	read_key
;~		cmp	ax,kbDown
;~		jne	@@D
;~		inc	cx
;~		cmp	cx,[si+4]
;~		jb	@@Again
;~		xor	cx,cx
;~		jmp	@@Again
;~@@D:
;~		cmp	ax,kbUp
;~		jne	@@U
;~		dec	cx
;~		jns	@@Again
;~		mov	cx,[si+4]
;~		dec	cx
;~		jmp	@@Again
;~@@U:
;~		call	CheckMoveKey
;~		jc	@@Again
;~		mov	dl,0
;~		mov	CBcur,-1
;~		call	DrawRadioButtons
;~		pop	cx bx
;~		ret
;~endp
;~
;~endif

;=============================================================================
; get_string_addr
;-----------------------------------------------------------------------------
; Expects: ax=number
; Returns: ax=@str
get_string_addr:
		push	dx
		mul	word [string_size]
		add	ax,string_buffer
		pop	dx
		ret
;=============================================================================
; point_len
;-----------------------------------------------------------------------------
point_len:
		xor	cx,cx
		push	si
@@next:
		lodsb
		cmp	al,0
		je	@@done
		cmp	al,'~'
		je	@@next
		inc	cx
		jmp	@@next
@@done:
		pop	si
		ret
;=============================================================================
; check_move_key
;-----------------------------------------------------------------------------
check_move_key:
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
@@0:
		xor	ax,ax
		ret
@@1:
		mov	ax,1
		ret
@@2:
		mov	ax,2
		ret
@@3:
		mov	ax,3
		ret
;=============================================================================

string_count	dw	0
string_ptr	dw	0
string_size	dw	64
list_start	dw	0
list_height	dw	0
cur_str		dw	-1
link_string	dw	0
list_choice	dw	-1
list_first	dw	0

;=============================================================================
; E0F
;=============================================================================


