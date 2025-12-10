#ifndef _FRACTAL_OMP_H_
#define _FRACTAL_OMP_H_

#include <cstdint>

void julia_openmp_regiones(double x_min, double y_min, double x_max,
                           double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer);
void julia_openmp_for(double x_min, double y_min, double x_max,
                      double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer);
// sera palalelismo y vectorial
void julia_openmp_for_simd(double x_min, double y_min, double x_max,
                           double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer);
#endif // _FRACTAL_OMP_H_