#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct TextLabel
{
  SDL_Texture *texture;
  float width, height;
} TextLabel;

// SDL e SDL_ttf devem estar inicializadas. Fonte relativa ao executavel.
TTF_Font *text_open_font(float size);
// Inicialize TextLabel com zero. Cada textura pertence a um renderizador.
bool text_label_set(TextLabel *label, SDL_Renderer *renderer, TTF_Font *font,
                    const char *text, SDL_Color color);
bool text_label_draw(SDL_Renderer *renderer, const TextLabel *label, float x, float y);
void text_label_destroy(TextLabel *label);

#endif
