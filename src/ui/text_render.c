#include "text_render.h"

TTF_Font *text_open_font(float size)
{
  const char *base = SDL_GetBasePath();
  if (!base) return NULL;
  char *path = NULL;
  if (SDL_asprintf(&path, "%sassets/fonts/PixelifySans-VariableFont_wght.ttf", base) < 0)
    return NULL;
  TTF_Font *font = TTF_OpenFont(path, size);
  if (!font) SDL_Log("Erro ao carregar fonte '%s': %s", path, SDL_GetError());
  SDL_free(path);
  return font;
}

bool text_label_set(TextLabel *label, SDL_Renderer *renderer, TTF_Font *font,
                    const char *text, SDL_Color color)
{
  if (!label || !renderer || !font || !text || !*text)
    return SDL_SetError("Parametros invalidos para renderizar texto.");
  SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
  if (!surface) return false;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  float width = (float)surface->w, height = (float)surface->h;
  SDL_DestroySurface(surface);
  if (!texture) return false;
  text_label_destroy(label);
  *label = (TextLabel){texture, width, height};
  return true;
}

bool text_label_draw(SDL_Renderer *renderer, const TextLabel *label, float x, float y)
{
  if (!label || !label->texture) return SDL_SetError("Texto nao preparado.");
  SDL_FRect rect = {x, y, label->width, label->height};
  return SDL_RenderTexture(renderer, label->texture, NULL, &rect);
}

void text_label_destroy(TextLabel *label)
{
  if (!label) return;
  SDL_DestroyTexture(label->texture);
  *label = (TextLabel){0};
}
