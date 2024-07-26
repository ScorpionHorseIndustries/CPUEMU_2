#include "common.h"

namespace sh {

    Window::Window() {
        font_vcr.loadFromFile("fonts\\vcr.TTF");


        


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
                    if (i + j >= sf::Keyboard::KeyCount) {
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

            if (keys[sf::Keyboard::Key::A].pressed) {
                screenBuffer.setPixel(SFML_WINDOW_WIDTH-10, SFML_WINDOW_HEIGHT-10, sf::Color::White);

            } else {
                screenBuffer.setPixel(SFML_WINDOW_WIDTH-10, SFML_WINDOW_HEIGHT-10, sf::Color::Black);            
            }

            auto v = cpu.memory[CPU_KEYSTATE_START];
            sf::Text text(std::format("{:04x}", v), font_vcr);
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::White);
            text.setPosition(SFML_WINDOW_WIDTH / 2, SFML_WINDOW_HEIGHT / 2);
            sf::Color c(255,0,0);
            for (int n = 0; n < 16; n += 1) {
                
                c.g = c.b = ((v & (1 << n)) ? 255 : 0);
                
                screenBuffer.setPixel(SFML_WINDOW_WIDTH-10-n, SFML_WINDOW_HEIGHT-12, c);  
                screenBuffer.setPixel(SFML_WINDOW_WIDTH-10-n, SFML_WINDOW_HEIGHT-13, c);  
                screenBuffer.setPixel(SFML_WINDOW_WIDTH-10-n, SFML_WINDOW_HEIGHT-14, c);  
            }
            
            tex.update(screenBuffer);

            //DRAW GUI
            
            window.clear();
            
            window.draw(sf::Sprite(tex));
            window.draw(text);


            window.display();
        }


        return 0;        
    }


}