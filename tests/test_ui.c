#include "ui/histogram_view.h"
#include "ui/button.h"
#include "image/image_loader.h"
#include "image/grayscale.h"
#include <SDL3_image/SDL_image.h>
#include <assert.h>
#include <stdio.h>

// Renderizacao em memoria: nao requer um servidor grafico para o teste.
int main(int argc, char **argv)
{
  assert(argc == 3);
  assert(SDL_Init(0));
  assert(TTF_Init());
  TTF_Font *font = text_open_font(20);
  if (!font) { SDL_Log("%s", SDL_GetError()); return 1; }
  SDL_Surface *canvas = SDL_CreateSurface(400, 700, SDL_PIXELFORMAT_RGBA32);
  assert(canvas);
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(canvas);
  assert(renderer);
  MyImage image = {0};
  assert(image_load_rgba32(argv[1], renderer, &image));
  grayscale_convert(renderer, &image);
  Histogram histogram;
  assert(histogram_compute(image.surface, &histogram));
  HistogramView view = {0};
  assert(histogram_view_update(&view, renderer, font, &histogram));
  // Atualizacao repetida libera as texturas antigas e mantem as estatisticas.
  assert(histogram_view_update(&view, renderer, font, &histogram));
  Button equalize = {0}, resolution = {0};
  button_init(&equalize, 20, 550, 360, 50, "Equalizar");
  button_init(&resolution, 20, 615, 360, 50, "Resolução original");
  SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
  SDL_RenderClear(renderer);
  assert(histogram_view_render(renderer, &view));
  assert(button_render(renderer, &equalize, font));
  SDL_Texture *cached = equalize.text.texture;
  assert(button_render(renderer, &equalize, font));
  assert(cached == equalize.text.texture);
  button_set_label(&equalize, "Ver original");
  assert(equalize.text.texture == NULL);
  assert(button_render(renderer, &equalize, font));
  button_set_label(&equalize, "Equalizar");
  assert(button_render(renderer, &equalize, font));
  assert(button_render(renderer, &resolution, font));
  assert(equalize.text.width < 340 && resolution.text.width < 340);
  assert(equalize.text.height < 50 && resolution.text.height < 50);
  button_set_label(&resolution, "Resolução original");
  assert(button_render(renderer, &resolution, font));
  assert(SDL_RenderPresent(renderer));
  assert(IMG_SavePNG(canvas, argv[2]));
  button_destroy(&equalize);
  button_destroy(&resolution);
  histogram_view_destroy(&view);
  image_destroy(&image);
  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroySurface(canvas);
  TTF_Quit();
  SDL_Quit();
  puts("Interface: fonte, acentos, cache, atualizacao, painel e PNG OK.");
  return 0;
}
