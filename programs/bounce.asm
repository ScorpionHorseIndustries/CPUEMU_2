
LODA #64
STRA @pos_x
STRA @pos_y
LODA #1
STRA @vel_x
loop:



update_ball:

    

update_ball_x_pos:
    LODA @pos_x
    INCR A
    COMP #127

update_ball_x_neg:


draw_ball:
    LODA @pos_y
    MULT #128
    LODB @pos_x
    ADDC B
    MOVE A,B
    LODA #$ffff
    STRA $bf00,B

