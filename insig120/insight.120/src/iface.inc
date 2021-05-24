;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; iface.inc
; User interface and other procedures.
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
; about
;-----------------------------------------------------------------------------
about:
		mov	si,about_dialog
_about:
		call	init_dialog
		call	exec_dialog
		jmp	draw_screen
;=============================================================================
; show_help
;-----------------------------------------------------------------------------
; Shows help screen.
;
show_help:
		mov	si,help_data
		xor	di,di
		mov	es,[video_seg]

		mov	bx,800fh
		xor	cx,cx
		mov	bp,1
		mov	dx,ds

		jmp	decompr_start_n2b
;-----------------------------------------------------------------------------
; Decompressor start
;-----------------------------------------------------------------------------
%include	 "n2b_d8e.inc"		;Slightly modified part of UPX
;-----------------------------------------------------------------------------
; Decompressor end
;-----------------------------------------------------------------------------
		call	read_key
		call	draw_screen
		ret
;=============================================================================
; any_key
;-----------------------------------------------------------------------------
any_key:
		call	check_shift
		jz	@@check_cheat_code
		mov	cx,4
		mov	bx,edit_procs
@@next:
		cmp	[bx],ah
		jne	@@skip
		call	[bx+1]
		clc
		ret
@@skip:
		add	bx,3
		loop	@@next

@@exit:
		stc
		ret

@@check_cheat_code:
		call	upcase
		xor	al,45h
		mov	bx,[magic_offs]
		cmp	[magic_code+bx],al
		je	@@next_char
		xor	bx,bx
		cmp	[magic_code],al
		jne	@@exit
@@next_char:
		inc	bx
		mov	[magic_offs],bx
		cmp	bl,5
		jne	@@exit
		mov	si,info_dialog
		call	_about
		clc
		ret
;=============================================================================
; data_watch
;-----------------------------------------------------------------------------
data_watch:
		cmp	word [data_watch_proc],0
		mov	word [data_watch_proc],0
		jne	@@update_watch
		mov	ax,[data_watch_temp]
		or	ax,ax
		jnz	@@on
		jmp	beep

@@on:
		mov	[data_watch_proc],ax
		call	ax
@@update_watch:
		jmp	update_watch_label
;=============================================================================
; switch_reg_mode
;-----------------------------------------------------------------------------
switch_reg_mode:
		call	pushr
		cmp	byte [zz_cpu],3
		jnc	@@toggle
		jmp	beep			;!!!

@@toggle:
		mov	ax,'32'
		xor	byte [regs_mode],1
		jz	@@skip_32
		mov	ax,'16'
@@skip_32:
		mov	[regs_menu_1],ax
		mov	[regs_menu_2],ax
		call	get_cursor
		push	cx
		call	draw_screen
		pop	cx
		call	set_cursor
		call	popr
		ret
;=============================================================================
; cur_down
;-----------------------------------------------------------------------------
cur_down:
		cmp	word [cur_line],CPU_HEIGHT-1
		jb	@@inc
		mov	si,[cur_ip]
		call	unassemble
		mov	[cur_ip],si
		jmp	@@quit
@@inc:
		inc	word [cur_line]
@@quit:
		jmp	update_commands
;=============================================================================
; cur_up
;-----------------------------------------------------------------------------
cur_up:
		cmp	word [cur_line],0
		ja	@@dec
		cmp	word [cur_ip],0
		je	@@quit
		mov	word [@@c],20
@@next_1:
		mov	si,[cur_ip]
		sub	si,[@@c]
		jnc	@@next
		xor	si,si
@@next:
		mov	bp,si
		call	unassemble
		cmp	si,[cur_ip]
		je	@@found
		cmp	si,[cur_ip]
		jb	@@next
		dec	word [@@c]
		jnz	@@next_1
		dec	word [cur_ip]
		jmp	@@quit
@@found:
		mov	[cur_ip],bp
		jmp	@@quit
@@dec:
		dec	word [cur_line]
@@quit:
		jmp	update_commands

@@c		dw	20
;=============================================================================
; page_down
;-----------------------------------------------------------------------------
page_down:
		mov	si,[cur_ip]
		mov	cx,CPU_HEIGHT-1
@@next:
		call	unassemble
		loop	@@next

		mov	[cur_ip],si
		jmp	update_commands
;=============================================================================
; page_up
;-----------------------------------------------------------------------------
page_up:
		mov	si,[cur_ip]
		or	si,si
		je	cur_home
		sub	si,5*CPU_HEIGHT
		jnc	@@3
		xor	si,si
@@3:
		mov	cx,CPU_HEIGHT-1
		xor	bx,bx
@@next:
		mov	[instr_table+bx],si
		call	unassemble
		inc	bx
		inc	bx
		loop	@@next
@@2:
		cmp	si,[cur_ip]
		jae	@@found
		mov	cx,CPU_HEIGHT-2
		cld
		push	si
		mov	si,instr_table
		mov	di,si
		lodsw
	rep	movsw
		pop	si
		mov	ax,si
		stosw
		call	unassemble
		jmp	@@2
@@found:
		mov	si,[instr_table]
		mov	[cur_ip],si
		jmp	update_commands
;=============================================================================
; cur_home
;-----------------------------------------------------------------------------
cur_home:
		mov	word [cur_line],0
		jmp	update_commands
;=============================================================================
; cur_end
;-----------------------------------------------------------------------------
cur_end:
		mov	word [cur_line],CPU_HEIGHT-1
		jmp	update_commands
;=============================================================================
; cur_right
;-----------------------------------------------------------------------------
cur_right:
		cmp	word [cur_x],MAX_CUR_X
		jae	@@quit
		inc	word [cur_x]
		call	update_commands
@@quit:
		ret
;=============================================================================
; cur_left
;-----------------------------------------------------------------------------
cur_left:
		cmp	word [cur_x],0
		je	@@quit
		dec	word [cur_x]
		call	update_commands
@@quit:
		ret
;=============================================================================
; go_to_origin
;-----------------------------------------------------------------------------
go_to_origin:
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	ax,[cpu_regs+REG_IP]
		mov	[cur_ip],ax
		mov	word [cur_line],0
		jmp	update_commands
;=============================================================================
; new_csip
;-----------------------------------------------------------------------------
new_csip:
		mov	ax,[cur_line_ip]
		mov	[cpu_regs+REG_IP],ax
		mov	ax,[unasm_seg]
		mov	[cpu_regs+REG_CS],ax
		jmp	update_regs_and_cmds
;=============================================================================
; byte_up
;-----------------------------------------------------------------------------
byte_up:
		cmp	word [cur_ip],0
		je	@@quit
		dec	word [cur_ip]
		call	update_commands
@@quit:
		ret
;=============================================================================
; byte_down
;-----------------------------------------------------------------------------
byte_down:
		inc	word [cur_ip]
		jmp	update_commands
;=============================================================================
; exec_user_screen
;-----------------------------------------------------------------------------
exec_user_screen:
		call	restore_screen
		call	read_key
		call	save_screen
		jmp	draw_screen
;=============================================================================
; restore_all
;-----------------------------------------------------------------------------
restore_all:
		call	restore_regs
		jmp	update_regs_and_cmds
;=============================================================================
; restore_flags
;-----------------------------------------------------------------------------
restore_flags:
		mov	di,cpu_regs+REG_FL
		mov	ax,[di+save_cpu_regs-cpu_regs]
		mov	[di],ax
		jmp	update_regs_and_cmds
;=============================================================================
; restore_csip
;-----------------------------------------------------------------------------
restore_csip:
		mov	di,cpu_regs+REG_IP
		mov	ax,[di+save_cpu_regs-cpu_regs]
		mov	[di],ax
		mov	di,cpu_regs+REG_CS
		mov	ax,[di+save_cpu_regs-cpu_regs]
		mov	[di],ax
		jmp	update_regs_and_cmds
;=============================================================================
; unasm_go_to
;-----------------------------------------------------------------------------
unasm_go_to:
		mov	ax,9*256+44
		mov	di,main_addr_string
		call	read_address
		jc	@@quit

		call	push_unasm_pos

		or	ch,ch
		jz	@@no_seg
		mov	[unasm_seg],dx
		cmp	ch,2
		je	@@no_offs
@@no_seg:
		mov	[cur_ip],ax
@@no_offs:
		mov	word [cur_line],0
		call	update_commands
@@quit:
		ret
;=============================================================================
; read_address
;-----------------------------------------------------------------------------
read_address:
		push	si
		push	di
		push	bp
		push	es
		push	ds
		pop	es
		mov	[d_addr_str],di
		mov	si,addr_dialog
		mov	[si],ax
		add	ax,31+7*256
		mov	[si+2],ax
		call	init_dialog

@@again:
		call	exec_dialog
		jz	@@error_exit

		mov	si,[d_addr_str]
		call	get_address
		jc	@@again

		call	draw_screen
		clc
		jmp	@@exit

@@error_exit:
		call	draw_screen
		stc
@@exit:
		pop	es
		pop	bp
		pop	di
		pop	si
		ret
;=============================================================================

;GetAddressCS	proc
;		push	cpu_regs.xcs
;		jmp	_GetAddress
;		endp
;
;GetAddressDS	proc
;		push	cpu_regs.xds
;
;	_GetAddress:
;		call	GetAddress
;		pop	bx

;=============================================================================
; get_address_ds
;-----------------------------------------------------------------------------
get_address_ds:
		push	ds
		pop	es
		call	get_address
		jc	@@exit

		mov	bx,[cpu_regs+REG_DS]
		cmp	ch,1
		je	@@exit
		cmp	ch,0
		je	@@def_seg
		xor	ax,ax
		jmp	@@exit

@@def_seg:
		mov	dx,bx

@@exit:
		ret
;=============================================================================
; get_address
;-----------------------------------------------------------------------------
get_address:
		call	get_address_
		jnc	@@exit

		mov	si,addr_error_msg
		call	_error_message
		stc
@@exit:
		mov	si,addr_dialog
		ret
;=============================================================================
; get_address_
;-----------------------------------------------------------------------------
get_address_:
		xor	bp,bp
		xor	dx,dx
		xor	cx,cx
		mov	[@@op],cl

@@next:
		lodsb
		cmp	al,' '
		je	@@next
		dec	si
		cmp	al,0
		je	@@done_1
		call	get_reg_value
		je	@@do_oper
		call	get_hex_value
		jc	@@error

@@do_oper:
		cmp	byte [@@op],'-'
		jne	@@plus
		neg	bx

@@plus:
		add	bp,bx
		mov	byte [@@op],0

@@skip:
		lodsb
		cmp	al,' '
		je	@@skip
		cmp	al,0
		je	@@done
		cmp	al,':'
		je	@@seg
		cmp	al,'+'
		je	@@p
		cmp	al,'-'
		jne	@@error

@@p:
		mov	[@@op],al
		jmp	@@next

@@seg:
		or	cl,cl
		jne	@@error
		inc	cx
		mov	dx,bp
		xor	bp,bp
		jmp	@@next

@@done_1:
		cmp	byte [@@op],0
		jne	@@error
		cmp	cx,1
		jne	@@ok
		inc	cx
@@ok:
		clc
		jmp	@@done

@@error:
@@error_exit:
		stc

@@done:
		mov	ax,bp
		mov	ch,cl
		ret

@@op		db	0
;=============================================================================
; get_reg_value
;-----------------------------------------------------------------------------
get_reg_value:
		push	ax
		push	cx
		mov	di,rreg
		mov	ax,[si]
		xchg	al,ah
		call	upcase
		xchg	al,ah
		call	upcase
		mov	cx,16
	repne	scasw
		jne	@@not
		cmp	ax,'GS'
		je	@@check_386
		cmp	ax,'FS'
		je	@@check_386
@@get_value:
		lodsw
		sub	di,rreg+2
		shl	di,1
		mov	bx,[cpu_regs+di]
		cmp	ax,ax
@@not:
		pop	cx
		pop	ax
		ret
@@check_386:
		cmp	byte [zz_cpu],3
		jb	@@not
		jmp	@@get_value
;=============================================================================
; get_hex_value
;-----------------------------------------------------------------------------
get_hex_value:
		push	ax
		push	cx
		mov	cl,4
		xor	bx,bx
		mov	al,[si]
		call	get_hex
		jc	@@not
@@set:
		test	bh,0f0h
		stc
		jnz	@@not
		shl	bx,cl
		or	bl,al
		inc	si
		mov	al,[si]
		call	get_hex
		jnc	@@set
		clc
@@not:
		pop	cx
		pop	ax
		ret
;=============================================================================
; get_hex
;-----------------------------------------------------------------------------
get_hex:
		call	upcase
		cmp	al,'0'
		jb	@@not
		cmp	al,'9'
		jbe	@@digit
		cmp	al,'A'
		jb	@@not
		cmp	al,'F'
		ja	@@not
		sub	al,7
@@digit:
		sub	al,'0'
		clc
		ret
@@not:
		stc
		ret
;=============================================================================
; get_counter
;-----------------------------------------------------------------------------
get_counter:
		mov	si,count_string
		call	get_address_
		jc	@@error
		cmp	ch,0
		jne	@@error
		mov	[count_value],ax
		clc
		ret
@@error:
		mov	si,count_error_msg
		call	_error_message
		stc
		ret
;=============================================================================

;SaveUserInts	proc
;		call	pushr
;		mov	di,offset UserIntTable
;		jmp	SaveInts
;endp

;=============================================================================
; save_int_table
;-----------------------------------------------------------------------------
save_int_table:
		call	pushr
		mov	di,interrupt_table
;SaveInts:
		cli
		xor	si,si
		mov	ds,si
		cld
		mov	cx,512
	rep	movsw
		sti
		call	popr
		ret
;=============================================================================

;RestoreUserInts	proc
;		call	pushr
;		mov	si,offset UserIntTable
;		jmp	RestoreInts
;endp

;=============================================================================
; restore_int_table
;-----------------------------------------------------------------------------
restore_int_table:
		call	pushr
		mov	si,interrupt_table
;RestoreInts:
		cli
		xor	di,di
		mov	es,di
		cld
		mov	cx,512
	rep	movsw
		sti
		call	popr
		ret
;=============================================================================
; create_program
;-----------------------------------------------------------------------------
create_program:
		call	set_my_pid
		call	make_environment
		mov	bp,es
		mov	ah,48h
		mov	bx,0ffffh
		int	21h
		mov	ah,48h
		int	21h
		mov	[user_pid],ax
		mov	[save_dta_seg],ax
		mov	[cpu_regs+REG_CS],ax
		mov	[cpu_regs+REG_DS],ax
		mov	[cpu_regs+REG_ES],ax
		mov	[cpu_regs+REG_SS],ax
		mov	word [cpu_regs+REG_IP],100h
		mov	word [cpu_regs+REG_SP],-2
		mov	es,ax
		push	ax			;
		mov	word [es:0fffeh],0
		dec	ax
		mov	es,ax
		inc	ax
		mov	[es:1],ax
		mov	si,[2]
		mov	dx,ax
		mov	ah,55h
		int	21h
		pop	es			;
		mov	di,80h
		mov	si,my_cmd_line
		lodsb
		stosb
		mov	cl,al
		mov	ch,0
	rep	movsb
		movsb
		mov	cx,100h
		sub	cx,di
		mov	al,0
	rep	stosb
		mov	[es:2ch],bp
		dec	bp
		mov	ax,es
		mov	es,bp
		mov	[es:1],ax
		push	cs
		pop	es
		call	init_registers
		mov	al,[my_port_21]
		mov	[user_port_21],al
		call	set_user_pid
		mov	word [save_dta_offs],80h
		jmp	restore_dta
;=============================================================================
; make_environment
;-----------------------------------------------------------------------------
make_environment:
		mov	es,[2ch]
		xor	di,di
		mov	al,0
		cld
		mov	cx,-1
@@again:
	repne	scasb
		jne	@@error
		dec	cx
		scasb
		jne	@@again
		not	cx
		mov	bx,cx
		add	bx,30
		shr	bx,1
		shr	bx,1
		shr	bx,1
		shr	bx,1
		mov	ah,48h
		int	21h
		jc	@@error
		push	es
		pop	ds
		mov	es,ax
		xor	di,di
		xor	si,si
	rep	movsb
		mov	ax,1
		stosw
		mov	si,env_name
		push	cs
		pop	ds
@@next:
		lodsb
		stosb
		cmp	al,0
		jne	@@next
@@error:
		ret
;=============================================================================
; load_program
;-----------------------------------------------------------------------------
load_program:
		call	set_my_pid
		mov	[cmd_line_seg],cs
		mov	[fcb1_seg],cs
		mov	[fcb2_seg],cs
		mov	dx,filename
		mov	bx,epb
		mov	ax,4b01h
		int	21h
		jc	@@quit
		mov	ax,[user_ss]
		mov	[cpu_regs+REG_SS],ax
		mov	ax,[user_sp]
		cmp	ax,0fffeh
		jae	@@001
		inc	ax
		inc	ax
@@001:
		mov	[cpu_regs+REG_SP],ax
		mov	ax,[user_cs]
		mov	[cpu_regs+REG_CS],ax
		mov	ax,[user_ip]
		mov	[cpu_regs+REG_IP],ax

		mov	ah,51h
		int	21h
		mov	[user_pid],bx
		mov	[cpu_regs+REG_DS],bx
		mov	[cpu_regs+REG_ES],bx
		mov	es,bx
		mov	al,[es:80h]
		mov	ah,0
		inc	ax
		inc	ax
		mov	di,80h
		add	di,ax
		mov	cx,80h
		sub	cx,ax
		mov	al,0
		cld
	rep	stosb
		push	cs
		pop	es
		mov	byte [file_specified],1
		call	init_registers
		mov	al,[my_port_21]
		mov	[user_port_21],al
@@quit:
		ret
;=============================================================================

epb:
env_seg		dw	0
cmd_line_offs	dw	my_cmd_line
cmd_line_seg	dw	0
fcb1_offs	dw	my_fcb1
fcb1_seg	dw	0
fcb2_offs	dw	my_fcb2
fcb2_seg	dw	0
user_sp		dw	0
user_ss		dw	0
user_ip		dw	0
user_cs		dw	0

;MyCmdLine	db	0, 0Dh
my_fcb1:
my_fcb2:
		db	0
times	11	db	' '
;=============================================================================
; init_registers
;-----------------------------------------------------------------------------
init_registers:
		xor	ax,ax
		mov	[cpu_regs+REG_AX],ax
		mov	[cpu_regs+REG_BX],ax
		mov	word [cpu_regs+REG_CX],00ffh
		mov	ax,[cpu_regs+REG_DS]
		mov	[cpu_regs+REG_DX],ax
		mov	ax,[cpu_regs+REG_IP]
		mov	[cpu_regs+REG_SI],ax
		mov	ax,[cpu_regs+REG_SP]
		mov	[cpu_regs+REG_DI],ax
		mov	word [cpu_regs+REG_BP],91ch
		mov	word [cpu_regs+REG_FL],07202h
		jmp	save_regs
;=============================================================================
; read_cmd_line
;-----------------------------------------------------------------------------
read_cmd_line:
		call	pushr
		cmp	byte [file_specified],0
		jne	@@1
		mov	si,no_specif_msg
		call	_error_message
		jmp	@@quit
@@1:
		mov	si,arg_dialog
		call	init_dialog
		call	exec_dialog
		call	draw_screen
		or	ax,ax
		jz	@@quit
		call	set_command_line
		call	popr
		jmp	reset_program
@@quit:
		call	popr
		ret
;=============================================================================
; set_command_line
;-----------------------------------------------------------------------------
set_command_line:
		call	pushr
		mov	si,cmd_line_string
		call	strlen
		mov	di,my_cmd_line
		mov	al,cl
		stosb
	rep	movsb
		mov	al,13
		stosb
		call	popr
		ret
;=============================================================================
; load_file
;-----------------------------------------------------------------------------
load_file:
		mov	dx,file_mask
		call	init_file_list
		mov	si,load_dialog
		call	init_dialog
		call	exec_dialog
		call	draw_screen

		mov	si,[string_ptr]
		call	_check_shared_area

		or	ax,ax
		je	@@quit
		call	full_prog_name
		mov	byte [file_specified],1
		jmp	reset_program

@@quit:
		ret
;=============================================================================
; full_prog_name
;-----------------------------------------------------------------------------
full_prog_name:
		mov	si,filename
		mov	di,filename
		mov	ah,60h
		int	21h
		ret
;=============================================================================
; init_file_list
;-----------------------------------------------------------------------------
init_file_list:
		call	pushr
		call	save_dta
		call	set_my_pid
		call	set_my_dta
		mov	ax,16
		call	init_string_list
		mov	ah,4eh
		mov	cx,27h
@@find_next:
		int	21h
		jc	@@end
		mov	si,9eh
		call	add_string
		mov	ah,4fh
		jmp	@@find_next
@@end:
		call	set_user_pid
		call	restore_dta
		call	popr
		ret
;=============================================================================
; reset_program
;-----------------------------------------------------------------------------
reset_program:
		call	pushr
		cmp	byte [file_specified],0
		je	@@no_spec
		call	unload_program
		call	load_program
		jnc	@@next
		mov	si,load_error_msg
		call	_error_message
		call	create_program
		mov	byte [file_specified],0
@@next:
..@program_created:
		call	set_user_pid
		call	set_term_handler
		call	update_screen
@@quit:
		call	popr
		ret
@@no_spec:
		mov	si,no_specif_msg
		call	_error_message
		jmp	@@quit
;=============================================================================
; new_program
;-----------------------------------------------------------------------------
new_program:
		call	pushr
		mov	byte [cmd_line_string],0
		mov	word [my_cmd_line],0d00h
		call	unload_program
		call	clear_all_breaks
		call	create_program
		mov	ax,[cpu_regs+REG_IP]
		mov	[cur_ip],ax
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	word [cur_line],0
		mov	byte [file_specified],0
		jmp	..@program_created
;=============================================================================
; set_user_pid
;-----------------------------------------------------------------------------
set_user_pid:
		mov	ah,50h
		mov	bx,[user_pid]
		int	21h
		ret
;=============================================================================
; set_my_pid
;-----------------------------------------------------------------------------
set_my_pid:
		mov	ah,50h
		mov	bx,cs
		int	21h
		ret
;=============================================================================
; save_dta
;-----------------------------------------------------------------------------
save_dta:
		push	bx
		push	es
		mov	ah,2fh
		int	21h
		mov	[save_dta_seg],es
		mov	[save_dta_offs],bx
		pop	es
		pop	bx
		ret
;=============================================================================
; set_my_dta
;-----------------------------------------------------------------------------
set_my_dta:
		push	dx
		push	cs
		pop	ds
		mov	dx,80h
		mov	ah,1ah
		int	21h
		pop	dx
		ret
;=============================================================================
; restore_dta
;-----------------------------------------------------------------------------
;!!! no dos func in ah ???
restore_dta:
		push	ds
		mov	ds,[save_dta_seg]
		mov	dx,[save_dta_offs]
		int	21h
		pop	ds
		ret
;=============================================================================
; unload_program
;-----------------------------------------------------------------------------
unload_program:
		call	set_user_pid
		mov	es,[user_pid]
		mov	[es:0ch],cs
		mov	ax,@@1
		mov	[es:0ah],ax
		pushf
		push	cs
		push	ax
		push	es
		push	ds
		push	bp
		push	di
		push	si
		push	dx
		push	cx
		push	bx
		push	ax
		mov	[2eh],sp
		mov	[30h],ss
		mov	ax,4c00h
		int	21h
@@1:
		push	cs
		pop	ds
		push	cs
		pop	es
		ret
;=============================================================================
; set_term_handler
;-----------------------------------------------------------------------------
set_term_handler:
		mov	es,[user_pid]
		mov	[es:0ch],cs
		mov	word [es:0ah],term_handler

;		mov	ds:[2Eh],StackPtr-24
		xor	cx,cx
		call	calc_prog_size
		sub	ax,24
		mov	[2eh],ax

		mov	[30h],ss
		push	cs
		pop	es
		ret
;=============================================================================
; term_handler
;-----------------------------------------------------------------------------
term_handler:
;		mov	sp,StackPtr

		mov	ax,cs
		mov	ds,ax
		mov	es,ax

		xor	cx,cx
		call	calc_prog_size
		mov	sp,ax

		call	restore_01_vector
		call	restore_03_vector
;		call	SaveUserInts
;		call	RestoreIntTable
		call	clear_breakpoints
		call	save_screen
		call	create_program
;		call	SetUserPID
		call	set_term_handler
;		mov	cur_line,0
		call	draw_screen
;-----------------------------------------------------------------------------
; Get return code
;-----------------------------------------------------------------------------
		mov	di,return_code
		mov	ax,DOS_GET_RETURN_CODE * 256 + '0'
		stosb
		stosb
		stosb
		int	21h
		add	ah,'0'
		mov	[term_method],ah
@@loop99:
		dec	di
		aam			;AH = AL / 10;  AL = AL % 10
		add	[di],al
		xchg	al,ah
		or	al,al
		jnz	@@loop99
;-----------------------------------------------------------------------------
		mov	si,terminate_msg
		call	_msg_box
		jmp	main_key_loop
;=============================================================================
; draw_cpu_window
;-----------------------------------------------------------------------------
draw_cpu_window:
		call	pushr
		call	no_cursor
		mov	es,[video_seg]
		xor	di,di
		cld
		mov	cx,2000
		xor	ax,ax
	rep	stosw

		mov	di,CPU_START
		mov	al,'Í'
		mov	ah,atCPUborder
		mov	cx,CPU_WIDTH
	rep	stosw
		mov	al,'¸'
		stosw
		mov	al,'³'
		mov	cx,CPU_HEIGHT

@@next_bord_line:
		add	di,158
		stosw
		loop	@@next_bord_line

		mov	di,CPU_START+(CPU_HEIGHT+1)*160
		mov	al,'Ä'
		mov	cx,CPU_WIDTH
	rep	stosw
		mov	al,'Á'
		stosw
		mov	cx,79-CPU_WIDTH
		mov	al,'Ä'
	rep	stosw

		mov	bx,FLAG16_START
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	bx,FLAG32_START
@@skip_32:
		lea	di,[bx-160-2]

		mov	al,'Ã'
		stosw
		mov	cx,79-CPU_WIDTH
		mov	al,'Ä'
	rep	stosw

		lea	di,[bx+2*160-2]

		mov	al,'Ã'
		stosw
		mov	cx,79-CPU_WIDTH
		mov	al,'Ä'
	rep	stosw


;flags names
		mov	ah,atRegName
		lea	di,[bx+2]
		mov	si,flags_str
		call	fill_str

		mov	ax,40-TITLE_LEN/2
		mov	bh,atTitle
		mov	si,main_title
		call	write_string

		mov	ax,CPU_START+5
		mov	bh,atCPUName
		mov	si,[cpu_name]
		call	write_string

		cmp	byte [regs_mode],1
		je	@@regs_32
		call	draw_registers_16
@@exit:

		mov	si,flags_msg
		call	fill_str

		call	popr
		ret

@@regs_32:
		call	draw_registers_32
		jmp	@@exit
;=============================================================================
; draw_registers_16
;-----------------------------------------------------------------------------
draw_registers_16:
		mov	di,REG_START
		mov	si,reg_names
		mov	ah,atRegName
		mov	cx,4

@@next_reg_line:
		push	cx
		mov	cx,3

@@next_reg_col:
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		add	di,12
		loop	@@next_reg_col

		add	di,160-18*3
		pop	cx
		loop	@@next_reg_line

		add	di,160
		mov	al,'I'
		stosw
		mov	al,'P'
		stosw
		mov	al,'='
		stosw
		add	di,24
		ret
;=============================================================================
; draw_registers_32
;-----------------------------------------------------------------------------
draw_registers_32:
		mov	di,REG_START
		mov	si,rreg
		mov	ah,atRegName
		mov	cx,6

@@next_reg_32:
		call	@@display_name_32
		loop	@@next_reg_32

		add	di,160
		call	@@display_name_32
		call	@@display_name_32

		mov	di,REG_START+2*18
		mov	cx,6

@@next_reg_16:
		call	@@display_name_16
		loop	@@next_reg_16

		add	di,160
		call	@@display_name_16

		sub	di,3*2
		ret

@@display_name_16:
		push	di
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		jmp	@@display_exit

@@display_name_32:
		push	di
		mov	al,'E'
		stosw
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		add	di,4*2
		mov	al,':'
@@display_exit:
		stosw
		pop	di
		add	di,160
		ret
;=============================================================================
; update_watch_label
;-----------------------------------------------------------------------------
update_watch_label:
		call	pushr

		mov	es,[video_seg]
		cld
		mov	di,WATCH_START

		cmp	word [data_watch_proc],0
		je	@@draw_line

		mov	si,[data_watch_label]
		mov	cx,5
		mov	ax,atDataWatchMem*256 + ' '
		stosw

@@next_2:
		lodsb
		stosw
		loop	@@next_2

		mov	al,' '
		stosw
@@exit:
		call	popr
		ret

@@draw_line:
		mov	ax,atCPUborder*256 + 'Ä'
		mov	cx,7
	rep	stosw
		jmp	@@exit
;=============================================================================
; fill_str
;-----------------------------------------------------------------------------
fill_str:
@@next:
		lodsb
		cmp	al,0
		je	@@quit
		stosw
		jmp	@@next
@@quit:
		ret
;=============================================================================
; update_registers
;-----------------------------------------------------------------------------
update_registers:
		call	pushr
		mov	es,[video_seg]

		mov	ax,FLAG16_START+160+4
		mov	cx,NUM_16_REGS
		mov	si,reg16_pos_table
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	ax,FLAG32_START+160+4
		mov	cl,NUM_32_REGS		;CL instead of CX
		mov	si,reg32_pos_table
@@skip_32:
		push	ax

@@next_reg:
		xor	dx,dx
		mov	dl,[si+REG_POS_X]
		shl	dx,1
		mov	al,[si+REG_POS_Y]
		mov	bl,SCRCOLS*2
		mul	bl
		add	dx,ax
		mov	di,[si+REG_POS_LINK]

		mov	ax,[di]

		mov	bh,atRegNewValue
		cmp	[di+save_cpu_regs-cpu_regs],ax
		jne	@@fill
		mov	bh,atRegValue
		cmp	byte [regs_mode],1
		jne	@@fill
		call	align_reg
		push	ax

		cpu	386	;-------- 386 code begin
		mov	eax,[di]
		cmp	[di+save_cpu_regs-cpu_regs],eax
		cpu	8086	;-------- 386 code end

		pop	ax
		je	@@fill
		mov	bh,atRegNewValue
@@fill:
		mov	di,dx
		call	fill_word
		add	si,SIZEOF_REG_POS
		loop	@@next_reg

		pop	di
;-----------------------------------------------------------------------------
@@draw_flags:
		mov	si,0000111011010101b
		;	   ----oditsz-a-p-c
		mov	cx,NUM_FLAGS

		mov	bx,[cpu_regs+REG_FL]
		mov	bp,[save_cpu_regs+REG_FL]
@@next_flag:
@@next_bit:
		mov	ax,atFlagValue*100h+'0'
		mov	dx,ax
		shl	bp,1
		adc	dl,ch		;CH instead of 0
		shl	bx,1
		adc	al,0
		shl	si,1
		jnc	@@next_bit

		xor	dx,ax
		jz	@@display_flag
		mov	ah,atFlagNewValue
@@display_flag:
		stosw
		scasw
		scasw
		loop	@@next_flag

		call	popr
		ret
;=============================================================================
; align_reg
;-----------------------------------------------------------------------------
align_reg:
		lea	di,[di-(cpu_regs-offset_0100h+100h)]
		and	di,11111100b
		lea	di,[di+cpu_regs]
		ret
;=============================================================================
; update_commands
;-----------------------------------------------------------------------------
update_commands:
		call	pushr
		mov	byte [cur_line_brk],0
		mov	word [cur_line_ip],0
		mov	word [cur_ip_line],-1
		mov	si,[cur_ip]
		mov	di,CMD_START
		mov	es,[video_seg]
		mov	cx,CPU_HEIGHT
		xor	bp,bp

next_cpu_line:
		xor	dx,dx
		mov	bx,[unasm_seg]
		cmp	bx,[cpu_regs+REG_CS]
		jne	@@1
		cmp	si,[cpu_regs+REG_IP]
		jne	@@1
		inc	dx
		mov	[cur_ip_line],bp
@@1:
		call	check_bp
		jc	@@8
		mov	dh,1
		je	@@8
		mov	dh,2
@@8:
		mov	bx,si
		call	unassemble
		mov	ax,cmd_normal
		cmp	dh,1
		jb	@@a
		mov	ax,cmd_breakpoint
		jz	@@a
		mov	ax,cmd_brk_disabled
@@a:
		or	dl,dl
		jz	@@2
		mov	word [buffer+4],1011h	;
@@2:
		cmp	bp,[cur_line]
		jne	@@display
		cmp	byte [focus],1
		jne	@@e
		cmp	ax,cmd_breakpoint
		mov	ax,cmd_selected
		jb	@@e
		mov	byte [cur_line_brk],1
		mov	ax,cmd_brk_selected
		je	@@e
		mov	ax,cmd_brk_dis_sel
@@e:
		mov	[cur_line_ip],bx
		call	update_cmd_mem

@@display:
		call	get_ref_addresses
		call	set_direction
		call	display_cpu_line

		add	di,160
		inc	bp

		call	display_code_marks

		loop	next_cpu_line

		mov	ah,LIGHT_CYAN	;!!!
		mov	bh,ah
		add	di,10
		mov	al,' '
		stosw
		mov	ax,[unasm_seg]
		call	fill_word
		mov	ah,bh
		mov	al,':'
		stosw
		mov	ax,[cur_line_ip]
		call	fill_word
		mov	ah,bh
		mov	al,' '
		stosw

		call	popr
		ret
;=============================================================================
; display_cpu_line
;-----------------------------------------------------------------------------
display_cpu_line:
		call	pushr
		xchg	ax,bp
		mov	si,buffer
		mov	dx,[cur_x]
		mov	cx,CPU_WIDTH-1

		mov	ah,[bp]
		mov	bx,CPU_ADDR_SIZE
		call	@@display

		mov	ah,[bp+1]
		mov	bx,CPU_OPCODE_SIZE
		call	@@display

		call	@@display_command

		call	popr
		ret

@@display:
		xchg	bx,cx
@@next_char:
		lodsb
		or	dx,dx
		jz	@@write_char
		dec	dx
		jmp	@@update_cx

@@write_char:
		stosw
		dec	bx

@@update_cx:
		loop	@@next_char
		xchg	bx,cx
		ret

@@display_command:
		lodsb

		mov	ah,[bp+4]
		cmp	al,18h
		je	@@write
		cmp	al,19h
		je	@@write

		mov	ah,[bp+5]
		cmp	al,1eh
		je	@@disabled_arrow
		cmp	al,1fh
		je	@@disabled_arrow

		mov	ah,[bp+2]
		cmp	al,'0'
		jb	@@write
		cmp	al,'9'
		jbe	@@digit
		cmp	al,'A'
		jb	@@write
		cmp	al,'F'
		ja	@@write
@@digit:
		mov	ah,[bp+3]

@@write:
		or	dx,dx
		jz	@@stosw
		dec	dx
		jmp	@@display_command
@@stosw:
		stosw
		loop	@@display_command
		ret

@@disabled_arrow:
		sub	al,1eh-18h
		jmp	@@write
;=============================================================================
; update_cmd_mem
;-----------------------------------------------------------------------------
update_cmd_mem:
		call	pushr
		mov	es,[video_seg]
		mov	di,MEM_START
		mov	al,'Ä'
		mov	ah,atCPUborder
		mov	cx,21
	rep	stosw
		cmp	byte [memory_flag],0
		je	@@exit
		call	calc_mem_address
		call	disp_mem_addr
		mov	bl,[memory_flag]
		mov	bh,0
		dec	bx
		cmp	bx,2
		ja	@@exit
		mov	ds,dx
		add	bx,bx
		call	[cs:display_mem_disp+bx]
@@exit:
		call	popr
		ret

calc_mem_disp	dw	calc_bxsi
		dw	calc_bxdi
		dw	calc_bpsi
		dw	calc_bpdi
		dw	calc_si
		dw	calc_di
		dw	calc_bp
		dw	calc_bx
		dw	calc_mem

calc_bxsi:	mov	ax,[cpu_regs+REG_BX]
		add	ax,[cpu_regs+REG_SI]
		ret

calc_bxdi:	mov	ax,[cpu_regs+REG_BX]
		add	ax,[cpu_regs+REG_DI]
		ret

calc_bpsi:	mov	ax,[cpu_regs+REG_BP]
		add	ax,[cpu_regs+REG_SI]
..@ss:		mov	dx,[cpu_regs+REG_SS]
		mov	cl,'S'
		ret

calc_bpdi:	mov	ax,[cpu_regs+REG_BP]
		add	ax,[cpu_regs+REG_DI]
		jmp	..@ss

calc_si:	mov	ax,[cpu_regs+REG_SI]
		ret

calc_di:	mov	ax,[cpu_regs+REG_DI]
		ret

calc_bx:	mov	ax,[cpu_regs+REG_BX]
		ret

calc_bp:	mov	ax,[cpu_regs+REG_BP]
		jmp	..@ss

calc_mem:	xor	ax,ax
		ret
;=============================================================================
; calc_mem_address
;-----------------------------------------------------------------------------
calc_mem_address:
		mov	bl,[mem_disp_type]
		mov	bh,0
		shl	bx,1
		mov	dx,[cpu_regs+REG_DS]
		mov	cl,'D'
		call	[calc_mem_disp+bx]
		add	ax,[mem_disp_offs]
		mov	si,ax
		mov	ax,[mem_prefix]
		or	ax,ax
		jz	@@exit

		cmp	ax,pfs
		jb	@@set_prefix
		cmp	byte [zz_cpu],3
		jb	@@exit
		cmp	byte [cpu_type],a386
		jb	@@exit

@@set_prefix:
		mov     bx,ax
		mov	cl,[bx]
		sub	cl,'a'-'A'
		add	bx,cpu_regs+REG_CS-pcs
		mov	dx,[bx]
@@exit:
		ret
;=============================================================================

display_mem_disp:
		dw	disp_byte
		dw	disp_word
		dw	disp_dword
disp_byte:
		mov	bh,ah
		lodsb
		call	fill_byte
		jmp	..@rest
disp_word:
		mov	bh,ah
		cmp	si,-1
		jne	@@lodsw
		lodsb
		mov	ah,al
		lodsb
@@lodsw:
		lodsw
		call	fill_word
		jmp	..@rest
disp_dword:
		mov	bh,ah
		lodsw
		xchg	ax,cx
		lodsw
		call	fill_word
		mov	ah,bh
		mov	al,':'
		stosw
		xchg	ax,cx
		call	fill_word
..@rest:
		mov	ah,bh
		mov	al,' '
		stosb
		ret
;=============================================================================
; disp_mem_addr
;-----------------------------------------------------------------------------
disp_mem_addr:
		mov	es,[video_seg]
		mov	di,MEM_START
		mov	ah,atMemAddr
		mov	al,' '
		stosw
		mov	al,cl
		stosw
		mov	al,'S'
		stosw
		mov	al,':'
		stosw
		mov	bh,ah
		mov	ax,si
		call	fill_word
		mov	ah,bh
		mov	al,' '
		stosw
		mov	al,'='
		stosw
		mov	al,' '
		stosw
		mov	ah,atMemValue
		ret
;=============================================================================
; _update_dump
;-----------------------------------------------------------------------------
_update_dump:
		cmp	word [data_watch_proc],0
		je	update_dump
		jmp	word [data_watch_proc]
update_dump:
		call	pushr
		mov	si,[dump_offs]
		mov	es,[video_seg]

		mov	ax,si
;-----------------------------------------------------------------------------
		mov	ah,atDumpAddr
		mov	bp,(DUMP_Y-1)*160+(DUMP_X+13+16*3)*2
		mov	di,(DUMP_Y-1)*160+(DUMP_X+12)*2
		mov	cx,16
@@next:
		and	al,0fh
		push	ax
		cmp	al,10
		sbb	al,69h
		das
		stosw
		xchg	di,bp
		stosw
		xchg	di,bp
		pop	ax
		scasw
		scasw
		inc	ax
		loop	@@next
;-----------------------------------------------------------------------------
		mov	ds,[dump_seg]
		mov	di,DUMP_START
		mov	cx,DUMP_HEIGHT
		sub	dl,16
		neg	dl

		push	dx

@@next_row:
		mov	bh,atDumpAddr
		mov	ax,ds
		call	fill_word
		mov	al,':'
		mov	ah,bh
		stosw
		mov	ax,si
		call	fill_word
		scasw
		scasw
		mov	bl,atDumpValue
		mov	ah,atDumpCursor
;		mov	ah,bh
		push	cx
		mov	cx,16
@@next_col:
		mov	bh,bl
		cmp	byte [cs:dump_edit_mode],0
		je	@@not_cur_1
		cmp	dh,0
		jne	@@not_cur_1
		cmp	dl,cl
		jne	@@not_cur_1
		mov	bh,ah
@@not_cur_1:
		lodsb
		call	fill_byte
		scasw
		loop	@@next_col

		pop	cx
		dec	dh
		add	di,42
		loop	@@next_row

		pop	dx
		mov	bh,atDumpValue
		mov	bl,atDumpCursor

		mov	si,[cs:dump_offs]
		mov	di,DUMP_START+122
		mov	cx,DUMP_HEIGHT
@@next_row_1:
		push	cx
		mov	cx,16
@@next_col_1:
		mov	ah,bh
		cmp	byte [cs:dump_edit_mode],0
		je	@@not_cur_2
		cmp	dh,0
		jne	@@not_cur_2
		cmp	dl,cl
		jne	@@not_cur_2
		mov	ah,bl
@@not_cur_2:
		lodsb
;		cmp	al,0FFh
;		je	@@2
		cmp	al,0
		jne	@@1
@@2:
		mov	al,'.'
@@1:
		stosw
		loop	@@next_col_1
		jmp	@@3
		jmp	@@1
@@3:
		pop	cx
		dec	dh
		add	di,128
		loop	@@next_row_1

		call	popr
		ret
;=============================================================================
; update_stack
;-----------------------------------------------------------------------------
update_stack:
		call	pushr

		mov	ax,STK16_X+STK16_Y*256
		mov	cx,STK16_LEN
		mov	dx,STK16_X+(STK16_Y+STK16_LEN-1)*256
		mov	di,STK16_START
		cmp	byte [regs_mode],1
		jne	@@skip_stack_32
		mov	ax,STK32_X+STK32_Y*256
		mov	cx,STK32_LEN
		mov	dx,STK32_X+(STK32_Y+STK32_LEN-1)*256
		mov	di,STK32_START
@@skip_stack_32:
		mov	si,stack_msg
		mov	bh,atStackName
		call	write_string
		mov	ax,dx
		call	write_string
		mov	es,[video_seg]
		mov	si,[cpu_regs+REG_SP]
		mov	ds,[cpu_regs+REG_SS]

@@next:
		mov	ah,atStackName
		mov	al,'S'
		stosw
		stosw
		mov	al,':'
		stosw
		mov	bh,ah
		mov	ax,si
		call	fill_word
		scasw
		scasw
		mov	bh,atStackValue
		lodsw
		call	fill_word
		sub	di,186
		loop	@@next

		call	popr
		ret
;=============================================================================
; update_regs_color
;-----------------------------------------------------------------------------
update_regs_color:
		cmp	byte [regs_saved],1
		je	@@exit
		inc	byte [regs_saved]
		call	save_regs
		call	update_registers
@@exit:
		ret
;=============================================================================
; display_focus
;-----------------------------------------------------------------------------
display_focus:
		inc	byte [focus]
		jmp	update_commands
;=============================================================================
; hide_focus
;-----------------------------------------------------------------------------
hide_focus:
		dec	byte [focus]
		jmp	update_commands
;=============================================================================
; edit_registers
;-----------------------------------------------------------------------------
edit_registers:
		call	hide_focus
		call	update_regs_color
		call	large_cursor

..@start_edit:
		mov	si,reg16_pos_table
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	si,reg32_pos_table
@@skip_32:
		xor	dx,dx

..@next_00:
		push	dx
		add	dx,[si]
		call	gotoxy
		pop	dx

@@read_key:
		call	read_key

		mov	di,..@_er_actions - 3
		call	action_short_jump
		jc	@@other
		add	di,..@_er_first_action
		jmp	di

..@_er_first_action:
@@other:
		cmp	al,'0'
		jb	..@next_00

		cmp	al,'9'
		jbe	@@digit
		call	upcase
		cmp	al,'A'
		jb	..@next_00
		cmp	al,'F'
		ja	..@next_00
		sub	al,7
@@digit:
		sub	al,'0'
		call	set_half_byte
		inc	dl
		cmp	dl,4
		jae	..@_er_move

..@_er_update:
		call	update_regs_and_cmds
		jmp	..@next_00

..@_er_move:
		mov	al,[si+REG_POS_RIGHT]
		xor	dx,dx
		jmp	..@get_next

..@_er_move_back:
		mov	al,[si+REG_POS_LEFT]
		mov	dl,0
		jmp	..@get_next

..@_er_right:
		inc	dl
		cmp	dl,3
		ja	..@_er_move
		jmp	..@next_00

..@_er_left:
		or	dl,dl
		jnz	@@dec
		mov	al,[si+REG_POS_LEFT]
		mov	dl,3
		jmp	..@get_next

@@dec:
		dec	dl
		jmp	..@next_00

..@_er_up:
		mov	al,[si+REG_POS_UP]
		jmp	..@get_next

..@_er_down:
		mov	al,[si+REG_POS_DOWN]
		jmp	..@get_next

..@_er_local_menu:
		push	si
		mov	si,regs_local_struc
		call	_local_menu
		pop	si
		jc	..@next_00
		jmp	ax

_switch_reg_mode:
		call	switch_reg_mode
		jmp	..@start_edit

_restore_all:
		push	si
		call	restore_all
		pop	si
		jmp	..@next_00

_restore_flags:
		call	restore_flags
		jmp	..@next_00

_restore_csip:
		call	restore_csip		;[0leg, 2006-12-31]
		jmp	..@next_00		;Was: restore_flags

inc_reg:
		mov	di,[si+REG_POS_LINK]
		cmp	byte [regs_mode],1
		je	@@inc_32
@@inc_16:
		inc	word [di]
		jmp	..@_er_update
@@inc_32:
		cmp	di,cpu_regs+REG_CS
		jae	@@inc_16
		call	align_reg

		cpu	386
		inc	dword [di]
		cpu	8086

		jmp	..@_er_update

dec_reg:
		mov	di,[si+REG_POS_LINK]
		cmp	byte [regs_mode],1
		je	@@dec_32
@@dec_16:
		dec	word [di]
		jmp	..@_er_update
@@dec_32:
		cmp	di,cpu_regs+REG_CS
		jae	@@dec_16
		call	align_reg

		cpu	386
		dec	dword [di]
		cpu	8086

		jmp	..@_er_update

zero_reg:
		mov	di,[si+REG_POS_LINK]
		call	align_reg
		mov	word [di],0
		mov	word [di+2],0
		jmp	..@_er_update

restore_reg:
		mov	di,[si+REG_POS_LINK]
		call	align_reg
		mov	ax,[di+save_cpu_regs-cpu_regs]
		mov	[di],ax
		mov	ax,[di+2+save_cpu_regs-cpu_regs]
		mov	[di+2],ax
		jmp	..@_er_update

@@quit:
		call	no_cursor
		call	update_screen
		jmp	display_focus

..@get_next:	cbw
		shl	ax,1
		shl	ax,1
		shl	ax,1
		add	si,ax
		jmp	..@_er_update

..@_er_actions	dw	kbEsc
		db	@@quit		- ..@_er_first_action
		dw	kbEnter
		db	@@quit		- ..@_er_first_action
		dw	kbTab
		db	..@_er_move	- ..@_er_first_action
		dw	kbShiftTab
		db	..@_er_move_back- ..@_er_first_action
		dw	kbRight
		db	..@_er_right	- ..@_er_first_action
		dw	kbLeft
		db	..@_er_left	- ..@_er_first_action
		dw	kbUp
		db	..@_er_up	- ..@_er_first_action
		dw	kbDown
		db	..@_er_down	- ..@_er_first_action
		dw	kbCtrlA
		db	_restore_all	- ..@_er_first_action
		dw	kbCtrlO
		db	restore_reg	- ..@_er_first_action
		dw	kbCtrlT
		db	_restore_flags	- ..@_er_first_action
		dw	kbCtrlC
		db	_restore_csip	- ..@_er_first_action
		dw	kbCtrlR
		db	_switch_reg_mode- ..@_er_first_action
		dw	kbCtrlI
		db	inc_reg		- ..@_er_first_action
		dw	kbCtrlD
		db	dec_reg		- ..@_er_first_action
		dw	kbCtrlZ
		db	zero_reg	- ..@_er_first_action
		dw	kbAltF10
		db	..@_er_local_menu- ..@_er_first_action
		dw	0
;=============================================================================
; set_half_byte
;-----------------------------------------------------------------------------
set_half_byte:
		mov	di,[si+REG_POS_LINK]
		mov	bx,0fff0h
		mov	cl,3
		sub	cl,dl
		shl	cl,1
		shl	cl,1
		rol	bx,cl
		and	bx,[di]
		mov	ah,0
		shl	ax,cl
		or	ax,bx
		mov	[di],ax
		ret
;=============================================================================
; edit_flags
;-----------------------------------------------------------------------------
edit_flags:
		call	hide_focus
		call	update_regs_color
		call	large_cursor
		xor	bx,bx
..@_ef_next:

		mov	dx,FLAG16_X+2+(FLAG16_Y+1)*256
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	dx,FLAG32_X+2+(FLAG32_Y+1)*256
@@skip_32:
		add	dl,bl
		add	dl,bl
		add	dl,bl
		call	gotoxy
		call	read_key

		mov	di,..@_ef_actions - 3
		call	action_short_jump
		jc	..@_ef_next
		add	di,..@_ef_first_action
		jmp	di

..@_ef_first_action:
..@_ef_clear:
		mov	cl,[reg_shift+bx]
		mov	ax,0fffeh
		rol	ax,cl
		and	[cpu_regs+REG_FL],ax
		jmp	..@_ef_update

..@_ef_set:
		mov	cl,[reg_shift+bx]
		mov	ax,1
		shl	ax,cl
		or	[cpu_regs+REG_FL],ax
		jmp	..@_ef_update

..@_ef_local_menu:
		mov	si,flags_local_struc
		call	_local_menu
		jc	..@_ef_next
		call	ax
		jmp	..@_ef_update

toggle_flag:
		jmp	@@toggle_flag

@@toggle:
		call	@@toggle_flag
..@_ef_update:
		call	update_regs_and_cmds
		jmp	..@_ef_next

@@toggle_flag:
		mov	cl,[reg_shift+bx]
		mov	ax,1
		shl	ax,cl
		xor	[cpu_regs+REG_FL],ax
		ret

@@right:
		inc	bl
		cmp	bl,NUM_FLAGS
		jb	..@_ef_next
		mov	bl,0
		jmp	..@_ef_next

@@left:
		cmp	bl,0
		ja	@@dec
		mov	bl,NUM_FLAGS-1
		jmp	..@_ef_next
@@dec:
		dec	bx
		jmp	..@_ef_next

@@restore_flags:
		call	restore_flags
		jmp	..@_ef_next

@@quit:
		call	no_cursor
		jmp	display_focus

..@_ef_actions	dw	kbEsc
		db	@@quit		- ..@_ef_first_action
		dw	kbEnter
		db	@@quit		- ..@_ef_first_action
		dw	kbRight
		db	@@right		- ..@_ef_first_action
		dw	kbLeft
		db	@@left		- ..@_ef_first_action
		dw	0b00h+'0'
		db	..@_ef_clear	- ..@_ef_first_action
		dw	0200h+'1'
		db	..@_ef_set	- ..@_ef_first_action
		dw	3900h+' '
		db	@@toggle	- ..@_ef_first_action
		dw	kbCtrlT
		db	@@restore_flags	- ..@_ef_first_action
		dw	kbAltF10
		db	..@_ef_local_menu- ..@_ef_first_action
		dw	0
;=============================================================================
; edit_dump
;-----------------------------------------------------------------------------
edit_dump:
		mov	byte [dump_edit_mode],1
		call	hide_focus

		push	es

@@next_r:
		mov	si,[dump_offs]
		mov	es,[dump_seg]
		xor	bx,bx
		xor	dx,dx
		xor	bp,bp

		call	update_dump

@@next:
		call	large_cursor
		push	dx
		or	bp,bp
		jz	@@hex_cur
		add	dl,DUMP_X+11+48+2
		jmp	@@set_cur

@@hex_cur:
		mov	al,dl
		add	dl,al
		add	dl,al
		add	dl,bl
		add	dl,DUMP_X+11
@@set_cur:
		add	dh,DUMP_Y
		call	gotoxy
		pop	dx

@@read_key:
		call	read_key

		mov	di,..@_ed_actions - 3
		call	action_short_jump
		jc	@@not_found
		add	di,..@_ed_first_action
		mov	ax,dump_csip
		jmp	di

@@not_found:
		or	bp,bp
		jnz	@@fill_char
;		call	beep
		call	upcase
		cmp	al,'0'
		jb	@@next
		cmp	al,'9'
		jbe	@@digit
		cmp	al,'A'
		jb	@@next
		cmp	al,'F'
		ja	@@next
		sub	al,7

@@digit:
		sub	al,'0'
		mov	cl,bl
		xor	cl,1
		shl	cl,1
		shl	cl,1
		mov	ah,0f0h
		rol	ah,cl
		and	ah,[es:si]
		shl	al,cl
		or	ah,al
		mov	[es:si],ah
		cmp	[es:si],ah
		je	..@_ed_right

@@not_ram:
		push	si
		mov	si,dump_not_in_ram
		call	_error_message
		pop	si
		jmp	@@update

..@_ed_first_action:
..@_ed_dump_follow:
		push	si
		mov	si,follow_menu_struc
		jmp	@@exec_menu

..@_ed_block:
		push	si
		mov	si,block_menu_struc
		jmp	@@exec_menu

..@_ed_local_menu:
		push	si
		mov	si,dump_menu_struc

@@exec_menu:
		call	_local_menu
		pop	si
		jc	@@jump_to_next

@@call_action:
		call	pushr
		call	ax
		call	popr
		jmp	@@select_next_jump

..@_ed_dump_go_to:
		call	dump_go_to

@@select_next_jump:
		jc	@@jump_to_next

@@jump_to_next_r:
		jmp	@@next_r

@@fill_char:
		cmp	al,0
		je	@@jump_to_next
		mov	[es:si],al
		cmp	[es:si],al
		jne	@@not_ram
..@_ed_right:
		or	bp,bp
		jnz	@@cm_1
		inc	bl
		cmp	bl,1
		jbe	@@update
		mov	bl,0
@@cm_1:
		inc	si
		inc	dl
		cmp	dl,15
		ja	@@next_line
@@update:
		call	update_dump
		call	update_commands
		call	update_stack

@@jump_to_next:
		jmp	@@next

..@_ed_dump_reference:
		mov	ax,dump_reference
		jmp	@@call_action

..@_ed_dump_cur_line:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_esdi:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_dssi:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_esbx:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_ssbp:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_dxax:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_dsdx:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_essi:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_dsdi:
		sub	ax,update_dump_window-dump_csip

..@_ed_dump_csip:
		jmp	@@call_action

..@_ed_search:
		mov	ax,data_search
		jmp	@@call_action

..@_ed_search_next:
		mov	ax,data_search_next
		jmp	@@call_action

@@next_line:
		mov	dl,0
@@down_1:
		inc	dh
		cmp	dh,4
		jbe	@@update
		add	word [dump_offs],16
		mov	dh,4
		jmp	@@update
..@_ed_down:
		add	si,16
		jmp	@@down_1
..@_ed_left:
		or	bp,bp
		jnz	@@cm_2
		dec	bl
		jz	@@update
		mov	bl,1
@@cm_2:
		dec	si
		dec	dl			;dl always >=0 before dec
		jns	@@update
		mov	dl,15
@@up_1:
		dec	dh
		jns	@@update		;dh always >=0 before dec
		mov	dh,0
		sub	word [dump_offs],16
		jmp	@@update
..@_ed_up:
		sub	si,16
		jmp	@@up_1
..@_ed_page_up:
		sub	word [dump_offs],16*5
		sub	si,16*5
		jmp	@@update
..@_ed_page_down:
		add	word [dump_offs],16*5
		add	si,16*5
		jmp	@@update

..@_ed_quit:
		call	no_cursor
		mov	byte [dump_edit_mode],0
		call	update_dump
		call	display_focus
		pop	es
		ret

..@_ed_byte_right:
		inc	word [dump_offs]
		inc	si
		jmp	@@update

..@_ed_byte_left:
		dec	word [dump_offs]
		dec	si
		jmp	@@update

..@_ed_switch:
		or	bp,bp
		jz	@@hex2char
		xor	bx,bx
		xor	bp,bp
		jmp	@@next

@@hex2char:
		inc	bp
		jmp	@@next

follow_data_seg:
		mov	ax,[es:si]
		mov	[dump_seg],ax
		xor	ax,ax
		jmp	..@_ed_follow_data

follow_far_data:
		mov	ax,[es:si+2]
		mov	[dump_seg],ax

follow_near_data:
		mov	ax,[es:si]
..@_ed_follow_data:
		mov	[dump_offs],ax
		call	update_dump
		clc
		ret

follow_code_seg:
		mov	ax,[es:si]
		mov	[unasm_seg],ax
		xor	ax,ax
		jmp	..@_ed_follow_code

follow_far_code:
		mov	ax,[es:si+2]
		mov	[unasm_seg],ax

follow_near_code:
		mov	ax,[es:si]
..@_ed_follow_code:
		mov	[cur_ip],ax
		mov	word [cur_line],0
		call	update_commands
		stc
		ret

data_search:
		call	search_bytes
		clc
		ret

data_search_next:
		call	search_next
		clc
		ret

..@_ed_actions	dw	kbEsc
		db	..@_ed_quit	- ..@_ed_first_action
		dw	kbEnter
		db	..@_ed_quit	- ..@_ed_first_action
		dw	kbRight
		db	..@_ed_right	- ..@_ed_first_action
		dw	kbLeft
		db	..@_ed_left	- ..@_ed_first_action
		dw	kbUp
		db	..@_ed_up	- ..@_ed_first_action
		dw	kbDown
		db	..@_ed_down	- ..@_ed_first_action
		dw	kbPgUp
		db	..@_ed_page_up	- ..@_ed_first_action
		dw	kbPgDn
		db	..@_ed_page_down- ..@_ed_first_action
		dw	kbCtrlLeft
		db	..@_ed_byte_left- ..@_ed_first_action
		dw	kbCtrlRight
		db	..@_ed_byte_right- ..@_ed_first_action
		dw	kbTab
		db	..@_ed_switch	- ..@_ed_first_action
		dw	kbCtrlD
		db	..@_ed_dump_go_to- ..@_ed_first_action
		dw	kbAltF10
		db	..@_ed_local_menu- ..@_ed_first_action
		dw	kbAltEqu
		db	..@_ed_dump_reference- ..@_ed_first_action
		dw	kbAltE
		db	..@_ed_dump_esdi- ..@_ed_first_action
		dw	kbAltD
		db	..@_ed_dump_dssi- ..@_ed_first_action
		dw	kbAltB
		db	..@_ed_dump_esbx- ..@_ed_first_action
		dw	kbAltS
		db	..@_ed_dump_ssbp- ..@_ed_first_action
		dw	kbAltA
		db	..@_ed_dump_dxax- ..@_ed_first_action
		dw	kbAltF
		db	..@_ed_dump_dsdx- ..@_ed_first_action
		dw	kbAltI
		db	..@_ed_dump_essi- ..@_ed_first_action
		dw	kbAltJ
		db	..@_ed_dump_dsdi- ..@_ed_first_action
		dw	kbAltC
		db	..@_ed_dump_csip- ..@_ed_first_action
		dw	kbAltL
		db	..@_ed_dump_cur_line- ..@_ed_first_action
		dw	kbCtrlF
		db	..@_ed_dump_follow- ..@_ed_first_action
		dw	kbCtrlB
		db	..@_ed_block	- ..@_ed_first_action
		dw	kbCtrlS
		db	..@_ed_search	- ..@_ed_first_action
		dw	kbCtrlL
		db	..@_ed_search_next- ..@_ed_first_action

		dw	0			;end of table
;=============================================================================
; block_menu
;-----------------------------------------------------------------------------
block_menu:
		mov	si,block_menu_struc
		call	_local_menu
		jc	@@exit
		call	ax
@@exit:
		ret
;=============================================================================
; dump_go_to
;-----------------------------------------------------------------------------
dump_go_to:
		mov	ax,16*256+40
		mov	di,dump_addr_string
		call	read_address
		jc	@@quit
		or	ch,ch
		jz	@@no_seg
		mov	[dump_seg],dx
		cmp	ch,2
		je	@@no_offs
@@no_seg:
		mov	[dump_offs],ax
@@no_offs:
		call	update_dump
		clc
@@quit:
		ret
;=============================================================================
; edit_commands
;-----------------------------------------------------------------------------
edit_commands:
		cmp	word [cur_x],0
		je	@@skip
		mov	word [cur_x],0
		call	update_commands
@@skip:
		push	word [read_exit_keys]
		mov	word [read_exit_keys],asm_exit_keys
		push	cs
		pop	es
@@next_line:
		mov	si,[cur_line_ip]
		call	unassemble
		mov	di,buffer+CMD_POS+MAX_ASM_SIZE-2
		mov	cx,MAX_ASM_SIZE-2
		std
		mov	al,' '
	repe	scasb
		cld
		scasw
		mov	al,0
		stosb
		add	cx,3
		mov	si,buffer+CMD_POS
		mov	di,asm_line
	rep	movsb
;		mov	AsmLine+MaxAsmSize-1,al
@@again:
		mov	di,asm_line
		mov	ah,[cur_line]
;		add	ah,CPUy
		mov	al,CMD_POS-3
		mov	cx,MAX_ASM_SIZE*256+ASM_LINE_SIZE-1
		mov	bx,atsAssembler*256+atAssembler
		cmp	byte [cur_line_brk],1
		jne	@@read_line
		mov	bx,atsBrkAssembler*256+atBrkAssembler
@@read_line:
		call	read_line
		cmp	ax,kbEsc
		je	@@quit
		cmp	ax,kbDown
		jne	@@1
		call	cur_down
		jmp	@@next_line
@@1:		cmp	ax,kbUp
		jne	@@2
		call	cur_up
		jmp	@@next_line
@@2:		cmp	ax,kbPgDn
		jne	@@3
		call	page_down
		jmp	@@next_line
@@3:		cmp	ax,kbPgUp
		jne	@@4
		call	page_up
		jmp	@@next_line
@@4:
		mov	si,asm_line
		mov	di,[cur_line_ip]
		push	es
		mov	es,[unasm_seg]
		mov	al,[es:di]
		inc	byte [es:di]
		inc	al
		cmp	al,[es:di]
		jne	@@rom_error
		dec	byte [es:di]
		mov	bx,di
		call	assemble
		pop	es
		jnc	@@ok
		cmp	ax,ER_EMPTY
		je	@@ok_1
		dec	ax
		dec	ax
		cmp	ax,9-2
		ja	@@unk_err
		mov	bx,ax
		shl	bx,1
		mov	si,[asm_errors+bx]
		jmp	@@err_msg
@@unk_err:
		mov	si,asm_error_msg
@@err_msg:
		call	_error_message
		jmp	@@again
@@ok:
		call	update_dump
@@ok_1:
		call	cur_down
		jmp	@@next_line
@@rom_error:
		pop	es
		mov	dx,code_not_in_ram
		call	_error_message
@@quit:
		call	update_commands
		pop	word [read_exit_keys]
		ret
;=============================================================================
; toggle_breakpoint
;-----------------------------------------------------------------------------
toggle_breakpoint:
		mov	si,[cur_line_ip]
		call	check_bp
		jc	@@set
		mov	byte [bx+BP_ACTIVE],BP_UNUSED
		jmp	@@ok
@@set:
		mov	bx,bp1
		mov	cx,MAX_BREAKPOINTS
@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		jne	@@skip
		mov	ax,[unasm_seg]
		mov	[bx+BP_SEG],ax
		mov	ax,[cur_line_ip]
		mov	[bx+BP_OFFS],ax
		mov	byte [bx+BP_SET],0
		call	test_breakpoint
		jc	@@not
		mov	byte [bx+BP_ACTIVE],BP_ENABLED
		jmp	@@ok
@@not:
		mov	si,bp_not_in_ram
		call	_error_message
		jmp	@@ok
@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp

		mov	si,many_bp_msg
		call	_error_message
@@ok:
		jmp	update_commands
;=============================================================================
; toggle_activity
;-----------------------------------------------------------------------------
toggle_activity:
		mov	si,[cur_line_ip]
		call	check_bp
		jnc	@@toggle
		jmp	beep

@@toggle:
		cmp	byte [bx+BP_ACTIVE],BP_ENABLED
		mov	byte [bx+BP_ACTIVE],BP_DISABLED
		je	@@exit

		mov	byte [bx+BP_ACTIVE],BP_ENABLED
@@exit:
		jmp	update_commands
;=============================================================================
; set_all_breaks
;-----------------------------------------------------------------------------
set_all_breaks:
		mov	bx,bp1
		mov	cx,MAX_BREAKPOINTS

@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		je	@@skip
		mov	byte [bx+BP_ACTIVE],al

@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp
		jmp	update_commands
;=============================================================================
; clear_all_breaks
;-----------------------------------------------------------------------------
clear_all_breaks:
		mov	al,BP_UNUSED
		jmp	set_all_breaks
;=============================================================================
; enable_all_breaks
;-----------------------------------------------------------------------------
enable_all_breaks:
		mov	al,BP_ENABLED
		jmp	set_all_breaks
;=============================================================================
; disable_all_breaks
;-----------------------------------------------------------------------------
disable_all_breaks:
		mov	al,BP_DISABLED
		jmp	set_all_breaks
;=============================================================================
; test_breakpoint
;-----------------------------------------------------------------------------
test_breakpoint:
		push	ax
		push	si
		push	es
		cli
		mov	es,[bx+BP_SEG]
		mov	si,[bx+BP_OFFS]
		mov	al,[es:si]
		mov	byte [es:si],0cch
		cmp	byte [es:si],0cch
		je	@@ok
		stc
@@ok:
		mov	[es:si],al
		sti
		pop	es
		pop	si
		pop	ax
		ret
;=============================================================================
; check_bp
;-----------------------------------------------------------------------------
check_bp:
		push	ax
		push	cx
		mov	bx,bp1
		mov	cx,MAX_BREAKPOINTS
@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		je	@@skip
		cmp	si,[bx+BP_OFFS]
		jne	@@skip
		mov	ax,[unasm_seg]
		cmp	ax,[bx+BP_SEG]
		jne	@@skip
		cmp	byte [bx+BP_ACTIVE],BP_ENABLED
		jmp	@@exit
@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp
		stc
@@exit:
		pop	cx
		pop	ax
		ret
;=============================================================================
; execute_menu
;-----------------------------------------------------------------------------
execute_menu:
		call	hide_focus
		mov	si,menu_structure
		call	exec_menu
		call	display_focus
		call	check_shared_area
		mov	al,[cpu_type]
		sub	al,a86
		mov	[opt_cpu],al
		cmp	byte [quit_flag],1
		jne	@@1
		jmp	quit_act
@@1:
		ret
;=============================================================================
; quit_proc
;-----------------------------------------------------------------------------
quit_proc:
		mov	byte [quit_flag],1
		ret
;=============================================================================
; set_cpu_88
;-----------------------------------------------------------------------------
set_cpu_88:
;		mov	OptCPU,0
		mov	byte [cpu_type],a86
		jmp	update_commands
;=============================================================================
; set_cpu_286
;-----------------------------------------------------------------------------
set_cpu_286:
;		mov	OptCPU,1
		mov	byte [cpu_type],a286
		jmp	update_commands
;=============================================================================
; set_cpu_386
;-----------------------------------------------------------------------------
set_cpu_386:
;		mov	OptCPU,2
		mov	byte [cpu_type],a386
		jmp	update_commands
;=============================================================================
; set_cpu_486
;-----------------------------------------------------------------------------
set_cpu_486:
;		mov	OptCPU,3
		mov	byte [cpu_type],a486
		jmp	update_commands
;=============================================================================
; set_cpu_auto
;-----------------------------------------------------------------------------
set_cpu_auto:
		mov	al,[zz_cpu]
		sub	al,1
		adc	al,0
		mov	[opt_cpu],al
		add	al,a86
		mov	[cpu_type],al
		jmp	update_commands
;=============================================================================
; set_swap_off
;-----------------------------------------------------------------------------
set_swap_off:
		mov	byte [swap_mode],0
		ret
;=============================================================================
; set_swap_smart
;-----------------------------------------------------------------------------
set_swap_smart:
		mov	byte [swap_mode],1
		ret
;=============================================================================
; set_swap_on
;-----------------------------------------------------------------------------
set_swap_on:
		mov	byte [swap_mode],2
		ret
;=============================================================================
; cpu_local_menu
;-----------------------------------------------------------------------------
cpu_local_menu:
		mov	si,cpu_local_struc
		call	_local_menu
		jc	@@exit
		jmp	ax
@@exit:
		ret
;=============================================================================
; continue_init
;-----------------------------------------------------------------------------
continue_init:
		call	init

		cmp	byte [loaded],1
		je	@@check_resident
		mov	si,load_error_msg
		call	_error_message

@@check_resident:
;		mov	si,CryptText2
;		mov	cx,CryptLen2
;		call	Decrypt
		test	byte [cmd_line_options],CMD_RESIDENT
		jz	start_key_loop
		call	_resident

start_key_loop:

		call	draw_screen
main_key_loop:
		mov	word [keys],main_loop
		call	key_loop
		cmp	byte [resident_mode],0
		jne	main_key_loop

;----- Exit from ACT ----------------------------------------------------------
quit_act:
		call	unload_program
		call	set_my_pid
		call	restore_int_table
		call	restore_screen

		mov	al,[my_port_21]
		out	21h,al
		mov	ax,4c00h
		int	21h
;=============================================================================
; _error_message
;-----------------------------------------------------------------------------
_error_message:
		call	error_message
		jmp	check_shared_area
;=============================================================================
; _msg_box
;-----------------------------------------------------------------------------
_msg_box:
		call	message_box
		jmp	check_shared_area
;=============================================================================
; _local_menu
;-----------------------------------------------------------------------------
_local_menu:
		call	local_menu
		pushf
		call	check_shared_area
		popf
		ret
;=============================================================================
; draw_screen
;-----------------------------------------------------------------------------
draw_screen:
		call	draw_cpu_window
update_screen:
		call	update_stack
		call	update_watch_label
update_regs_and_cmds:
		call	_update_dump
		call	update_registers
		jmp	update_commands
;=============================================================================
; E0F
;=============================================================================

