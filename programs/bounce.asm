
LODA #64
STRA @pos_x
STRA @pos_y
LODA #1
STRA @vel_x
loop:



update_ball:
    LODA @pos_x
    LODB @vel_x
    ADDC B
    COMP #127


draw_ball:
    LODA @pos_y
    MULT #128
    LODB @pos_x
    ADDC B
    MOVE A,B
    LODA #$ffff
    STRA $bf00,B

