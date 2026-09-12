#include "window_secondary.h"

//------------------------------------------------------------------------------
bool window_secondary_create(SDL_Window *parent, MyWindow *out_secondary)
{
  if (!parent || !out_secondary)
  {
    SDL_Log("\t*** Erro: parametro invalido em window_secondary_create().");
    return false;
  }

  if (!SDL_CreateWindowAndRenderer("Histograma",
    SECONDARY_WINDOW_WIDTH, SECONDARY_WINDOW_HEIGHT, 0,
    &out_secondary->window, &out_secondary->renderer))
  {
    SDL_Log("\t*** Erro ao criar a janela secundaria: %s", SDL_GetError());
    return false;
  }

  // Requisito 3: janela secundaria e filha da principal.
  if (!SDL_SetWindowParent(out_secondary->window, parent))
  {
    SDL_Log("\t*** Aviso: nao foi possivel definir a janela secundaria como filha da principal: %s",
      SDL_GetError());
  }

  // Requisito 3: posicionada em (0,0) da tela.
  SDL_SetWindowPosition(out_secondary->window, 0, 0);
  SDL_SetWindowResizable(out_secondary->window, false);

  return true;
}

//------------------------------------------------------------------------------
void window_secondary_destroy(MyWindow *secondary)
{
  if (!secondary)
  {
    return;
  }

  if (secondary->renderer)
  {
    SDL_DestroyRenderer(secondary->renderer);
    secondary->renderer = NULL;
  }

  if (secondary->window)
  {
    SDL_DestroyWindow(secondary->window);
    secondary->window = NULL;
  }
}
