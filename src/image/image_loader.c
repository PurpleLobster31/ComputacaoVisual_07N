// Adaptado do exemplo "04-invert_image" fornecido pelo Prof. André Kishimoto
// (https://kishimoto.com.br/), licenciado sob Apache-2.0.

#include "image_loader.h"
#include <SDL3_image/SDL_image.h>

//------------------------------------------------------------------------------
bool image_load_rgba32(const char *filename, SDL_Renderer *renderer, MyImage *output_image)
{
  SDL_Log(">>> image_load_rgba32(\"%s\")", filename ? filename : "(null)");

  if (!filename || !renderer || !output_image)
  {
    SDL_Log("\t*** Erro: parametro invalido (filename, renderer ou output_image == NULL).");
    SDL_Log("<<< image_load_rgba32()");
    return false;
  }

  image_destroy(output_image);

  SDL_Surface *loaded = IMG_Load(filename);
  if (!loaded)
  {
    // Requisito 1: cobre tanto "arquivo nao encontrado" quanto "formato invalido",
    // ja que IMG_Load retorna NULL para ambos os casos.
    SDL_Log("\t*** Erro ao carregar a imagem '%s': %s", filename, SDL_GetError());
    SDL_Log("<<< image_load_rgba32()");
    return false;
  }

  output_image->surface = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(loaded);
  if (!output_image->surface)
  {
    SDL_Log("\t*** Erro ao converter a imagem para RGBA32: %s", SDL_GetError());
    SDL_Log("<<< image_load_rgba32()");
    return false;
  }

  output_image->texture = SDL_CreateTextureFromSurface(renderer, output_image->surface);
  if (!output_image->texture)
  {
    SDL_Log("\t*** Erro ao criar a texture: %s", SDL_GetError());
    SDL_Log("<<< image_load_rgba32()");
    return false;
  }

  output_image->rect.x = 0.0f;
  output_image->rect.y = 0.0f;
  SDL_GetTextureSize(output_image->texture, &output_image->rect.w, &output_image->rect.h);

  SDL_Log("\tImagem '%s' carregada (%dx%d).", filename,
    output_image->surface->w, output_image->surface->h);
  SDL_Log("<<< image_load_rgba32()");
  return true;
}

//------------------------------------------------------------------------------
void image_destroy(MyImage *image)
{
  if (!image)
  {
    return;
  }

  if (image->texture)
  {
    SDL_DestroyTexture(image->texture);
    image->texture = NULL;
  }

  if (image->surface)
  {
    SDL_DestroySurface(image->surface);
    image->surface = NULL;
  }

  image->rect.x = image->rect.y = image->rect.w = image->rect.h = 0.0f;
}

//------------------------------------------------------------------------------
bool image_refresh_texture(SDL_Renderer *renderer, MyImage *image)
{
  if (!renderer || !image || !image->surface)
    return SDL_SetError("Parametros invalidos para recriar textura.");
  SDL_Texture *next = SDL_CreateTextureFromSurface(renderer, image->surface);
  if (!next) return false;
  SDL_DestroyTexture(image->texture);
  image->texture = next;
  return true;
}
