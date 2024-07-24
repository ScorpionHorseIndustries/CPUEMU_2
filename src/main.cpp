#include "common.h"

int main(int argc, char* argv[]) {

    sh::Funcs::CmdArgumentCount = argc;
    
    sh::CPU cpu;
    sh::Assembler assembler;
    std::string input_file_path = "";
    std::string output_file_path = "";



    
    for (int i = 0; i < sh::Funcs::CmdArgumentCount; i += 1) {
        sh::Funcs::CmdArguments.push_back(argv[i]);
    }

    for (int i = 0; i < sh::Funcs::CmdArgumentCount; i += 1) {
        std::cout << sh::Funcs::CmdArguments.at(i) << std::endl;


    }

    std::cout << R"USEAGE999( 

    #####################################################
    #...................................................#    
    #..#####..#####..#....#....######..##...##..#....#..#                    
    #..#......#...#..#....#....#.......#.#.#.#..#....#..#             
    #..#......#...#..#....#....#.......#..#..#..#....#..#             
    #..#......#####..#....#....####....#.....#..#....#..#               
    #..#......#......#....#....#.......#.....#..#....#..#            
    #..#......#......#....#....#.......#.....#..#....#..#            
    #..#####..#......######....######..#.....#..######..#                
    #...................................................#    
    #####################################################

    )USEAGE999" << std::endl;
    if (sh::Funcs::CmdOptionExists("-h", "--help")) {
        std::cout << R"USEAGE999(
  Arguments          |  Description
---------------------|--------------------------------------
  -c, --compile         Compile assembly ---> program
                            Required: -i, -o
  -r, --run             Run a program 
                            Required: -i
      --debug           Show Debug output when running (slow)
  -i, --input           Input File 
  -o, --output          Output File
  -w, --window          Open Window
)USEAGE999" << std::endl;
        return 0;
    }

    bool useWindow = (sh::Funcs::CmdOptionExists("-w", "-window"));

    bool ok = true;
    int return_value = 0;
    if (sh::Funcs::CmdOptionExists("-c", "--compile")) {

        input_file_path = sh::Funcs::GetCmdOption("-i", "--input");
        output_file_path = sh::Funcs::GetCmdOption("-o", "--output");

        if (input_file_path.length() > 0 && output_file_path.length() > 0) {
            assembler.ParseFromFile(input_file_path, output_file_path);

        } else {
            std::cout << "invalid arguments" << std::endl;
            return_value = -1;
        }

    } else if (sh::Funcs::CmdOptionExists("-r", "--run")) {

        input_file_path = sh::Funcs::GetCmdOption("-i", "--input");

        cpu.debug_output = sh::Funcs::CmdOptionExists("--debug");

        if (input_file_path.length() > 0) {

            if (cpu.LoadProgram(input_file_path)) {
                
                if (useWindow) {
                    sh::Window window;
                    window.Loop(cpu);
                } else {
                    while (!cpu.flags.HALT) {
                        cpu.Tick();
                    }
                }
                cpu.PrintNonZeroMemory();
                std::cout << "\n\n" << cpu.str() << std::endl;
            } else {
                std::cout << "Program could not be loaded" << std::endl;
                return_value = -1;
            }

            
        } else {
            std::cout << "invalid arguments" << std::endl;
            return_value = -1;
        }
    }

    return return_value;
}