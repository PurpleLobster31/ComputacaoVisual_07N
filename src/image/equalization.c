#include "equalization.h"
#include "histogram.h"
#include <string.h>

bool equalization_create(SDL_Renderer *renderer, const MyImage *original, MyImage *output)
{
  if (!renderer || !original || !output || original == output)
    return SDL_SetError("Parametros invalidos para equalizacao.");
  Histogram histogram;
  if (!histogram_compute(original->surface, &histogram)) return false;

  Uint8 map[256];
  uint64_t cumulative = 0, minimum = 0;
  for (int i = 0; i < 256; ++i)
  {
    cumulative += histogram.bins[i];
    if (!minimum && histogram.bins[i]) minimum = cumulative;
    // Imagens constantes sao mantidas, evitando divisao por zero.
    if (histogram.peak == histogram.total)
      map[i] = (Uint8)i;
    else if (!minimum)
      map[i] = 0;
    else
      map[i] = (Uint8)SDL_round(255.0 * (double)(cumulative - minimum) /
                               (double)(histogram.total - minimum));
  }

  MyImage next = {0};
  next.surface = SDL_DuplicateSurface(original->surface);
  if (!next.surface) return false;
  if (!SDL_LockSurface(next.surface))
  {
    image_destroy(&next);
    return false;
  }
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(next.surface->format);
  for (int y = 0; y < next.surface->h; ++y)
  {
    Uint8 *row = (Uint8 *)next.surface->pixels + (size_t)y * next.surface->pitch;
    for (int x = 0; x < next.surface->w; ++x)
    {
      Uint32 pixel;
      Uint8 r, g, b, a;
      memcpy(&pixel, row + (size_t)x * 4, 4);
      SDL_GetRGBA(pixel, format, NULL, &r, &g, &b, &a);
      pixel = SDL_MapRGBA(format, NULL, map[r], map[r], map[r], a);
      memcpy(row + (size_t)x * 4, &pixel, 4);
    }
  }
  SDL_UnlockSurface(next.surface);
  next.texture = SDL_CreateTextureFromSurface(renderer, next.surface);
  if (!next.texture)
  {
    image_destroy(&next);
    return false;
  }
  next.rect = original->rect;
  image_destroy(output);
  *output = next;
  return true;
}
