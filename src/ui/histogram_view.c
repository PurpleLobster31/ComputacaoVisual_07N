#include "histogram_view.h"
#include <inttypes.h>
#include <stdio.h>

enum { TITLE, FREQUENCY, PEAK, ZERO, MIDDLE, LAST, AXIS, MEAN, DEVIATION,
       BRIGHTNESS, CONTRAST, COUNT };

bool histogram_view_update(HistogramView *view, SDL_Renderer *renderer,
                           TTF_Font *font, const Histogram *histogram)
{
  if (!view || !histogram || !histogram->total || !histogram->peak)
    return SDL_SetError("Histograma vazio para exibicao.");
  HistogramView next = {0};
  next.histogram = *histogram;
  char strings[12][96] = {"Histograma", "Frequência (pixels)", "", "0", "128", "255",
                          "Intensidade de cinza"};
  snprintf(strings[PEAK], sizeof strings[PEAK], "Pico: %" PRIu64, histogram->peak);
  snprintf(strings[MEAN], sizeof strings[MEAN], "Média: %.2f", histogram->mean);
  snprintf(strings[DEVIATION], sizeof strings[DEVIATION], "Desvio padrão: %.2f", histogram->stddev);
  snprintf(strings[BRIGHTNESS], sizeof strings[BRIGHTNESS], "Imagem: %s", histogram_brightness(histogram->mean));
  snprintf(strings[CONTRAST], sizeof strings[CONTRAST], "Contraste: %s", histogram_contrast(histogram->stddev));
  snprintf(strings[COUNT], sizeof strings[COUNT], "Pixels: %" PRIu64, histogram->total);
  SDL_Color ink = {35, 45, 60, 255};
  for (int i = 0; i < 12; ++i)
  {
    if (!text_label_set(&next.labels[i], renderer, font, strings[i], ink))
    {
      histogram_view_destroy(&next);
      return false;
    }
  }
  histogram_view_destroy(view);
  *view = next;
  return true;
}

bool histogram_view_render(SDL_Renderer *renderer, const HistogramView *view)
{
  if (!renderer || !view || !view->histogram.peak)
    return SDL_SetError("Painel de histograma invalido.");
  const SDL_FRect plot = {40, 112, 320, 220};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &plot);
  SDL_SetRenderDrawColor(renderer, 218, 225, 233, 255);
  for (int i = 0; i <= 4; ++i)
  {
    float y = plot.y + plot.h * i / 4.0f;
    SDL_RenderLine(renderer, plot.x, y, plot.x + plot.w, y);
  }
  SDL_SetRenderDrawColor(renderer, 40, 110, 220, 255);
  for (int i = 0; i < 256; ++i)
  {
    float height = (float)((double)view->histogram.bins[i] / (double)view->histogram.peak * plot.h);
    if (height <= 0) continue;
    SDL_FRect bar = {plot.x + plot.w * i / 256.0f, plot.y + plot.h - height,
                     plot.w / 256.0f, height};
    SDL_RenderFillRect(renderer, &bar);
  }
  SDL_SetRenderDrawColor(renderer, 70, 80, 95, 255);
  SDL_RenderRect(renderer, &plot);
  const float positions[12][2] = {
    {20, 18}, {20, 52}, {20, 80}, {40, 338}, {183, 338}, {330, 338},
    {85, 367}, {20, 422}, {20, 452}, {20, 482}, {20, 512}, {20, 394}
  };
  for (int i = 0; i < 12; ++i)
    if (!text_label_draw(renderer, &view->labels[i], positions[i][0], positions[i][1])) return false;
  return true;
}

void histogram_view_destroy(HistogramView *view)
{
  if (!view) return;
  for (int i = 0; i < 12; ++i) text_label_destroy(&view->labels[i]);
  *view = (HistogramView){0};
}
