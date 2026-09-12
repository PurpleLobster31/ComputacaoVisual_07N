#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <SDL3/SDL.h>

//------------------------------------------------------------------------------
// Estados do botão. As cores associadas a cada estado (item 5/6 do escopo:
// "azul" neutro, "azul claro" hover, "azul escuro" clicado) são aplicadas em
// button_render().
//------------------------------------------------------------------------------
typedef enum ButtonState
{
  BUTTON_STATE_NEUTRAL,
  BUTTON_STATE_HOVER,
  BUTTON_STATE_PRESSED,
} ButtonState;

typedef struct Button Button;
struct Button
{
  SDL_FRect rect;
  char label[64];
  ButtonState state;
};

//------------------------------------------------------------------------------
void button_init(Button *button, float x, float y, float w, float h, const char *label);

//------------------------------------------------------------------------------
// Atualiza o estado do botão a partir de um evento SDL. `window_id` é o ID da
// janela à qual este botão pertence — eventos de outras janelas são ignorados.
//
// Se o clique for concluído dentro do botão (mouse down + mouse up sem sair
// da área), `*out_clicked` é setado para true nesse frame.
//------------------------------------------------------------------------------
void button_handle_event(Button *button, const SDL_Event *event, SDL_WindowID window_id, bool *out_clicked);

//------------------------------------------------------------------------------
// Desenha o botão com SDL_RenderFillRect/SDL_RenderRect, usando a cor
// correspondente ao estado atual. O texto do rótulo (button->label) ainda
// depende do módulo de renderização de texto (item 8, SDL_ttf) — por ora,
// apenas o retângulo é desenhado.
//------------------------------------------------------------------------------
void button_render(SDL_Renderer *renderer, const Button *button);

//------------------------------------------------------------------------------
// Troca o texto do rótulo do botão (ex.: "Equalizar" <-> "Ver original").
//------------------------------------------------------------------------------
void button_set_label(Button *button, const char *label);

#endif // BUTTON_H