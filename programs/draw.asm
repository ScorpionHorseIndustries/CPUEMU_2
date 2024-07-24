LODA #$0000
STRA @px
STRA @py
loop:
    JSUB draw_pixel
    JUMP loop



draw_pixel:
    LODA $befc
    STRA @colour
    LODA $befd
    STRA @px
    LODA $befe
    STRA @py

    LODA @py
    LODB #128
    MULT B
    LODB @px
    ADDC B
    MOVE A,B
    LODA @colour
    STRA $bf00,B
    RSUB

end:
    HALT
