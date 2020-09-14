.pc02

.include "../Zakero_X16.inc"

screen_addr_hi  = $00
screen_addr_mid = $c8
screen_addr_lo  = $00

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
	lda	#Vera::Scale::x2	; Not needed. Added for explicitness
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

		; Color Index 0
		ldx	#$00	; G  B
		ldy	#$0d	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		lda	#$ff

		; Color Index 1
		ldx	#$d0	; G  B
		ldy	#$00	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		dea

		; Color Index 2
		ldx	#$0d	; G  B
		ldy	#$00	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		dea

		ldx	#$00	; G  B
		ldy	#$00	; -  R
palette_fill:
		stx	Vera::port_0
		sty	Vera::port_0
		dea
		bne	palette_fill
	.endscope

	ldx	#03	; Color Index
	Vera_Bitmap_Fill Vera::port_0, #screen_addr_hi, #screen_addr_mid

	ldx	#00	; Color Index
	ldy	#160	; Length
	Vera_Bitmap_Line_VD Vera::port_0, #screen_addr_hi, #screen_addr_mid, #screen_addr_lo

	view_x		= $02 ; The "view port" to draw in
	view_y		= $03 ; view_x and view_y is the top-left corner
	offset_x	= $01 ; The X offset insid the "view port"

	view_addr_hi 	= $05
	view_addr_mid	= $06
	view_addr_lo	= $07
	;--------------------
	inc_hi 		= $08
	inc_mid		= $09

	line_addr_hi	= $0d
	line_addr_mid	= $0e
	line_addr_lo	= $0f


	; The "view port" is a conceptual thing that can be though of as
	; a simple transform:
	; - Calculate the location of the "view port"
	; - Calculate a location in the "view port"

	lda	#$08
	sta	view_x

	lda	#$08
	sta	view_y

	lda	#$00

	Vera_Bitmap_Addr_X view_x, #screen_addr_hi, #screen_addr_mid, #screen_addr_lo, view_addr_hi, view_addr_mid, view_addr_lo
	Vera_Bitmap_Addr_Y view_y, view_addr_hi, view_addr_mid, view_addr_lo, view_addr_hi, view_addr_mid, view_addr_lo

draw_view:
	ldx	#01	; Color Index - Green
	ldy	#160	; Length
	Vera_Bitmap_Line_VD Vera::port_0, view_addr_hi, view_addr_mid, view_addr_lo


	lda	#$02
	sta	offset_x

;stp ;---------------------------------------------------------------------------

calculate_line_addr_lo:
	lda	view_addr_lo
	adc	offset_x
	sta	line_addr_lo

calculate_line_addr_mid:
	lda	view_addr_mid
	adc	#$00
	sta	line_addr_mid

calculate_line_addr_hi:
	lda	view_addr_hi
	adc	#$00
	sta	line_addr_hi

update_done:
	ldx	#02	; Color Index - Blue
	ldy	#160	; Length
	Vera_Bitmap_Line_VD Vera::port_0, line_addr_hi, line_addr_mid, line_addr_lo

exit:
	jmp exit
	Vera_Reset
	X16_Soft_Reset

