#include "fractal_serial.h"

#include <complex>
#include <immintrin.h> // avx

#include "palette.h"

extern int max_iterations; // viene de main
extern std::complex<double> c;
// no se puede empaquetar el complejo completo porque el vector quedaria cero, por ello se debe tener un vector por nos enteros y uno por los complejos ( por ende se tendra una copia por cada uno ellos)
void julia_simd(double x_min, double y_min, double x_max,
                double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer)

{
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    // esto hara el siguiente vector:  x_min x_min x_min x_min x_min x_min x_min x_min
    __m256 xmin = _mm256_set1_ps(x_min);
    __m256 ymax = _mm256_set1_ps(y_max); // igual 8 copias de y_max
    __m256 xscale = _mm256_set1_ps(dx);
    __m256 yscale = _mm256_set1_ps(dy);
    __m256 c_real = _mm256_set1_ps(c.real());
    __m256 c_imag = _mm256_set1_ps(c.imag());

    // se neceita la copia de las 4 normas para calcular cuando se menor a 4
    __m256 threshold = _mm256_set1_ps(4.0f);

    __m256 one = _mm256_set1_ps(1.0f);

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j += 8)
        {
            // i es ancho y j alto estas igual se crea un vectro de 8 com de la j
            __m256 mx = _mm256_set1_ps(i);
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4, j + 3, j + 2, j + 1, j ); // 8 copias de j siempre se saca al reves y se debe tener cuidado de tomarlo de forma correcta para graficar el pixel

            // los tipos de datos en con 2_ mientras que las operacion con 1_

            // primero se multipla
            __m256 cr = _mm256_add_ps(_mm256_mul_ps(mx, xscale), xmin); // i*dx + x_min

            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(my, yscale) ); // y_max - j*dy

            int iter = 1;

            __m256 zr = cr;
            __m256 zi = ci;

            __m256 mk = _mm256_set1_ps(iter); // contador de iteraciones para cada uno de los 8 puntos

            while (iter < max_iterations)
            {
                // Zn+1 = Zn^2 + c
                __m256 zr2 = _mm256_mul_ps(zr, zr);  // zr^2
                __m256 zi2 = _mm256_mul_ps(zi, zi);  // zi^2
                __m256 zrzi = _mm256_mul_ps(zr, zi); // zr*zi

                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real);      // zr^2 - zi^2 + cr
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), c_imag);    // 2*zr*zi + ci
                                                                          // ahora hallar la norma
                zr2 = _mm256_mul_ps(zr, zr);                              // zr^2
                zi2 = _mm256_mul_ps(zi, zi);                              // zi^2
                __m256 mag2 = _mm256_add_ps(zr2, zi2);                    // norma = zr^2 + zi^2
                                                                          // comparar las 8 normas
                __m256 mask = _mm256_cmp_ps(mag2, threshold, _CMP_LE_OS); // mag2 < 4.0 seran 8 resultados, es decir que es la mascara

                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk);

                if (_mm256_movemask_ps(mask) == 0)
                {
                    // todos ya divergen
                    break;
                }
                iter++;
            }
            float d[8];
            _mm256_storeu_ps(d, mk); // guarda el resultado la cantidad de iteracion hizo cada uno

            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i;
                if (index < width * height) // para segurar que no se va a salir de la imagen
                {
                    if (d[it] < max_iterations)
                    {
                        int color_idx = (int)d[it] % PALETTE_SIZE;
                        auto color = color_ramp[color_idx];
                        pixel_buffer[index] = color;
                    }
                    else
                    {
                        pixel_buffer[index] = 0xFF000000;
                    }
                }
            }
        }
    }
}