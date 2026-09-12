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
#include "ui/window.h"
#include "ui/window_secondary.h"
#include "ui/button.h"

//------------------------------------------------------------------------------
enum constants
{
  WINDOW_WIDTH = 1024,  // Requisito 3: janela principal 1024x768 inicialmente.
  WINDOW_HEIGHT = 768,
};

static MyWindow g_window = { .window = NULL, .renderer = NULL };
static MyWindow g_secondaryWindow = { .window = NULL, .renderer = NULL };
static MyImage g_image = {
  .surface = NULL,
  .texture = NULL,
  .rect = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f }
};

static Button g_equalizeButton;
static Button g_resolutionButton;

//------------------------------------------------------------------------------
static void shutdown(void)
{
  image_destroy(&g_image);

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
  SDL_RenderTexture(g_window.renderer, g_image.texture, NULL, &g_image.rect);
  SDL_RenderPresent(g_window.renderer);
}

//------------------------------------------------------------------------------
static void render_secondary_window(void)
{
  SDL_SetRenderDrawColor(g_secondaryWindow.renderer, 245, 245, 245, 255);
  SDL_RenderClear(g_secondaryWindow.renderer);

  // TODO (Fase 3): desenhar o histograma e as informacoes de media/desvio
  // padrao acima dos botoes.

  button_render(g_secondaryWindow.renderer, &g_equalizeButton);
  button_render(g_secondaryWindow.renderer, &g_resolutionButton);

  SDL_RenderPresent(g_secondaryWindow.renderer);
}

//------------------------------------------------------------------------------
static void loop(void)
{
  SDL_Event event;
  bool isRunning = true;

  const SDL_WindowID secondaryWindowId = SDL_GetWindowID(g_secondaryWindow.window);

  render_main_window();
  render_secondary_window();

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

      bool equalizeClicked = false;
      bool resolutionClicked = false;
      button_handle_event(&g_equalizeButton, &event, secondaryWindowId, &equalizeClicked);
      button_handle_event(&g_resolutionButton, &event, secondaryWindowId, &resolutionClicked);

      if (equalizeClicked)
      {
        // TODO (Fase 4): alternar entre imagem equalizada e original,
        // atualizar botao para "Ver original" / "Equalizar".
        SDL_Log("Botao 'Equalizar' clicado.");
      }

      if (resolutionClicked)
      {
        // TODO (Fase 5): alternar resolucao original <-> 1024x768 e
        // redimensionar/reposicionar a janela principal.
        SDL_Log("Botao de resolucao clicado.");
      }
    }

    render_main_window();
    render_secondary_window();
  }
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
  SDL_SetWindowPosition(g_window.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_SetWindowResizable(g_window.window, true);

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
    grayscale_convert(g_window.renderer, &g_image);
  }

  // Requisito 3: janela secundaria, filha da principal.
  if (!window_secondary_create(g_window.window, &g_secondaryWindow))
  {
    SDL_Log("Erro ao criar a janela secundaria.");
    return 1;
  }

  // Botoes posicionados dentro da janela secundaria (400x700). O espaco
  // acima deles (y < 540) fica reservado para o histograma (Fase 3).
  button_init(&g_equalizeButton, 20.0f, 550.0f, 360.0f, 50.0f, "Equalizar");
  button_init(&g_resolutionButton, 20.0f, 615.0f, 360.0f, 50.0f, "1024x768");

  loop();

  return 0;
}