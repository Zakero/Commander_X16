.pc02

.include "../Zakero_X16.inc"

;-------------------------------------------------------------------------------
; Vera Memory Map
; $00000 - $1f9bf	Video RAM
;	$0b800 - $0c7ff 64x32 Tilemap of 8x8 Tiles (Layer 1)
;	$0c800 - $1f3ff	320x240 Bitmap (Layer 0)
;	$1f400 - $1f9bf	Unused (1472 bytes)
; $1f9c0 - $1f9ff	PSG registers
; $1fa00 - $1fbff	Palette
; $1fc00 - $1ffff	Sprite attributes
;-------------------------------------------------------------------------------

screen_addr_hi  = $00
screen_addr_mid = $c8
screen_addr_lo  = $00

tilemap_addr_hi  = $00
tilemap_addr_mid = $b8
tilemap_addr_lo  = $00

tileset_addr_hi  = $00
tileset_addr_mid = $00
tileset_addr_lo  = $00

start:
	stz	Vera::control

	;--- Vera Video Configuration ---
	.scope
		config .set 0
		config .set Vera::Video::Video_VGA	| config
		config .set Vera::Video::Chroma_Enable	| config
		config .set Vera::Video::Layer0_Enable	| config
		config .set Vera::Video::Layer1_Enable	| config
		config .set Vera::Video::Sprite_Enable	| config
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
		;config .set Vera::Layer_Config::Map_Width_256_Tiles	| config
		;config .set Vera::Layer_Config::Map_Height_256_Tiles	| config
		;config .set Vera::Layer_Config::Enable_256_Color_Mode	| config
		config .set Vera::Layer_Config::Mode_Bitmap		| config
		config .set Vera::Layer_Config::Color_8_Bits_Per_Pixel	| config

		lda	#config
		sta	Vera::layer0_config

		stz	Vera::layer0_mapbase	; Not Used

		lda	#(screen_addr_hi | (screen_addr_mid >> 1))
		sta	Vera::layer0_tilebase
	.endscope

	;--- Vera Layer1 Configuration ---
	.scope
		layer .set 0
		layer .set Vera::Layer_Config::Mode_Tilemap		| layer
		layer .set Vera::Layer_Config::Map_Width_64_Tiles	| layer
		layer .set Vera::Layer_Config::Map_Height_32_Tiles	| layer
		layer .set Vera::Layer_Config::Color_2_Bits_Per_Pixel	| layer

		lda	#layer
		sta	Vera::layer1_config

		lda	#(tilemap_addr_hi | (tilemap_addr_mid >> 1))
		sta	Vera::layer1_mapbase

		tilebase .set 0
		tilebase .set Vera::Tilebase::Tile_Width_8_Pixels	| tilebase
		tilebase .set Vera::Tilebase::Tile_Height_8_Pixels	| tilebase
		lda	#tilebase
		sta	Vera::layer1_tilebase
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
		ldy	#$00	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		lda	#$ff

		; Color Index 1
		ldx	#$00	; G  B
		ldy	#$0d	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		dea

		; Color Index 2
		ldx	#$d0	; G  B
		ldy	#$00	; -  R
		stx	Vera::port_0
		sty	Vera::port_0
		dea

		; Color Index 3
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

	ldx	#00	; Color Index
	Vera_Bitmap_Fill Vera::port_0, #screen_addr_hi, #screen_addr_mid

	Vera_Tilemap_Fill Vera::port_0, #tilemap_addr_hi, #tilemap_addr_mid, #64, #32, #01, #00

	lda	#(Vera::Address::Advance_1_Byte | tileset_addr_hi)
	sta	Vera::address_hi

	lda	#tileset_addr_mid
	sta	Vera::address_mid

	lda	#tileset_addr_lo
	sta	Vera::address_lo

	lda	#%00000000
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0
	sta	Vera::port_0

	lda	#%00111111
	sta	Vera::port_0
	lda	#%11000000
	sta	Vera::port_0
	lda	#%11000000
	sta	Vera::port_0
	lda	#%00110000
	sta	Vera::port_0
	lda	#%11000000
	sta	Vera::port_0
	lda	#%00110000
	sta	Vera::port_0
	lda	#%11111111
	sta	Vera::port_0
	lda	#%11110000
	sta	Vera::port_0
	lda	#%11000000
	sta	Vera::port_0
	lda	#%00110000
	sta	Vera::port_0
	lda	#%11000000
	sta	Vera::port_0
	lda	#%00110000
	sta	Vera::port_0
	lda	#%11000000
	sta	Vera::port_0
	lda	#%00110000
	sta	Vera::port_0
	lda	#%00000000
	sta	Vera::port_0
	lda	#%00000000
	sta	Vera::port_0


	lda	#tilemap_addr_hi
	ora	#(Vera::Address::Advance_1_Byte | Vera::Address::Forward)
	sta	Vera::address_hi
	lda	#tilemap_addr_mid
	sta	Vera::address_mid
	stz	Vera::address_lo

	lda	#01
	sta	Vera::port_0
	lda	#00
	sta	Vera::port_0

	lda	#01
	sta	Vera::port_0
	lda	#00
	sta	Vera::port_0

	;lda	#00
	;sta	$9f39
exit:
	jmp	exit

