;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006, 2007).
;-----------------------------------------------------------------------------
; insight.inc
; Constants.
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

%define			SERIAL_NUMBER	"Insight 1.20"
%define			BUILD		"2007-01-01"

%define			WINDOW_BUF_SIZE	2800
%define			STRING_BUF_SIZE	(WINDOW_BUF_SIZE - 256 * 3)

;No more needed
%define			PaletteBuffer	(window_buffer + STRING_BUF_SIZE)

%define			follow_stack	PaletteBuffer

SCRCOLS			equ	80
SCRROWS			equ	25

SEARCH_FILL_LEN		equ	50
FILENAME_LEN		equ	80

STACK_SIZE		equ	800h
MAX_PROGRAM_SIZE	equ	65000

;~MaxIniFileBuff		equ	(200+2)

NUM_FLAGS		equ	8

NUM_16_REGS		equ	14
NUM_32_REGS		equ	24

MAX_BREAKPOINTS		equ	10	;Number of breakpoints
MAX_ITEMS		equ	32	;Number of ???
                	
;~RPTlength		equ	14

%define			CMD_POS	21

;~NameStrSize		equ	64
;~NameStrLen		equ	100
MAX_FILENAME		equ	150	;??? Not used ???
ADDRESS_STRING_LENGTH	equ	24
                	
CPU_ADDR_SIZE		equ	6
CPU_OPCODE_SIZE		equ     15
                	
CPU_X			equ	0
CPU_Y			equ	0
CPU_START		equ	CPU_Y*160+CPU_X*2
CMD_START		equ	(CPU_Y+1)*160+CPU_X*2
CPU_WIDTH		equ	54
CPU_HEIGHT		equ	17

REG_X			equ	CPU_X+CPU_WIDTH+1
REG_Y			equ	0
REG_START		equ	REG_Y*160+REG_X*2

DUMP_HEIGHT		equ	5
DUMP_X			equ	0
DUMP_Y			equ	25-DUMP_HEIGHT
DUMP_START		equ	DUMP_Y*160+DUMP_X*2

STK16_X			equ	REG_X+1
STK16_Y			equ	10
STK16_LEN		equ	8
STK16_START		equ	(STK16_Y+STK16_LEN-1)*160+(STK16_X+7)*2

STK32_X			equ	REG_X+1
STK32_Y			equ	13
STK32_LEN		equ	5
STK32_START		equ	(STK32_Y+STK32_LEN-1)*160+(STK32_X+7)*2

FLAG16_X		equ	REG_X
FLAG16_Y		equ	(STK16_Y-3)
FLAG16_START		equ	FLAG16_Y*160+FLAG16_X*2

FLAG32_X		equ	REG_X
FLAG32_Y		equ	(STK32_Y-3)
FLAG32_START		equ	FLAG32_Y*160+FLAG32_X*2
                	
MEM_START		equ	(CPU_Y+CPU_HEIGHT+1)*160+40
WATCH_START		equ	(CPU_Y+CPU_HEIGHT+1)*160+40+42+8
                	
MAX_CUR_X		equ	80-CPU_WIDTH
ASM_LINE_SIZE		equ	CPU_WIDTH-1-CMD_POS
MAX_ASM_SIZE		equ	80-CMD_POS
ADDR_STR_SIZE		equ	24

SCAN_CODE		equ	1
SHIFT_CODE		equ	4

MAGIC_REQUEST_CODE	equ	0C0DEh
DEVELOPER_ID 		equ	0FACEh
APPLICATION_ID		equ	1
DEBUGGER_VERSION	equ	0103h
MAGIC_ANSWER_CODE	equ	0FADEh

;-----------------------------------------------------------------------------
BP_UNUSED		equ	0
BP_ENABLED		equ	1
BP_DISABLED		equ	2
;-----------------------------------------------------------------------------
BP_SEG			equ	0	;w
BP_OFFS			equ	2	;w
BP_BYTE			equ	4	;b
BP_SET			equ	5	;b
BP_ACTIVE		equ	6	;b

SIZEOF_BREAKPOINT	equ	7	;Size of breakpoint structure
;-----------------------------------------------------------------------------
AP_REG_NUMBER		equ	0	;b, -1
AP_VALUE		equ	1	;w, 0
AP_MEM_FLAG		equ	3	;b, 0
AP_BX_FLAG		equ	4	;b, 0
AP_BP_FLAG		equ	5	;b, 0
AP_SI_FLAG		equ	6	;b, 0
AP_DI_FLAG		equ	7	;b, 0
AP_WORD_FLAG		equ	8	;b, 0

SIZEOF_ASM_PARAM	equ	9
;-----------------------------------------------------------------------------
REG_POS_X		equ	0	;b \ register
REG_POS_Y		equ	1	;b / position
REG_POS_LINK		equ	2	;w ptr to saved reg
REG_POS_LEFT		equ	4	;b -.
REG_POS_RIGHT		equ	5	;b  | distance between this reg. struc
REG_POS_UP		equ	6	;b  | and linked struc
REG_POS_DOWN		equ	7	;b -'

SIZEOF_REG_POS		equ	8
;-----------------------------------------------------------------------------
REG_AX			equ	0
REG_BX			equ	4
REG_CX			equ	8
REG_DX			equ	12
;
REG_SI			equ	16
REG_DI			equ	20
REG_BP			equ	24
REG_SP			equ	28
;
REG_CS			equ	32
REG_DS			equ	36
REG_ES			equ	40
REG_SS			equ	44
;
REG_FS			equ	48
REG_GS			equ	52
REG_IP			equ	56
REG_FL			equ	60

SIZEOF_REGISTERS_32	equ	64
;-----------------------------------------------------------------------------

DOS_GET_RETURN_CODE	equ	4dh

;=============================================================================

; Keyboard constants

kbF1		equ	3B00h
kbF2		equ	3C00h
kbF3		equ	3D00h
kbF4		equ	3E00h
kbF5		equ	3F00h
kbF6		equ	4000h
kbF7		equ	4100h
kbF8		equ	4200h
kbF9		equ	4300h
kbF10		equ	4400h

kbCtrlF1	equ	5E00h
kbCtrlF2	equ	5F00h
kbCtrlF3	equ	6000h
kbCtrlF4	equ	6100h
kbCtrlF5	equ	6200h
kbCtrlF6	equ	6300h
kbCtrlF7	equ	6400h
kbCtrlF8	equ	6500h
kbCtrlF9	equ	6600h
kbCtrlF10	equ	6700h

kbAltF1		equ	6800h
kbAltF2		equ	6900h
kbAltF3		equ	6A00h
kbAltF4		equ	6B00h
kbAltF5		equ	6C00h
kbAltF6		equ	6D00h
kbAltF7		equ	6E00h
kbAltF8		equ	6F00h
kbAltF9		equ	7000h
kbAltF10	equ	7100h

kbShiftF1	equ	5400h
kbShiftF2	equ	5500h
kbShiftF3	equ	5600h
kbShiftF4	equ	5700h
kbShiftF5	equ	5800h
kbShiftF6	equ	5800h
kbShiftF7	equ	5A00h
kbShiftF8	equ	5B00h
kbShiftF9	equ	5C00h
kbShiftF10	equ	5D00h

kbAltEqu	equ	8300h

kbAlt1		equ	7800h
kbAlt2		equ	7900h
kbAlt3		equ	7A00h
kbAlt4		equ	7B00h
kbAlt5		equ	7C00h
kbAlt6		equ	7D00h
kbAlt7		equ	7E00h
kbAlt8		equ	7F00h
kbAlt9		equ	8000h
kbAlt0		equ	8100h

kbAltQ		equ	1000h
kbAltW		equ	1100h
kbAltE		equ	1200h
kbAltR		equ	1300h
kbAltT		equ	1400h
kbAltY		equ	1500h
kbAltU		equ	1600h
kbAltI		equ	1700h
kbAltO		equ	1800h
kbAltP		equ	1900h

kbAltA		equ	1E00h
kbAltS		equ	1F00h
kbAltD		equ	2000h
kbAltF		equ	2100h
kbAltG		equ	2200h
kbAltH		equ	2300h
kbAltJ		equ	2400h
kbAltK		equ	2500h
kbAltL		equ	2600h

kbAltZ		equ	2C00h
kbAltX		equ	2D00h
kbAltC		equ	2E00h
kbAltV		equ	2F00h
kbAltB		equ	3000h
kbAltN		equ	3100h
kbAltM		equ	3200h

kbCtrlA		equ	1E01h
kbCtrlB		equ	3002h
kbCtrlC		equ	2E03h
kbCtrlD		equ	2004h
kbCtrlE		equ	1205h
kbCtrlF		equ	2106h
kbCtrlG		equ	2207h
kbCtrlH		equ	2308h
kbCtrlI		equ	1709h
kbCtrlJ		equ	240Ah
kbCtrlK		equ	250Bh
kbCtrlL		equ	260Ch
kbCtrlM		equ	320Dh
kbCtrlN		equ	310Eh
kbCtrlO		equ	180Fh
kbCtrlP		equ	1910h
kbCtrlQ		equ	1011h
kbCtrlR		equ	1312h
kbCtrlS		equ	1F13h
kbCtrlT		equ	1414h
kbCtrlU		equ	1615h
kbCtrlV		equ	2F16h
kbCtrlW		equ	1117h
kbCtrlX		equ	2D18h
kbCtrlY		equ	1519h
kbCtrlZ		equ	2C1Ah

kbHome		equ	4700h
kbUp		equ	4800h
kbPgUp		equ	4900h
kbLeft		equ	4B00h
kbRight		equ	4D00h
kbEnd		equ	4F00h
kbDown		equ	5000h
kbPgDn		equ	5100h

kbShiftHome	equ	4737h
kbShiftUp	equ	4838h
kbShiftPgUp	equ	4939h
kbShiftLeft	equ	4B34h
kbShiftRight	equ	4D36h
kbShiftEnd	equ	4F31h
kbShiftDown	equ	5032h
kbShiftPgDn	equ	5133h

kbIns		equ	5200h
kbDel		equ	5300h

kbEsc		equ	011Bh
kbEnter		equ	1C0Dh
kbTab		equ	0F09h
kbBackSpace	equ	0E08h
kbShiftTab	equ	0F00h
kbCtrlEnter	equ	1C0Ah
kbSpace		equ	3920h
kbGrayPlus	equ	4E2Bh
kbGrayMinus	equ	4A2Dh

kbCtrlLeft	equ	7300h
kbCtrlRight	equ	7400h

kbAltAlone	equ	0FFFEh
;=============================================================================
; Colors
;
BLACK		equ	0
BLUE		equ	1
GREEN		equ	2
CYAN		equ	3
RED		equ	4
MAGENTA		equ	5
BROWN		equ	6
LIGHT_GRAY	equ	7

DARK_GRAY	equ	8
LIGHT_BLUE	equ	9
LIGHT_GREEN	equ	10
LIGHT_CYAN	equ	11
LIGHT_RED	equ	12
LIGHT_MAGENTA	equ	13
YELLOW		equ	14
WHITE		equ	15

;-----------------------------------------------------------------------------
%macro		reg_pos	7
		db	REG_X+%1
		db	REG_Y+%2
		dw	cpu_regs+REG_%3
		db	%4
		db	%5
		db	%6
		db	%7
%endmacro
;
;-----------------------------------------------------------------------------
CMD_HELP	equ	1b
CMD_RESIDENT	equ	10b
CMD_SKIP_MOD	equ	100b
CMD_SKIP_INI	equ	1000b
CMD_UNLOAD	equ	10000b
CMD_EGA		equ	100000b
CMD_ERROR	equ	10000000b
;-----------------------------------------------------------------------------
;******** Assembler ***********
;
ER_EMPTY	equ	1
ER_MNEMONIC	equ	2
ER_INV_PARAM	equ	3
ER_MEM_PARAM	equ	4
ER_EXTRA	equ	5
ER_BIG_NUMBER	equ	6
ER_REG_BITS	equ	7
erParamType	equ	8
ER_REP_ERROR	equ	8

regAL		equ	0
regCL		equ	1
regDL		equ	2
regBL		equ	3
regAH		equ	4
regCH		equ	5
regDH		equ	6
regBH		equ	7
regAX		equ	8
regCX		equ	9
regDX		equ	10
regBX		equ	11
regSP		equ	12
regBP		equ	13
regSI		equ	14
regDI		equ	15

MAX_LETTER	equ	7
;=============================================================================
;****** CPU ********

I8086		equ	0
I8088		equ	1
I80286		equ	2
I80386		equ	3
I80486		equ	4

;=============================================================================
;******** dialogs ********

BF_DEFAULT	equ	1
CM_CANCEL	equ	0
CM_DEFAULT	equ	1
CM_OK		equ	1

;=============================================================================
;******** follow ***********
RF_FOLLOW	equ	1b
RF_JUMP		equ	10b
RF_BACKWARD	equ	100b
RF_CONDITION	equ	1000b
RF_TRUE		equ	10000b
RF_PREFIX	equ	100000b
RF_ADDRESS_32	equ	1000000b
RF_OPERAND_32	equ	10000000b

RF_32		equ	(RF_OPERAND_32+RF_ADDRESS_32)

;=============================================================================
;******* unasm ********
%define		CMD_X		CMD_POS+buffer
%define		OPER_X		CMD_POS+8+buffer
%define		BUF_SIZE	80
;=============================================================================
;******* windows *********

SUB_MENU	equ	1

;=============================================================================
SEPARATOR	equ	0
DISABLED	equ	2
;=============================================================================

COLOR_SEG	equ	0b800h

%define		DOUBLE_BORDER	double_border_struc
%define		SINGLE_BORDER	single_border_struc

;=============================================================================
; E0F
;=============================================================================


