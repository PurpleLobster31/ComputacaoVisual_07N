#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>

//------------------------------------------------------------------------------
// Struct compartilhada entre a janela principal e a secundária.
//------------------------------------------------------------------------------
typedef struct MyWindow MyWindow;
struct MyWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
};

#endif // WINDOW_H