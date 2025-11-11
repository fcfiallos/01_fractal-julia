#include <iostream>
#include <fmt/core.h>
#include <SFML/Graphics.hpp>


int main() {
    // std::cout<<"Hello,Fractal Julia" << std::endl;

    // std::string nombre ="abc";
    // fmt::println("Hello, Fractal Julia with fmt library!");
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML window");

    // Create a graphical text to display
    const sf::Font font("fuente.ttf");
    //const sf:: Font font (Roboto-Italic-VariableFont_wdth,wght_ttf,sizeof(Roboto-Italic-VariableFont_wdth,wght_ttf));
    sf::Text text(font, "Hello SFML", 50);
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Clear screen
        window.clear();

        // Draw the string
        window.draw(text);

        // Update the window
        window.display();
    }
    return 0;
}