// esto se importa
// tipo de variable que tiene el uint32_t
#ifndef FRACTAL_SERIAL_H // evita duplicidad
#define FRACTAL_SERIAL_H
 
#pragma once
 
#include <cstdint>
 
void julia_serial1(double x_min,double y_min, double x_max,
                     double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);
 
#endif // FRACTAL_SERIAL_H
