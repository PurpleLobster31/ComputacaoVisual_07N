#include "resolution.h"

bool resolution_plan(int imageWidth, int imageHeight, bool original,
                     const SDL_Rect *display, SDL_Rect *layout)
{
  if (imageWidth <= 0 || imageHeight <= 0 || !display || !layout ||
      display->w <= 0 || display->h <= 0)
    return SDL_SetError("Dimensoes invalidas para planejar a resolucao.");
  SDL_Rect next = {0, 0, original ? imageWidth : DEFAULT_IMAGE_WIDTH,
                          original ? imageHeight : DEFAULT_IMAGE_HEIGHT};
  if (next.w <= display->w && next.h <= display->h)
  {
    next.x = display->x + (display->w - next.w) / 2;
    next.y = display->y + (display->h - next.h) / 2;
  }
  *layout = next;
  return true;
}

const char *resolution_button_label(bool original)
{
  return original ? "1024x768" : "Resolução original";
}

bool resolution_apply(SDL_Window *window, int imageWidth, int imageHeight, bool original)
{
  if (!window) return SDL_SetError("Janela principal ausente.");
  SDL_DisplayID primary = SDL_GetPrimaryDisplay();
  SDL_Rect display, next, previous;
  if (!primary || !SDL_GetDisplayBounds(primary, &display) ||
      !resolution_plan(imageWidth, imageHeight, original, &display, &next) ||
      !SDL_GetWindowSize(window, &previous.w, &previous.h)) return false;
  bool hasPreviousPosition = SDL_GetWindowPosition(window, &previous.x, &previous.y);

  // Tamanho/posicao nao sao aplicados a janelas maximizadas ou em fullscreen.
  SDL_WindowFlags flags = SDL_GetWindowFlags(window);
  if ((flags & SDL_WINDOW_FULLSCREEN) &&
      (!SDL_SetWindowFullscreen(window, false) || !SDL_SyncWindow(window))) return false;
  if ((flags & (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED)) &&
      (!SDL_RestoreWindow(window) || !SDL_SyncWindow(window))) return false;

  // Cocoa pode limitar janelas redimensionaveis ao desktop durante setFrame.
  // Aplicar como tamanho fixo e restaurar a interacao evita essa limitacao.
  bool wasResizable = (SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE) != 0;
  if (wasResizable && !SDL_SetWindowResizable(window, false)) return false;
  SDL_SetWindowPosition(window, next.x, next.y);
  bool resized = SDL_SetWindowSize(window, next.w, next.h) && SDL_SyncWindow(window);
  char resizeError[512] = "";
  if (!resized) SDL_strlcpy(resizeError, SDL_GetError(), sizeof resizeError);
  // Reposicionar ainda como tamanho fixo: Cocoa pode limitar a altura ao mover
  // uma janela redimensionavel, mesmo depois de aceitar o tamanho solicitado.
  if (resized && (!SDL_SetWindowPosition(window, next.x, next.y) || !SDL_SyncWindow(window)))
    SDL_Log("Aviso: tamanho alterado, mas o SO nao confirmou a posicao (%d,%d): %s",
            next.x, next.y, SDL_GetError());
  if (wasResizable && !SDL_SetWindowResizable(window, true))
  {
    if (resized) SDL_strlcpy(resizeError, SDL_GetError(), sizeof resizeError);
    resized = false;
  }
  if (!resized) SDL_SetError("%s", resizeError);
  int actualWidth = 0, actualHeight = 0;
  if (resized)
  {
    resized = SDL_GetWindowSize(window, &actualWidth, &actualHeight);
    if (resized && (actualWidth != next.w || actualHeight != next.h))
      resized = SDL_SetError("SO limitou o tamanho pedido (%dx%d) para %dx%d.",
                             next.w, next.h, actualWidth, actualHeight);
  }
  if (!resized)
  {
    char reason[512];
    SDL_strlcpy(reason, SDL_GetError(), sizeof reason);
    if (!SDL_SetWindowSize(window, previous.w, previous.h) || !SDL_SyncWindow(window))
      SDL_Log("Aviso: nao foi possivel restaurar o tamanho anterior: %s", SDL_GetError());
    if (hasPreviousPosition) SDL_SetWindowPosition(window, previous.x, previous.y);
    return SDL_SetError("Falha ao alterar resolucao: %s", reason);
  }

  int x, y;
  if (SDL_GetWindowPosition(window, &x, &y) && (x != next.x || y != next.y))
    SDL_Log("Aviso: SO ajustou a posicao solicitada (%d,%d) para (%d,%d).", next.x, next.y, x, y);
  SDL_Log("Resolucao %s: %dx%d; posicao solicitada: (%d,%d).",
          original ? "original" : "1024x768", next.w, next.h, next.x, next.y);
  return true;
}
