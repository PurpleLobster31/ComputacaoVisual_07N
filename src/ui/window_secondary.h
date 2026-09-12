#ifndef WINDOW_SECONDARY_H
#define WINDOW_SECONDARY_H

#include <stdbool.h>
#include "window.h"

enum
{
  SECONDARY_WINDOW_WIDTH = 400,
  SECONDARY_WINDOW_HEIGHT = 700,
};

//------------------------------------------------------------------------------
// Requisito 3: cria a janela secundária como filha de `parent`, com tamanho
// fixo (400x700) e posicionada em (0,0) da tela.
//------------------------------------------------------------------------------
bool window_secondary_create(SDL_Window *parent, MyWindow *out_secondary);

//------------------------------------------------------------------------------
// Libera renderer e window da janela secundária.
//------------------------------------------------------------------------------
void window_secondary_destroy(MyWindow *secondary);

#endif // WINDOW_SECONDARY_H