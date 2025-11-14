// esto va al cmake

// sirve que viene de algun lado que no pertence al modulo (busca en todo el proyecto) no es compilacion
#include "fractal_serial.h"
 
#include <complex>
 
// variables externas
extern int max_iterations; // viene de main
extern std::complex<double> c;
 
int divergente_1(std::complex<double> z0)
{
 
    /**
     * los valores dados son c y Zo
     * Zn+1 = zN^2 + c -- > lo que buscamos
     * c = -0.7 + 0.27015i ---> viene del main
     */
    int iter = 1;
    std::complex<double> z = z0;
    while (std::abs(z) < 2.0 && iter < max_iterations) // en laximo de iteraciones viene de main
    {
        z = z * z + c;
        iter++;
    }
 
    if (iter < max_iterations)
    {                      // mandamos un color
        return 0xFF0000FF; // color rojo
    }
 
    return 0xFF000000; // color negro
}
 
void julia_serial1(double x_min, double y_min, double x_max,
                    double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{
 
    double dx = (x_max - x_min) / (width);
    double dy = (y_max - y_min) / (height);
 
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double x = x_min + i * dx;
            double y = y_min + j * dy;
 
            std::complex<double> z(x, y); /// nuestro punto complejo
 
            auto color = divergente_1(z);  //auto es igual a var --> inferencia de tipos
               
            pixel_buffer[j * width + i] = color; // asignamos el color al pixel
       
           
        }
    }
}