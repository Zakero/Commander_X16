.pc02

.include "../Zakero_X16.inc"

;-------------------------------------------------------------------------------
; The lower 10-bits of the screen address can not be configured with Vera, so 
; the lower byte of the starting address will always be $00.  Also, the 2 LSB 
; of the middle screen address byte will be 0 as well.
screen_addr_hi	= $00
screen_addr_mid	= $00 ;$c8 ;$ca ;$cc
screen_addr_lo	= $00

start:
	stz	Vera::control

	;--- Vera Video Configuration ---
	.scope
		config .set 0
		config .set Vera::Video::Video_VGA	| config
		config .set Vera::Video::Chroma_Enable	| config
		config .set Vera::Video::Layer0_Enable	| config
		config .set Vera::Video::Layer1_Disable	| config
		config .set Vera::Video::Sprite_Disable	| config
		lda	#config
		sta	Vera::video
	.endscope

	;--- Vera Output Scaling ---
	lda	#Vera::Scale::x2
	sta	Vera::scale_h
	lda	#Vera::Scale::x2
	sta	Vera::scale_v

	;--- Vera Layer0 Configuration ---
	.scope
		config .set 0
		;config .set Vera::Layer_Config::Map_Height_256_Tiles	| config
		;config .set Vera::Layer_Config::Map_Width_256_Tiles	| config
		;config .set Vera::Layer_Config::Enable_256_Color_Mode	| config
		config .set Vera::Layer_Config::Mode_Bitmap		| config
		config .set Vera::Layer_Config::Color_8_Bits_Per_Pixel	| config

		lda	#config
		sta	Vera::layer0_config

		stz	Vera::layer0_mapbase	; Not Used

		lda	#(screen_addr_hi | (screen_addr_mid >> 1))
		sta	Vera::layer0_tilebase	; Bitmap Address High
	.endscope

	;--- Pallette
	.scope
		advance .set 0
		advance .set Vera::Address::Advance_1_Byte	| advance
		advance .set Vera::Address::Forward		| advance
		lda	#(advance | 1)
		ldx	#$FA
		ldy	#$00
		sta	Vera::address_hi
		stx	Vera::address_mid
		sty	Vera::address_lo

		lda	#$00
		ldx	#$07	; G  B
		ldy	#$00	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		ina

		ldx	#$ff	; G  B
		ldy	#$0f	; -  R

palette_fill:
		stx	Vera::port_0
		sty	Vera::port_0
		ina
		cmp	#$ff
		bne	palette_fill
	.endscope

fill_screen:
	ldx	#$01
	Vera_Bitmap_Fill Vera::port_0, #screen_addr_hi, #screen_addr_mid
	ldx	#$00
	Vera_Bitmap_Fill Vera::port_0, #screen_addr_hi, #screen_addr_mid

loop:
	;jmp	fill_screen
	jmp	loop

exit:
	Vera_Reset
	X16_Soft_Reset
