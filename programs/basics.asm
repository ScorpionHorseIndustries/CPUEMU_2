;do some basic stuff

LODA #$0000
STRA @var1
LODB #$0020

loop:
    LODA @var1
    INCR A
    STRA @var1
    FCST
    SUBC B
    JMPZ end
    JUMP loop
end:
HALT