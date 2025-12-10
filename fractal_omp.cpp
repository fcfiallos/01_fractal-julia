#include "fractal_omp.h"
#include <omp.h>
#include <immintrin.h>

#include "palette.h"

#include <complex>

// variables externas
extern int max_iterations; // viene de main
extern std::complex<double> c;

// Es metodo que hace todos los calculos

uint32_t divergente_openmp(double x, double y)
{
    int iter = 1;
    double zr = x;
    double zi = y;

    while ((zr * zr + zi * zi) < 4.0 && iter < max_iterations)
    {
        double dr = zr * zr - zi * zi + c.real();
        double di = 2.0 * zr * zi + c.imag();

        zr = dr;
        zi = di;

        iter++;
    }

    if (iter < max_iterations)
    {
        int index = iter % PALETTE_SIZE;
        return color_ramp[index];
        // return 0xFF0000FF; color rojo
    }

    return 0xFF000000; // color negro
}

void julia_openmp_regiones(double x_min, double y_min, double x_max,
                           double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / (width);
    double dy = (y_max - y_min) / (height);
    /*
    // esta seccion de codigo se debe paralelizar
    //for de fuera queda igual y el de adentro queda como una franga para dibujar los pixeles usando los hilos
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double x = x_min + i * dx;
            double y = y_min + j * dy;

            auto color = divergente_2(x, y);
            pixel_buffer[j * width + i] = color;
        }

    }*/

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int thread_count = omp_get_num_threads();

        int delta = std::ceil(width * 1.0 / thread_count); // se divide por el cual se va dibujar este caso el ancho para los hilos disponibles

        int start = thread_id * delta;
        int end = std::min(start + delta, (int)width);
        // podemos visualizar que se dibuja en franjas verticales end-20
        for (int i = start; i < end; i++)
        {
            for (int j = 0; j < height; j++)
            {
                double x = x_min + i * dx;
                double y = y_min + j * dy;

                auto color = divergente_openmp(x, y);
                pixel_buffer[j * width + i] = color;
                // como cada uno dibija en cada pixel no hay que generar ninguna union
                // memoria compartida con el buffer del pixel
            }
        }
    }
}

void julia_openmp_for(double x_min, double y_min, double x_max,
                      double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / (width);
    double dy = (y_max - y_min) / (height);
// de esta forma no se tiene control sobre la asignacion de los hilos
//#pragma omp parallel for
// con este se puede controlar la asignacion de las tareas a los hilos y mejor el rendimiento schedule y con el collapse de cuantos for necesito que paralelice en algunos caso si consigue mejorar
//#pragma omp parallel for schedule(dynamic, 1) collapse(2)
// con default(none) se obliga a definir todas las variables compartidas y privadas mientras que shared es para definir las variables compartidas
//si se deja default(shared) todas las variables seran compartidas a menos que se definan como privadas
#pragma omp parallel for default(none) shared(pixel_buffer, x_min, y_min, dx, dy, width, height)
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double x = x_min + i * dx;
            double y = y_min + j * dy;

            auto color = divergente_openmp(x, y);
            pixel_buffer[j * width + i] = color;
        }
    }
}
void julia_openmp_for_simd(double x_min, double y_min, double x_max,
                           double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / (width);
    double dy = (y_max - y_min) / (height);
    __m256 xmin = _mm256_set1_ps(x_min);
    __m256 ymax = _mm256_set1_ps(y_max); 
    __m256 xscale = _mm256_set1_ps(dx);
    __m256 yscale = _mm256_set1_ps(dy);
    __m256 c_real = _mm256_set1_ps(c.real());
    __m256 c_imag = _mm256_set1_ps(c.imag());

    __m256 threshold = _mm256_set1_ps(4.0f);

    __m256 one = _mm256_set1_ps(1.0f);
    #pragma omp parallel for
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j += 8)
        {
            
            __m256 mx = _mm256_set1_ps(i);
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4, j + 3, j + 2, j + 1, j ); 
            __m256 cr = _mm256_add_ps(_mm256_mul_ps(mx, xscale), xmin); 

            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(my, yscale) ); 

            int iter = 1;

            __m256 zr = cr;
            __m256 zi = ci;

            __m256 mk = _mm256_set1_ps(iter); 
            while (iter < max_iterations)
            {
                __m256 zr2 = _mm256_mul_ps(zr, zr);  
                __m256 zi2 = _mm256_mul_ps(zi, zi);  
                __m256 zrzi = _mm256_mul_ps(zr, zi); 

                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real);      
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), c_imag);    
                zr2 = _mm256_mul_ps(zr, zr); 
                zi2 = _mm256_mul_ps(zi, zi); 
                __m256 mag2 = _mm256_add_ps(zr2, zi2);                    
                __m256 mask = _mm256_cmp_ps(mag2, threshold, _CMP_LE_OS); 
                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk);

                if (_mm256_movemask_ps(mask) == 0)
                {
                    break;
                }
                iter++;
            }
            float d[8];
            _mm256_storeu_ps(d, mk); 

            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i;
                if (index < width * height) 
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