#include "grayscale.h"
#include <math.h>

//------------------------------------------------------------------------------
bool grayscale_is_image_grayscale(const MyImage *image)
{
  if (!image || !image->surface)
  {
    SDL_Log("\t*** Erro: imagem invalida em grayscale_is_image_grayscale().");
    return false;
  }

  // SDL_LockSurface/UnlockSurface tambem funcionam em const-cast local, pois
  // aqui so fazemos leitura dos pixels.
  SDL_Surface *surface = image->surface;
  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = (size_t)surface->w * (size_t)surface->h;
  const Uint32 *pixels = (const Uint32 *)surface->pixels;

  bool isGrayscale = true;
  Uint8 r = 0, g = 0, b = 0, a = 0;

  for (size_t i = 0; i < pixelCount; ++i)
  {
    SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);
    if (r != g || g != b)
    {
      isGrayscale = false;
      break;
    }
  }

  SDL_UnlockSurface(surface);
  return isGrayscale;
}

//------------------------------------------------------------------------------
void grayscale_convert(SDL_Renderer *renderer, MyImage *image)
{
  if (!renderer || !image || !image->surface)
  {
    SDL_Log("\t*** Erro: parametro invalido em grayscale_convert().");
    return;
  }

  SDL_Surface *surface = image->surface;
  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = (size_t)surface->w * (size_t)surface->h;
  Uint32 *pixels = (Uint32 *)surface->pixels;

  Uint8 r = 0, g = 0, b = 0, a = 0;

  for (size_t i = 0; i < pixelCount; ++i)
  {
    SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);

    // Formula exigida pelo enunciado: Y = 0.2125*R + 0.7154*G + 0.0721*B
    double y = 0.2125 * r + 0.7154 * g + 0.0721 * b;
    Uint8 gray = (Uint8)SDL_round(y);

    pixels[i] = SDL_MapRGBA(format, NULL, gray, gray, gray, a);
  }

  SDL_UnlockSurface(surface);

  image_refresh_texture(renderer, image);
}
