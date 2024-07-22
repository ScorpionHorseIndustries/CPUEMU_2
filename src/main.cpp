#include "common.h"

int main(int argc, char* argv[]) {

    sh::CPU cpu;
    sh::Assembler assembler;
    std::string file_name = "";
    bool ok = true;
    if (sh::CmdOptionExists(argv, argv+argc, "-f")) {

        file_name = sh::GetCmdOption(argv, argv+argc,"-f");
    }
    if (file_name.length() > 0) {
        ok = assembler.LoadFile(file_name);
        if (ok) {
            ok = assembler.Tokenise();
        } else {
            std::cout << "failed to load file [" << file_name << "]" << std::endl;
        }
        if (ok) {
            assembler.PrintTokenLines();
           
        } else {
            std::cout << "failed to tokenise [" << file_name << "]" << std::endl;
        }
        if (ok) {
            ok = assembler.Assemble();
        } else {
            std::cout << "failed to assemble [" << file_name << "]" << std::endl;
        }
        for (auto& out : assembler.outputLines) {
            std::cout << 
                std::format("[{:04x}]{:02x}{:02x} Length[{}] Value[{:04x}] DL[{:5}] LB[{:5}] VR[{:5}] LABEL[{}]", 
                    out.address,
                    out.instruction, 
                    out.address_mode,
                    out.length,
                    out.value,
                    out.isDeclareLabel,
                    out.valueIsLabel,
                    out.valueIsVar,
                    out.label
                    ) << std::endl;
        }
        // while (!cpu.flags.HALT) {
        //     cpu.Tick();
        // }
    }
    

    // std::cout << cpu.str() << std::endl;
    // for (int i = 0; i < argc; i += 1) {
    //     std::cout << argv[i] << std::endl;
    // }
    // std::cout << "hello" << std::endl;
    // sh::Window window;
    
    // return window.Loop();
    return 0;
}