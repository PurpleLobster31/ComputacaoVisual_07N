#define SDL_MAIN_HANDLED
#define main application_main
#include "../src/main.c"
#undef main
#include <SDL3_image/SDL_image.h>
#include <assert.h>
#include <string.h>

// Executado pelo Makefile em build/save-test, nunca na pasta do arquivo de entrada.
static void check_png(const char *path, SDL_Surface *expected)
{
  SDL_Surface *loaded = IMG_Load(path);
  assert(loaded);
  SDL_Surface *rgba = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(loaded);
  assert(rgba && rgba->w == expected->w && rgba->h == expected->h);
  for (int y = 0; y < rgba->h; ++y)
    assert(!memcmp((Uint8 *)rgba->pixels + (size_t)y * rgba->pitch,
                   (Uint8 *)expected->pixels + (size_t)y * expected->pitch, (size_t)rgba->w * 4));
  SDL_DestroySurface(rgba);
}

int main(void)
{
  assert(SDL_Init(0));
  SDL_Surface *canvas = SDL_CreateSurface(1024, 768, SDL_PIXELFORMAT_RGBA32);
  assert(canvas);
  g_window.renderer = SDL_CreateSoftwareRenderer(canvas);
  assert(g_window.renderer);
  // Dois pixels por linha, com quatro bytes de padding que nao sao imagem.
  Uint8 storage[24];
  memset(storage, 0x77, sizeof storage);
  g_image.surface = SDL_CreateSurfaceFrom(2, 2, SDL_PIXELFORMAT_RGBA32, storage, 12);
  assert(g_image.surface);
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(g_image.surface->format);
  const Uint8 colors[4][4] = {{255,0,0,255},{0,255,0,0},{0,0,255,128},{255,255,255,77}};
  const Uint8 gray[4] = {54,182,18,255};
  for (int i = 0; i < 4; ++i)
  {
    Uint32 pixel = SDL_MapRGBA(format, NULL, colors[i][0], colors[i][1], colors[i][2], colors[i][3]);
    memcpy(storage + (i / 2) * 12 + (i % 2) * 4, &pixel, 4);
  }
  assert(!grayscale_is_image_grayscale(&g_image));
  assert(grayscale_convert(g_window.renderer, &g_image));
  assert(grayscale_is_image_grayscale(&g_image));
  for (int i = 0; i < 4; ++i)
  {
    Uint32 pixel;
    Uint8 r,g,b,a;
    memcpy(&pixel, storage + (i / 2) * 12 + (i % 2) * 4, 4);
    SDL_GetRGBA(pixel, format, NULL, &r,&g,&b,&a);
    assert(r == gray[i] && g == r && b == r && a == colors[i][3]);
  }
  assert(storage[8] == 0x77 && storage[20] == 0x77);
  assert(equalization_create(g_window.renderer, &g_image, &g_equalizedImage));

  SDL_RemovePath("result.png");
  SDL_RemovePath("output_image.png");
  bool overwritten = true;
  assert(image_save_png(&g_image, "result.png", &overwritten) && !overwritten);
  check_png("result.png", g_image.surface);
  assert(image_save_png(&g_equalizedImage, "result.png", &overwritten) && overwritten);
  check_png("result.png", g_equalizedImage.surface);
  assert(!image_save_png(&g_image, ".", &overwritten) && overwritten);
  assert(!image_save_png(NULL, "result.png", &overwritten));
  check_png("result.png", g_equalizedImage.surface);

  SDL_Event key = {0};
  key.type = SDL_EVENT_KEY_DOWN;
  key.key.key = SDLK_S;
  key.key.windowID = 1;
  assert(handle_save_event(&key));
  check_png("output_image.png", g_image.surface);
  g_isEqualized = true;
  key.key.repeat = true;
  assert(handle_save_event(&key));
  check_png("output_image.png", g_image.surface);
  key.key.repeat = false;
  key.type = SDL_EVENT_KEY_UP;
  assert(handle_save_event(&key));
  key.type = SDL_EVENT_KEY_DOWN;
  key.key.key = SDLK_A;
  assert(handle_save_event(&key));
  check_png("output_image.png", g_image.surface);
  key.key.key = SDLK_S;
  key.key.mod = SDL_KMOD_SHIFT;
  key.key.windowID = 2;
  assert(handle_save_event(&key));
  check_png("output_image.png", g_equalizedImage.surface);
  // Mudar o modo de exibicao nao redimensiona os pixels exportados.
  g_isOriginalResolution = true;
  assert(handle_save_event(&key));
  check_png("output_image.png", g_equalizedImage.surface);
  g_isEqualized = false;
  assert(handle_save_event(&key));
  check_png("output_image.png", g_image.surface);
  assert(SDL_RemovePath("output_image.png"));
  assert(SDL_CreateDirectory("output_image.png"));
  assert(!handle_save_event(&key));
  assert(!g_isEqualized && g_image.surface);
  assert(SDL_RemovePath("output_image.png"));
  assert(handle_save_event(&key));
  check_png("output_image.png", g_image.surface);
  SDL_RemovePath("result.png");
  SDL_RemovePath("output_image.png");
  shutdown();
  SDL_DestroySurface(canvas);
  puts("PNG: criacao, sobrescrita, RGBA exato, tecla S, repeticao, reversao e erro recuperavel OK.");
  puts("Grayscale: formula exigida, deteccao, pitch e alfa OK.");
  return 0;
}
