#ifndef __WINDOW_H_
#define __WINDOW_H_
#include "common.h"

namespace sh {
    class Window {

        public:
        sf::Font font_vcr;
        struct KeyState {
            bool pressed= false;
            bool justPressed = false;
            bool justReleased = false;
            KeyState() : pressed (false), justPressed(false), justReleased(false) {
                

            }
        };

        std::array<KeyState, sf::Keyboard::KeyCount> keys;
        Window();
        int Loop(CPU& cpu);

    
    };
}


#endif