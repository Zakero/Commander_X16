.pc02

.include "../Zakero_X16.inc"

;-------------------------------------------------------------------------------
; The lower 10-bits of the screen address can not be configured with Vera, so 
; the lower byte of the starting address will always be $00.  Also, the 2 LSB 
; of the middle screen address byte will be 0 as well.
screen_addr_hi	= $00
screen_addr_mid	= $c8 ;$c8 ;$ca ;$cc
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
		config .set Vera::Layer_Config::Mode_Bitmap		| config
		config .set Vera::Layer_Config::Color_8_Bits_Per_Pixel	| config

		lda	#config
		sta	Vera::layer0_config

		stz	Vera::layer0_mapbase	; Not Used

		lda	#(screen_addr_hi | (screen_addr_mid >> 1))
		sta	Vera::layer0_tilebase	; Bitmap Address High
	.endscope

load_palette:
	X16_File_Load_To_RAM palette, #(palette_end - palette)
	Vera_Palette_Load $9000 ; The palette file specifies $9000

load_image:
	X16_File_Load_To_VRAM0 image, #(image_end - image)
	bcc all_good
	stp
all_good:
loop:
	jmp 	loop

.rodata
image: .byte	"image.x16b"
image_end:
palette: .byte	"image.x16p"
palette_end:

