#ifndef HISTOGRAM_VIEW_H
#define HISTOGRAM_VIEW_H

#include "text_render.h"
#include "../image/histogram.h"

typedef struct HistogramView
{
  Histogram histogram;
  TextLabel labels[12];
} HistogramView;

// Inicialize com zero; atualizar somente quando a imagem mudar.
bool histogram_view_update(HistogramView *view, SDL_Renderer *renderer,
                           TTF_Font *font, const Histogram *histogram);
bool histogram_view_render(SDL_Renderer *renderer, const HistogramView *view);
void histogram_view_destroy(HistogramView *view);

#endif
