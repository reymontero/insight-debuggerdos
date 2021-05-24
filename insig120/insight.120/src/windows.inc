;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; windows.inc
; Procedures for creating windows and menus.
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
; exec_menu
;-----------------------------------------------------------------------------
; Menu structure:	dw	HorCnt, @HorStr, @HorSub
; Expects: si=@Menu
; Returns: ax=ExitCode
;
exec_menu:
		xor	bx,bx
		mov	dx,79
		call	save_window

		cld
		lodsw
		mov	cx,ax
		mov	bx,[h_pos]
@@next:
		call	update_hmenu
@@read:
		call	read_key
		cmp	ax,kbEsc
		je	@@esc
		cmp	ax,kbAltAlone
		je	@@esc
		cmp	ax,kbEnter
		je	@@enter
		cmp	ax,kbDown
		je	@@enter
		cmp	ax,kbRight
		je	@@right
		cmp	ax,kbLeft
		je	@@left
		push	si
		mov	si,[si]
		call	find_menu_letter
		pop	si
		jc	@@read
		mov	bx,ax
		call	update_hmenu
		jmp	@@enter
@@right:
		inc	bx
		cmp	bx,cx
		jb	@@next
		xor	bx,bx
		jmp	@@next
@@left:
		dec	bx
		jns	@@next
		mov	bx,cx
		dec	bx
		jmp	@@next
@@enter:
		push	si
		mov	si,[si+2]
		call	exec_sub_menu
		pop	si
		cmp	ax,2
		je	@@left_exp
		cmp	ax,3
		je	@@right_exp
		cmp	ax,100h
		jae	@@quit
@@esc:
		xor	ax,ax
@@quit:
		mov	word [point_options],0
		mov	[h_pos],bx
		xor	bx,bx
		mov	dx,79
		call	restore_window
		or	ax,ax
		jz	@@ret
		call	ax
@@ret:
		ret
@@left_exp:
		dec	bx
		jns	@@exp
		mov	bx,cx
		dec	bx
		jmp	@@exp
@@right_exp:
		inc	bx
		cmp	bx,cx
		jb	@@exp
		xor	bx,bx
@@exp:
		call	update_hmenu
		jmp	@@enter
;=============================================================================
; exec_sub_menu
;-----------------------------------------------------------------------------
; bx = menu item # (zero based), si = offset menu actions table
;
exec_sub_menu:
		push	bx
		push	cx
		push	dx
		push	si
		push	word [sub_struc]
		push	word [sub_coord]
		shl	bx,1
		shl	bx,1
		mov	ax,[bx+si]
		test	al,DISABLED
		jz	@@enabled
		mov	ax,1
		jmp	@@quit_2
@@enabled:
		and	ax,~DISABLED
		or	ax,ax
		jnz	@@sub_menu
		mov	ax,[bx+si+2]
;		call	word ptr [bx+si+2]
;		xor	ax,ax
		jmp	@@quit_2
@@sub_menu:
		mov	si,[bx+si+2]
		lodsw
		mov	[sub_coord],ax
		mov	bx,ax
		lodsw
		mov	dx,ax
		mov	ax,(MenuNormalAttr << 8) | 1	;!!!
		push	bx
		push	dx
		push	si
		mov	si,emp_title
		mov	cx,SINGLE_BORDER
		call	open_window_bord
		pop	si

		mov	[sub_struc],si
@@next_menu:
		lodsw
		mov	cx,ax
		lodsw
		mov	dx,ax
		lodsw
		mov	bp,ax
		mov	[point_options],ax
		lodsw
		mov	si,ax
		mov	ax,[sub_coord]
		add	ax,0101h
		mov	bl,MenuNormalAttr	;!!!
		mov	bh,MenuSelAttr		;!!!
		call	vert_menu
		mov	si,[sub_struc]
		mov	[si+2],ah
		cmp	al,-3			;???
		jae	@@quit_a		;???

		push	si
		mov	bl,al
		mov	bh,0
		mov	si,bp
		call	exec_sub_menu
		pop	si
		cmp	ax,1
		je	@@next_menu

@@quit:
		pop	dx
		pop	bx
		call	close_window
@@quit_2:
		pop	word [sub_coord]
		pop	word [sub_struc]
		pop	si
		pop	dx
		pop	cx
		pop	bx
		ret
@@quit_a:
		neg	al
		mov	ah,0
		jmp	@@quit
;=============================================================================
; update_hmenu
;-----------------------------------------------------------------------------
update_hmenu:
		call	pushr
		mov	si,[si]
		mov	es,[video_seg]
		xor	di,di
		xor	bp,bp
		mov	ah,MenuNormalAttr	;!!!
		mov	al,' '
		stosw
@@next_point:
		mov	ah,MenuNormalAttr	;!!!
		mov	dh,MenuNormalLetter	;!!!
		cmp	bx,bp
		jne	@@1
		mov	ah,MenuSelAttr		;!!!
		mov	dh,MenuSelLetter	;!!!
@@1:
		mov	al,' '
		stosw
@@next:
		lodsb
		cmp	al,0
		je	@@end
		cmp	al,'~'
		je	@@sw
		stosw
		jmp	@@next
@@sw:
		xchg	ah,dh
		jmp	@@next
@@end:
		mov	al,' '
		stosw
		inc	bp
		cmp	bp,cx
		jb	@@next_point

		mov	cx,160
		sub	cx,di
		shr	cx,1
		mov	al,' '
		mov	ah,MenuNormalAttr	;!!!
	rep	stosw

		call	popr
		ret
;=============================================================================
; local_menu
;-----------------------------------------------------------------------------
; Expects: si=@struc
;
local_menu:
		call	pushr

		call	get_cursor
		push	cx
		call	no_cursor
		xor	bx,bx
		mov	[fake_table+2],si
		mov	si,fake_table
		call	exec_sub_menu
		mov	[fake_table+2],ax
		pop	cx
		call	set_cursor

		call	popr
		mov	ax,[fake_table+2]
		cmp	ax,100h
		ret

fake_table	dw	SUB_MENU, 0

;=============================================================================

h_pos		dw	0
sub_struc	dw	0
sub_coord	dw	0

;------------------------------------------------------------------------------
;~ifndef __ACT__
;~ReadString	proc
;~	; Expects: al=x, ah=y, di=@buffer, cl=width, ch=maxlen, si=@title
;~		push	bx dx
;~		mov	bx,ax
;~		mov	dx,ax
;~		add	dl,cl
;~		add	dl,5
;~		add	dh,2
;~		push	bx dx
;~		push	ax
;~		mov	ah,atReadWindow
;~		call	OpenWindow
;~		pop	ax
;~		mov	bl,atReadString
;~		mov	bh,atsReadString
;~		call	ReadLine
;~		pop	dx bx
;~		call	CloseWindow
;~		pop	dx bx
;~		ret
;~endp
;~endif

;=============================================================================
; read_line
;-----------------------------------------------------------------------------
read_line:
		call	pushr
		push	cs
		pop	es
		mov	byte [string_changed],1
		mov	[read_string_attr],bx		;word!
		add	ax,0102h
		mov	[str_start],ax
		push	di
		call	get_addr
		pop	di
;		mov	bp,ax
		mov	[str_length],cl			;byte!
		mov	[max_str_length],ch		;byte!
		mov	[str_buffer],di
		mov	si,di
		add	si,[max_str_length]
		mov	byte [si],0
		push	cx
		call	get_cursor
		mov	[read_line_cur],cx
		mov	[read_line_cur_c],dx
		call	small_cursor
		pop	cx
;		mov	ch,0

		mov	si,di
		call	strlen

		xor	bx,bx
		xor	dx,dx
		jmp	@@next_1
@@next:
		mov	byte [string_changed],0
@@next_1:
		call	update_string
@@read:
		call	read_key
		call	check_exit_read
		jc	@@1
		jmp	@@ignore
@@1:
		cmp	ax,kbRight
		je	@@right
		cmp	ax,kbLeft
		je	@@left
		cmp	ax,kbHome
		jne	@@5
		jmp	@@home
@@5:
		cmp	ax,kbEnd
		jne	@@4
		jmp	@@end
@@4:
		cmp	ax,kbDel
		je	@@delete
		cmp	ax,kbBackSpace
		je	@@backspace
		cmp	bl,[max_str_length]		;byte!
		jae	@@next
		call	[test_char]
		jc	@@read
		cmp	byte [string_changed],1
		je	@@clear
@@cleared:
		push	cx
		mov	cx,[max_str_length]
		mov	di,[str_buffer]
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
		mov	si,[str_buffer]
;		add	si,bx
		mov	[si+bx],al
		pop	cx
		cmp	cx,[max_str_length]
		jae	@@right
		inc	cx
@@right:
		cmp	bl,cl
		jne	@@2
@@loc_read:
		jmp	@@read
@@2:
		inc	bx
		mov	ax,bx
		sub	ax,dx
		cmp	ax,[str_length]
		jbe	@@loc_next
		inc	dx
		jmp	@@next
@@clear:
		mov	si,[str_buffer]
		mov	byte [si],0
		xor	cx,cx
		xor	bx,bx
		jmp	@@cleared
@@left:
		or	bl,bl
		je	@@loc_next
		dec	bx
		cmp	bx,dx
		jae	@@loc_next
		mov	dx,bx
@@loc_next:
		jmp	@@next
@@backspace:
		or	bx,bx
		je	@@loc_read
		dec	bx
@@delete:
		or	cx,cx
		je	@@loc_read
		cmp	bx,cx
		jae	@@loc_read
		mov	di,[str_buffer]
		add	di,bx
		lea	si,[di+1]
		push	cx
		mov	cx,[max_str_length]
		sub	cx,bx
		cld
	rep	movsb
		mov	al,0
		stosb
		pop	cx
		dec	cx
		or	dx,dx
		jz	@@loc_next
		mov	ax,cx
		sub	ax,dx
		cmp	ax,[str_length]
		jae	@@loc_next
		dec	dx
		jmp	@@next
@@home:
		xor	bx,bx
		xor	dx,dx
		jmp	@@next
@@end:
		mov	bx,cx
		cmp	bx,[str_length]
		jbe	@@loc_next
		mov	dx,bx
		sub	dx,[str_length]
		jmp	@@next

@@ignore:
;		mov	@@Result,0
;		jmp	@@Quit
@@enter:
		mov	[read_line_result],ax
@@quit:
		mov	cx,[read_line_cur]
		call	set_cursor
		mov	dx,[read_line_cur_c]
		call	gotoxy
		call	popr
		mov	ax,[read_line_result]
		ret
;=============================================================================

read_line_result dw	0
string_changed	db	1
read_line_cur	dw	0607h
read_line_cur_c	dw	0
str_length	dw	0
max_str_length	dw	0
str_buffer	dw	0
str_start	dw	0

read_string_attr	db	1*16+15
read_string_sel_attr	db	2*16+15
;=============================================================================
; update_string
;-----------------------------------------------------------------------------
update_string:
		call	pushr
		mov	ax,dx
		mov	dx,[str_start]
		add	dl,bl
		sub	dl,al
		inc	dl
		call	gotoxy
		mov	dx,ax
		mov	di,bp
		mov	es,[video_seg]
		mov	si,[str_buffer]
		add	si,ax
		mov	ah,[read_string_attr]
		xor	dx,dx
		mov	al,' '
		stosw
		cmp	byte [string_changed],0
		je	@@not_sel_1
		mov	ah,[read_string_sel_attr]
@@not_sel_1:
		jcxz	@@skip
		cmp	cx,[str_length]
		jbe	@@next
		mov	cx,[str_length]
@@next:
		lodsb
		stosw
		inc	dx
		loop	@@next
@@skip:
		mov	ah,[read_string_attr]
		sub	dx,[str_length]
		ja	@@quit
		mov	cx,dx
		neg	cx
		inc	cx
		mov	al,' '
	rep	stosw
@@quit:
		call	popr
		ret

test_char	dw	default_test_char
;=============================================================================
; default_test_char
;-----------------------------------------------------------------------------
default_test_char:
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
;=============================================================================
; check_exit_read
;-----------------------------------------------------------------------------
check_exit_read:
		push	ax
		push	bx
		push	si
		mov	bx,ax
		mov	si,[read_exit_keys]
@@next:
		lodsw
		cmp	ax,bx
		je	@@quit
		or	ax,ax
		jnz	@@next
@@end:
		stc
@@quit:
		pop	si
		pop	bx
		pop	ax
		ret

;=============================================================================

read_exit_keys		dw	default_exit_keys
default_exit_keys	dw	kbEsc, kbEnter, 0

;=============================================================================
; vert_menu
;-----------------------------------------------------------------------------
;Expects: al=x1, ah=y1, cx=count, bl=normal attr, bh=sel attr,
;	 dl=selected point, dh = width, si=@items
;	Items: 'First', 0, 'Second', 0, ...
;Returns: al=point (-1 if Esc pressed, -2 if Left, -3 if Right)
;
vert_menu:
		call	pushr
		push	word [keys]
		push	word [test_key]
		call	get_addr
		mov	es,[video_seg]
;		mov	bp,ax
		call	write_items
		mov	word [keys],menu_keys
		mov	word [test_key],vm_test_key
		call	key_loop
		cmp	ax,kbEsc
		je	@@cancel
		cmp	ax,kbLeft
		je	@@left
		cmp	ax,kbRight
		je	@@right
		mov	al,dl
		jmp	@@quit
@@cancel:
		mov	al,-1
@@quit:
		mov	[vert_menu_result],al
		mov	[vert_menu_result+1],dl
		pop	word [test_key]
		pop	word [keys]
		call	popr
		mov	ax,[vert_menu_result]
		ret
@@left:
		mov	al,-2
		jmp	@@quit
@@right:
		mov	al,-3
		jmp	@@quit

vert_menu_result dw	00ffh

..@vm_down:
		inc	dl
		cmp	dl,cl
		jb	@@ret
..@vm_home:
		mov	dl,0
@@ret:
		jmp	write_items

..@vm_up:
		dec	dl
		cmp	dl,-1
		jne	@@ret
..@vm_end:
		mov	dl,cl
		dec	dl
		jmp	@@ret
;=============================================================================
; vm_test_key
;-----------------------------------------------------------------------------
vm_test_key:
		mov	al,[cur_test_code]		;byte!
		call	find_menu_letter
		jc	@@quit
		mov	dl,al
		call	write_items
		mov	ax,kbEnter
@@quit:
		ret
;=============================================================================

menu_keys	dw	kbEsc,   0
		dw	kbEnter, 0
		dw	kbLeft,  0
		dw	kbRight, 0
		dw	kbUp,   ..@vm_up
		dw	kbDown, ..@vm_down
		dw	kbHome, ..@vm_home
		dw	kbEnd,  ..@vm_end
		dw	kbPgUp, ..@vm_home
		dw	kbPgDn, ..@vm_end
		dw	0,	0

menu_items	dw	0
point_options	dw	0
;=============================================================================
; write_items
;-----------------------------------------------------------------------------
write_items:
		push	ax
		push	cx
		push	si
		push	bp
		mov	al,0
@@next:
		push	cx
@@next_str:
		mov	di,bp
		mov	ah,bl
		cmp	byte [si],0
		je	@@separator
		cmp	word [point_options],0
		je	@@no_opt
		push	bx
		mov	bl,al
		mov	bh,0
		add	bx,bx
		add	bx,bx
		add	bx,[point_options]
		test	byte [bx],DISABLED
		pop	bx
		jz	@@no_opt
		mov	ch,atDisabled
		mov	ah,atDisabled
		cmp	al,dl
		jne	@@write
		mov	ch,atSelDisabled
		mov	ah,atSelDisabled
		jmp	@@write
@@no_opt:
		mov	ch,[normal_letter]
		cmp	al,dl
		jne	@@write
		mov	ah,bh
		mov	ch,[accent_letter]
@@write:
		call	write_point
		add	bp,160
		inc	ax
		pop	cx
		loop	@@next

		pop	bp
		pop	si
		pop	cx
		pop	ax
		ret
@@separator:
		push	ax
		inc	si
		sub	di,2
		mov	al,'Ã'
		stosw
		mov	al,'Ä'
		mov	cl,dh
		mov	ch,0
		inc	cx
	rep	stosw
		mov	al,'´'
		stosw
		add	bp,160
		pop	ax
		jmp	@@next_str
;=============================================================================
; write_point
;-----------------------------------------------------------------------------
; Expects: si=@str, ah=attr, es:di=@scr, dh=width, dh=0 if don't care
;
write_point:

		push	ax
		push	bx
		push	cx
		cld
		mov	bl,ch
		mov	cl,dh
		mov	ch,0
		mov	al,' '
		stosw
@@next:
		lodsb
		cmp	al,0
		je	@@adjust
		cmp	al,'~'
		je	@@sw
		stosw
		dec	cx
		jmp	@@next
@@sw:
		xchg	ah,bl
		jmp	@@next
@@adjust:
		cmp	cx,0
		jle	@@quit
		mov	al,' '
	rep	stosw
@@quit:
		pop	cx
		pop	bx
		pop	ax
		ret
;=============================================================================

normal_letter	db	7*16+RED	;!!!
accent_letter	db	GREEN*16+RED	;!!!
;=============================================================================
; find_menu_letter
;------------------------------------------------------------------------------
find_menu_letter:
		push	bx
		push	si
		call	upcase
		mov	ah,al
		xor	bx,bx
@@next:
		lodsb
		cmp	al,0
		je	@@end_point
		cmp	al,'~'
		jne	@@next
		lodsb
		inc	si
		call	upcase
		cmp	al,ah
		jne	@@next
		mov	ax,bx
		jmp	@@quit
@@end_point:
		cmp	byte [si],0
		jne	@@1
		inc	si
@@1:
		inc	bx
		cmp	bx,cx
		jb	@@next
		stc
@@quit:
		pop	si
		pop	bx
		ret
;=============================================================================
; error_message
;-----------------------------------------------------------------------------
error_message:
		mov	word [msg_title],error_title
		mov	byte [msg_box_attr],atErrorBox
		mov	byte [message_attr],atErrorMsg
		jmp	exec_message
;=============================================================================
; message_box
;-----------------------------------------------------------------------------
message_box:
		mov	word [msg_title],message_title
		mov	byte [msg_box_attr],atMessageBox
		mov	byte [message_attr],atMessage
;=============================================================================
; exec_message
;------------------------------------------------------------------------------
; Expects: si=@Message
;
exec_message:
		call	beep
		call	pushr

		cmp	byte [exec_msg_type],1
		ja	@@skip_open

		call	get_cursor
		mov	[exec_message_c],cx
		call	no_cursor
		mov	bh,10
		mov	dh,12
		call	strlen
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

		mov	ah,[msg_box_attr]

		mov	bp,si
		mov	si,[msg_title]

		mov	[exec_message_bx],bx
		mov	[exec_message_dx],dx

		push	ax
		mov	al,1

		call	open_window
		pop	ax

		mov	ah,11
		mov	bh,[message_attr]
		mov	si,bp
		call	write_string

@@skip_open:
		cmp	byte [exec_msg_type],1
		je	@@skip_close

		cmp	byte [exec_msg_type],2
		je	@@skip_read_key

		call	flush_key
		call	read_key

@@skip_read_key:
		mov	bx,[exec_message_bx]
		mov	dx,[exec_message_dx]

		call	close_window
		mov	cx,[exec_message_c]
		call	set_cursor

@@skip_close:
		call	popr
		ret

exec_message_c	dw	2020h
exec_message_bx	dw	0		;?
exec_message_dx	dw	0		;?

msg_title	dw	message_title
message_title	db	9, BLUE*16+LIGHT_CYAN, ' Message '	;!!!
error_title	db	7, RED*16+YELLOW, ' Error '		;!!!
msg_box_attr	db	atMessageBox
message_attr	db	atMessage
exec_msg_type	db	0

;=============================================================================
; strlen
;-----------------------------------------------------------------------------
; Get length of string
; Expects:	DS:SI -> string
;		DF = 0
; Returns:	CX -- length (terminating \0 not counted)
;
strlen:
		push	ax
		push	si
		xor	cx,cx
@@next:
		inc	cx
		lodsb
		or	al,al
		jnz	@@next
		dec	cx
		pop	si
		pop	ax
		ret
;=============================================================================
; upcase
;-----------------------------------------------------------------------------
; Expects: al=char
; Returns: al=upper case
upcase:
		cmp	al,'a'
		jb	@@quit
		cmp	al,'z'
		ja	@@quit
		sub	al,20h
@@quit:
		ret
;=============================================================================

double_border_struc	db	'ÉÍ»ººÈÍ¼'
single_border_struc	db	'ÚÄ¿³³ÀÄÙ'

;=============================================================================
; open_window
;-----------------------------------------------------------------------------
; Expects: bl=x1, bh=y1, dl=x2, dh=y2, ah=attr, si=@title
; Returns: None
open_window:
		call	pushr
		mov	cx,DOUBLE_BORDER
		jmp	open_window_2
;=============================================================================
; open_window_bord
;-----------------------------------------------------------------------------
; Expects: bl=x1, bh=y1, dl=x2, dh=y2, ah=attr, si=@title, cx=@Border
;	 ACT: al = SaveWindow (1 = save)
; Returns: None
open_window_bord:
		call	pushr
open_window_2:
		mov	bp,cx
		push	ax
		or	al,al
		jz	@@skip_save_window
		push	dx
		add	dx,0102h
		call	save_window
		pop	dx
@@skip_save_window:
		mov	ax,bx
		push	bp
		call	get_addr
		pop	bp
;		mov	di,ax
		mov	[owb_bp],ax
		mov	es,[video_seg]
		pop	ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0
		dec	cx
		mov	al,[ds:bp]		;'É'
		stosw

		push	cx
		sub	cl,[si]
		shr	cx,1
		mov	bl,cl
		mov	al,[ds:bp+1]	;'Í'
	rep	stosw

		push	ax
		lodsw
		add	bl,al
		mov	cl,al
		jcxz	@@skip_2
@@next_2:
		lodsb
		stosw
		loop	@@next_2
@@skip_2:
		pop	ax
		pop	cx
		push	cx
		sub	cl,bl
		mov	al,[ds:bp+1]	;'Í'
	rep	stosw
		pop	cx
		mov	al,[ds:bp+2]	;'»'
		stosw
		inc	bh
@@next:
		cmp	bh,dh
		je	@@end_loop
		inc	bh
		add	word [owb_bp],160
		mov	di,[owb_bp]
		mov	al,[ds:bp+3]	;'º'
		stosw
		push	cx
		mov	al,' '
	rep	stosw
		pop	cx
		mov	al,[ds:bp+4]	;'º'
		stosw
		mov	al,7
		inc	di
		stosb
		inc	di
		stosb

		jmp	@@next
@@end_loop:
		add	word [owb_bp],160
		mov	di,[owb_bp]
		mov	al,[ds:bp+5]	;'È'
		stosw
		mov	al,[ds:bp+6]	;'Í'
		push	cx
	rep	stosw
		pop	cx
		mov	al,[ds:bp+7]	;'¼'
		stosw
		mov	al,7
		inc	di
		stosb
		inc	di
		stosb

		add	word [owb_bp],165
		mov	di,[owb_bp]
		mov	al,7
		inc	cx
		inc	cx
@@next_1:
		stosb
		inc	di
		loop	@@next_1

		call	popr
		ret

owb_bp		dw	0
emp_title	db	0, 0, 0

;=============================================================================
; close_window
;-----------------------------------------------------------------------------
; Expects: bl=x1, bh=y1, dl=x2, dh=y2
; Returns: None
;
close_window:
		add	dx,0102h
		jmp	restore_window
;=============================================================================
; save_window
;-----------------------------------------------------------------------------
save_window:
		call	pushr
		mov	ax,bx
		call	get_addr
;		mov	bp,ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0		; cx=width
		inc	cx
		push	cs
		pop	es
		mov	di,[buf_pos]
		mov	ds,[video_seg]
;		inc	dh
@@next:
		mov	si,bp
		push	cx
	rep	movsw
		pop	cx
		add	bp,160
		cmp	bh,dh
		jae	@@1
		inc	bh
		jmp	@@next
@@1:
		mov	[cs:buf_pos],di
		mov	[cs:buf_pos_mark],di
		call	popr
		ret
;=============================================================================
; restore_window
;-----------------------------------------------------------------------------
restore_window:
		call	pushr
;		inc	dh
		mov	ax,dx
		call	get_addr
;		mov	bp,ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0		; cx=width
		inc	cx
		mov	si,[buf_pos]
		mov	es,[video_seg]
		std
		lodsw
@@next:
		mov	di,bp
		push	cx
	rep	movsw
		pop	cx
		sub	bp,160
		cmp	bh,dh
		jae	@@1
		inc	bh
		jmp	@@next
@@1:
		cld
		lodsw
		mov	[buf_pos],si
		call	popr
		ret
;=============================================================================
; write_string
;-----------------------------------------------------------------------------
write_string:
		push	di
		push	bp
		push	es
		call	get_addr
		mov	es,[video_seg]
;		mov	di,ax
		mov	ah,bh
@@next:
		lodsb
		cmp	al,0
		je	@@quit
		stosw
		jmp	@@next
@@quit:
		pop	es
		pop	bp
		pop	di
		ret
;=============================================================================
; fill_word, fill_byte
;-----------------------------------------------------------------------------
; Prints hex value of AX (fill_word) or AL (fill_byte).
; In:	AX -- word OR
;	AL -- byte
;	BH -- color
;	ES:DI -> place in video RAM
;
fill_word:
		xchg	al,ah
		call	fill_byte
		xchg	al,ah

fill_byte:
		push	ax
		push	cx
		mov	ah,bh
		mov	cl,4
@@next:
		push	ax
		shr	al,cl
		and	al,0fh
		cmp	al,10
		sbb	al,69h
		das
		stosw
		pop	ax
		sub	cl,4
		jz	@@next

		pop	cx
		pop	ax
		ret
;=============================================================================
; get_addr
;-----------------------------------------------------------------------------
get_addr:
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
;=============================================================================
; gotoxy
;-----------------------------------------------------------------------------
; Expects: dl=x, dh=y
;
gotoxy:
		push	ax
		push	bx
		mov	ah,2
		mov	bh,0
		int	10h
		pop	bx
		pop	ax
		ret
;=============================================================================
; get_cursor
;-----------------------------------------------------------------------------
; Returns: dx=coord, cx=shape
;
get_cursor:
		push	ax
		push	bx
		mov	ah,3
		mov	bh,0
		int	10h
		pop	bx
		pop	ax
		ret
;=============================================================================
; no_cursor, small_cursor, large_cursor, set_cursor
;-----------------------------------------------------------------------------
; Turns cursor off, sets cursor like '_', like 'Û', or reads parameters from CX
;
no_cursor:
		mov	cx,2020h	;### 2000h ?
		jmp	set_cursor
;
small_cursor:
		mov	cx,0607h	;### 0d0eh ?
		jmp	set_cursor
;
large_cursor:
		mov	cx,000dh	;### 000fh ?
;
set_cursor:
		push	ax
		mov	ah,1
		int	10h
		pop	ax
		ret
;=============================================================================
; beep
;-----------------------------------------------------------------------------
beep:
		push	ax
		push	cx
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
		pop	cx
		pop	ax
		ret
;=============================================================================
; write_message
;-----------------------------------------------------------------------------
write_message:
		mov	ah,9
		int	21h
		ret
;=============================================================================

video_seg	dw	COLOR_SEG
buf_pos		dw	window_buffer
buf_pos_mark	dw	window_buffer

;=============================================================================
; E0F
;=============================================================================

