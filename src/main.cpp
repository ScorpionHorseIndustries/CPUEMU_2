#include "common.h"

int main(int argc, char* argv[]) {

    sh::CPU cpu;
    while (!cpu.flags.HALT) {
        cpu.Tick();
    }

    std::cout << cpu.str() << std::endl;
    // for (int i = 0; i < argc; i += 1) {
    //     std::cout << argv[i] << std::endl;
    // }
    // std::cout << "hello" << std::endl;
    // sh::Window window;
    
    // return window.Loop();
    return 0;
}