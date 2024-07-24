#include "common.h"


namespace sh {

    int Window::Loop(CPU& cpu) {
        auto window = sf::RenderWindow( {CPU_SCREEN_SIZE*2,CPU_SCREEN_SIZE*2}, "CPU EMU");

        window.setFramerateLimit(60);
        sf::Image screenBuffer;
        screenBuffer.create(CPU_SCREEN_SIZE*2, CPU_SCREEN_SIZE*2);


        sf::Clock clock;
        sf::Clock deltaClock;
        while (window.isOpen()) {
            float dt = deltaClock.restart().asSeconds();

            for (auto event = sf::Event(); window.pollEvent(event);) {



                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }


            for (int y = 0; y < CPU_SCREEN_SIZE; y += 1) {
                for (int x = 0; x < CPU_SCREEN_SIZE; x += 1) {
                    int idx = CPU_VRAM_START + (y * CPU_SCREEN_SIZE + x);

                    
                }
            }
            

            //DRAW GUI
            
            window.clear();
     

            window.display();
        }


        return 0;        
    }


}