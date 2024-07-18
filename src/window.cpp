#include "common.h"


namespace sh {

    int Window::Loop() {
        auto window = sf::RenderWindow( {256,256}, "CPU EMU");

        window.setFramerateLimit(60);

        if (!ImGui::SFML::Init(window)) {
            return -1;
        }

        sf::Clock clock;
        sf::Clock deltaClock;
        while (window.isOpen()) {
            float dt = deltaClock.restart().asSeconds();

            for (auto event = sf::Event(); window.pollEvent(event);) {

                ImGui::SFML::ProcessEvent(window, event);

                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            ImGui::SFML::Update(window, clock.restart());
            //DRAW GUI
            
            window.clear();
            ImGui::SFML::Render(window);

            window.display();
        }

        ImGui::SFML::Shutdown();
        return 0;        
    }


}