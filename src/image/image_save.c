#include "image_save.h"
#include <SDL3_image/SDL_image.h>

bool image_save_requested(const SDL_Event *event)
{
  return event && event->type == SDL_EVENT_KEY_DOWN &&
         event->key.key == SDLK_S && !event->key.repeat;
}

bool image_save_png(const MyImage *image, const char *path, bool *overwritten)
{
  if (!image || !image->surface || !path || !*path)
    return SDL_SetError("Imagem ou caminho invalido para salvamento.");
  bool existed = SDL_GetPathInfo(path, NULL);
  SDL_ClearError();
  if (!IMG_SavePNG(image->surface, path))
  {
    SDL_Log("Erro ao salvar '%s': %s", path, SDL_GetError());
    return false;
  }
  if (overwritten) *overwritten = existed;
  SDL_Log("Arquivo '%s' %s (%dx%d).", path,
          existed ? "sobrescrito" : "criado", image->surface->w, image->surface->h);
  return true;
}
