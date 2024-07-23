LODA #$00ff
STRA @counter


loop:
    LODA @counter
    FCST 
    COMP #0
    JMPE end
    JSUB dec_a
    JUMP loop
    

dec_a:
    LODA @counter
    DECR A
    STRA @counter
    RSUB

end:
    HALT
