#include "grayscale.h"
#include <string.h>

//------------------------------------------------------------------------------
bool grayscale_is_image_grayscale(const MyImage *image)
{
  if (!image || !image->surface || image->surface->format != SDL_PIXELFORMAT_RGBA32)
  {
    SDL_Log("\t*** Erro: imagem invalida em grayscale_is_image_grayscale().");
    return false;
  }

  // SDL_LockSurface/UnlockSurface tambem funcionam em const-cast local, pois
  // aqui so fazemos leitura dos pixels.
  SDL_Surface *surface = image->surface;
  if (!SDL_LockSurface(surface)) return false;

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  bool isGrayscale = true;
  for (int y = 0; y < surface->h && isGrayscale; ++y)
  {
    const Uint8 *row = (const Uint8 *)surface->pixels + (size_t)y * surface->pitch;
    for (int x = 0; x < surface->w; ++x)
    {
      Uint32 pixel;
      Uint8 r, g, b, a;
      memcpy(&pixel, row + (size_t)x * 4, 4);
      SDL_GetRGBA(pixel, format, NULL, &r, &g, &b, &a);
      if (r != g || g != b) { isGrayscale = false; break; }
    }
  }

  SDL_UnlockSurface(surface);
  return isGrayscale;
}

//------------------------------------------------------------------------------
bool grayscale_convert(SDL_Renderer *renderer, MyImage *image)
{
  if (!renderer || !image || !image->surface || image->surface->format != SDL_PIXELFORMAT_RGBA32)
  {
    SDL_Log("\t*** Erro: parametro invalido em grayscale_convert().");
    return SDL_SetError("Parametros invalidos para conversao em cinza.");
  }

  SDL_Surface *surface = image->surface;
  if (!SDL_LockSurface(surface)) return false;

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  for (int rowIndex = 0; rowIndex < surface->h; ++rowIndex)
  {
    Uint8 *row = (Uint8 *)surface->pixels + (size_t)rowIndex * surface->pitch;
    for (int x = 0; x < surface->w; ++x)
    {
      Uint32 pixel;
      Uint8 r, g, b, a;
      memcpy(&pixel, row + (size_t)x * 4, 4);
      SDL_GetRGBA(pixel, format, NULL, &r, &g, &b, &a);
      double y = 0.2125 * r + 0.7154 * g + 0.0721 * b;
      Uint8 gray = (Uint8)SDL_round(y);
      pixel = SDL_MapRGBA(format, NULL, gray, gray, gray, a);
      memcpy(row + (size_t)x * 4, &pixel, 4);
    }
  }

  SDL_UnlockSurface(surface);

  return image_refresh_texture(renderer, image);
}
