#include "common.h"

int main(int argc, char* argv[]) {

    for (int i = 0; i < argc; i += 1) {
        std::cout << argv[i] << std::endl;
    }
    std::cout << "hello" << std::endl;
    sh::Window window;
    
    return window.Loop();
}