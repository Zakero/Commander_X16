.pc02

.include "../Zakero_X16.inc"

tilemap_addr_hi		= $00
tilemap_addr_mid	= $b8
tilemap_addr_lo		= $00

tileset_addr_hi		= $00
tileset_addr_mid	= $00
tileset_addr_lo		= $00

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
		config .set Vera::Layer_Config::Mode_Tilemap		| config
		config .set Vera::Layer_Config::Map_Width_32_Tiles	| config
		config .set Vera::Layer_Config::Map_Height_32_Tiles	| config
		config .set Vera::Layer_Config::Color_2_Bits_Per_Pixel	| config
		lda	#config
		sta	Vera::layer0_config

		lda	#((tilemap_addr_hi << 7) | (tilemap_addr_mid >> 1))
		sta	Vera::layer0_mapbase

		tilebase .set 0
		tilebase .set Vera::Tilebase::Tile_Width_8_Pixels	| tilebase
		tilebase .set Vera::Tilebase::Tile_Height_8_Pixels	| tilebase
		lda	#tilebase
		sta	Vera::layer0_tilebase
	.endscope

load_data:
	X16_File_Load_To_VRAM0 font_name, #(font_name_end - font_name)

	Vera_Tilemap_Fill Vera::port_0, #tilemap_addr_hi, #tilemap_addr_mid, #64, #32, #$10, #$00
	;X16_File_Load_To_VRAM0 tilemap_name, #(tilemap_name_end - tilemap_name)

wait_for_key:
	jsr	X16::Kernal::character_read
	cmp	#$00
	beq	wait_for_key
	tax

	ldy	#$00
display_key:
	lda	#tilemap_addr_hi
	ora	#(Vera::Address::Advance_1_Byte | Vera::Address::Forward)
	sta	Vera::address_hi

	lda	#tilemap_addr_mid
	sta	Vera::address_mid

	stz	Vera::address_lo


	txa
	ror
	ror
	ror
	ror
	and	#$0f
	sta	Vera::port_0
	sty	Vera::port_0
	txa
	and	#$0f
	sta	Vera::port_0
	sty	Vera::port_0
	jmp	wait_for_key

exit:
	Vera_Reset
	X16_Soft_Reset

.rodata
font_name:	.byte	"font.x16t"
font_name_end:
;tilemap_name:	.byte	"map.x16m"
;tilemap_name_end:
