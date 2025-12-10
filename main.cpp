#include <iostream>
#include <complex> //nos permite trabajar con complejos
#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <omp.h>

#include "arial.ttf.h"
#include "fractal_serial.h"
#include "fractal_simd.h"
#include "fractal_omp.h"
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
int max_iterations = 10;
std::complex<double> c(-0.7, 0.27015);

// Dimensiones de la imagen
#define WIDTH 1600
#define HEIGHT 900

// Esta es la imagen
uint32_t *pixel_buffer = nullptr; // WXH este es para la escala de grises
// uint8_t* imagen_data = nullptr; //WXHX4 (RGBA) este si es para los 4 colores
enum class runtime_type
{
    SERIAL_1 = 0,
    SERIAL_2,
    SIMD,
    OPENMP_REGIONES,
    OPENMP_FOR,
    OPENMP_For_Simd
};

int main()
{
    int thread_count ;

    #pragma omp parallel
    {
        #pragma omp master
        {
            thread_count = omp_get_num_threads();
            std::cout << "Número de hilos: " << thread_count << std::endl;
        }
    }
    // - inicializar
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];

    runtime_type r_type = runtime_type::SERIAL_1;

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

    std::string options = "OPTIONS: [1] Serial 1 [2] Serial 2 [3] SIMD [4] OpenMP Regiones [5] OpenMP For [6] OpenMP For SIMD | UP/DOWN: Change Iterations";
    sf::Text textOptions(font, options, 24);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getView().getSize().y - 40});

    // FPS
    int frame = 0;
    int fps = 0; // cuantos mas fps aumenta signifa que dibuja mucho mas rapido
    sf::Clock clockFrames;

    while (window.isOpen())
    {

        while (const std::optional event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>())
                window.close();
            // Añadir mas eventos de teclado
            // KeyReleased es para cuando suelto la tecla
            // KeyPressed es para cuando presiono la tecla
            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();

                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iterations += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iterations -= 10;
                    if (max_iterations < 10)
                        max_iterations = 10;
                    break;
                case sf::Keyboard::Scan::Num1:
                    r_type = runtime_type::SERIAL_1;
                    break;
                case sf::Keyboard::Scan::Num2:
                    r_type = runtime_type::SERIAL_2;
                    break;
                case sf::Keyboard::Scan::Num3:
                    r_type = runtime_type::SIMD;
                    break;
                case sf::Keyboard::Scan::Num4:
                    r_type = runtime_type::OPENMP_REGIONES;
                    break;
                case sf::Keyboard::Scan::Num5:
                    r_type = runtime_type::OPENMP_FOR;
                    break;
                case sf::Keyboard::Scan::Num6:
                    r_type = runtime_type::OPENMP_For_Simd;
                    break;
                }
            }
        }
        std::string mode = "";
        if (r_type == runtime_type::SERIAL_1)
        {

            mode = "Serial 1";
            julia_serial1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SERIAL_2)
        {
            mode = "Serial 2";
            julia_serial_2(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SIMD)
        {
            mode = "SIMD";
            julia_simd(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::OPENMP_REGIONES)
        {
            mode =  fmt::format("OPENMP REGIONES (Threads: {})", thread_count);
            julia_openmp_regiones(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::OPENMP_FOR)
        {
            mode =  fmt::format("OPENMP FOR (Threads: {})", thread_count);
            julia_openmp_for(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::OPENMP_For_Simd)
        {
            mode =  fmt::format("OPENMP FOR SIMD (Threads: {})", thread_count);
            julia_openmp_for_simd(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }

        texture.update((const uint8_t *)pixel_buffer);

        // contar FPS
        frame++;
        if (clockFrames.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frame;
            frame = 0;
            clockFrames.restart();
        }

        // actualizar el titulo
        auto msg = fmt::format("Julia Set: Iteraciones: {}, FPS: {}, Mode: {}", max_iterations, fps, mode);
        text.setString(msg);

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