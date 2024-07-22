label:
    LODA #$44           ; immediate hex
    LODA @var           ; absolute var
    LODA @var
    LODA #51            ; immediate dec
    LODA $44            ; absolute hex
    LODA 51             ; absolute dec
    LODA (51)           ; indirect dec
    LODA ($44)          ; indirect 
    LODB @var
    LODC @var
jump_to:
    JUMP $44            ; absolute hex
    JUMP 51             ; absolute dec
    JUMP label          ; absolute label
    JUMP ($44)          ; indirect hex
    JUMP (label)        ; indirect label
    JUMP (51)           ; indirect dec
    JUMP jump_to        ;
    LODB @var2          ;

quick:
    SWAP A,B            ;SWAP registers A and B
    MOVE B,C            ;copy value from B over the top of C
    MOVE X,A
    JUMP quick

INCR A
DECR B
FCCL
FZCL
INCR X
INCR X
LODA #$10
ADDC @var2
;label:              ;declare label
;@var
;#$44                ;literal
;
;$44     ;address
;#$44    ;literal
;12      ;absolute
;#12     ;literal
;($44)   ;indirect
;($44),B ;indirect, B
;($44,C) ;indirect, C
;$33,B   ;absolute, B
;12,C    ;absolute, C
;A       ;Reg
;B       ;Reg
;A,B     ;reg2
;
;identifier          [a-z][a-z0-9_]+
;label_declare       identifier:
;label_use           :identifier:
;var_use             @identifer
;
;address             hex|dec|var_use|label_use
;indirect            (address)
;
;
