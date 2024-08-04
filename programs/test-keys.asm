!COLOUR_PURPLE   $7C1F


LODA #$00
STRA @test_key_address
STRA @test_key_mask
STRA @test_key_return

loop:

    LODA #!KEY_A_ADDRESS
    STRA @test_key_address 
    LODA #!KEY_A_MASK
    STRA @test_key_mask
    JSUB test_key
    LODA @test_key_return
    COMP #1
    JMPE draw_white
    JUMP draw_black
after_draw:
    JUMP loop



draw_white:
    LODA #!COLOUR_BLACK
    JUMP draw_pixel
draw_black:
    LODA #!COLOUR_PURPLE
draw_pixel:
    STRA !CPU_VRAM_START
    JUMP after_draw





test_key:
    LODA #0
    STRA @test_key_return
    LODA (@test_key_address)
    LODB @test_key_mask
    BAND B
    JMPZ test_key_end
    LODA #1
    STRA @test_key_return
test_key_end:
    RSUB
