;===============================================================================
;==  Commander X16 : Vera : Constants  =========================================
;===============================================================================
; Copyright 2020 Andrew Moore
;
; This Source Code Form is subject to the terms of the Mozilla Public
; License, v. 2.0. If a copy of the MPL was not distributed with this
; file, You can obtain one at https://mozilla.org/MPL/2.0/.
;
;===============================================================================

.pc02

.scope Vera

;-------------------------------------------------------------------------------
;--  Vera Access Map  ----------------------------------------------------------
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
; Vera Addressing
;-------------------------------------------------------------------------------
;
; The address bit format in Vera is: IIIID..HMMMMMMMMLLLLLLLL
;
; Address Low
;  Bit | Value | Meaning
; ---------------------------------------------------------------
;  7-0 | 00-ff | The Low Byte
;
; Address Mid
;  Bit | Value | Meaning
; ---------------------------------------------------------------
;  7-0 | 00-ff | The Middle Byte
;
; Address Hi
;  Bit | Value | Meaning
; ---------------------------------------------------------------
;  7-4 |  0000 | Do not modify the internal pointer
;      |  0001 |   1 Byte Modifier
;      |  0010 |   2 Byte Modifier
;      |  0011 |   4 Byte Modifier
;      |  0100 |   8 Byte Modifier
;      |  0101 |  16 Byte Modifier
;      |  0110 |  32 Byte Modifier
;      |  0111 |  64 Byte Modifier
;      |  1000 | 128 Byte Modifier
;      |  1001 | 256 Byte Modifier
;      |  1010 | 512 Byte Modifier
;      |  1011 |  40 Byte Modifier
;      |  1100 |  80 Byte Modifier
;      |  1101 | 160 Byte Modifier
;      |  1110 | 320 Byte Modifier
;      |  1111 | 640 Byte Modifier
; ---------------------------------------------------------------
;   3  |   0   | Add the modifier to the internal pointer
;      |   1   | Subtract the modifier from the internal pointer
; ---------------------------------------------------------------
;  2-1 |   -   | Unused
; ---------------------------------------------------------------
;   0  | 00-01 | The High Bit of the address
;
;-------------------------------------------------------------------------------
;
; Example:
; 	advance .set Address::Advance_80_Bytes | Vera_Address::Forward
; 	lda	#(advance | 1)
; 	ldx	#$23
; 	ldy	#$45
; 	sta	address_hi
; 	stx	address_mid
; 	sty	address_lo
;
;-------------------------------------------------------------------------------

	address_lo		:= $9f20
	address_mid		:= $9f21
	address_hi		:= $9f22

.enum	Address
	Advance_0_Bytes		= %00000000
	Advance_1_Byte		= %00010000
	Advance_2_Bytes		= %00100000
	Advance_4_Bytes		= %00110000
	Advance_8_Bytes		= %01000000
	Advance_16_Bytes	= %01010000
	Advance_32_Bytes	= %01100000
	Advance_64_Bytes	= %01110000
	Advance_128_Bytes	= %10000000
	Advance_256_Bytes	= %10010000
	Advance_512_Bytes	= %10100000
	Advance_40_Bytes	= %10110000
	Advance_80_Bytes	= %11000000
	Advance_160_Bytes	= %11010000
	Advance_320_Bytes	= %11100000
	Advance_640_Bytes	= %11110000
	Forward			= %00000000
	Backward		= %00001000
.endenum


;-------------------------------------------------------------------------------
; Vera Data Ports
;-------------------------------------------------------------------------------

	port_0		:= $9f23
	port_1		:= $9f24


;-------------------------------------------------------------------------------
; Vera Control
;-------------------------------------------------------------------------------
;
;  Bit | Value | Meaning
; --------------------------
;   7  |   0   | Ignored
;      |   1   | Reset Vera
; --------------------------
;  6-2 |   -   | Unused
; --------------------------
;   1  |   0   | Use Port 0
;      |   1   | Use Port 1
; --------------------------
;   0  |   0   | Use Address 0
;      |   1   | Use Address 1
;
;-------------------------------------------------------------------------------

	control		:= $9f25

.enum	Control
	Reset	= %10000000
.endenum


;-------------------------------------------------------------------------------
; Vera IEN
;-------------------------------------------------------------------------------

	ien		:= $9f26


;-------------------------------------------------------------------------------
; Vera ISR
;-------------------------------------------------------------------------------

	isr		:= $9f27


;-------------------------------------------------------------------------------
; Vera IRQ
;-------------------------------------------------------------------------------

	irq		:= $9f28


;-------------------------------------------------------------------------------
; Vera Video
;
;  Bit | Value | Meaning
; -----------------------
;   7  |   -   | READ-ONLY : The active interlaced field for Composite and RGBi
;   6  |   0   | Disable Sprites
;      |   1   | Enable Sprites
;   5  |   0   | Disable Layer 1
;      |   1   | Enable Layer 1
;   4  |   0   | Disable Layer 0
;      |   1   | Enable Layer 0
;   3  |   -   | Unused
;   2  |   0   | Enable Chroma
;      |   1   | Disable Chroma
;  1-0 |   00  | Video Disabled
;      |   01  | VGA Output
;      |   10  | NTSC Output
;      |   11  | RGBi (RGB interlaced) - Uses the VGA connector
;
;-------------------------------------------------------------------------------
;
; Example:
;	config .set Video::Video_VGA
;	config .set Video::Chroma_Enable	| config
;	config .set Video::Layer0_Enable	| config
;	config .set Video::Layer1_Enable	| config
;	config .set Video::Sprite_Enable	| config
;	lda	#config
;	sta	video
;
;-------------------------------------------------------------------------------

	video		:= $9f29

.enum	Video
	Chroma_Disable	= %00000100
	Chroma_Enable	= %00000000
	Layer0_Disable	= %00000000
	Layer0_Enable	= %00010000
	Layer1_Disable	= %00000000
	Layer1_Enable	= %00100000
	Sprite_Disable	= %00000000
	Sprite_Enable	= %01000000
	Video_Disable	= %00000000
	Video_NTSC	= %00000010
	Video_VGA	= %00000001
	Video_RGBi	= %00000011
.endenum


;-------------------------------------------------------------------------------
; Vera Scaling
;-------------------------------------------------------------------------------

	scale_h		:= $9f2a
	scale_v		:= $9f2b

.enum	Scale
	x1	= 128	; $80
	x2	= 64	; $40
	x4	= 32	; $20
	x8	= 16	; $10
	x16	= 8	; $08
	x32	= 4	; $04
	x64	= 2	; $02
	x128	= 1	; $01
.endenum


;-------------------------------------------------------------------------------
; Vera Border Color
;-------------------------------------------------------------------------------

	border_color	:= $9f2c


;-------------------------------------------------------------------------------
; Vera Layer Configuration
;
;  Bit | value | Meaning
; --------------------------------------------------------------
;  7-6 |   00  | Map Height of 32 tiles
;      |   01  | Map Height of 64 tiles
;      |   10  | Map Height of 128 tiles
;      |   11  | Map Height of 256 tiles
; --------------------------------------------------------------
;  5-4 |   00  | Map Width of 32 tiles
;      |   01  | Map Width of 64 tiles
;      |   10  | Map Width of 128 tiles
;      |   11  | Map Width of 256 tiles
; --------------------------------------------------------------
;   3  |   0   | Use 256 colors per tile
;      |   1   | Tiles have 16 foreground and background colors
; --------------------------------------------------------------
;   2  |   0   | Tilemap Mode
;      |   1   | Bitmap Mode
; --------------------------------------------------------------
;  1-0 |   00  | Color Depth of 1 bit per pixel
;      |   01  | Color Depth of 2 bits per pixel
;      |   10  | Color Depth of 4 bits per pixel
;      |   11  | Color Depth of 8 bits per pixel
;
;-------------------------------------------------------------------------------
;
; Example:
;
;	config .set Layer_Config::Mode_Tilemap
;	config .set Layer_Config::Map_Width_64_Tiles		| config
;	config .set Layer_Config::Map_Height_64_Tiles		| config
;	config .set Layer_Config::Enable_256_Color_Mode		| config
;	config .set Layer_Config::Color_8_Bits_Per_Pixel	| config
;	lda	#config
;	sta	layer0_config
;
;-------------------------------------------------------------------------------

	layer0_config	:= $9f2d
	layer1_config	:= $9f34

.enum	Layer_Config
	Map_Width_32_Tiles	= %00000000
	Map_Width_64_Tiles	= %00010000
	Map_Width_128_Tiles	= %00100000
	Map_Width_256_Tiles	= %00110000
	Map_Height_32_Tiles	= %00000000
	Map_Height_64_Tiles	= %01000000
	Map_Height_128_Tiles	= %10000000
	Map_Height_256_Tiles	= %11000000
	Enable_256_Color_Mode	= %00001000
	Mode_Bitmap		= %00000100
	Mode_Tilemap		= %00000000
	Color_1_Bit_Per_Pixel	= %00000000
	Color_2_Bits_Per_Pixel	= %00000001
	Color_4_Bits_Per_Pixel	= %00000010
	Color_8_Bits_Per_Pixel	= %00000011
.endenum


;-------------------------------------------------------------------------------
; Vera Layer Mapbase
;
; The mapbase holds the top 8 bits (16:9) of the memory address.  This allows a
; maximum addressable area of $1fe00.  However, The top of Vera's addressable
; space is $1f9bf.  Combining these two factors means that the largest directly
; accessable address is $1f800.
;-------------------------------------------------------------------------------

	layer0_mapbase		:= $9f2e


;-------------------------------------------------------------------------------
; Vera Layer Mapbase

	layer0_tilebase		:= $9f2f

.enum	Tilebase
	Tile_Width_8_Pixels	= %00000000
	Tile_Width_16_Pixels	= %00000001
	Tile_Height_8_Pixels	= %00000000
	Tile_Height_16_Pixels	= %00000010
.endenum

;-------------------------------------------------------------------------------
	layer0_h_scroll_lo	:= $9f30
	layer0_h_scroll_hi	:= $9f31
	layer0_v_scroll_lo	:= $9f32
	layer0_v_scroll_hi	:= $9f33

	layer1_mapbase		:= $9f35
	layer1_tilebase		:= $9f36
	layer1_h_scroll_lo	:= $9f37
	layer1_h_scroll_hi	:= $9f38
	layer1_v_scroll_lo	:= $9f39
	layer1_v_scroll_hi	:= $9f3a

.endscope ; Vera


;===============================================================================
;==  Commander X16 : Vera : Macros  ============================================
;===============================================================================

;-------------------------------------------------------------------------------
; Calculate the X offset address
;
; From the source address (src_hi, src_mid, src_lo), calculate the address that
; is X pixels to the right.  The resulting address will be placed in dst_hi,
; dst_mid, dst_lo.
; 
; There are no bounds checks in place, so it is possible to receive an invalid
; address.
;-------------------------------------------------------------------------------
; Parameters
;   x_offset - The distance to the right (8-bit)
;   src_hi --- The high address bit of the source Vera memory location
;   src_mid -- The middle address byte of the source Vera memory location
;   src_lo --- The low address byte of the source Vera memory location
;   dst_hi --- The high address bit of the destination Vera memory location
;   dst_mid -- The middle address byte of the destination Vera memory location
;   dst_lo --- The low address byte of the destination Vera memory location
;-------------------------------------------------------------------------------
;  Reg | Start       | End
; -----|-------------|-------------
;   A  | Scratch Pad | Scratch Pad
;   X  | Not Used    | Not Used
;   Y  | Not Used    | Not Used
;-------------------------------------------------------------------------------
; Stack
;   Not Used
;-------------------------------------------------------------------------------
.macro	Vera_Bitmap_Addr_X	x_offset, src_hi, src_mid, src_lo, dst_hi, dst_mid, dst_lo
	;---------------------------------------
	; Initialization
	.scope
		clc
	.endscope

	;---------------------------------------
	; Calculate the low address byte
	.scope
		lda	src_lo
		adc	x_offset
		sta	dst_lo
	.endscope

	;---------------------------------------
	; Calculate the middle address byte
	.scope
		lda	src_mid
		adc	#$00
		sta	dst_mid
	.endscope

	;---------------------------------------
	; Calculate the high address byte
	.scope
		lda	src_hi
		adc	#$00
		sta	dst_hi
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Calculate the Y offset address
;
; 320x240 Resolution ONLY
; 
; From the source address (src_hi, src_mid, src_lo), calculate the address that
; is Y pixels down.  The resulting address will be placed in dst_hi, dst_mid,
; dst_lo.
;
; This is a VERY Cycle expensive macro!
;
; There are no bounds checks in place, so it is possible to receive an invalid
; address.
;-------------------------------------------------------------------------------
; Parameters
;   y_offset - The distance down
;   src_hi --- The high address bit of the source Vera memory location
;   src_mid -- The middle address byte of the source Vera memory location
;   src_lo --- The low address byte of the source Vera memory location
;   dst_hi --- The high address bit of the destination Vera memory location
;   dst_mid -- The middle address byte of the destination Vera memory location
;   dst_lo --- The low address byte of the destination Vera memory location
;-------------------------------------------------------------------------------
; Registers
;      | Start   | End
; -----|---------|----------
;   A  | Scratch | Scratch
;   X  | Scratch | Scratch
;   Y  | Scratch | Scratch
;-------------------------------------------------------------------------------
; Stack
;   Not Used
;-------------------------------------------------------------------------------
; This macro uses the math side-effects of incrementing by 320 ($140) to do as
; much precalculating as possible.  Below is a description of the process:
; - Separate the incrementing value of 320 ($140) into 2 bytes
;   - $01 for the middle byte
;   - $40 for the low byte
; - Middle Byte Increment is  M(y) = y * $01
; - Low Byte Increment is L(y) = y * $40
; - However, every 4 groups of $40 is actually an increment of the middle byte
;   - M(y) = (y * $01) + (y >> 2) = y + (y >> 2)
;   - L(y) = $40 * (y & $03)
;
; Proofs:
; y_offset = $01
; mid = M($01) = $01 + ($01 >> $02) = $01 + $00 = $01
; lo  = L($01) = $40 * ($01 & $03)  = $40 * $01 = $40
;
; y_offset = $04
; mid = M($04) = $04 + ($04 >> $02) = $04 + $01 = $05
; lo  = L($04) = $40 * ($04 & $03)  = $40 * $00 = $00
;
; y_offset = $05
; mid = M($05) = $05 + ($05 >> $02) = $05 + $01 = $06
; lo  = L($05) = $40 * ($05 & $03)  = $40 * $01 = $40
;
; y_offset = $ef (239 is the maximum Y Offset from 0)
; mid = M($ef) = $ef + ($ef >> $02) = $ef + $3b = $2A (Plus a Carry Bit)
; lo  = L($ef) = $40 * ($ef & $03)  = $40 * $03 = $C0
;-------------------------------------------------------------------------------
.macro	Vera_Bitmap_Addr_Y	y_offset, src_hi, src_mid, src_lo, dst_hi, dst_mid, dst_lo
	;---------------------------------------
	; Initialization
	.scope
		clc
		ldx	#$00
	.endscope

	;---------------------------------------
	; Calculate the low address byte
	.scope
		lda	y_offset
		and	#$03
		beq	addr_offset_ready

		; Register A is either 1, 2, or 3
		; $40 = $40 * 1 = 1 << 6
		; $80 = $40 * 2 = 2 << 6
		; $C0 = $40 * 3 = 3 << 6
		asl
		asl
		asl
		asl
		asl
		asl

	addr_offset_ready:
		adc	src_lo
		sta	dst_lo
		; Save the Carry Bit in Register 'Y' for the Middle Byte
		lda	#$00
		adc	#$00
		tay
	.endscope

	;---------------------------------------
	; Calculate the middle address byte
	.scope
		lda	y_offset
		ror			; divide by 2
		ror			; divide by 2 (divide by 4)
		and	#$3f		; (and + clc) is faster than (3x clc)
		clc
		adc	y_offset
		bcc	addr_offset_ready
		; Save the Carry Bit in Register 'X' for the High Byte
		inx
		clc
	addr_offset_ready:
		adc	src_mid
		bcc	save_carry_bit_done
		inx
	save_carry_bit_done:
		dey
		bne	store_addr	; Y - 1 == 0?
		ina
		bne	store_addr	; Did the Middle Byte "roll-over"?
		inx
	store_addr:
		sta	dst_mid
	.endscope

	;---------------------------------------
	; Calculate the high address byte
	.scope
		txa
		adc	src_hi
		sta	dst_hi
	.endscope

.endmacro


;-------------------------------------------------------------------------------
; Fill bitmapped memory with a single color index.
; 
; This macro will fill the contents of a screen with the resolution of 320x240
; and an 8-bit color depth.
;-------------------------------------------------------------------------------
; Parameters
;   data_port - The Vera Data Port to use:
;                 - port_0 ($9f23)
;                 - port_1 ($9f24)
;   addr_hi --- The high address bit of the Vera memory location
;   addr_mid -- The middle address byte of the Vera memory location
;   color_index - The value to use when filling the memory
;-------------------------------------------------------------------------------
; Registers
;   A - Scratch Pad
;   X - The Color Index
;   Y - Scratch Pad
;-------------------------------------------------------------------------------
; Stack
;   Unused
;-------------------------------------------------------------------------------
.macro	Vera_Bitmap_Fill	data_port, addr_hi, addr_mid
	;---------------------------------------
	; Initialize the starting Vera address
	.scope
		lda	addr_hi
		ora	#(Vera::Address::Advance_1_Byte | Vera::Address::Forward)
		sta	Vera::address_hi
		lda	addr_mid
		sta	Vera::address_mid
		stz	Vera::address_lo

		lda	#$00
	.endscope

	;---------------------------------------
	; 320x240 is 76,800 ($12c00) bytes.
	;
	; 20 * 256 * 15 = 76,800
	; y = 15
	; while y != 0
	;    x = 0
	;    do {
	;       write to the data port 20 times
	;       x++ (expect roll-over)
	;    } while x != 0
	;    y--
	;---------------------------------------

	;---------------------------------------
	; Loop - Fill Byte-By-Byte
	.scope
		ldy	#$0f
	loop:
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		stx	data_port
		ina
		bne	loop
		dey
		bne	loop
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Draw a vertical line, in a downward direction
; 
; Starting at the provided Vera Address, a line will be drawn downward.  Before
; using this macro:
; - Register 'X' must contain the Color Index
; - Register 'Y' must hold the length of the line
; 
; Memory corruption should be expected if the line extends beyond the bitmap's
; allotted area.
;-------------------------------------------------------------------------------
; Parameters
;   data_port - The Vera Data Port to use:
;                 - port_0 ($9f23)
;                 - port_1 ($9f24)
;   addr_hi --- The high address bit of the Vera memory location
;   addr_mid -- The middle address byte of the Vera memory location
;   addr_lo --- The low address byte of the Vera memory location
;-------------------------------------------------------------------------------
; Registers
;            | Input       | Output
; -----------|-------------|-------------
;      A     | Scratch Pad | Scratch Pad
;      X     | Color Index | Color Index
;      Y     | Line Length | #$00
;-------------------------------------------------------------------------------
; Stack
;   Unused
;-------------------------------------------------------------------------------
.macro	Vera_Bitmap_Line_VD	data_port, addr_hi, addr_mid, addr_lo
	;---------------------------------------
	; Initialize the starting Vera address
	.scope
		lda	addr_hi
		ora	#(Vera::Address::Advance_320_Bytes | Vera::Address::Forward)
		sta	Vera::address_hi
		lda	addr_mid
		sta	Vera::address_mid
		lda	addr_lo
		sta	Vera::address_lo
	.endscope

	;---------------------------------------
	; Draw the line
	.scope
	draw_line:
		stx	data_port
		dey
		bne	draw_line
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Load Palette
;-------------------------------------------------------------------------------
; Registers
;            | Input       | Output
; -----------|-------------|-------------
;      A     | Scratch Pad | Scratch Pad
;      X     | Scratch Pad | Scratch Pad
;      Y     | Scratch Pad | Scratch Pad
;-------------------------------------------------------------------------------
; Stack
;   Unused
;-------------------------------------------------------------------------------
.macro	Vera_Palette_Load address
	.scope
		advance .set 0
		advance .set Vera::Address::Advance_1_Byte	| advance
		advance .set Vera::Address::Forward		| advance
		lda	#(advance | 1)
		ldx	#$fa
		ldy	#$00
		sta	Vera::address_hi
		stx	Vera::address_mid
		sty	Vera::address_lo

		ldx	#$02
palette_fill:
		lda	address,y
		sta	Vera::port_0
		iny
		bne	palette_fill
		dex
		bne	palette_fill
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; Reset Vera
;-------------------------------------------------------------------------------
; Registers
;   A - Scratch Pad
;-------------------------------------------------------------------------------
; Stack
;   Unused
;-------------------------------------------------------------------------------
.macro	Vera_Reset
	.scope
		lda	#(Vera::Control::Reset)
		sta	Vera::control
		stz	Vera::control
	.endscope
.endmacro


; Future
.macro	Vera_Tilemap_Fill	data_port, addr_hi, addr_mid, map_width, map_height, tile_byte_0, tile_byte_1
	;---------------------------------------
	; Pass One
	.scope
		lda	addr_hi
		ora	#(Vera::Address::Advance_2_Bytes | Vera::Address::Forward)
		sta	Vera::address_hi
		lda	addr_mid
		sta	Vera::address_mid
		stz	Vera::address_lo

		lda	tile_byte_0
		ldy	map_height
	loop_y:
		ldx	map_width
	loop_x:
		sta	data_port
		dex
		bne	loop_x
		dey
		bne	loop_y
	.endscope

	;---------------------------------------
	; Pass Two
	.scope
		lda	addr_hi
		ora	#(Vera::Address::Advance_2_Bytes | Vera::Address::Forward)
		sta	Vera::address_hi
		lda	addr_mid
		sta	Vera::address_mid
		lda	#$01
		sta	Vera::address_lo

		lda	tile_byte_1
		ldy	map_height
	loop_y:
		ldx	map_width
	loop_x:
		sta	data_port
		dex
		bne	loop_x
		dey
		bne	loop_y
	.endscope
.endmacro


;-------------------------------------------------------------------------------
; vi:syntax=asm
