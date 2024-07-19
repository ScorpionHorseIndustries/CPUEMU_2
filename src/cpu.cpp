#include "common.h"

namespace sh {
    CPU::CPU() {
        memory.fill(0);

        A = B = C = X = Y = Z = 0;
        instruction = 0;
        address_absolute = 0;
        address_mode = 0;

        for (auto [instr,amlist] : VALID_OPCODES_LOOKUP) {
            
            for (auto am : amlist) {
                u16 opcode = ((instr << 8) & 0xff00) | (am & 0x00ff);
                if (std::find(ValidOpcodes.begin(), ValidOpcodes.end(), opcode) == ValidOpcodes.end()) {
                    ValidOpcodes.push_back(opcode);
                }
            }
        }

        //write a program
        mwAddress = 0;
        u16 address_loop;
        mwWrite(LODA, IMM, 0, true);                //2
        mwWrite(STRA, ABS, CPU_RAM_START, true);    //2
        // mwWrite(INCR, RGB);             
        // mwWrite(MOVE, RG2, MakeReg2(REG_INDEX::RIB, REG_INDEX::RIC), true); //5
        address_loop = mwAddress;
        mwWrite(LODA, ABS, CPU_RAM_START, true); //2
        mwWrite(INCR, RGA);                     //1
        mwWrite(STRA, ABS, CPU_RAM_START, true);//2
        mwWrite(FCCL, IMP);//1
        mwWrite(SUBC, IMM, 20, true);
        mwWrite(JMPZ, ABS, 18, true);   //8
        mwWrite(JUMP, ABS, address_loop, true);    //10
        
        mwWrite(HALT, IMP);             //18
        
        PC = 0;

        std::cout << "MEMORY" << std::endl;
        for (int i = 0; i < 30; i += 1) {
            std::cout << std::format("{:4} {:04x}", i, memory[i]) << std::endl;
        }


    }
    u16 CPU::MakeReg2(u8 _from, u8 to) {
        return (_from << 8) | (to&0xff);
    }

    std::string CPU::str() {
        std::string output = "\n";

        output += std::format("    PC[{:04x}]", PC);
        output += std::format(" A[{:04x}]", A);
        output += std::format(" B[{:04x}]", B);
        output += std::format(" C[{:04x}]", C);
        output += std::format(" X[{:04x}]", X);
        output += std::format(" Y[{:04x}]", Y);
        output += std::format(" Z[{:04x}]", Z);
        output += std::format(" 0[{:04x}]", memory[CPU_RAM_START]);
        output += std::format(" 1[{:04x}]", memory[CPU_RAM_START+1]);
        output += std::format(" 2[{:04x}]", memory[CPU_RAM_START+2]);
        output += std::format(" 3[{:04x}]", memory[CPU_RAM_START+3]);

        
        return output;
    }


    void CPU::Tick() {
        instruction = 0;
        address_mode = 0;
        opcode = 0;
        address_absolute = 0;
        u16 which_registers = 0;

        src_reg = nullptr;
        dst_reg = nullptr;

        opcode = memory[PC];
        PC += 1;

        instruction = (opcode >> 8) & 0xff;
        address_mode = opcode & 0xff;

        


        switch (address_mode) {
            case IMP: {
                //nada
                break;
            }
            case IMM: {
                address_absolute = PC;
                PC += 1;
                break;
            }
            case ABS: {
                address_absolute = Read(PC);
                PC += 1;
                break;
            }
            case ABB: {
                address_absolute = Read(PC) + B;
                PC += 1;
                
                break;
            }
            case ABC: {
                address_absolute = Read(PC) + C;
                PC += 1;            
                break;
            }
            case IND: {
                address_absolute = Read(PC);
                PC += 1;
                address_absolute = Read(address_absolute);
                break;
            }
            case INB: {
                address_absolute = Read(PC);
                PC += 1;
                address_absolute = Read(address_absolute+B);
                break;
            }
            case INC: {

                address_absolute = Read(PC);
                PC += 1;
                address_absolute = Read(address_absolute)+C;            
                break;
            }
            case RGA: {
                src_reg = &A;
                dst_reg = &A;
                break;
            }
            case RGB: {
                src_reg = &B;
                dst_reg = &B;
                break;
            }            
            case RGC: {
                src_reg = &C;
                dst_reg = &C;
                break;
            }       
            case RGX: {
                src_reg = &X;
                dst_reg = &X;
                break;
            }
            case RGY: {
                src_reg = &Y;
                dst_reg = &Y;
                break;
            }            
            case RGZ: {
                src_reg = &Z;
                dst_reg = &Z;
                break;
            }                                    
            case RG2: {
                which_registers = Read(PC);
                PC += 1;
                int src = ((which_registers >> 8) & 0xff);
                int dst = (which_registers & 0xff);
                src_reg = regPointers[src];
                dst_reg = regPointers[dst];
                break;
            }
        }

        switch (instruction) {
            case ADDC: {
                Fetch();
                temp = (u32)A + (u32)fetched + (u32)(flags.CARRY ? 1 : 0);
                flags.CARRY = temp > 0xffff;
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;
                A = temp & 0xffff;
                break;
            }
            case SUBC: {
                Fetch();
                u32 value = ((u32)fetched) ^ 0xffff;
                temp = (u32)A + value + (u32)(flags.CARRY ? 1 : 0);
                flags.CARRY = temp > 0xffff;
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;
                A = temp & 0xffff;                
                break;
            }
            case MULT: {
                Fetch();
                temp = (u32)A * (u32)fetched;
                flags.CARRY = temp > 0xffff;
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;
                A = temp & 0xffff;            
                break;
            }
            case INCR: {
                if (dst_reg != nullptr) {
                    temp = ((u32)*dst_reg) + (u32)1;
                    flags.CARRY = temp > 0xffff;
                    flags.ZERO = temp & 0xffff == 0;
                    flags.NEGATIVE = temp & 0x8000 != 0;
                    *dst_reg = temp & 0xffff;                       

                }
                break;
            }
            case DECR: {
                if (dst_reg != nullptr) {
                    temp = ((u32)*dst_reg) - (u32)1;
                    flags.CARRY = temp > 0xffff;
                    flags.ZERO = temp & 0xffff == 0;
                    flags.NEGATIVE = temp & 0x8000 != 0;
                    *dst_reg = temp & 0xffff;                       

                }            
                break;
            }
            case BAND: {
                Fetch();
                A = A & fetched;
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;                

                break;
            }
            case BBOR: {
                Fetch();
                A = A | fetched;
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;                            
                break;
            }
            case BNOR: {
                Fetch();
                A = ~(A | fetched);
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;                            
                break;
            }
            case BXOR: {
                Fetch();
                A = A ^ fetched;
                flags.ZERO = temp & 0xffff == 0;
                flags.NEGATIVE = temp & 0x8000 != 0;                            
                break;
            }
            case PUSH: {
                Fetch();
                StackPush(fetched);
                break;
            }
            case POPS: {
                stackReturn = StackPop();
                if (dst_reg != nullptr) {
                    *dst_reg = stackReturn;
                }
                break;
            }
            case LODA: {
                Fetch();
                A = fetched;
                break;
            }
            case LODB: {
                Fetch();
                B = fetched;
                break;
            }
            case LODC: {
                Fetch();
                C = fetched;
                break;
            }
            case STRA: {
                Write(address_absolute, A);
                break;
            }
            case STRB: {
                Write(address_absolute, B);
                break;
            }
            case STRC: {
                Write(address_absolute, C);
                break;
            }
            case SWAP: {
                if (src_reg != nullptr && dst_reg != nullptr) {
                    temp = *src_reg;
                    *src_reg = *dst_reg;
                    *dst_reg = temp;
                
                }

                break;
            }
            case MOVE: {
                if (src_reg != nullptr && dst_reg != nullptr) {
                    *dst_reg = *src_reg;
                }
                break;
            }
            case JUMP: {
                PC = address_absolute;
                break;
            }
            case JMPC: {
                if (flags.CARRY) {
                    PC = address_absolute;
                }
                break;
            }
            case JMPZ: {
                if (flags.ZERO) {
                    PC = address_absolute;
                }            
                break;
            }
            case JMPN: {
                if (flags.NEGATIVE) {
                    PC = address_absolute;
                }            
                break;
            }
            case JSUB: {
                StackPush(PC-1);
                PC = address_absolute;
                break;
            }
            case RSUB: {
                PC = StackPop();
                PC += 1;
                break;
            }
            case HALT: { 
                flags.HALT = true;
                break;
            }
            case FCST: { 
                flags.CARRY = true;
                break;
            }
            case FNST: { 
                flags.NEGATIVE = true;
                break;
            }
            case FZST: { 
                flags.ZERO = true;
                break;
            }
            case FVST: { 
                flags.OVERFLOW = true;
                break;
            }
            case FFST: { 
                flags.FLOAT = true;
                break;
            }
            case FCCL: { 
                flags.CARRY = false;
                break;
            }
            case FNCL: { 
                flags.NEGATIVE = false;
                break;
            }
            case FZCL: { 
                flags.ZERO = false;
                break;
            }
            case FVCL: { 
                flags.OVERFLOW = false;
                break;
            }
            case FFCL: { 
                flags.FLOAT = false;
                break;
            }
        }

        std::cout << std::format("PC[{:04x}] IN[{:02x}] AM[{:02x}] FE[{:02x}]", PC, instruction, address_mode, fetched) << str() << std::endl;

    
    }

    u16 CPU::Fetch() {

        switch (address_mode) {
            case IMP:
                break;
            case RGA:
            case RGB:
            case RGC:
            case RGX:
            case RGY:
            case RGZ: 
            case RG2: 
                fetched = *src_reg;
                break;
            default:
                fetched = Read(address_absolute);
                break;
        }

        return fetched;
        

    
    }

    void CPU::mwWrite(u8 _instr, u8 _addr_mode, u16 arg, bool extra) {

        auto& valid_address_modes = VALID_OPCODES_LOOKUP[_instr];
        if (std::find(valid_address_modes.begin(), valid_address_modes.end(), _addr_mode) == valid_address_modes.end()) {
            std::string error = std::format("Invalid Combination INSTR[{}] ADDRM[{}]", _instr, _addr_mode);
            throw std::runtime_error(error);
        } else {
            memory[mwAddress] = MakeOpcode(_instr, _addr_mode);
            mwAddress += 1;
            if (extra) {
                memory[mwAddress] = arg;
                mwAddress += 1;
            }

        }
    }

    u16 CPU::MakeOpcode(u8 _instr, u8 _addr_mode) {
        return (_instr << 8) | _addr_mode;
    
    }

    u16 CPU::Read(u16 address) {
        return memory[address];
    }
    void CPU::Write(u16 address, u16 value) {
        memory[address] = value;
    }
    void CPU::StackPush(u16 value) {
        stackPointer += 1;
        Write(CPU_STACK_START + stackPointer, value);    

    }
    u16 CPU::StackPop() {
        stackReturn = Read(CPU_STACK_START + stackPointer);
        stackPointer -= 1;    
        return stackReturn;
    }


}