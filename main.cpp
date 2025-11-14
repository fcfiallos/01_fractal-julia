#include <iostream>
#include <complex> //nos permite trabajar con complejos
#include <fmt/core.h>
#include <SFML/Graphics.hpp>

#include "arial.ttf.h"
#include "fractal_serial.h"
// para evitar que se copie dos veces la variable se añade un condicional en el .h
// el menor <> busca en el sistema y mientas el "" busca en el proyecto
#ifdef _WIN32
#include <windows.h>
#endif
// Parametros estos definiran los ejes X e Y
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

// Estas son las iteraciones
int max_iterations = 500;
std::complex<double> c(-0.7, 0.27015);

// Dimensiones de la imagen
#define WIDTH 1600
#define HEIGHT 900

// Esta es la imagen
uint32_t *pixel_buffer = nullptr; // WXH este es para la escala de grises
// uint8_t* imagen_data = nullptr; //WXHX4 (RGBA) este si es para los 4 colores

int main()
{
    // - inicializar
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];
    julia_serial1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);

    // inicializar el SFML
    // los :: hace estatica la varibles y los otros convierte un namespace para hacer un paquete en java com.ejercicio
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    // idea crear esa ventana y maximizarla
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");
// para maximizar la ventana depedne de el sistema operativo
// en openGL solo hay es bidimensional
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({WIDTH, HEIGHT});
    texture.update((const uint8_t *)pixel_buffer);

    sf::Sprite sprite(texture);
    // - escalar el sprite para llenar la ventana

    // -- textos
    const sf::Font font(arial_ttf, sizeof(arial_ttf));
    sf::Text text(font, "Julia Set", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    std::string options = "OPTIONS: [1] Serial 1";
    sf::Text textOptions(font, options, 24);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getView().getSize().y - 40});

    while (window.isOpen())
    {

        while (const std::optional event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        {
            window.draw(sprite);
            window.draw(text);
            window.draw(textOptions);
        }
        window.display();
    }
    return 0;
}