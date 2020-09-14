;===============================================================================
;==  Commander X16 : Constants  ================================================
;===============================================================================
; Copyright 2020 Andrew Moore
;
; This Source Code Form is subject to the terms of the Mozilla Public
; License, v. 2.0. If a copy of the MPL was not distributed with this
; file, You can obtain one at https://mozilla.org/MPL/2.0/.
;
;===============================================================================

.pc02

.scope X16

;-------------------------------------------------------------------------------
; 16-bit registers
;-------------------------------------------------------------------------------
	r0	:= $02
	r0L	:= r0
	r0H	:= r0 + 1
	r1	:= $04
	r1L	:= r1
	r1H	:= r1 + 1
	r2	:= $06
	r2L	:= r2
	r2H	:= r2 + 1
	r3	:= $08
	r3L	:= r3
	r3H	:= r3 + 1
	r4	:= $0a
	r4L	:= r4
	r4H	:= r4 + 1
	r5	:= $0c
	r5L	:= r5
	r5H	:= r5 + 1
	r6	:= $0e
	r6L	:= r6
	r6H	:= r6 + 1
	r7	:= $10
	r7L	:= r7
	r7H	:= r7 + 1
	r8	:= $12
	r8L	:= r8
	r8H	:= r8 + 1
	r9	:= $14
	r9L	:= r9
	r9H	:= r9 + 1
	ra	:= $16
	raL	:= ra
	raH	:= ra + 1
	rb	:= $18
	rbL	:= rb
	rbH	:= rb + 1
	rc	:= $1a
	rcL	:= rc
	rcH	:= rc + 1
	rd	:= $1c
	rdL	:= rd
	rdH	:= rd + 1
	re	:= $1e
	reL	:= re
	reH	:= re + 1
	rf	:= $20
	rfL	:= rf
	rfH	:= rf + 1
	r10	:= ra
	r10L	:= r10
	r10H	:= r10 + 1
	r11	:= rb
	r11L	:= r11
	r11H	:= r11 + 1
	r12	:= rc
	r12L	:= r12
	r12H	:= r12 + 1
	r13	:= rd
	r13L	:= r13
	r13H	:= r13 + 1
	r14	:= re
	r14L	:= r14
	r14H	:= r14 + 1
	r15	:= rf
	r15L	:= r15
	r15H	:= r15 + 1

;-------------------------------------------------------------------------------
; Text Colors
;-------------------------------------------------------------------------------
.scope Color
	black		= $00	; Can also be transparent
	white		= $10
	red		= $20
	cyan		= $30
	purple		= $40
	green		= $50
	blue		= $60
	yellow		= $70
	orange		= $80
	brown		= $90
	pink		= $a0
	gray_dark	= $b0
	gray		= $c0
	green_light	= $d0
	blue_light	= $e0
	gray_light	= $f0
.endscope

;-------------------------------------------------------------------------------
; Kernal Subroutine Jump-Table
;-------------------------------------------------------------------------------
.scope Kernal
	; Short Names
	ACPTR	:= $ffa5
	CHKIN	:= $ffc6
	CHKOUT	:= $ffc9
	CHRIN	:= $ffcf
	CHROUT	:= $ffd2
	CINT	:= $ff81
	CIOUT	:= $ffa8
	CLALL	:= $ffe7
	CLOSE	:= $ffc3
	CLRCHN	:= $ffcc
	GETIN	:= $ffe4
	IOBASE	:= $fff3
	IOINIT	:= $ff84
	LISTEN	:= $ffb1
	LOAD	:= $ffd5
	MEMBOT	:= $ff9c
	MEMTOP	:= $ff99
	OPEN	:= $ffc0
	PLOT	:= $fff0
	RAMTAS	:= $ff87
	RDTIM	:= $ffde
	READST	:= $ffb7
	RESTOR	:= $ff8a
	SAVE	:= $ffd8
	SCNKEY	:= $ff9f
	SCREEN	:= $ffed
	SECOND	:= $ff93
	SETLFS	:= $ffba
	SETMSG	:= $ff90
	SETNAM	:= $ffbd
	SETTIM	:= $ffdb
	SETTMO	:= $ffa2
	STOP	:= $ffe1
	TALK	:= $ffb4
	TKSA	:= $ff96
	UDTIM	:= $ffea
	UNLSN	:= $ffae
	UNTLK	:= $ffab
	VECTOR	:= $ff8d

	; Long Names
	serial_bus_read		:= ACPTR	; Read byte from the serial bus
	channel_open_to_read	:= CHKIN	; Open channel for reading
	channel_open_to_write	:= CHKOUT	; Open channel for writing
	channel_read		:= CHRIN	; Read byte from a channel
	channel_write		:= CHROUT	; Write byte to a channel
	serial_bus_write	:= CIOUT	; Write byte to the serial bus
	file_configure		:= SETLFS	; Setup a logical file
	file_set_name		:= SETNAM	; Set the file name
	file_load		:= LOAD		; Load the file
	character_read		:= GETIN	; Read character from channel

	; X16 Unique
	memory_fill		:= $fee4
	memory_copy		:= $fee7
.endscope

.endscope ; X16


;===============================================================================
;==  Commander X16 : Macros  ===================================================
;===============================================================================

;-------------------------------------------------------------------------------
; Tell the KERNAL to load a file from disk
; 
; If there was an error, the Carry Bit will be set and Register A will have the
; error code.  Common errors are:
;	$00	Run/Stop was pressed during loading
;	$04	File not found
; 	$05	Device not found
;	$1d	Load error
;-------------------------------------------------------------------------------
.macro	X16_File_Load_To_Addr filename_address, filename_length, ram_address
	;---------------------------------------
	; Set the file name
	.scope
		lda	filename_length
		ldx	#<filename_address
		ldy	#>filename_address
		jsr	X16::Kernal::file_set_name
	.endscope

	;---------------------------------------
	; Setup the logical file to use
	.scope
		;lda	#$01	; Loading doesn't need a file number
		ldx	#$08	; Device 8 = Disk Drive
		ldy	#$00	; Secondary address of 0
				; Address will be given during the load
		jsr	X16::Kernal::file_configure
	.endscope

	;---------------------------------------
	; Load the file
	.scope
		lda	#$00	; load to memory (1 = verify)
		ldx	#<ram_address
		ldy	#>ram_address
		jsr	X16::Kernal::file_load
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Tell the KERNAL to load a file from disk
; 
; The first two bytes in the file will be used to specify the target address.
; 
; If there was an error, the Carry Bit will be set and Register A will have the
; error code.  Common errors are:
;	$00	Run/Stop was pressed during loading
;	$04	File not found
; 	$05	Device not found
;	$1d	Load error
;-------------------------------------------------------------------------------
.macro	X16_File_Load_To_RAM	filename_address, filename_length
	;---------------------------------------
	; Set the file name
	.scope
		lda	filename_length
		ldx	#<filename_address
		ldy	#>filename_address
		jsr	X16::Kernal::file_set_name
	.endscope

	;---------------------------------------
	; Setup the logical file to use
	.scope
		;lda	#$01	; Loading doesn't need a file number
		ldx	#$08	; Default to Device 8
		ldy	#$01	; Address in first 2 bytes of file
		jsr	X16::Kernal::file_configure
	.endscope

	;---------------------------------------
	; Load the file
	.scope
		lda	#$00	; load to memory
		jsr	X16::Kernal::file_load
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Tell the KERNAL to load a file from disk
; 
; If there was an error, the Carry Bit will be set and Register A will have the
; error code.  Common errors are:
;	$00	Run/Stop was pressed during loading
;	$04	File not found
; 	$05	Device not found
;	$1d	Load error
;-------------------------------------------------------------------------------
.macro	X16_File_Load_To_VRAM0	filename_address, filename_length
	;---------------------------------------
	; Set the file name
	.scope
		lda	filename_length
		ldx	#<filename_address
		ldy	#>filename_address
		jsr	X16::Kernal::file_set_name
	.endscope

	;---------------------------------------
	; Setup the logical file to use
	.scope
		;lda	#$01	; Loading doesn't need a file number
		ldx	#$08	; Default to Device 8
		ldy	#$01	; Address: $0.$file_byte_1.$file_byte_0
		jsr	X16::Kernal::file_configure
	.endscope

	;---------------------------------------
	; Load the file
	.scope
		lda	#$02	; load into low VRAM
		jsr	X16::Kernal::file_load
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Tell the KERNAL to load a file from disk
; 
; If there was an error, the Carry Bit will be set and Register A will have the
; error code.  Common errors are:
;	$00	Run/Stop was pressed during loading
;	$04	File not found
; 	$05	Device not found
;	$1d	Load error
;-------------------------------------------------------------------------------
.macro X16_File_Load_To_VRAM1 filename_address, filename_length
	;---------------------------------------
	; Set the file name
	.scope
		lda	filename_length
		ldx	#<filename_address
		ldy	#>filename_address
		jsr	X16::Kernal::file_set_name
	.endscope

	;---------------------------------------
	; Setup the logical file to use
	.scope
		;lda	#$01	; Loading doesn't need a file number
		ldx	#$08	; Default to Device 8
		ldy	#$01	; Address: $1.$file_byte_1.$file_byte_0
		jsr	X16::Kernal::file_configure
	.endscope

	;---------------------------------------
	; Load the file
	.scope
		lda	#$03	; load to into high VRAM
		jsr	X16::Kernal::file_load
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; A very simple and stupid Random Number Generator.
;
; If the same seed is used with the same value for the Carry Flag, the sequence
; will always result in the same series of numbers.  The result will be stored
; in Register A.
;-------------------------------------------------------------------------------
; Parameters
;   seed - The random number seed
;-------------------------------------------------------------------------------
; Registers
;            | Input       | Output
; -----------|-------------|------------
;      A     | Scratch Pad | The result
;      X     | Unused      | Unused
;      Y     | Unused      | Unused
;-------------------------------------------------------------------------------
; Stack
;   Unused
;-------------------------------------------------------------------------------
.macro	X16_Rng	seed
	.scope
		lda	seed
		rol
		rol
		rol
		adc	#1
		eor	seed
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Tell the X16 to reset.
;
; This marco does not end/return.
;-------------------------------------------------------------------------------
; Parameters
;   None
;-------------------------------------------------------------------------------
; Registers
;            | Input       | Output
; -----------|-------------|-------------
;      A     | Scratch Pad | Scratch Pad
;      X     | Unused      | Unused
;      Y     | Unused      | Unused
;-------------------------------------------------------------------------------
; Stack
;   Unused
;-------------------------------------------------------------------------------
.macro	X16_Soft_Reset
	.scope
		stz	$9f60
		jmp	($fffc)
	.endscope
.endmacro


.macro X16_Wipe_And_Reset
	.scope
	wipe_and_reset:
		ldx	#(wipe_and_reset_end - wipe_and_reset_data - 1)
	wipe_and_reset_copy:
		lda	wipe_and_reset_data,x
		sta	$0100,x
		dex
		bne	wipe_and_reset_copy
		lda	wipe_and_reset_data,x
		sta	$0100,x
		jmp	$0100
	wipe_and_reset_data:
		stz	X16::r0L
		lda	#$04
		sta	X16::r0H
		lda	#$9a
		sta	X16::r1H
		lda	#$ff
		sta	X16::r1L
		ina
		jsr	X16::Kernal::memory_fill
		Vera_Reset
		X16_Soft_Reset
	wipe_and_reset_end:
	.endscope
.endmacro


;===============================================================================
;==  Commander X16 : Includes  =================================================
;===============================================================================

.include "Zakero_X16_Vera.inc"


;-------------------------------------------------------------------------------
; vi:syntax=asm
