#include "histogram.h"
#include <string.h>

bool histogram_compute(SDL_Surface *surface, Histogram *output)
{
  if (!surface || !output || surface->format != SDL_PIXELFORMAT_RGBA32 ||
      surface->w <= 0 || surface->h <= 0)
    return SDL_SetError("Histograma requer uma superficie RGBA32 valida.");

  if (!SDL_LockSurface(surface)) return false;
  Histogram result = {0};
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  for (int y = 0; y < surface->h; ++y)
  {
    const Uint8 *row = (const Uint8 *)surface->pixels + (size_t)y * surface->pitch;
    for (int x = 0; x < surface->w; ++x)
    {
      Uint32 pixel;
      Uint8 r, g, b, a;
      memcpy(&pixel, row + (size_t)x * sizeof(pixel), sizeof(pixel));
      SDL_GetRGBA(pixel, format, NULL, &r, &g, &b, &a);
      if (r != g || g != b)
      {
        SDL_UnlockSurface(surface);
        return SDL_SetError("Histograma requer uma imagem em escala de cinza.");
      }
      ++result.bins[r];
    }
  }
  SDL_UnlockSurface(surface);

  result.total = (uint64_t)surface->w * (uint64_t)surface->h;
  for (int i = 0; i < 256; ++i)
  {
    result.mean += (double)i * (double)result.bins[i];
    if (result.bins[i] > result.peak) result.peak = result.bins[i];
  }
  result.mean /= (double)result.total;
  // Variancia populacional: todos os pixels da imagem, nao uma amostra.
  double variance = 0.0;
  for (int i = 0; i < 256; ++i)
  {
    double distance = i - result.mean;
    variance += distance * distance * (double)result.bins[i];
  }
  result.stddev = SDL_sqrt(variance / (double)result.total);
  *output = result;
  return true;
}

// Limiares adotados neste prototipo, documentados no README.
const char *histogram_brightness(double mean)
{
  return mean < 85.0 ? "escura" : mean < 170.0 ? "média" : "clara";
}

const char *histogram_contrast(double stddev)
{
  return stddev < 40.0 ? "baixo" : stddev < 80.0 ? "médio" : "alto";
}
