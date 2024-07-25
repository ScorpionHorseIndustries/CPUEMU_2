LODA #$0000
STRA @px
STRA @py
loop:
    JSUB draw_pixel
    JUMP loop



draw_pixel:
    LODA !CPU_RAND_01
    STRA @colour
    LODA !CPU_RAND_02
    STRA @px
    LODA !CPU_RAND_03
    STRA @py

    LODA @py
    LODB #!CPU_SCREEN_SIZE
    MULT B
    LODB @px
    ADDC B
    MOVE A,B
    LODA @colour
    STRA !CPU_VRAM_START,B
    RSUB

end:
    HALT
