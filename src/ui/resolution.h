#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <SDL3/SDL.h>

enum { DEFAULT_IMAGE_WIDTH = 1024, DEFAULT_IMAGE_HEIGHT = 768 };

// Calcula tamanho e posicao em coordenadas de janela/desktop da SDL.
bool resolution_plan(int imageWidth, int imageHeight, bool original,
                     const SDL_Rect *display, SDL_Rect *layout);
// Consulta o monitor principal a cada troca. Em falha de tamanho tenta restaurar
// a geometria anterior. Restricoes de posicionamento do SO geram aviso.
bool resolution_apply(SDL_Window *window, int imageWidth, int imageHeight, bool original);
const char *resolution_button_label(bool original);

#endif
