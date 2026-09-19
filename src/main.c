// Estrutura de janela/loop adaptada do exemplo "04-invert_image" fornecido
// pelo Prof. André Kishimoto (https://kishimoto.com.br/), licenciado sob
// Apache-2.0. Lógica de carregamento e escala de cinza movida para módulos
// próprios (src/image/).

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "image/image_loader.h"
#include "image/grayscale.h"
#include "image/equalization.h"
#include "image/image_save.h"
#include "ui/window.h"
#include "ui/window_secondary.h"
#include "ui/button.h"
#include "ui/histogram_view.h"
#include "ui/resolution.h"

//------------------------------------------------------------------------------
enum constants
{
  WINDOW_WIDTH = DEFAULT_IMAGE_WIDTH,  // Requisito 3: janela principal 1024x768 inicialmente.
  WINDOW_HEIGHT = DEFAULT_IMAGE_HEIGHT,
};

static MyWindow g_window = { .window = NULL, .renderer = NULL };
static MyWindow g_secondaryWindow = { .window = NULL, .renderer = NULL };
static MyImage g_image = {
  .surface = NULL,
  .texture = NULL,
  .rect = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f }
};

static MyImage g_equalizedImage;
static bool g_isEqualized;
static bool g_isOriginalResolution;

static HistogramView g_histogramView;
static TTF_Font *g_font;
static bool g_ttfInitialized;

static Button g_equalizeButton;
static Button g_resolutionButton;

//------------------------------------------------------------------------------
static void shutdown(void)
{
  image_destroy(&g_equalizedImage);
  image_destroy(&g_image);

  histogram_view_destroy(&g_histogramView);
  button_destroy(&g_equalizeButton);
  button_destroy(&g_resolutionButton);
  if (g_font) TTF_CloseFont(g_font);
  if (g_ttfInitialized) TTF_Quit();
  window_secondary_destroy(&g_secondaryWindow);

  if (g_window.renderer)
  {
    SDL_DestroyRenderer(g_window.renderer);
    g_window.renderer = NULL;
  }

  if (g_window.window)
  {
    SDL_DestroyWindow(g_window.window);
    g_window.window = NULL;
  }

  SDL_Quit();
}

//------------------------------------------------------------------------------
static void render_main_window(void)
{
  SDL_SetRenderDrawColor(g_window.renderer, 30, 30, 30, 255);
  SDL_RenderClear(g_window.renderer);
  MyImage *displayed = g_isEqualized ? &g_equalizedImage : &g_image;
  // Destino NULL ocupa toda a saida real do renderizador, inclusive em HiDPI.
  // A transformacao e apenas de exibicao; os pixels analisados ficam intactos.
  SDL_SetTextureScaleMode(displayed->texture, SDL_SCALEMODE_LINEAR);
  SDL_RenderTexture(g_window.renderer, displayed->texture, NULL, NULL);
  SDL_RenderPresent(g_window.renderer);
}

//------------------------------------------------------------------------------
static bool render_secondary_window(void)
{
  SDL_SetRenderDrawColor(g_secondaryWindow.renderer, 245, 245, 245, 255);
  SDL_RenderClear(g_secondaryWindow.renderer);

  if (!histogram_view_render(g_secondaryWindow.renderer, &g_histogramView) ||
      !button_render(g_secondaryWindow.renderer, &g_equalizeButton, g_font) ||
      !button_render(g_secondaryWindow.renderer, &g_resolutionButton, g_font))
  {
    SDL_Log("Erro ao desenhar interface: %s", SDL_GetError());
    return false;
  }
  return SDL_RenderPresent(g_secondaryWindow.renderer);
}

// Prepara todos os recursos antes de alterar o estado visivel.
static bool toggle_equalization(void)
{
  if (!g_isEqualized && !g_equalizedImage.surface &&
      !equalization_create(g_window.renderer, &g_image, &g_equalizedImage)) return false;

  bool nextState = !g_isEqualized;
  MyImage *nextImage = nextState ? &g_equalizedImage : &g_image;
  const char *label = nextState ? "Ver original" : "Equalizar";
  Histogram histogram;
  TextLabel nextText = {0};
  if (!histogram_compute(nextImage->surface, &histogram) ||
      !text_label_set(&nextText, g_secondaryWindow.renderer, g_font, label,
                      (SDL_Color){255, 255, 255, 255})) return false;
  if (!histogram_view_update(&g_histogramView, g_secondaryWindow.renderer, g_font, &histogram))
  {
    text_label_destroy(&nextText);
    return false;
  }
  button_set_label(&g_equalizeButton, label);
  g_equalizeButton.text = nextText;
  g_isEqualized = nextState;
  SDL_Log("Imagem %s: media=%.2f (%s), desvio=%.2f (%s).",
          nextState ? "equalizada" : "original em cinza", histogram.mean,
          histogram_brightness(histogram.mean), histogram.stddev, histogram_contrast(histogram.stddev));
  return true;
}

static bool toggle_resolution(void)
{
  if (!g_image.surface) return SDL_SetError("Imagem nao carregada.");
  bool nextState = !g_isOriginalResolution;
  const char *label = resolution_button_label(nextState);
  TextLabel nextText = {0};
  if (!text_label_set(&nextText, g_secondaryWindow.renderer, g_font, label,
                      (SDL_Color){255, 255, 255, 255})) return false;
  if (!resolution_apply(g_window.window, g_image.surface->w, g_image.surface->h, nextState))
  {
    text_label_destroy(&nextText);
    return false;
  }
  button_set_label(&g_resolutionButton, label);
  g_resolutionButton.text = nextText;
  g_isOriginalResolution = nextState;
  // Algumas plataformas movem a janela filha junto com a principal.
  if (!SDL_SetWindowPosition(g_secondaryWindow.window, 0, 0))
    SDL_Log("Aviso ao reposicionar janela secundaria: %s", SDL_GetError());
  return true;
}

static bool handle_save_event(const SDL_Event *event)
{
  if (!image_save_requested(event)) return true;
  const MyImage *active = g_isEqualized ? &g_equalizedImage : &g_image;
  return image_save_png(active, "output_image.png", NULL);
}

//------------------------------------------------------------------------------
static bool loop(void)
{
  SDL_Event event;
  bool isRunning = true;

  const SDL_WindowID secondaryWindowId = SDL_GetWindowID(g_secondaryWindow.window);

  render_main_window();
  if (!render_secondary_window()) return false;

  while (isRunning)
  {
    while (SDL_PollEvent(&event))
    {
      // Fechar qualquer uma das duas janelas encerra o programa.
      if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
      {
        isRunning = false;
        break;
      }

      // Tecla S funciona com qualquer uma das duas janelas em foco.
      // Uma falha de escrita e informada, mas nao encerra a sessao.
      handle_save_event(&event);

      bool equalizeClicked = false;
      bool resolutionClicked = false;
      button_handle_event(&g_equalizeButton, &event, secondaryWindowId, &equalizeClicked);
      button_handle_event(&g_resolutionButton, &event, secondaryWindowId, &resolutionClicked);

      if (equalizeClicked)
      {
        if (!toggle_equalization())
          SDL_Log("Erro ao alternar equalizacao: %s", SDL_GetError());
      }

      if (resolutionClicked)
      {
        if (!toggle_resolution())
          SDL_Log("Erro ao alternar resolucao: %s", SDL_GetError());
      }
    }

    render_main_window();
    if (!render_secondary_window()) return false;
    SDL_Delay(16);
  }
  return true;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  atexit(shutdown);

  if (argc < 2)
  {
    SDL_Log("Uso: %s <caminho_da_imagem>", argv[0]);
    return 1;
  }

  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("Erro ao iniciar a SDL: %s", SDL_GetError());
    return 1;
  }

  if (!SDL_CreateWindowAndRenderer("Proj1 - Processamento de Imagens",
    WINDOW_WIDTH, WINDOW_HEIGHT, 0, &g_window.window, &g_window.renderer))
  {
    SDL_Log("Erro ao criar a janela e/ou renderizador: %s", SDL_GetError());
    return 1;
  }

  // Requisito 3: janela principal centralizada no monitor principal.
  // Ajustavel pelo usuario (apenas a secundaria tem tamanho fixo, conforme
  // o enunciado).
  SDL_SetWindowResizable(g_window.window, true);
  if (!resolution_apply(g_window.window, WINDOW_WIDTH, WINDOW_HEIGHT, false))
  {
    SDL_Log("Erro ao preparar janela principal: %s", SDL_GetError());
    return 1;
  }

  // Requisito 1: carregamento com tratamento de erro.
  if (!image_load_rgba32(argv[1], g_window.renderer, &g_image))
  {
    SDL_Log("Nao foi possivel carregar a imagem '%s'.", argv[1]);
    return 1;
  }

  // Requisito 2: deteccao + conversao para escala de cinza.
  if (grayscale_is_image_grayscale(&g_image))
  {
    SDL_Log("Imagem de entrada: escala de cinza.");
  }
  else
  {
    SDL_Log("Imagem de entrada: colorida. Convertendo para escala de cinza...");
    if (!grayscale_convert(g_window.renderer, &g_image))
    {
      SDL_Log("Erro ao converter imagem para cinza: %s", SDL_GetError());
      return 1;
    }
  }

  // Requisito 3: janela secundaria, filha da principal.
  if (!window_secondary_create(g_window.window, &g_secondaryWindow))
  {
    SDL_Log("Erro ao criar a janela secundaria.");
    return 1;
  }

  if (!TTF_Init())
  {
    SDL_Log("Erro ao iniciar SDL_ttf: %s", SDL_GetError());
    return 1;
  }
  g_ttfInitialized = true;
  g_font = text_open_font(20.0f);
  if (!g_font) return 1;

  Histogram histogram;
  if (!histogram_compute(g_image.surface, &histogram) ||
      !histogram_view_update(&g_histogramView, g_secondaryWindow.renderer, g_font, &histogram))
  {
    SDL_Log("Erro ao preparar histograma: %s", SDL_GetError());
    return 1;
  }
  SDL_Log("Histograma: media=%.2f (%s), desvio=%.2f (%s).", histogram.mean,
          histogram_brightness(histogram.mean), histogram.stddev, histogram_contrast(histogram.stddev));

  // Botoes abaixo do histograma e suas estatisticas.
  button_init(&g_equalizeButton, 20.0f, 550.0f, 360.0f, 50.0f, "Equalizar");
  button_init(&g_resolutionButton, 20.0f, 615.0f, 360.0f, 50.0f, resolution_button_label(g_isOriginalResolution));

  return loop() ? 0 : 1;
}