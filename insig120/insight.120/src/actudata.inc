;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; actudata.inc
; Data: uninitialized variables.
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


save_cpu_regs	resw	16*2		;16 regs * 2 words
cpu_regs	resw	16*2		;

bp0		resb	SIZEOF_BREAKPOINT
bp1		resb	MAX_BREAKPOINTS * SIZEOF_BREAKPOINT

save_int_01	resd	1
int_01_set	resb	1
save_int_03	resd	1
int_03_set	resb	1
save_int_09	resd	1
int_09_set	resb	1
save_int_16	resd	1
save_int_24	resd	1

save_sp		resw	1

zz_cpu		resb	1

save_dta_seg	resw	1

cur_x		resw	1
cur_line	resw	1
cur_line_ip	resw	1
user_pid	resw	1
memory_flag	resb	1
mem_disp_type	resb	1
mem_disp_offs	resw	1
mem_prefix	resw	1
cmd_text_size	resw	1
my_port_21	resb	1
user_port_21	resb	1

dlg_win_a	resw	1
dlg_win_b	resw	1
;DlgRunPtr	dw	?
dlg_items	resw	1
dlg_active_item	resw	1
dlg_items_data	resw	MAX_ITEMS

quit_flag	resb	1
resident_mode	resb	1
dump_edit_mode	resb	1
code_mark_exist	resw	1
file_specified	resb	1
cmd_line_options resb	1
regs_mode	resb	1
cur_line_brk	resb	1
video_type	resb	1
decimal_counter	resb	1

unasm_cur_ip	resw	1
cur_cmd		resb	1
; Second_byte	db	?
cur_param	resw	1
word_flag	resb	1
dword_flag	resb	1
print_flag	resb	1
cur_prefix	resw	1
ignore_prefix	resb	1
unasm_seg	resw	1
inst_286	resb	1
op_size_flag	resb	1
addr_size_flag	resb	1
str_flag	resb	1
cur_rep		resw	1
jcnear		resb	1
db_flag		resb	1


;SavedByte	db	?
;TmpFlags	dw	?


follow_offs	resw	1
follow_seg	resw	1

data_watch_proc	resw	1
data_watch_temp	resw	1
data_watch_label resw	1

search_offs	resw	1
search_seg	resw	1
search_len	resw	1
search_options	resw	1

dump_seg	resw	1
dump_offs	resw	1
new_dump_seg	resw	1
new_dump_offs	resw	1
dump_proc	resw	1

minus_flag	resb	1
mov_flag	resb	1
lxx_flag	resb	1
jump_flag	resb	1
param_count	resb	1
rep_pref	resb	1
far_value	resw	1

from_addr	resd	1
to_addr		resd	1
count_value	resw	1

magic_offs	resw	1

instr_table	resw	CPU_HEIGHT-1
asm_buffer	resb	20
buffer		resb	BUF_SIZE
name_buf	resb	MAX_LETTER+2
my_cmd_line	resb	80h
cmd_line_string	resb	80h
filename_string	resb	FILENAME_LEN+1
search_string	resb	SEARCH_FILL_LEN+1
search_bin	resb	SEARCH_FILL_LEN+1
filename	resb	128
asm_line	resb	MAX_ASM_SIZE+1

dump_addr_string resb	ADDRESS_STRING_LENGTH+1
main_addr_string resb	ADDRESS_STRING_LENGTH+1
search_addr_str:
from_string	resb	ADDRESS_STRING_LENGTH+1
to_string	resb	ADDRESS_STRING_LENGTH+1
count_string	resb	ADDRESS_STRING_LENGTH+1
fill_string	resb	SEARCH_FILL_LEN+1
fill_bin	resb	SEARCH_FILL_LEN+1

interrupt_table	resb	1024
;UserIntTable	db	1024 dup (?)
;~UserScreen	resb	4000

old_mode_num	resb	1
old_cursor	resw	1
old_cur_shape	resw	1
old_video_regs	resb	79h
font		resb	4096
old_screen	resw	SCRCOLS*SCRROWS

;=============================================================================
; E0F
;=============================================================================



