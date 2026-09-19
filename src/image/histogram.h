#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <SDL3/SDL.h>
#include <stdint.h>

typedef struct Histogram
{
  uint64_t bins[256];
  uint64_t total;
  uint64_t peak;
  double mean;
  double stddev;
} Histogram;

// Entrada: superficie RGBA32 em escala de cinza. Respeita o pitch das linhas.
// Saida so e substituida se todo o calculo terminar com sucesso.
bool histogram_compute(SDL_Surface *surface, Histogram *output);
const char *histogram_brightness(double mean);
const char *histogram_contrast(double stddev);

#endif
