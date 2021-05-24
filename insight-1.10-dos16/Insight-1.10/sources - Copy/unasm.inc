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



ifdef _UnAssemble

 CMD_POS	equ	21
 CMD_X		equ	CMD_POS+offset Buffer
 OPER_X		equ	CMD_POS+8+offset Buffer
 BUF_SIZE	equ	80

CPUtype		db	a86

;-------------------------------------------------------------------------------
 UnAssemble	proc	near 	; Unassembles command from UNASM_SEG:SI
		push	ax bx cx dx
ifdef __ACT__
		push	di
endif
		push	ds es
		cld
		xor	ax,ax
ifdef __ACT__
		mov	MemoryFlag,al
		mov	MemDispOfs,ax
		mov	JCNear,al
		mov	MemDispType,8
		mov	MemPrefix,ax
endif
		mov	ds,cs:Unasm_seg
		push	cs
		pop	es
		mov	cs:Cur_IP,si
		mov	cs:Ignore_prefix,al
 Get_next_cmd:
		call	Clear_Buffer
		mov	di,CMD_X
		xor	ax,ax
		mov	cs:OpSizeFlag,al
		mov	cs:AddrSizeFlag,al
		mov	cs:StrFlag,al
		mov	cs:CurRep,ax
		mov	cs:DBflag,al
 Get_next_byte:
		lodsb
		mov	cs:Cur_cmd,al
		xor	bx,bx
		mov	bl,al
		shl	bx,1
		mov	cs:Inst286,0
		mov	cs:Print_flag,0
		mov	dx,cs:Cmd_parameters[bx]
		mov	cs:Cur_param,dx
		mov	dx,cs:cmd_addrs[bx]
		cmp	dx,offset SizePrefixes
		jb	Not_size_prefix
		jmp	Analyze_size_prefix
Not_size_prefix:
		cmp	dx,offset SEG_prefixes
		jb	Not_seg_prefix
		jmp	Analyze_seg_prefix
Not_seg_prefix:
		cmp	dx,offset REP_prefixes
		jb	Get_code_name
		jmp	Analyze_rep_prefix
 Get_code_name:
		call	Analyze_code
		mov	dx,cs:Cur_param
		cmp	cs:Inst286,0
		je	Param8086

		mov	bl,dh
		mov	bh,0
		add	bx,bx
		mov	dx,cs:Table286param[bx]
		mov	cs:Cur_param,dx

Param8086:
		or	dx,dx
		jz	NoParams
		or	dh,dh
		jne	@@No_Print
		mov	cs:Print_flag,1
 @@No_Print:
		mov	al,dl
		call	Analyze_param
		cmp	cs:DBflag,0
		jne	Make_DB_command
		mov	dx,cs:Cur_param
		or	dh,dh
		jz	One_param
		cmp	dh,aemp
		je	One_param
		call	Delimit
		mov	al,dh
		call	Analyze_param
		cmp	cs:DBflag,0
		jne	Make_DB_command
 NoParams:
 One_param:
		cmp	cs:CurRep,0
		je	NoRep
		cmp	cs:StrFlag,1
		je	NoRep
		mov	cs:CurRep,0
Make_DB_command:
		call	Clear_buffer
		mov	dx,offset cdb
		mov	cs:Cur_param,0
		mov	di,CMD_X
		mov	si,cs:Cur_IP
		lodsb
		jmp	Get_code_name
NoRep:
		cmp	cs:Cur_prefix,0
		jne	Prefix_not_used

 Prefix_now_used:
						;[1245 - 1.01]
		cmp	cs:OpSizeFlag,0
		je	@@SkipParam32
		mov	cs:MemoryFlag,3

 @@SkipParam32:
						;[1245 - 1.01]
		push	di
		mov	cx,si
		sub	cx,cs:Cur_IP
		mov	cs:Cmd_length,cx
		mov	di,offset Buffer+6
		call	Fill_codes
		mov	di,offset Buffer
		call	Fill_addr
		pop	di
		pop	es ds
ifdef __ACT__
		lea	ax,[di-(CMD_X-@Startup+100h)]
		mov	CmdTextSize,ax
		pop	di
endif
		pop	dx cx bx ax
		ret

 Cmd_length	dw	0

; Return from UnAssemble --------------------------
 Analyze_size_prefix:
		mov	ah,byte ptr cs:Cur_param
		cmp	ah,cs:CPUtype
		ja	BadPrefix
		cmp	dx,offset OpSize
		je	SetOpSize
		mov	cs:AddrSizeFlag,1
		jmp	Get_next_byte
SetOpSize:
		mov	cs:OpSizeFlag,1
		jmp	Get_next_byte

 Prefix_not_used:
 Many_prefs:
		mov	si,cs:Cur_IP
		mov	cs:Cur_prefix,0
		mov	cs:Ignore_prefix,1
		jmp	Get_next_cmd

 Analyze_seg_prefix:
		mov	ah,byte ptr cs:Cur_param
		cmp	ah,cs:CPUtype
		jbe	PrefOk
BadPrefix:
		mov	dx,offset cdb
locGet_code_name:
		jmp	Get_code_name
PrefOk:
		cmp	cs:Ignore_prefix,1
		je	locGet_code_name
		cmp	cs:Cur_prefix,0
		jne	Many_prefs
		mov	cs:Cur_prefix,dx
		jmp	Get_next_byte

 Analyze_rep_prefix:
		mov	bx,dx
		call	Fill_CS_BX
		mov	di,OPER_X
		mov	cs:CurRep,dx
		jmp	Get_next_byte


 UnAssemble	endp

;-------------------------------------------------------------------------------
 Analyze_code	proc	near
		cmp	al,0Fh
		je	CheckPMinst
		mov	bx,cs:Cur_param
		cmp	bl,a86
		jbe	Not286

Check286inst:
		cmp	bl,80h
		jae	Not286
		cmp	bl,cs:CPUtype
		jbe	@@Ok
		mov	dx,offset cdb
		jmp	@@Go_ahead

@@Ok:
		mov	cs:Inst286,1
		jmp	@@TestExt

 Ch_second:
		lodsb
		cmp	al,0Ah
		je	@@Go_ahead
		mov	byte ptr cs:Cur_param,aim8
		dec	si
		jmp	@@Go_ahead

CheckPMinst:
		cmp	cs:CPUtype,a86
		je	Not286
		jmp	AnalyzePMInst

Not286:
		cmp	al,0D4h
		je	Ch_second
		cmp	al,0D5h
		je	Ch_second
@@TestExt:
		cmp	cs:AddrSizeFlag,0
		je	@@SkipAddr
		cmp	al,0E3h
		je	Check_JECXZ
@@SkipAddr:
		cmp	cs:OpSizeFlag,0
		je	@@SkipOp
		mov	bx,offset ExtTable
@@NextExt:
		mov	ah,cs:[bx]
		or	ah,ah
		jz	@@Go_ahead
		cmp	al,ah
		je	@@FoundExt
		add	bx,3
		jmp	@@NextExt
 @@FoundExt:
		mov	dx,cs:[bx+1]
		jmp	@@Go_ahead
@@SkipOp:
;		cmp	al,0D8h
;		jb	@@Go_ahead
;		cmp	al,0DFh
;		jbe	Coprocessor
 @@Go_ahead:
		push	si
		cmp	dx,6
		jbe	Groups
 Make_DB:
		mov	si,dx
		cmp	dx,offset cdb
		jne	@@Quit
		call	Fill_str
		pop	si
		mov	si,cs:Cur_IP
		lodsb
		mov	di,OPER_X
		call	Fill_Byte
		mov	cs:Cur_param,0
		ret
 Check_JECXZ:
		mov	dx,offset cjecxz
		jmp	@@Go_ahead

 Groups:
		mov	cs:Print_flag,1
		mov	al,ds:[si]
		xor	bx,bx
		mov	bl,al
		and	bl,00111000b
		cmp	dx,Grp1
		jne	@@1
		cmp	bl,00000000b
		jne	@@1
		mov	byte ptr cs:Cur_param+1,aim8
		cmp	byte ptr cs:Cur_param,arm8
		je	@@1
		mov	byte ptr cs:Cur_param+1,aim16
 @@1:
		shr	bl,1
		shr	bl,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		add	bx,dx
		mov	si,cs:Grp_commands[bx]
		cmp	si,offset qqq
		jne	@@Quit
		mov	dx,offset cdb
		mov	cs:Cur_param,0
		jmp	Make_DB
 @@Quit:
		call	Fill_str
		pop	si
		cmp	di,OPER_X
		ja	@@Exit
		mov	di,OPER_X
		ret
 @@Exit:
		inc	di
		ret

AnalyzePMInst:
		push	si
		lodsb
		cmp	al,1
		jbe	PM_group
		cmp	al,6
		jbe	PM_cmd
		cmp	cs:CPUtype,a386
		jae	Analyze386inst
BadPMcommand:
		pop	si
		jmp	@@Go_ahead
PM_cmd:
		add	al,14
		mov	bl,al
		mov	bh,0
		shl	bx,1
		jmp	GetPMname
PM_group:
		mov	bl,al
		lodsb
		and	al,00111000b
		shr	al,1
		shr	al,1
		mov	cl,4
		shl	bl,cl
		or	bl,al
		mov	bh,0
GetPMname:
		mov	dx,cs:PMcommands[bx]
		cmp	dx,offset cdb
		je	BadPMcommand
		cmp	dx,offset cinvlpg
		je	Check_INVLPG
INVLPG_OK:
		mov	ax,cs:PMparam[bx]
		mov	cs:Cur_param,ax
		pop	si
		inc	si
		jmp	@@Go_ahead
Check_INVLPG:
		cmp	CPUtype,a486
		jae	INVLPG_OK
		mov	dx,offset cdb
		jmp	BadPMcommand

Analyze386inst:
		push	di es
		push	cs
		pop	es
		mov	di,offset Table386loc
		mov	cx,T3length
		cmp	cs:CPUtype,a486
		jne	@@3
		mov	cx,T4length
@@3:
		repne	scasb
		mov	bx,di
		pop	es di
		jne	BadPMcommand
		sub	bx,offset Table386loc+1
		add	bx,bx
		mov	dx,cs:Name386[bx]
		mov	ax,cs:Param386[bx]
		mov	cs:Cur_param,ax
		cmp	dx,BtGrp
		je	AnalyzeBTgrp
DoneBT:
		pop	si
		inc	si
		mov	cs:JCNear,1
		jmp	@@Go_ahead
AnalyzeBTgrp:
		lodsb
		and	al,00111000b
		shr	al,1
		shr	al,1
		cbw
		mov	bx,ax
		mov	dx,cs:BTcmd[bx]
		jmp	DoneBT

ExtTable	label	byte
		db	60h
		dw	cpushad
		db	61h
		dw	cpopad
		db	6Dh
		dw	cinsd
		db	6Fh
		dw	coutsd
		db	98h
		dw	ccwde
		db	99h
		dw	ccdq
		db	9Ch
		dw	cpushfd
		db	9Dh
		dw	cpopfd
		db	0A5h
		dw	cmovsd
		db	0A7h
		dw	ccmpsd
		db	0ABh
		dw	cstosd
		db	0ADh
		dw	clodsd
		db	0AFh
		dw	cscasd
		db	0CFh
		dw	ciretd
		db	0

PMcommands	label	word
		dw	csldt
		dw	cstr
		dw	clldt
		dw	cltr
		dw	cverr
		dw	cverw
		dw	cdb
		dw	cdb

		dw	csgdt
		dw	csidt
		dw	clgdt
		dw	clidt
		dw	csmsw
		dw	cdb
		dw	clmsw
		dw	cinvlpg

		; Skipped 2 words
		dw	clar
		dw	clsl
		dw	cdb
		dw	cloadall
		dw	cclts
		; Skipped 1 word

PMparam		label	word
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	ano, ano
		db	ano, ano

		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	ano, ano
		db	arm16, aemp
		db	arm16, aemp

		; Skipped 2 words
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	ano, ano
		db	ano, ano
		db	ano, ano
		; Skipped 1 word

BTcmd		label	word
		dw	cdb
		dw	cdb
		dw	cdb
		dw	cdb
		dw	cbt
		dw	cbts
		dw	cbtr
		dw	cbtc

Table386loc	label	byte

		db	20h, 21h, 22h, 23h, 24h, 26h	; mov CR, DR, TR

		db	80h, 81h, 82h, 83h, 84h, 85h, 86h, 87h
		db	88h, 89h, 8Ah, 8Bh, 8Ch, 8Dh, 8Eh, 8Fh

		db	90h, 91h, 92h, 93h, 94h, 95h, 96h, 97h
		db	98h, 99h, 9Ah, 9Bh, 9Ch, 9Dh, 9Eh, 9Fh

		db	0A0h, 0A1h
		db	0A3h, 0A4h, 0A5h
		db	0A8h, 0A9h
		db	0ABh, 0ACh, 0ADh, 0AFh, 0B2h
		db	0B3h, 0B4h, 0B5h, 0B6h, 0B7h, 0BAh, 0BBh
		db	0BCh, 0BDh, 0BEh, 0BFh

T3length	equ	$-Table386loc

Table486loc	label	byte

		db	08h, 09h, 0B0h, 0B1h, 0C0h, 0C1h
		db	0C8h, 0C9h, 0CAh, 0CBh, 0CCh, 0CDh, 0CEh, 0CFh

T4length	equ	$-Table386loc


Name386		label	word

		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov

		dw	cjo
		dw	cjno
		dw	cjb
		dw	cjnb
		dw	cjz
		dw	cjnz
		dw	cjbe
		dw	cja
		dw	cjs
		dw	cjns
		dw	cjp
		dw	cjnp
		dw	cjl
		dw	cjge
		dw	cjle
		dw	cjg

		dw	cseto
		dw	csetno
		dw	csetb
		dw	csetae
		dw	csete
		dw	csetne
		dw	csetbe
		dw	cseta
		dw	csets
		dw	csetns
		dw	csetp
		dw	csetnp
		dw	csetl
		dw	csetge
		dw	csetle
		dw	csetg

		dw	cpush
		dw	cpop

		dw	cbt
		dw	cshld
		dw	cshld

		dw	cpush
		dw	cpop

		dw	cbts
		dw	cshrd
		dw	cshrd
		dw	cimul
		dw	clss
		dw	cbtr
		dw	clfs
		dw	clgs
		dw	cmovzx
		dw	cmovzx
		dw	BtGrp
		dw	cbtc
		dw	cbsf
		dw	cbsr
		dw	cmovsx
		dw	cmovsx

Name486		label	word
		dw	cinvd
		dw	cwbinvd
		dw	ccmpxchg
		dw	ccmpxchg
		dw	cxadd
		dw	cxadd

		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap


Param386	label	word

		db	r_cr, aemp
		db	r_dr, aemp
		db	cr_r, aemp
		db	dr_r, aemp
		db	r_tr, aemp
		db	tr_r, aemp

		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano

		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano

		db	afs, ano
		db	afs,ano

		db	arm16r, aemp
		db	arm16r, aim8
		db	arm16r, acl

		db	ags, ano
		db	ags, ano

		db	arm16r, aemp
		db	arm16r, aim8
		db	arm16r, acl
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	arm16r, aemp
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	ar8_16, aemp
		db	ar16_32, aemp
		db	arm16, aim8
		db	arm16r, aemp
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	ar8_16, aemp
		db	ar16_32, aemp

Param486	label	word
		db	ano, ano
		db	ano, ano
		db	arm8r, aemp
		db	arm16r, aemp
		db	arm8r, aemp
		db	arm16r, aemp

		db	aax, ano
		db	acx, ano
		db	adx, ano
		db	abx, ano
		db	asp, ano
		db	abp, ano
		db	asi, ano
		db	adi, ano

;Coprocessor:
;		and	al,111b
;		mov	bl,al
;		mov	bh,0
;		lodsb
;		and	al,00111000b
;		or	bl,al
;		add	bx,bx
;		mov	si,cs:CoprocCmd[bx]
;		jmp	@@Quit

		endp
;-------------------------------------------------------------------------------
 Analyze_param	proc
		mov	cs:Word_flag,0
		mov	cs:DWord_flag,0
		test	al,80h
		jz	@@byte
		inc	cs:Word_flag
 @@byte:
		and	al,7Fh

		cmp	al,40h
		jae	Immed_reg

		mov	bx,offset Sub_table
Next_param:	mov	ah,cs:[bx]
		cmp	al,ah
		je	Execute
		add	bx,3
		or	ah,ah
		jnz	Next_param
		ret
Execute:	jmp	cs:[bx+1]

Sub_table	label	byte

		db	aim8
		dw	Immed_value

		db	a1
		dw	Immed_1

		db	amem8
		dw	Memory

		db	asht
		dw	Jump_short

		db	anear
		dw	Jump_near

		db	afar
		dw	Jump_far

		db	arm8
		dw	RegMem

		db	arm8r
		dw	RegMem_Reg

		db	ar8rm
		dw	Reg_RegMem

		db	arm_seg
		dw	RegMem_Seg

		db	aseg_rm
		dw	Seg_RegMem

		db	aesc
		dw	Esc_param

		db	aim8_16
		dw	Im8_to_16

		db	ar8_16
		dw	Reg8_to_16

		db	ar16_32
		dw	Reg16_to_32

		db	r_cr
		dw	CR_to_reg

		db	cr_r
		dw	reg_to_CR

		db	r_dr
		dw	DR_to_reg

		db	dr_r
		dw	reg_to_DR

		db	r_tr
		dw	TR_to_reg

		db	tr_r
		dw	reg_to_TR

		db	astr
		dw	StrParam

		db	astrn
		dw	StrNParam

		db	0

 Immed_reg:
 		cmp	al,aal
 		jae	@@ByteReg
 		mov	cs:Word_flag,1
@@ByteReg:
		xor	bx,bx
		mov	bl,al
		sub	bl,40h
		shl	bx,1
		call	InsertOpSize
		mov	ax,cs:Registers[bx]
		stosw
		ret

 Immed_value:
		cmp	cs:Word_flag,0
		je	Im8
		cmp	cs:OpSizeFlag,1
		je	Im32
 Im8:
		jmp	Fill_WB
 Im32:
		jmp	Fill_DWord

 Im8_to_16:
		lodsb
		cbw
		cmp	cs:OpSizeFlag,1
		jne	Im816
		cwd
		xchg	ax,dx
		call	Fill_Word
		xchg	ax,dx
 Im816:
		jmp	Fill_Word

 Immed_1:
		mov	al,'1'
		stosb
		ret

 Memory:
		call	Print_Ptr
		mov	al,'['
		stosb
		cmp	cs:AddrSizeFlag,0
		je	@@1
		call	Fill_DWord
		jmp	@@2
@@1:
		lodsw
ifdef __ACT__
		mov	cs:MemDispOfs,ax
		mov	cs:MemDispType,8
		mov	ch,cs:Word_flag
		inc	ch
		mov	cs:MemoryFlag,ch
endif
		call	Fill_Word
@@2:
		mov	al,']'
		stosb
		ret

 Reg8_to_16:
		lodsb
		mov	cs:Word_flag,1
		call	Analyze_RM_reg
		call	Delimit
		mov	cs:Word_flag,0
						;[1245 - 1.01]
		mov	cs:OpSizeFlag,0
		mov	cs:Print_flag,1
						;[1245 - 1.01]
		jmp	Analyze_RM_disp

 Reg16_to_32:
		lodsb
		mov	cs:OpSizeFlag,1
		mov	cs:Word_flag,1
		call	Analyze_RM_reg
		call	Delimit
		mov	cs:OpSizeFlag,0
		mov	cs:Print_flag,1
		jmp	Analyze_RM_disp

StrParam:
		cmp	cs:Cur_prefix,0
		je	StrNParam
		mov	bx,cs:Cur_prefix
		mov	cs:Cur_prefix,0
		call	Fill_CS_BX
StrNParam:
		mov	cs:StrFlag,1
		ret
 Jump_short:
		cmp	cs:Cur_cmd,0EBh
		jne	not_print
		mov	bx,offset Short_str
		call	Fill_CS_BX
 Not_print:
		lodsb
		cbw
@@PrintJump:
		call	Check$
		jc	@@ret1
		add	ax,si
		call	Fill_Word
@@ret1:
		ret
Check$:
		or	ax,ax
		jz	@@$p
		mov	bx,cs:Cur_IP
		sub	bx,si
		cmp	ax,bx
		je	@@$
		clc
		ret
@@$:
		mov	al,'$'
		stosb
		stc
		ret
@@$p:
		mov	ax,'+$'
		stosw
		mov	ax,si
		sub	ax,cs:Cur_IP
		add	al,'0'
		stosb
		stc
		ret

 Short_str	db	'short ',0
; Near_str	db	'near ',0
 Far_str	db	'far ',0

 Jump_near:

;		call	Fill_CS_BX
		cmp	cs:AddrSizeFlag,1
		je	NearFar
		lodsw
		jmp	@@PrintJump
 NearFar:
		lodsw
		mov	bx,ax
		lodsw
		add	bx,si
		adc	ax,0
		call	Fill_Word
		mov	ax,bx
		jmp	Fill_Word

 Jump_far:
		mov	bx,offset Far_str
		call	Fill_CS_BX
		cmp	cs:AddrSizeFlag,1
		je	FarFar
		lodsw
		xchg	ax,bx
		lodsw
		call	Fill_Word
		mov	al,':'
		stosb
		xchg	ax,bx
		jmp	Fill_Word
 FarFar:
		lodsw
		mov	bx,ax
		lodsw
		mov	cx,ax
		lodsw
		call	Fill_Word
		mov	al,':'
		stosb
		mov	ax,cx
		call	Fill_Word
		mov	ax,bx
		jmp	Fill_Word

 RegMem:
		lodsb
		cmp	cs:Cur_cmd,0FEh
		jae	@@Jumps
 @@RegMem:
		jmp	Analyze_RM_disp
 @@Jumps:
		push	ax
		and	al,00111000b
		cmp	al,00011000b
		jne	@@Ch_Jump
		mov	cs:DWord_flag,1
		jmp	@@Ret
 @@Ch_Jump:
		cmp	al,00101000b
		jne	@@Ret
		mov	cs:DWord_flag,1
 @@Ret:
		pop	ax
		jmp	@@RegMem

 RegMem_Reg:
		lodsb
		call	Analyze_RM_disp
		call	Delimit
		jmp	Analyze_RM_reg

 Reg_RegMem:
		lodsb
		call	Analyze_RM_reg
		call	Delimit
		jmp	Analyze_RM_disp

 RegMem_Seg:
		lodsb
		mov	cs:Word_flag,1
		call	Analyze_RM_disp
		call	Delimit
		jmp	Analyze_RM_seg

 Seg_RegMem:
		lodsb
		mov	cs:Word_flag,1
		call	Analyze_RM_seg
		call	Delimit
		jmp	Analyze_RM_disp

 Esc_param:
		dec	si
		lodsb
		and	al,00000111b
		mov	ah,al
		lodsb
		push	ax
		and	al,00111000b
		or	al,ah
		call	Fill_Byte
		pop	ax
		call	Delimit
		jmp	Analyze_RM_disp
		endp

CR_to_reg:
		lodsb
		call	GetCRreg
		mov	ah,'c'
		jmp	@GetCRnum
DR_to_reg:
		lodsb
		call	GetCRreg
		mov	ah,'d'
		jmp	@GetCRnum
TR_to_reg:
		lodsb
		call	GetCRreg
		mov	ah,'t'
@GetCRnum:
		call	Delimit
		jmp	GetCRnum
reg_to_CR:
		lodsb
		mov	ah,'c'
		jmp	@GetCRreg
reg_to_DR:
		lodsb
		mov	ah,'d'
		jmp	@GetCRreg
reg_to_TR:
		lodsb
		mov	ah,'t'
@GetCRreg:
		call	GetCRnum
		call	Delimit
		jmp	GetCRreg
GetCRreg:
		push	ax
		and	al,00000111b
		shl	al,1
		cbw
		mov	bx,ax
		mov	al,'e'
		stosb
		mov	ax,cs:Registers[bx]
		stosw
		pop	ax
		ret
GetCRnum:
		push	ax
		and	al,00111000b
		shr	al,1
		shr	al,1
		shr	al,1
		add	al,'0'
		xchg	al,ah
		stosb
		mov	al,'r'
		stosw
		pop	ax
		ret

;------------------------------------------------------------------------------
 Analyze_RM_disp proc
		push	ax
ifdef __ACT__
		mov	ch,cs:Word_flag
		inc	ch
		add	ch,cs:DWord_flag
		mov	cs:MemoryFlag,ch
endif
		cmp	cs:AddrSizeFlag,1
		je	Disp32bit
		mov	ch,al
		and	al,11000000b
		cmp	al,00000000b
		je	No_offs
		cmp	al,01000000b
		je	Byte_offs
		cmp	al,10000000b
		je	Word_offs
Reg2:
ifdef __ACT__
		mov	cs:MemoryFlag,0
endif
		mov	al,ch 			; r/m = Register
		and	al,00000111b
		shl	al,1
		cbw
		call	InsertOpSize
		call	Get_register
		stosw
		jmp	@@Exit
 Word_offs:
		mov	al,ch
		call	Get_disp
		mov	al,'+'
		stosb
		lodsw
		jmp	@@Fill_Word
 Byte_offs:
		mov	al,ch
		call	Get_disp
		lodsb
ifdef __ACT__
		cbw
		mov	cs:MemDispOfs,ax
endif
		mov	ah,'+'
		test	al,80h
		jz	Plus8
		mov	ah,'-'
		neg	al
 Plus8:
		mov	es:[di],ah
		inc	di
		call	Fill_Byte
		jmp	@@Quit
 No_offs:
ifdef __ACT__
		mov	cs:MemDispOfs,0
endif
		mov	al,ch
		and	al,00000111b
		cmp	al,00000110b
		jne	Read_disp
		call	Print_Ptr
		mov	al,'['
		stosb
		lodsw
ifdef __ACT__
		mov	cs:MemDispType,8
endif

 @@Fill_Word:

ifdef __ACT__
		mov	cs:MemDispOfs,ax
endif
		call	Fill_Word
		jmp	@@Quit
 Read_disp:
		call	Get_disp
 @@Quit:
		mov	al,']'
		stosb
 @@Exit:
		pop	ax
		ret

Disp32bit:
		mov	ch,al
		and	al,11000000b
		cmp	al,11000000b
		je	Reg2

		mov	cl,al
		mov	al,ch
		and	al,00000111b
		cmp	al,00000100b
		je	AnalyzeSIB

		xchg	al,cl
		cmp	al,00000000b
		je	NoOfs32
		cmp	al,10000000b
		je	Ofs32
Ofs8:
		mov	al,cl
		call	Get32disp
		mov	al,'+'
		stosb
		lodsb
		call	Fill_Byte
		jmp	@@Quit
Ofs32:
		mov	al,cl
		call	Get32disp
		mov	al,'+'
		stosb
		call	Fill_DWord
		jmp	@@Quit
;;
NoOfs32:
		cmp	cl,00000101b
		jne	NoDisp32
		mov	al,'['
		stosb
		call	Fill_DWord
		jmp	@@Quit
NoDisp32:
		mov	al,cl
		call	Get32disp
		jmp	@@Quit
AnalyzeSIB:
		lodsb
		mov	ch,al
		cmp	cl,00000000b
		je	SIB_no_ofs
		cmp	cl,01000000b
		je	SIB_ofs8
SIB_ofs32:
		and	al,00000111b
		call	Get32disp
		call	GetScale
		mov	al,'+'
		stosb
		call	Fill_DWord
		jmp	@@Quit
SIB_no_ofs:
		and	al,00000111b
		cmp	al,00000101b
		jne	GetSIBdisp
		call	Print_Ptr
		mov	al,'['
		stosb
		call	Fill_DWord
		jmp	@@GetScale
GetSIBdisp:
		call	Get32disp
@@GetScale:
		call	GetScale
		jmp	@@Quit
SIB_ofs8:
		and	al,00000111b
		call	Get32disp
		call	GetScale
		mov	al,'+'
		stosb
		lodsb
		call	Fill_Byte
		jmp	@@Quit

endp

;-------------------------------------------------------------------------------
 Analyze_RM_reg	proc
		push	ax
		and	al,00111000b
		shr	al,1
		shr	al,1
		cbw
		call	InsertOpSize
		call	Get_register
		stosw
		pop	ax
		ret
		endp

;-------------------------------------------------------------------------------
 Get_register	proc
		push	bx
		mov	bl,cs:Word_flag
		xor	bl,1
		xor	bh,bh
		shl	bx,1
		shl	bx,1
		shl	bx,1
		shl	bx,1
		add	bx,ax
		mov	ax,cs:Registers[bx]
		inc	bx
		pop	bx
		ret
		endp

;-------------------------------------------------------------------------------
InsertOpSize	proc
		cmp	cs:OpSizeFlag,0
		je	@@ret
		cmp	cs:Word_flag,0
		je	@@ret
		push	ax
		mov	al,'e'
		stosb
		pop	ax
@@ret:
		ret
endp

;-------------------------------------------------------------------------------
 Analyze_RM_seg	proc
		push	ax
		and	al,00111000b
		cmp	cs:CPUtype,a386
		jae	@@3
		cmp	al,00011000b
		jbe	@@1
@@BadSegReg:
		mov	cs:DBflag,1
		pop	ax
		ret
@@3:
		cmp	al,00101000b
		ja	@@BadSegReg
@@1:
		shr	al,1
		shr	al,1
		cbw
		mov	bx,ax
		mov	ax,cs:SegRegs[bx]
		stosw
		pop	ax
		ret
		endp

;-------------------------------------------------------------------------------
 Get_disp	proc	near
		call	Print_Ptr
		and	al,00000111b
		cbw
ifdef __ACT__
		mov	cs:MemDispType,al
endif
		shl	ax,1
		mov	bx,ax
		mov	bx,cs:Disp_adrs[bx]
		jmp	Fill_CS_BX
		endp

 Disp_adrs	label	word
		dw	of000
		dw	of001
		dw	of010
		dw	of011
		dw	of100
		dw	of101
		dw	of110
		dw	of111

 of000		db	'[bx+si',0
 of001		db	'[bx+di',0
 of010		db	'[bp+si',0
 of011		db	'[bp+di',0
 of100		db	'[si',0
 of101		db	'[di',0
 of110		db	'[bp',0
 of111		db	'[bx',0


Get32disp	proc
		cmp	cs:OpSizeFlag,0
		je	@@1
		mov	DWord_flag,1
@@1:
		call	Print_Ptr
		cbw
		mov	bx,ax
		add	bx,bx
		mov	ax,'e['
		stosw
		mov	ax,cs:Registers[bx]
		stosw
		ret
endp

GetScale	proc
		mov	al,ch
		and	al,00111000b
		cmp	al,00100000b
		je	NoScale
		shr	al,1
		shr	al,1
		cbw
		mov	bx,ax
		mov	ax,'e+'
		stosw
		mov	ax,cs:Registers[bx]
		stosw
		mov	al,'*'
		stosb
		mov	al,ch
		and	al,11000000b
		rol	al,1
		rol	al,1
		cbw
		mov	bx,ax
		mov	al,cs:Scaler[bx]
		stosb
NoScale:
		ret
endp

Scaler		db	'1248'


 ptByte		db	'byte ',0
 ptDword	db	'd'
 ptWord		db	'word ',0
 ptPtr		db	'ptr ',0

;-------------------------------------------------------------------------------
 Print_Ptr	proc
		push	ax si
		cmp	cs:Print_flag,0
		je	@@Quit
		mov	si,offset ptByte
		cmp	cs:Word_flag,0
		je	@@Byte
		mov	si,offset ptWord

						;[1245 - 1.01]
		cmp	cs:OpSizeFlag,0
		je	@@CheckDword
		mov	cs:DWord_flag,1
;		mov	cs:MemoryFlag,3
 @@CheckDword:
						;[1245 - 1.01]

		cmp	cs:DWord_flag,0
		je	@@Byte
		dec	si
 @@Byte:
		call	Fill_str
		mov	si,offset ptPtr
		call	Fill_str
 @@Quit:
		cmp	cs:Cur_prefix,0
		je	@@Exit
		mov	si,cs:Cur_prefix
ifdef __ACT__
		mov	cs:MemPrefix,si
endif
		call	Fill_str
 @@Exit:
		mov	cs:Cur_prefix,0
		pop	si ax
		ret
		endp

;-------------------------------------------------------------------------------
 Fill_str	proc	near
		push	ds
		push	cs
		pop	ds
 @@Next:	lodsb
		or	al,al
		jz	@@Quit
		stosb
		jmp	@@Next
 @@Quit:	pop	ds
		ret
 Fill_str	endp

;-------------------------------------------------------------------------------
 Fill_CS_BX	proc	near
 @@Next:	mov	al,cs:[bx]
		or	al,al
		jz	@@Quit
		stosb
		inc	bx
		jmp	@@Next
 @@Quit:
		ret
 Fill_CS_BX	endp

;-------------------------------------------------------------------------------
 Delimit	proc	near
		push	ax
		mov	al,','
		stosb
		pop	ax
		ret
 Delimit	endp

;-------------------------------------------------------------------------------
 Fill_addr	proc	near
		mov	ax,cs:Cur_IP
		jmp	Fill_Word
 Fill_addr	endp

;-------------------------------------------------------------------------------
 Fill_codes	proc	near
		push	si
		mov	si,cs:Cur_IP
 @@NextByte:
		cmp	di,CMD_X-1
		jae	@@Trunc
		lodsb
		call	Fill_Byte
		loop	@@NextByte
 @@Quit:
		pop	si
		ret
 @@Trunc:
		mov	al,'+'
		stosb
		jmp	@@Quit
 Fill_codes	endp

;-------------------------------------------------------------------------------
 Fill_WB	proc	near
		cmp	cs:Word_flag,1
		je	@@Out_Word
		lodsb
		jmp	Fill_Byte
 @@Out_Word:
		lodsw
		jmp	Fill_Word
 Fill_WB	endp

;-------------------------------------------------------------------------------
Fill_DWord	proc
		lodsw
		push	ax
		lodsw
		call	Fill_Word
		pop	ax
		jmp	Fill_Word
		endp

;-------------------------------------------------------------------------------
 Fill_Word	proc	near
		push	ax
		mov	al,ah
		call	Fill_Byte
		pop	ax
		jmp	Fill_Byte
 Fill_Word	endp

;-------------------------------------------------------------------------------
 Fill_Byte	proc	near
		mov	ah,al
		shr	al,1
		shr	al,1
		shr	al,1
		shr	al,1
		call	sub_23
		mov	al,ah
		and	al,1111b
 Sub_23:
		add	al,'0'
		cmp	al,'9'
		jbe	@@0_9
		add	al,7
 @@0_9:
		stosb
		ret
 Fill_Byte	endp

;-------------------------------------------------------------------------------
; Print_Command	proc	near
;		push	si
;		mov	ax,0A0Dh
;		stosw
;		mov	cx,di
;		mov	dx,offset Buffer
;		sub	cx,dx
;		mov	ah,40h
;		mov	bx,1
;		int	21h
;		pop	si
;		ret
; Print_Command	endp

;-------------------------------------------------------------------------------
 Clear_buffer	proc	near
		mov	di,offset Buffer
		push	di
		mov	al,' '
		mov	cx,BUF_SIZE
		rep	stosb
		pop	di
		ret
 Clear_buffer	endp

 Registers	label	word
		db	'axcxdxbxspbpsidi'
		db	'alcldlblahchdhbh'
 SegRegs	label	word
		db	'escsssdsfsgs'


 Commands	label	byte
 cadd		db	'add',0
 cor		db	'or',0
 cadc		db	'adc',0
 csbb		db	'sbb',0
 cand		db	'and',0
 csub		db	'sub',0
 cxor		db	'xor',0
 ccmp		db	'cmp',0
 cinc		db	'inc',0
 cdec		db	'dec',0
 cpush		db	'push',0
 cpop		db	'pop',0
 cmov		db	'mov',0
 cxchg		db	'xchg',0
 cshl		db	'shl',0
 cshr		db	'shr',0
 crcl		db	'rcl',0
 crcr		db	'rcr',0
 csar		db	'sar',0
 crol		db	'rol',0
 cror		db	'ror',0
 ctest		db	'test',0
 cnot		db	'not',0
 cneg		db	'neg',0
 cmul		db	'mul',0
 cimul		db	'imul',0
 cdiv		db	'div',0
 cidiv		db	'idiv',0
 ccbw		db	'cbw',0
 ccwd		db	'cwd',0
 caaa		db	'aaa',0
 caad		db	'aad',0
 caam		db	'aam',0
 caas		db	'aas',0
 cdaa		db	'daa',0
 cdas		db	'das',0
 cmovsb		db	'movsb',0
 cmovsw		db	'movsw',0
 cstosb		db	'stosb',0
 cstosw		db	'stosw',0
 clodsb		db	'lodsb',0
 clodsw		db	'lodsw',0
 ccmpsb		db	'cmpsb',0
 ccmpsw		db	'cmpsw',0
 cscasb		db	'scasb',0
 cscasw		db	'scasw',0

 ccall		db	'call',0
 cjmp		db	'jmp',0
 cret		db	'ret',0
 cretf		db	'retf',0
 cloop		db	'loop',0
 cloope		db	'loope',0
 cloopne	db	'loopne',0
 cint		db	'int',0
 cinto		db	'into',0
 cint3		db	'int3',0
 ciret		db	'iret',0
 cpushf		db	'pushf',0
 cpopf		db	'popf',0
 csahf		db	'sahf',0
 clahf		db	'lahf',0
 cclc		db	'clc',0
 ccmc		db	'cmc',0
 cstc		db	'stc',0
 ccld		db	'cld',0
 cstd		db	'std',0
 ccli		db	'cli',0
 csti		db	'sti',0
 chlt		db	'hlt',0
 cnop		db	'nop',0
 clea		db	'lea',0
 cles		db	'les',0
 clds		db	'lds',0
 cxlat		db	'xlat',0
 cjcxz		db	'jcxz',0
 cin		db	'in',0
 cout		db	'out',0
 cesc		db	'esc',0
 cwait		db	'wait',0

 cjo		db	'jo',0
 cjno		db	'jno',0
 cjb		db	'jb',0
 cjnb		db	'jnb',0
 cjz		db	'jz',0
 cjnz		db	'jnz',0
 cjbe		db	'jbe',0
 cja		db	'ja',0
 cjs		db	'js',0
 cjns		db	'jns',0
 cjp		db	'jp',0
 cjnp		db	'jnp',0
 cjl		db	'jl',0
 cjge		db	'jge',0
 cjle		db	'jle',0
 cjg		db	'jg',0

; 286 instructions

 cpusha		db	'pusha',0
 cpopa		db	'popa',0
 cbound		db	'bound',0
 cinsb		db	'insb',0
 coutsb		db	'outsb',0
 cinsw		db	'insw',0
 coutsw		db	'outsw',0
 center		db	'enter',0
 cleave		db	'leave',0

; 286 Protected Mode instructions

 clgdt		db	'lgdt',0
 csgdt		db	'sgdt',0
 clidt		db	'lidt',0
 csidt		db	'sidt',0
 clldt		db	'lldt',0
 csldt		db	'sldt',0
 clmsw		db	'lmsw',0
 csmsw		db	'smsw',0
 cltr		db	'ltr',0
 cstr		db	'str',0
 clar		db	'lar',0
 clsl		db	'lsl',0
 carpl		db	'arpl',0
 cverr		db	'verr',0
 cverw		db	'verw',0
 cclts		db	'clts',0
 cloadall	db	'loadall',0
; c		db	'',0

; 386 instructions

 cmovsx		db	'movsx',0
 cmovzx		db	'movzx',0
 clfs		db	'lfs',0
 clgs		db	'lgs',0
 clss		db	'lss',0
 ccwde		db	'cwde',0
 ccdq		db	'cdq',0
 cshld		db	'shld',0
 cshrd		db	'shrd',0
 cbt		db	'bt',0
 cbts		db	'bts',0
 cbtr		db	'btr',0
 cbtc		db	'btc',0
 cbsf		db	'bsf',0
 cbsr		db	'bsr',0
 clodsd		db	'lodsd',0
 cstosd		db	'stosd',0
 cinsd		db	'insd',0
 coutsd		db	'outsd',0
 cmovsd		db	'movsd',0
 ccmpsd		db	'cmpsd',0
 cscasd		db	'scasd',0

 cseta		db	'seta',0
 csetae		db	'setae',0
 csetb		db	'setb',0
 csetbe		db	'setbe',0
 csete		db	'sete',0
 csetne		db	'setne',0
 csetp		db	'setp',0
 csetnp		db	'setnp',0
 csetg		db	'setg',0
 csetge		db	'setge',0
 csetl		db	'setl',0
 csetle		db	'setle',0
 csets		db	'sets',0
 csetns		db	'setns',0
 cseto		db	'seto',0
 csetno		db	'setno',0

 cjecxz		db	'jecxz',0
 ciretd		db	'iretd',0
 cpushad	db	'pushad',0
 cpopad		db	'popad',0
 cpushfd	db	'pushfd',0
 cpopfd		db	'popfd',0
 
; 486 instructions

 cinvd		db	'invd',0
 cwbinvd	db	'wbinvd',0
 cinvlpg	db	'invlpg',0
 ccmpxchg	db	'cmpxchg',0
 cxadd		db	'xadd',0
 cbswap		db	'bswap',0
; c		db	'',0

comment #

 Cfld		db	'fld',0
 cfst		db	'fst',0
 cfstp		db	'fstp',0
 cfxch		db	'fxch',0
 cfcom		db	'fcom',0
 cfcomp		db	'fcomp',0
 cfcompp	db	'fcompp',0
 cftst		db	'ftst',0
 cfxam		db	'fxam',0
 cfadd		db	'fadd',0
 cfsub		db	'fsub',0
 cfmul		db	'fmul',0
 cfdiv		db	'fdiv',0
 cfsqrt		db	'fsqrt',0
 cfscale	db	'fscale',0
 cfprem		db	'fprem',0
 cfrndint	db	'frndint',0
 cfxtract	db	'fxtract',0
 cfabs		db	'fabs',0
 cfchs		db	'fchs',0
 cfptan		db	'fptan',0
 cfpatan	db	'fpatan',0
 cf2xm1		db	'f2xm1',0
 cfyl2x		db	'fyl2x',0
 cfyl2xp1	db	'fyl2xp1',0
 cfldz		db	'fldz',0
 cfld1		db	'fld1',0
 cfldpi		db	'fldpi',0
 cfldl2t	db	'fldl2t',0
 cfldl2e	db	'fldl2e',0
 cfldlg2	db	'fldlg2',0
 cfldln2	db	'fldln2',0
 cfinit		db	'finit',0
 cfeni		db	'feni',0
 cfdisi		db	'fdisi',0
 cfldcw		db	'fldcw',0
 cfstcw		db	'fstcw',0
 cfldsw		db	'fldsw',0
 cfstsw		db	'fstsw',0
 cfclex		db	'fclex',0
 cfstenv	db	'fstenv',0
 cfldenv	db	'fldenv',0
 cfsave		db	'fsave',0
 cfrstor	db	'frstor',0
 cfincstp	db	'fincstp',0
 cfdecstp	db	'fdecstp',0
 cffree		db	'ffree',0
 cffnop		db	'fnop',0
#

 cdb		db	'db',0
 qqq		db	'???',0

 Prefixes	label	byte

 plock		db	'lock',0
 REP_prefixes:
 prep		db	'rep ',0
 prepe		db	'repe ',0
 prepne		db	'repne ',0
 SEG_prefixes:
 pcs		db	'cs:',0
 pds		db	'ds:',0
 pes		db	'es:',0
 pss		db	'ss:',0
 pfs		db	'fs:',0
 pgs		db	'gs:',0
SizePrefixes:
 OpSize		db	0
 AddrSize	db	0

;		db	'',0
 ArOp1		equ	0
 ArOp2		equ	1
 ShiftOp	equ	2
 Grp1		equ	3
 Grp2		equ	4
 Grp3		equ	5
 MovGrp		equ	6

 BtGrp		equ	6

 Cmd_addrs	label	word

		dw	cadd 			; 0
		dw	cadd
		dw	cadd
		dw	cadd
		dw	cadd
		dw	cadd
		dw	cpush
		dw	cpop

		dw	cor
		dw	cor
		dw	cor
		dw	cor
		dw	cor
		dw	cor
		dw	cpush
		dw	cdb

		dw	cadc 			; 1
		dw	cadc
		dw	cadc
		dw	cadc
		dw	cadc
		dw	cadc
		dw	cpush
		dw	cpop

		dw	csbb
		dw	csbb
		dw	csbb
		dw	csbb
		dw	csbb
		dw	csbb
		dw	cpush
		dw	cpop

		dw	cand 			; 2
		dw	cand
		dw	cand
		dw	cand
		dw	cand
		dw	cand
		dw	pes
		dw	cdaa

		dw	csub
		dw	csub
		dw	csub
		dw	csub
		dw	csub
		dw	csub
		dw	pcs
		dw	cdas

		dw	cxor 			; 3
		dw	cxor
		dw	cxor
		dw	cxor
		dw	cxor
		dw	cxor
		dw	pss
		dw	caaa

		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	pds
		dw	caas

		dw	cinc 			; 4
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc

		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec

		dw	cpush 			; 5
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush

		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop

		dw	cpusha 			; 6
		dw	cpopa
		dw	cbound
		dw	carpl
		dw	pfs
		dw	pgs
		dw	OpSize
		dw	AddrSize

		dw	cpush
		dw	cimul
		dw	cpush
		dw	cimul
		dw	cinsb
		dw	cinsw
		dw	coutsb
		dw	coutsw

		dw	cjo 			; 7
		dw	cjno
		dw	cjb
		dw	cjnb
		dw	cjz
		dw	cjnz
		dw	cjbe
		dw	cja

		dw	cjs
		dw	cjns
		dw	cjp
		dw	cjnp
		dw	cjl
		dw	cjge
		dw	cjle
		dw	cjg

		dw	ArOp1 			; 8
		dw	ArOp1
		dw	ArOp2
		dw	ArOp2
		dw	ctest
		dw	ctest
		dw	cxchg
		dw	cxchg

		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	clea
		dw	cmov
		dw	cpop

		dw	cnop 			; 9
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg

		dw	ccbw
		dw	ccwd
		dw	ccall
		dw	cwait
		dw	cpushf
		dw	cpopf
		dw	csahf
		dw	clahf

		dw	cmov 			; A
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmovsb
		dw	cmovsw
		dw	ccmpsb
		dw	ccmpsw

		dw	ctest
		dw	ctest
		dw	cstosb
		dw	cstosw
		dw	clodsb
		dw	clodsw
		dw	cscasb
		dw	cscasw

		dw	cmov 			; B
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov

		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov

		dw	ShiftOp			; C
		dw	ShiftOp
		dw	cret
		dw	cret
		dw	cles
		dw	clds
		dw	MovGrp
		dw	MovGrp

		dw	center
		dw	cleave
		dw	cretf
		dw	cretf
		dw	cint3
		dw	cint
		dw	cinto
		dw	ciret

		dw	ShiftOp 		; D
		dw	ShiftOp
		dw	ShiftOp
		dw	ShiftOp
		dw	caam
		dw	caad
		dw	cdb
		dw	cxlat

		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc

		dw	cloopne 		; E
		dw	cloope
		dw	cloop
		dw	cjcxz
		dw	cin
		dw	cin
		dw	cout
		dw	cout

		dw	ccall
		dw	cjmp
		dw	cjmp
		dw	cjmp
		dw	cin
		dw	cin
		dw	cout
		dw	cout

		dw	plock 			; F
		dw	cdb
		dw	prepne
		dw	prepe
		dw	chlt
		dw	ccmc
		dw	Grp1
		dw	Grp1

		dw	cclc
		dw	cstc
		dw	ccli
		dw	csti
		dw	ccld
		dw	cstd
		dw	Grp2
		dw	Grp3

 Grp_commands	label	word
 OpArOp1	dw	cadd, cor, cadc, csbb, cand, csub, cxor,ccmp
 OpArOp2	dw	cadd, cor, cadc, csbb, cand, csub, cxor,ccmp
 OpShiftOp	dw	crol, cror,crcl, crcr, cshl, cshr, qqq, csar
 OpGrp1		dw	ctest,qqq, cnot, cneg, cmul, cimul,cdiv,cidiv
 OpGrp2		dw	cinc, cdec,ccall,ccall,cjmp, cjmp, cpush,qqq
 OpGrp3		dw	cinc, cdec,ccall,ccall,cjmp, cjmp, cpush,qqq
 OpMovGrp	dw	cmov, qqq, qqq,  qqq,  qqq,  qqq,  qqq,  qqq


 ano		equ	00h
 aemp		equ	3Fh
 arm8		equ	01h
 arm16		equ	81h

 aim8		equ	03h
 aim16		equ	83h
 amem8		equ	04h
 amem16		equ	84h
 a1		equ	05h
 asht		equ	06h
 anear		equ	07h
 afar		equ	08h
;aseg		equ	09h
 arm8r		equ	0Ah
 arm16r		equ	8Ah
 ar8rm		equ	0Bh
 ar16rm		equ	8Bh
 arm_seg	equ	0Ch
 aseg_rm	equ	0Dh
 aesc		equ	0Eh
 aim8_16	equ	0Fh
 ar8_16		equ	10h
 ar16_32	equ	11h
 arm16r8	equ	12h
 r_cr		equ	13h
 cr_r		equ	14h
 r_dr		equ	15h
 dr_r		equ	16h
 r_tr		equ	17h
 tr_r		equ	18h
 astr		equ	19h
 astrn		equ	1Ah

 aax		equ	40h
 acx		equ	41h
 adx		equ	42h
 abx		equ	43h
 asp		equ	44h
 abp		equ	45h
 asi		equ	46h
 adi		equ	47h

 aal		equ	48h
 acl		equ	49h
 adl		equ	4Ah
 abl		equ	4Bh
 aah		equ	4Ch
 ach		equ	4Dh
 adh		equ	4Eh
 abh		equ	4Fh

 aes		equ	50h
 acs		equ	51h
 ass		equ	52h
 ads		equ	53h
 afs		equ	54h
 ags		equ	55h

 a86		equ	70h
 a286		equ	71h
 a386		equ	72h
 a486		equ	73h
 aprot		equ	7Fh

Table286param	label	word

$nopar		equ	0
		db	 0,	 0
$bound		equ	1
		db	ar16rm,	aemp
$push16		equ	2
		db	aim16,	 0
$push8		equ	3
		db	aim8, 0
$imul16		equ	4
		db	ar16rm,	aim16
$imul8		equ	5
		db	ar16rm,	aim8
$enter		equ	6
		db	aim16,	aim8
$shift8		equ	7
		db	arm8,	aim8
$shift16	equ	8
		db	arm16,	aim8
$arpl		equ	9
		db	arm16r,	aemp
$strn		equ	10
		db	astrn, ano


Cmd_parameters label	word
		db	arm8r,aemp 		; 0
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	aes,ano
		db	aes,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	acs,ano
		db	aprot,ano

		db	arm8r,aemp 		; 1
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ass,ano
		db	ass,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ads,ano
		db	ads,ano

		db	arm8r,aemp 		; 2
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	arm8r,aemp 		; 3
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	aax,ano 		; 4
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	asp,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	aax,ano
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	asp,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	aax,ano 		; 5
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	asp,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	aax,ano
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	asp,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	a286,$nopar 		; 6
		db	a286,$nopar
		db	a286,$bound
		db	a286,$arpl
		db	a386,ano
		db	a386,ano
		db	a386,ano
		db	a386,ano

		db	a286,$push16
		db	a286,$imul16
		db	a286,$push8
		db	a286,$imul8
		db	a286,$strn
		db	a286,$strn
		db	a286,$strn
		db	a286,$strn

		db	asht,ano 		; 7
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano

		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano

		db	arm8,aim8 		; 8
		db	arm16,aim16	; 8, 16
		db	arm8,aim8
		db	arm16,aim8_16	;;
		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	arm_seg,aemp
		db	ar16rm,aemp
		db	aseg_rm,aemp
		db	arm16,ano

		db	ano,ano 		; 9
		db	aax,acx
		db	aax,adx
		db	aax,abx
		db	aax,asp
		db	aax,abp
		db	aax,asi
		db	aax,adi

		db	ano,ano
		db	ano,ano
		db	afar,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano

		db	aal,amem8 		; A
		db	aax,amem16
		db	amem8,aal
		db	amem16,aax
		db	astr,ano
		db	astr,ano
		db	astr,ano
		db	astr,ano

		db	aal,aim8
		db	aax,aim16
		db	astrn,ano
		db	astrn,ano
		db	astr,ano
		db	astr,ano
		db	astrn,ano
		db	astrn,ano

		db	aal,aim8 		; B
		db	acl,aim8
		db	adl,aim8
		db	abl,aim8
		db	aah,aim8
		db	ach,aim8
		db	adh,aim8
		db	abh,aim8

		db	aax,aim16
		db	acx,aim16
		db	adx,aim16
		db	abx,aim16
		db	asp,aim16
		db	abp,aim16
		db	asi,aim16
		db	adi,aim16

		db	a286,$shift8 		; C
		db	a286,$shift16
		db	aim16,ano
		db	ano,ano
		db	ar16rm,aemp
		db	ar16rm,aemp
		db	arm8,aim8
		db	arm16,aim16

		db	a286,$enter
		db	a286,$nopar
		db	aim16,ano
		db	ano,ano
		db	ano,ano
		db	aim8,ano
		db	ano,ano
		db	ano,ano

		db	arm8,a1 		; D
		db	arm16,a1
		db	arm8,acl
		db	arm16,acl
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano

		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp

		db	asht,ano 		; E
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	aal,aim8
		db	aax,aim8
		db	aim8,aal
		db	aim8,aax

		db	anear,ano
		db	anear,ano
		db	afar,ano
		db	asht,ano
		db	aal,adx
		db	aax,adx
		db	adx,aal
		db	adx,aax

		db	ano,ano 		; F
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	arm8,ano
		db	arm16,ano

		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	arm8,ano
		db	arm16,ano

ifndef __ACT__
Buffer		db	BUF_SIZE dup (' ')
		db	-1
endif

endif

ifdef _CommandLength

CommandLength	proc
		ret
endp

LengthTable	label	byte
		db	-1, -1, -1, -1, -1, -1, 1, 1	; 0
		db	-1, -1, -1, -1, -1, -1, 1, 1
		db	-1, -1, -1, -1, -1, -1, 1, 1	; 1
		db	-1, -1, -1, -1, -1, -1, 1, 1

		db	-1, -1, -1, -1, -1, -1, 0, 1	; 2
		db	-1, -1, -1, -1, -1, -1, 0, 1
		db	-1, -1, -1, -1, -1, -1, 0, 1	; 3
		db	-1, -1, -1, -1, -1, -1, 0, 1

		db	1, 1, 1, 1, 1, 1, 1, 1		; 4
		db	1, 1, 1, 1, 1, 1, 1, 1
		db	1, 1, 1, 1, 1, 1, 1, 1		; 5
		db	1, 1, 1, 1, 1, 1, 1, 1

		db	1, 1, 1, 1, 1, 1, 1, 1		; 6
		db	1, 1, 1, 1, 1, 1, 1, 1

		db	2, 2, 2, 2, 2, 2, 2, 2		; 7
		db	2, 2, 2, 2, 2, 2, 2, 2

		db	-2, -2, -2, -2, -1, -1, -1, -1	; 8
		db	-1, -1, -1, -1, -1, -1, -1, -1

		db	1, 1, 1, 1, 1, 1, 1, 1		; 9
		db	1, 1, 5, 1, 1, 1, 1, 1

		db	3, 3, 3, 3, 1, 1, 1, 1		; A
		db	-2, -2, 1, 1, 1, 1, 1, 1

		db	2, 2, 2, 2, 2, 2, 2, 2		; B
		db	3, 3, 3, 3, 3, 3, 3, 3

		db	1, 1, 3, 1, -1, -1, -1, -1	; C
		db	1, 1, 3, 1, 1, 2, 1, 1

		db	-1, -1, -1, -1, 2, 2, 1, 1	; D
		db	-1, -1, -1, -1, -1, -1, -1, -1

		db	2, 2, 2, 2, 2, 2, 2, 2		; E
		db	3, 3, 5, 2, 1, 1, 1, 1

		db	0, 1, 0, 0, 1, 1, 81h, 81h	; F
		db	1, 1, 1, 1, 1, 1, 82h, 83h

LengthTable2	label	byte
		db	1, 2, 3, 1

endif

