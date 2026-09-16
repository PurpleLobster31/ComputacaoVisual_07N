#define SDL_MAIN_HANDLED
#define main application_main
#include "../src/main.c"
#undef main
#include <assert.h>
#include <string.h>

static void geometry_tests(void)
{
  SDL_Rect display = {0, 0, 1920, 1080}, result;
  assert(resolution_plan(768, 512, false, &display, &result));
  assert(result.w == 1024 && result.h == 768 && result.x == 448 && result.y == 156);
  assert(resolution_plan(768, 512, true, &display, &result));
  assert(result.w == 768 && result.h == 512 && result.x == 576 && result.y == 284);
  display = (SDL_Rect){-1920, 100, 1920, 1080};
  assert(resolution_plan(768, 512, true, &display, &result));
  assert(result.x == -1344 && result.y == 384);
  assert(resolution_plan(1920, 1080, true, &display, &result));
  assert(result.x == -1920 && result.y == 100);
  assert(resolution_plan(1921, 100, true, &display, &result));
  assert(result.x == 0 && result.y == 0 && result.w == 1921);
  assert(resolution_plan(100, 1081, true, &display, &result));
  assert(result.x == 0 && result.y == 0 && result.h == 1081);
  display = (SDL_Rect){0, 0, 800, 600};
  assert(resolution_plan(20, 20, false, &display, &result));
  assert(result.x == 0 && result.y == 0 && result.w == 1024 && result.h == 768);
  display = (SDL_Rect){0, 0, 1921, 1081};
  assert(resolution_plan(768, 512, true, &display, &result));
  assert(result.x == 576 && result.y == 284);
  SDL_Rect previous = result;
  assert(!resolution_plan(0, 10, true, &display, &result));
  assert(!memcmp(&previous, &result, sizeof result));
  assert(!resolution_plan(10, 10, true, NULL, &result));
  assert(!resolution_plan(10, 10, true, &display, NULL));
  assert(!strcmp(resolution_button_label(false), "Resolução original"));
  assert(!strcmp(resolution_button_label(true), "1024x768"));
  puts("Resolucao: tamanhos, centralizacao, limites, monitores deslocados e rotulos OK.");
}

static void rendering_test(void)
{
  assert(SDL_Init(0));
  SDL_Surface *canvas = SDL_CreateSurface(10, 8, SDL_PIXELFORMAT_RGBA32);
  SDL_Surface *source = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_RGBA32);
  assert(canvas && source);
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(source->format);
  assert(SDL_FillSurfaceRect(source, NULL, SDL_MapRGBA(format, NULL, 127, 127, 127, 255)));
  g_window.renderer = SDL_CreateSoftwareRenderer(canvas);
  assert(g_window.renderer);
  g_image.texture = SDL_CreateTextureFromSurface(g_window.renderer, source);
  assert(g_image.texture);
  g_image.rect = (SDL_FRect){0, 0, 2, 2};
  render_main_window();
  // O ultimo pixel tambem deve estar preenchido, fora do retangulo original 2x2.
  Uint32 last;
  Uint8 r, g, b, a;
  memcpy(&last, (Uint8 *)canvas->pixels + 7 * canvas->pitch + 9 * 4, 4);
  SDL_GetRGBA(last, format, NULL, &r, &g, &b, &a);
  assert(r == 127 && g == 127 && b == 127 && a == 255);
  image_destroy(&g_image);
  SDL_DestroyRenderer(g_window.renderer);
  g_window.renderer = NULL;
  SDL_DestroySurface(source);
  SDL_DestroySurface(canvas);
  SDL_Quit();
  puts("Renderizacao: imagem preenche a saida completa, sem recorte pelo tamanho original.");
}

static void window_size(int width, int height)
{
  int w, h;
  assert(SDL_GetWindowSize(g_window.window, &w, &h));
  assert(w == width && h == height);
  const char *driver = SDL_GetCurrentVideoDriver();
  if (driver && strcmp(driver, "wayland") != 0)
  {
    SDL_Rect display, expected;
    int x, y;
    assert(SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display));
    assert(resolution_plan(width, height, true, &display, &expected));
    assert(SDL_GetWindowPosition(g_window.window, &x, &y));
    assert(x == expected.x && y == expected.y);
  }
}

static void click_resolution(void)
{
  SDL_Event event = {0};
  bool clicked;
  event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
  event.button.windowID = SDL_GetWindowID(g_secondaryWindow.window);
  event.button.button = SDL_BUTTON_LEFT;
  event.button.x = 30;
  event.button.y = 635;
  button_handle_event(&g_resolutionButton, &event, event.button.windowID, &clicked);
  assert(!clicked);
  event.type = SDL_EVENT_MOUSE_BUTTON_UP;
  button_handle_event(&g_resolutionButton, &event, event.button.windowID, &clicked);
  assert(clicked && toggle_resolution());
  assert(SDL_GetWindowFlags(g_window.window) & SDL_WINDOW_RESIZABLE);
}

static void window_tests(const char *imagePath)
{
  assert(SDL_Init(SDL_INIT_VIDEO));
  assert(TTF_Init());
  g_ttfInitialized = true;
  assert(SDL_CreateWindowAndRenderer("Teste de resolucao", 1024, 768, SDL_WINDOW_RESIZABLE,
                                    &g_window.window, &g_window.renderer));
  assert(resolution_apply(g_window.window, 768, 512, false));
  assert(window_secondary_create(g_window.window, &g_secondaryWindow));
  assert(SDL_GetWindowParent(g_secondaryWindow.window) == g_window.window);
  assert(!(SDL_GetWindowFlags(g_secondaryWindow.window) & SDL_WINDOW_RESIZABLE));
  int childWidth, childHeight, childX, childY;
  assert(SDL_SyncWindow(g_secondaryWindow.window));
  assert(SDL_GetWindowSize(g_secondaryWindow.window, &childWidth, &childHeight));
  assert(childWidth == 400 && childHeight == 700);
  assert(SDL_GetWindowPosition(g_secondaryWindow.window, &childX, &childY));
  SDL_Log("Auditoria janela filha: 400x700 fixa; posicao efetiva (%d,%d).", childX, childY);
  g_font = text_open_font(20);
  assert(g_font);
  assert(image_load_rgba32(imagePath, g_window.renderer, &g_image));
  grayscale_convert(g_window.renderer, &g_image);
  Histogram original;
  assert(histogram_compute(g_image.surface, &original));
  assert(histogram_view_update(&g_histogramView, g_secondaryWindow.renderer, g_font, &original));
  button_init(&g_equalizeButton, 20, 550, 360, 50, "Equalizar");
  button_init(&g_resolutionButton, 20, 615, 360, 50, resolution_button_label(false));
  SDL_Surface *originalSurface = g_image.surface;
  window_size(1024, 768);
  render_main_window();
  assert(render_secondary_window());
  click_resolution();
  window_size(g_image.surface->w, g_image.surface->h);
  assert(g_isOriginalResolution && !strcmp(g_resolutionButton.label, "1024x768"));
  assert(g_image.surface == originalSurface && !g_isEqualized);
  assert(!memcmp(&original, &g_histogramView.histogram, sizeof original));
  assert(toggle_equalization());
  SDL_Surface *equalizedSurface = g_equalizedImage.surface;
  Histogram equalized = g_histogramView.histogram;
  click_resolution();
  window_size(1024, 768);
  assert(!g_isOriginalResolution && !strcmp(g_resolutionButton.label, "Resolução original"));
  assert(g_isEqualized && g_equalizedImage.surface == equalizedSurface);
  assert(!memcmp(&equalized, &g_histogramView.histogram, sizeof equalized));
  assert(toggle_equalization());
  assert(g_image.surface == originalSurface && !g_isEqualized);
  window_size(1024, 768);
  // Redimensionamento manual seguido de clique deve voltar ao modo escolhido.
  assert(SDL_SetWindowSize(g_window.window, 600, 400) && SDL_SyncWindow(g_window.window));
  render_main_window();
  click_resolution();
  window_size(g_image.surface->w, g_image.surface->h);
  TTF_Font *font = g_font;
  g_font = NULL;
  assert(!toggle_resolution());
  assert(g_isOriginalResolution && !strcmp(g_resolutionButton.label, "1024x768"));
  window_size(g_image.surface->w, g_image.surface->h);
  g_font = font;
  click_resolution();
  // Pedido maior que o monitor: o SO pode aceitar ou limitar o tamanho.
  SDL_Rect display;
  assert(SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display));
  if (resolution_apply(g_window.window, display.w + 100, display.h + 100, true))
  {
    int w, h;
    assert(SDL_GetWindowSize(g_window.window, &w, &h));
    assert(w == display.w + 100 && h == display.h + 100);
    puts("Janela maior que o monitor aceita; posicao solicitada (0,0).");
  }
  else
  {
    SDL_Log("Teste de limite do SO: %s", SDL_GetError());
    window_size(1024, 768);
  }
  assert(resolution_apply(g_window.window, g_image.surface->w, g_image.surface->h, false));
  window_size(1024, 768);
  render_main_window();
  assert(render_secondary_window());
  shutdown();
  puts("Janelas: cliques, dimensoes reais, modos independentes, resize manual e falha de texto OK.");
}

int main(int argc, char **argv)
{
  geometry_tests();
  rendering_test();
  if (argc == 2) window_tests(argv[1]);
  return 0;
}
