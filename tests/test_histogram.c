#include "image/histogram.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void fill(SDL_Surface *surface, int value)
{
  const SDL_PixelFormatDetails *f = SDL_GetPixelFormatDetails(surface->format);
  assert(SDL_FillSurfaceRect(surface, NULL, SDL_MapRGBA(f, NULL, value, value, value, 255)));
}

int main(void)
{
  Histogram h = {0};
  SDL_Surface *surface = SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_RGBA32);
  assert(surface);
  fill(surface, 0);
  assert(histogram_compute(surface, &h));
  assert(h.total == 256 && h.peak == 256 && h.bins[0] == 256);
  assert(h.mean == 0 && h.stddev == 0);
  fill(surface, 255);
  assert(histogram_compute(surface, &h));
  assert(h.bins[0] == 0 && h.bins[255] == 256 && h.mean == 255 && h.stddev == 0);

  // Distribuicao uniforme dos 256 niveis: media 127.5, variancia 5461.25.
  const SDL_PixelFormatDetails *f = SDL_GetPixelFormatDetails(surface->format);
  for (int i = 0; i < 256; ++i)
  {
    Uint32 pixel = SDL_MapRGBA(f, NULL, i, i, i, 255);
    memcpy((Uint8 *)surface->pixels + (i / 16) * surface->pitch + (i % 16) * 4, &pixel, 4);
  }
  assert(histogram_compute(surface, &h));
  assert(h.peak == 1 && h.mean == 127.5);
  assert(SDL_fabs(h.stddev - 73.90027063549903) < 1e-9);
  for (int i = 0; i < 256; ++i) assert(h.bins[i] == 1);
  SDL_DestroySurface(surface);

  // Uma coluna, duas linhas e bytes extras entre elas: padding nao e pixel.
  Uint8 storage[24] = {0};
  Uint32 white = SDL_MapRGBA(f, NULL, 255, 255, 255, 0);
  memcpy(storage + 12, &white, 4);
  surface = SDL_CreateSurfaceFrom(1, 2, SDL_PIXELFORMAT_RGBA32, storage, 12);
  assert(surface && histogram_compute(surface, &h));
  assert(h.total == 2 && h.bins[0] == 1 && h.bins[255] == 1);
  assert(h.mean == 127.5 && h.stddev == 127.5);
  SDL_DestroySurface(surface);

  assert(!strcmp(histogram_brightness(84.99), "escura"));
  assert(!strcmp(histogram_brightness(85), "média"));
  assert(!strcmp(histogram_brightness(169.99), "média"));
  assert(!strcmp(histogram_brightness(170), "clara"));
  assert(!strcmp(histogram_contrast(39.99), "baixo"));
  assert(!strcmp(histogram_contrast(40), "médio"));
  assert(!strcmp(histogram_contrast(79.99), "médio"));
  assert(!strcmp(histogram_contrast(80), "alto"));

  Histogram before = h;
  assert(!histogram_compute(NULL, &h));
  assert(!memcmp(&h, &before, sizeof h));
  surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
  assert(surface);
  Uint32 red = SDL_MapRGBA(f, NULL, 255, 0, 0, 255);
  memcpy(surface->pixels, &red, 4);
  assert(!histogram_compute(surface, &h));
  assert(!memcmp(&h, &before, sizeof h));
  SDL_DestroySurface(surface);
  puts("Histograma: contagens, estatisticas, pitch, alfa, limites e entradas invalidas OK.");
  return 0;
}
