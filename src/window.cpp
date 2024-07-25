#include "common.h"

namespace sh {

    Window::Window() {
        keys.fill({});

    }

    int Window::Loop(CPU& cpu) {
        auto window = sf::RenderWindow( {SFML_WINDOW_WIDTH, SFML_WINDOW_HEIGHT}, "CPU EMU");

        window.setFramerateLimit(60);
        sf::Image screenBuffer;
        sf::Texture tex;
        screenBuffer.create(SFML_WINDOW_WIDTH, SFML_WINDOW_HEIGHT);
        tex.create(SFML_WINDOW_WIDTH, SFML_WINDOW_HEIGHT);

        sf::Clock clock;
        sf::Clock deltaClock;
        while (window.isOpen()) {

            for (auto& ks : keys) {

                ks.justPressed = false;
                ks.justReleased = false;
            }

            
            float dt = deltaClock.restart().asSeconds();

            for (auto event = sf::Event(); window.pollEvent(event);) {



                if (event.type == sf::Event::Closed) {
                    window.close();
                } else if (event.type == sf::Event::KeyPressed) {
 
                    if (!keys[event.key.code].pressed) {
                         keys[event.key.code].justPressed = true;
                    }
                    keys[event.key.code].pressed = true;
                } else if (event.type == sf::Event::KeyReleased) {
                    if (keys[event.key.code].pressed) {
                        keys[event.key.code].justReleased = true;
                    }
                    keys[event.key.code].pressed = false;
                }
            }
            u16 key_position = 0;
            for (int i = 0; i < keys.size(); i += 16) {
                u16 k = 0;
                for (int j = 0; j < 16; j += 1) {
                    if (i + j > sf::Keyboard::KeyCount < 1) {
                        break;
                    }

                    if (keys[i+j].pressed) {
                        k |= (1 << j);
                    }
                }
                cpu.memory[CPU_KEYSTATE_START + key_position ] = k;
                key_position += 1;
            }


            if (!cpu.flags.HALT) {
                for(int i = 0; i < 5000; i += 1) {
                    cpu.Tick();
                    if (cpu.flags.HALT) break;
                }
            }


            for (int y = 0; y < CPU_SCREEN_SIZE; y += 1) {
                for (int x = 0; x < CPU_SCREEN_SIZE; x += 1) {
                    int idx = CPU_VRAM_START + (y * CPU_SCREEN_SIZE + x);
                    auto c = cpu.memory[idx];
                    for (int yy =0; yy < SFML_RES; yy += 1) {
                        int py = (y * SFML_RES) + yy;
                        if (py >= SFML_WINDOW_HEIGHT) break;
                        for (int xx = 0; xx < SFML_RES; xx += 1) {
                            int px = (x * SFML_RES) + xx;
                            if (px >= SFML_WINDOW_WIDTH) break;
                            screenBuffer.setPixel(px,py,Funcs::GetColour(c));
                        }
                    }
                }
            }
            
            tex.update(screenBuffer);
            //DRAW GUI
            
            window.clear();
            
            window.draw(sf::Sprite(tex));

            window.display();
        }


        return 0;        
    }


}