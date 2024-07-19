#ifndef __CPU_H_
#define __CPU_H_


#include "common.h"

namespace sh {
    class CPU {
        public:

            enum INSTRUCTIONS : u8{
                //          
                ADDC=1,  //                         //add with carry    
                SUBC,    //                         //subtract with carry
                MULT,    //                         //multiply
                INCR,    //   
                DECR,    //   
                BAND,    //         
                BBOR,    //         
                BNOR,    //   
                BXOR,    //   
                PUSH,    //   
                POPS,    //   
                LODA,    //         // load register with memory or immediate
                LODB,    //   
                LODC,    //   
                STRA,    //         // store register to memory
                STRB,    //   
                STRC,    //   
                SWAP,    //         //swap registers
                MOVE,    //         //move register to register, overwrites dst, does not change source 
                JUMP,    //         //jump
                JMPC,    //         //jump on carry
                JMPZ,    //         //jump on zero
                JMPN,    //         //jump on negative
                JSUB,    //         //jump to subroutine
                RSUB,    //         //return from subroutine
                HALT,    //         Flag Halt           Set
                FCST,    //         Flag Carry          Set
                FNST,    //         Flag Negative       Set
                FZST,    //         Flag Zero           Set
                FVST,    //         Flag Overflow       Set
                FFST,    //         Flag Floating Point Set
                FCCL,    //         Flag Carry          Clear
                FNCL,    //         Flag Negative       Clear
                FZCL,    //         Flag Zero           Clear
                FVCL,    //         Flag Overflow       Clear
                FFCL,    //         Flag Floating Point Clear
                
            
            };

            enum ADDRESS_MODES : u8 {
                IMP=1,      //implied        
                IMM,        //immediate
                ABS,        //absolute
                ABB,        //absolute+b
                ABC,        //absolute+c
                IND,        //indirect
                INB,        //indirect+b
                INC,        //indurect+c
                RGA,        //one register A
                RGB,        //one register B
                RGC,        //one register C
                RGX,        //one register X
                RGY,        //one register Y
                RGZ,        //one register Z
                RG2,        //two registers                                                                      
            };

            enum REG_INDEX : u8 {
                RIA = 0,
                RIB,
                RIC,
                RIX,
                RIY,
                RIZ,
            };
            //registers
            u16 A,B,C,X,Y,Z;
            u32 temp;
            u16* src_reg = nullptr;
            u16* dst_reg = nullptr;            
            std::array<u16*, 6> regPointers = {
                &A,
                &B,
                &C,
                &X,
                &Y,
                &Z
            };
            u16 opcode;
            u8 instruction;
            u8 address_mode;
            u16 address_absolute;
            u16 fetched;
            u16 PC;
            std::array<u16, CPU_MEMORY_SIZE> memory;
            u8 stackPointer;
            u16 stackReturn;
            u16 mwAddress = 0;
            void mwWrite(u8 instruction, u8 address_mode, u16 arg = 0, bool extra = false);
            u16 MakeOpcode(u8 _instr, u8 _addr_mode);
            u16 MakeReg2(u8 _from, u8 to);
            std::string str();
            


            struct Flags {
                bool HALT = false;
                bool CARRY = false;
                bool NEGATIVE = false;
                bool ZERO = false;
                bool OVERFLOW = false;
                bool FLOAT = false;
            } flags;
            
            CPU();
            void Tick();
            u16 Fetch();
            u16 Read(u16 address);
            void Write(u16 address, u16 value);
            void StackPush(u16 value);
            u16 StackPop();
            inline static std::map<u16, std::vector<u16>> VALID_OPCODES_LOOKUP = {
                { ADDC, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { SUBC, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { MULT, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { INCR, {                                         RGA, RGB, RGC, RGX, RGY, RGZ      }},
                { DECR, {                                         RGA, RGB, RGC, RGX, RGY, RGZ      }},
                { BAND, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { BBOR, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { BNOR, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { BXOR, {      IMM, ABS, ABB, ABC, IND, INB, INC,      RGB, RGC, RGX, RGY, RGZ      }},
                { PUSH, {                                         RGA, RGB, RGC, RGX, RGY, RGZ      }},
                { POPS, {                                         RGA, RGB, RGC, RGX, RGY, RGZ      }},
                { LODA, {      IMM, ABS, ABB, ABC, IND, INB, INC                                    }},
                { LODB, {      IMM, ABS,      ABC, IND,      INC                                    }},
                { LODC, {      IMM, ABS, ABB,      IND, INB,                                        }},
                { STRA, {           ABS, ABB, ABC, IND, INB, INC                                    }},
                { STRB, {           ABS,      ABC, IND,      INC                                    }},
                { STRC, {           ABS, ABB,      IND, INB,                                        }},
                { SWAP, {                                                                       RG2 }},
                { MOVE, {                                                                       RG2 }},
                { JUMP, {           ABS,           IND                                              }},
                { JMPC, {           ABS,           IND                                              }},
                { JMPZ, {           ABS,           IND                                              }},
                { JMPN, {           ABS,           IND                                              }},
                { JSUB, {           ABS,           IND                                              }},
                { RSUB, { IMP                                                                       }},
                { HALT, { IMP                                                                       }},
                { FCST, { IMP                                                                       }},
                { FNST, { IMP                                                                       }},
                { FZST, { IMP                                                                       }},
                { FVST, { IMP                                                                       }},
                { FFST, { IMP                                                                       }},
                { FCCL, { IMP                                                                       }},
                { FNCL, { IMP                                                                       }},
                { FZCL, { IMP                                                                       }},
                { FVCL, { IMP                                                                       }},
                { FFCL, { IMP                                                                       }},
            };
            std::vector<u16> ValidOpcodes;

    };

}


#endif