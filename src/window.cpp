#include "common.h"


namespace sh {

    int Window::Loop() {
        auto window = sf::RenderWindow( {256,256}, "CPU EMU");

        window.setFramerateLimit(60);

        sf::Clock clock;
        sf::Clock deltaClock;
        while (window.isOpen()) {
            float dt = deltaClock.restart().asSeconds();

            for (auto event = sf::Event(); window.pollEvent(event);) {



                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }


            //DRAW GUI
            
            window.clear();
     

            window.display();
        }


        return 0;        
    }


}