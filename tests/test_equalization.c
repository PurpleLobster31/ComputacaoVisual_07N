// Inclui o coordenador para exercitar a mesma alternancia usada pelo clique,
// sem abrir janelas e sem adicionar uma API de teste ao programa.
#define SDL_MAIN_HANDLED
#define main application_main
#include "../src/main.c"
#undef main
#include <SDL3_image/SDL_image.h>
#include <assert.h>
#include <string.h>

static void create_pixels(const Uint8 *values, int count)
{
  image_destroy(&g_equalizedImage);
  image_destroy(&g_image);
  g_isEqualized = false;
  g_image.surface = SDL_CreateSurface(count, 1, SDL_PIXELFORMAT_RGBA32);
  assert(g_image.surface);
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(g_image.surface->format);
  for (int i = 0; i < count; ++i)
  {
    Uint32 pixel = SDL_MapRGBA(format, NULL, values[i], values[i], values[i], (Uint8)i);
    memcpy((Uint8 *)g_image.surface->pixels + i * 4, &pixel, 4);
  }
  g_image.texture = SDL_CreateTextureFromSurface(g_window.renderer, g_image.surface);
  assert(g_image.texture);
  g_image.rect = (SDL_FRect){0, 0, count, 1};
}

static void check_values(SDL_Surface *surface, const Uint8 *values, int count)
{
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  for (int i = 0; i < count; ++i)
  {
    Uint32 pixel;
    Uint8 r, g, b, a;
    memcpy(&pixel, (Uint8 *)surface->pixels + i * 4, 4);
    SDL_GetRGBA(pixel, format, NULL, &r, &g, &b, &a);
    assert(r == values[i] && g == r && b == r && a == (Uint8)i);
  }
}

static void click_equalize(void)
{
  SDL_Event event = {0};
  bool clicked = false;
  event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
  event.button.windowID = 42;
  event.button.button = SDL_BUTTON_LEFT;
  event.button.x = 30;
  event.button.y = 570;
  button_handle_event(&g_equalizeButton, &event, 42, &clicked);
  assert(!clicked);
  event.type = SDL_EVENT_MOUSE_BUTTON_UP;
  button_handle_event(&g_equalizeButton, &event, 42, &clicked);
  assert(clicked && toggle_equalization());
}

int main(int argc, char **argv)
{
  assert(argc == 3);
  assert(SDL_Init(0) && TTF_Init());
  g_ttfInitialized = true;
  SDL_Surface *mainCanvas = SDL_CreateSurface(1024, 768, SDL_PIXELFORMAT_RGBA32);
  SDL_Surface *uiCanvas = SDL_CreateSurface(400, 700, SDL_PIXELFORMAT_RGBA32);
  assert(mainCanvas && uiCanvas);
  g_window.renderer = SDL_CreateSoftwareRenderer(mainCanvas);
  g_secondaryWindow.renderer = SDL_CreateSoftwareRenderer(uiCanvas);
  assert(g_window.renderer && g_secondaryWindow.renderer);
  g_font = text_open_font(20);
  assert(g_font);
  button_init(&g_equalizeButton, 20, 550, 360, 50, "Equalizar");
  button_init(&g_resolutionButton, 20, 615, 360, 50, "Resolução original");

  const Uint8 source[] = {50, 50, 100, 150};
  const Uint8 expected[] = {0, 0, 128, 255};
  create_pixels(source, 4);
  SDL_Surface *originalSurface = g_image.surface;
  SDL_Texture *originalTexture = g_image.texture;
  click_equalize();
  assert(g_isEqualized && !strcmp(g_equalizeButton.label, "Ver original"));
  check_values(g_equalizedImage.surface, expected, 4);
  check_values(g_image.surface, source, 4);
  assert(g_histogramView.histogram.bins[0] == 2 && g_histogramView.histogram.bins[128] == 1);
  SDL_Surface *cachedSurface = g_equalizedImage.surface;
  SDL_Texture *cachedTexture = g_equalizedImage.texture;
  click_equalize();
  assert(!g_isEqualized && !strcmp(g_equalizeButton.label, "Equalizar"));
  assert(g_image.surface == originalSurface && g_image.texture == originalTexture);
  assert(g_histogramView.histogram.mean == 87.5 && g_histogramView.histogram.bins[50] == 2);
  check_values(g_image.surface, source, 4);
  click_equalize();
  assert(g_equalizedImage.surface == cachedSurface && g_equalizedImage.texture == cachedTexture);
  // Falha de preparacao nao pode deixar imagem e painel em estados diferentes.
  TTF_Font *font = g_font;
  g_font = NULL;
  assert(!toggle_equalization());
  assert(g_isEqualized && !strcmp(g_equalizeButton.label, "Ver original"));
  assert(g_histogramView.histogram.bins[0] == 2);
  g_font = font;
  assert(!equalization_create(g_window.renderer, &g_image, &g_image));
  assert(!equalization_create(NULL, &g_image, &g_equalizedImage));
  assert(g_equalizedImage.surface == cachedSurface);

  // Imagens constantes e distribuicao ja uniforme permanecem identicas.
  for (int value = 0; value <= 255; ++value)
  {
    Uint8 constant[] = {(Uint8)value};
    create_pixels(constant, 1);
    assert(equalization_create(g_window.renderer, &g_image, &g_equalizedImage));
    check_values(g_equalizedImage.surface, constant, 1);
  }
  Uint8 ramp[256];
  for (int i = 0; i < 256; ++i) ramp[i] = (Uint8)i;
  create_pixels(ramp, 256);
  assert(equalization_create(g_window.renderer, &g_image, &g_equalizedImage));
  check_values(g_equalizedImage.surface, ramp, 256);

  // Resultado real para inspecao visual. O arquivo de entrada e lido uma vez.
  image_destroy(&g_equalizedImage);
  assert(image_load_rgba32(argv[1], g_window.renderer, &g_image));
  grayscale_convert(g_window.renderer, &g_image);
  g_isEqualized = false;
  assert(toggle_equalization());
  assert(render_secondary_window());
  assert(IMG_SavePNG(uiCanvas, argv[2]));
  assert(toggle_equalization());
  shutdown();
  SDL_DestroySurface(mainCanvas);
  SDL_DestroySurface(uiCanvas);
  puts("Equalizacao: CDF, constantes, alfa, original intacta, cliques, cache e falha atomica OK.");
  return 0;
}
