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
            void mwWrite(u8 instruction, u8 address_mode = ADDRESS_MODES::IMP, u16 arg = 0);
            static u16 EncodeOpcode(u8 _instr, u8 _addr_mode);
            static void DecodeOpcode(u16 _opcode, u8* _out_instr_ptr, u8* _out_am_ptr);
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
            inline static const std::map<u8, std::vector<u8>> OPCODES_LOOKUP = {
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
            // std::vector<u16> ValidOpcodes;

            inline static const std::map<u8, u8> ADDRESS_MODE_LENGTH = {
                {IMP, 1},
                {IMM, 2},
                {ABS, 2},
                {ABB, 2},
                {ABC, 2},
                {IND, 2},
                {INB, 2},
                {INC, 2},
                {RGA, 1},
                {RGB, 1},
                {RGC, 1},
                {RGX, 1},
                {RGY, 1},
                {RGZ, 1},
                {RG2, 2},
            };
            inline static const std::map<u8, std::string> ADDRESS_MODE_NAMES = {
                {IMP, "IMP"},
                {IMM, "IMM"},
                {ABS, "ABS"},
                {ABB, "ABB"},
                {ABC, "ABC"},
                {IND, "IND"},
                {INB, "INB"},
                {INC, "INC"},
                {RGA, "RGA"},
                {RGB, "RGB"},
                {RGC, "RGC"},
                {RGX, "RGX"},
                {RGY, "RGY"},
                {RGZ, "RGZ"},
                {RG2, "RG2"},
            };

            inline static const std::map<std::string, u8> ADDRESS_MODES_BY_NAME = {
                {"IMP", IMP },
                {"IMM", IMM },
                {"ABS", ABS },
                {"ABB", ABB },
                {"ABC", ABC },
                {"IND", IND },
                {"INB", INB },
                {"INC", INC },
                {"RGA", RGA },
                {"RGB", RGB },
                {"RGC", RGC },
                {"RGX", RGX },
                {"RGY", RGY },
                {"RGZ", RGZ },
                {"RG2", RG2 },
            };            

            inline static const std::map<u8, std::string> INSTRUCTION_NAMES = {
                { ADDC, "ADDC"},
                { SUBC, "SUBC"},
                { MULT, "MULT"},
                { INCR, "INCR"},
                { DECR, "DECR"},
                { BAND, "BAND"},
                { BBOR, "BBOR"},
                { BNOR, "BNOR"},
                { BXOR, "BXOR"},
                { PUSH, "PUSH"},
                { POPS, "POPS"},
                { LODA, "LODA"},
                { LODB, "LODB"},
                { LODC, "LODC"},
                { STRA, "STRA"},
                { STRB, "STRB"},
                { STRC, "STRC"},
                { SWAP, "SWAP"},
                { MOVE, "MOVE"},
                { JUMP, "JUMP"},
                { JMPC, "JMPC"},
                { JMPZ, "JMPZ"},
                { JMPN, "JMPN"},
                { JSUB, "JSUB"},
                { RSUB, "RSUB"},
                { HALT, "HALT"},
                { FCST, "FCST"},
                { FNST, "FNST"},
                { FZST, "FZST"},
                { FVST, "FVST"},
                { FFST, "FFST"},
                { FCCL, "FCCL"},
                { FNCL, "FNCL"},
                { FZCL, "FZCL"},
                { FVCL, "FVCL"},
                { FFCL, "FFCL"},
            };

            inline static const std::map<u8, std::string> INSTRUCTION_LONG_NAMES = {
                { ADDC, "Add With Carry"},
                { SUBC, "Subtract With Borrow"},
                { MULT, "Multiply"},
                { INCR, "Increment"},
                { DECR, "Decrement"},
                { BAND, "Bitwise AND"},
                { BBOR, "Bitwise OR"},
                { BNOR, "Bitwise NOR"},
                { BXOR, "Bitwise XOR"},
                { PUSH, "Push To Stack"},
                { POPS, "Pop From Stack"},
                { LODA, "Load A"},
                { LODB, "Load B"},
                { LODC, "Load C"},
                { STRA, "Store A"},
                { STRB, "Store B"},
                { STRC, "Store C"},
                { SWAP, "Swap Registers"},
                { MOVE, "Move Registers"},
                { JUMP, "Jump"},
                { JMPC, "Jump On Carry"},
                { JMPZ, "Jump On Zero"},
                { JMPN, "Jump On Negative"},
                { JSUB, "Jump To Subroutine"},
                { RSUB, "Return From Subroutine"},
                { HALT, "Halt"},
                { FCST, "Flag C Set"},
                { FNST, "Flag N Set"},
                { FZST, "Flag Z Set"},
                { FVST, "Flag V Set"},
                { FFST, "Flag F Set"},
                { FCCL, "Flag C Clear"},
                { FNCL, "Flag N Clear"},
                { FZCL, "Flag Z Clear"},
                { FVCL, "Flag V Clear"},
                { FFCL, "Flag F Clear"},
            };


            inline static const std::map<std::string, u8> INSTRUCTIONS_BY_NAME = {
                { "ADDC", ADDC },
                { "SUBC", SUBC },
                { "MULT", MULT },
                { "INCR", INCR },
                { "DECR", DECR },
                { "BAND", BAND },
                { "BBOR", BBOR },
                { "BNOR", BNOR },
                { "BXOR", BXOR },
                { "PUSH", PUSH },
                { "POPS", POPS },
                { "LODA", LODA },
                { "LODB", LODB },
                { "LODC", LODC },
                { "STRA", STRA },
                { "STRB", STRB },
                { "STRC", STRC },
                { "SWAP", SWAP },
                { "MOVE", MOVE },
                { "JUMP", JUMP },
                { "JMPC", JMPC },
                { "JMPZ", JMPZ },
                { "JMPN", JMPN },
                { "JSUB", JSUB },
                { "RSUB", RSUB },
                { "HALT", HALT },
                { "FCST", FCST },
                { "FNST", FNST },
                { "FZST", FZST },
                { "FVST", FVST },
                { "FFST", FFST },
                { "FCCL", FCCL },
                { "FNCL", FNCL },
                { "FZCL", FZCL },
                { "FVCL", FVCL },
                { "FFCL", FFCL },
            };     


            inline static std::string GetInstructionName(u8 __instr) {
                if (INSTRUCTION_NAMES.contains(__instr)) {
                    return INSTRUCTION_NAMES.at(__instr);
                } else {
                    return "UNKN";
                }
            }

            inline static std::string GetInstructionFullName(u8 __instr) {
                return std::format("{}[{:02x}]", GetInstructionName(__instr), __instr);
            }

            
            inline static u8 GetInstructionCode(const std::string& __instr_name) {
                if (INSTRUCTIONS_BY_NAME.contains(__instr_name)) {
                    return INSTRUCTIONS_BY_NAME.at(__instr_name);
                } else {
                    return 0;
                }
            }    

            inline static std::string GetAddressModeName(u8 __am) {
                if (ADDRESS_MODE_NAMES.contains(__am)) {
                    return ADDRESS_MODE_NAMES.at(__am);
                } else {
                    return "UNK";
                }
            }
            inline static u8 GetAddressModeCode(std::string __am_name) {
                if (ADDRESS_MODES_BY_NAME.contains(__am_name)) {
                    return ADDRESS_MODES_BY_NAME.at(__am_name);
                } else {
                    return 0;
                }
            } 

            inline static std::string GetAddressModeFullName(u8 __am) {
                return std::format("{}[{:02x}]", GetAddressModeName(__am), __am);
            }

            inline static std::string GetOpcodeFullName(u8 __instr, u8 __am) {
                return GetInstructionFullName(__instr)+":"+GetAddressModeFullName(__am);
            }

            inline static std::string GetOpcodeFullName(u16 __opcode) {
                u8 _instr = 0;
                u8 _am    = 0;
                DecodeOpcode(__opcode, &_instr, &_am);

                return GetOpcodeFullName(_instr, _am);
            }
            

            inline static bool IsValidOpcode(u8 __instr, u8 __am) {
               if (OPCODES_LOOKUP.contains(__instr)) {
                    auto& list = OPCODES_LOOKUP.at(__instr);
                    if (std::find(list.begin(), list.end(), __am) != list.end()) {
                        return true;
                    }
                } 

                return false;
            }

            inline static bool IsValidOpcode(u16 __opcode) {
                u8 _instr = 0;
                u8 _am    = 0;
                DecodeOpcode(__opcode, &_instr, &_am);

                return IsValidOpcode(_instr, _am);
            } 



    };

}


#endif