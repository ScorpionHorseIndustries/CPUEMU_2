#include "common.h"

int main(int argc, char* argv[]) {

    sh::CPU cpu;
    sh::Assembler assembler;
    std::string input_file_path = "";
    std::string output_file_path = "";


    std::cout << R"USEAGE999( 
                        ##########################
                        #                        #    
                        #  #####  #####  #    #  #                    
                        #  #      #   #  #    #  #             
                        #  #      #   #  #    #  #             
                        #  #      #####  #    #  #               
                        #  #      #      #    #  #            
                        #  #####  #      ######  #                
                        #                        #    
                        ##########################
    )USEAGE999" << std::endl;
    if (sh::CmdOptionExists(argv, argv+argc, "-h") || sh::CmdOptionExists(argv, argv+argc, "--help")) {
        std::cout << R"USEAGE999(

-c, --compile           Compile assembly ---> program
                            Required: -i, -o
-r, --run               Run a program 
                            Required: -i
    --debug             Show Debug output when running (slow)
-i, --input             Input File 
-o, --output            Output File


        
        )USEAGE999" << std::endl;
        return 0;
    }

    bool ok = true;
    if (sh::CmdOptionExists(argv, argv+argc, "-c") || sh::CmdOptionExists(argv, argv+argc, "--compile")) {

        if (sh::CmdOptionExists(argv, argv+argc, "-i")) {
            input_file_path = sh::GetCmdOption(argv, argv+argc, "-i");
        }

        if (sh::CmdOptionExists(argv, argv+argc, "--input")) {
            input_file_path = sh::GetCmdOption(argv, argv+argc, "--input");
        }

        if (sh::CmdOptionExists(argv, argv+argc, "-o")) {
            output_file_path = sh::GetCmdOption(argv, argv+argc, "-o");
        }

        if (sh::CmdOptionExists(argv, argv+argc, "--output")) {
            output_file_path = sh::GetCmdOption(argv, argv+argc, "--output");
        }        

        if (input_file_path.length() > 0 && output_file_path.length() > 0) {
            assembler.ParseFromFile(input_file_path, output_file_path);

        } else {
            std::cout << "invalid arguments" << std::endl;
            return -1;
        }


    } else if (sh::CmdOptionExists(argv, argv+argc, "-r") || sh::CmdOptionExists(argv, argv+argc, "--run")) {
        if (sh::CmdOptionExists(argv, argv+argc, "-i")) {
            input_file_path = sh::GetCmdOption(argv, argv+argc, "-i");
        }

        if (sh::CmdOptionExists(argv, argv+argc, "--input")) {
            input_file_path = sh::GetCmdOption(argv, argv+argc, "--input");
        }

        if (sh::CmdOptionExists(argv, argv+argc, "--debug")) {
            cpu.debug_output = true;
        }
        if (input_file_path.length() > 0) {

            if (cpu.LoadProgram(input_file_path)) {

                while (!cpu.flags.HALT) {
                    cpu.Tick();
                }
                cpu.PrintNonZeroMemory();
                std::cout << "\n\n" << cpu.str() << std::endl;
            } else {
                std::cout << "Program could not be loaded" << std::endl;
            }

            
        } else {
            std::cout << "invalid arguments" << std::endl;
            return -1;
        }
    }


    return 0;
}