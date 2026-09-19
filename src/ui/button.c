#include "button.h"
#include <string.h>

//------------------------------------------------------------------------------
void button_init(Button *button, float x, float y, float w, float h, const char *label)
{
  if (!button)
  {
    return;
  }

  *button = (Button){0};
  button->rect.x = x;
  button->rect.y = y;
  button->rect.w = w;
  button->rect.h = h;
  button->state = BUTTON_STATE_NEUTRAL;
  button_set_label(button, label);
}

//------------------------------------------------------------------------------
void button_set_label(Button *button, const char *label)
{
  if (!button)
  {
    return;
  }

  text_label_destroy(&button->text);
  if (label)
  {
    SDL_strlcpy(button->label, label, sizeof(button->label));
  }
  else
  {
    button->label[0] = '\0';
  }
}

//------------------------------------------------------------------------------
void button_handle_event(Button *button, const SDL_Event *event, SDL_WindowID window_id, bool *out_clicked)
{
  if (!button || !event)
  {
    return;
  }

  if (out_clicked)
  {
    *out_clicked = false;
  }

  switch (event->type)
  {
    case SDL_EVENT_MOUSE_MOTION:
    {
      if (event->motion.windowID != window_id)
      {
        return;
      }

      SDL_FPoint point = { .x = event->motion.x, .y = event->motion.y };
      bool inside = SDL_PointInRectFloat(&point, &button->rect);

      // Nao sobrescreve o estado "pressionado": o botao so sai desse estado
      // via mouse button up (abaixo).
      if (button->state != BUTTON_STATE_PRESSED)
      {
        button->state = inside ? BUTTON_STATE_HOVER : BUTTON_STATE_NEUTRAL;
      }
      break;
    }

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
      if (event->button.windowID != window_id || event->button.button != SDL_BUTTON_LEFT)
      {
        return;
      }

      SDL_FPoint point = { .x = event->button.x, .y = event->button.y };
      if (SDL_PointInRectFloat(&point, &button->rect))
      {
        button->state = BUTTON_STATE_PRESSED;
      }
      break;
    }

    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
      if (event->button.windowID != window_id || event->button.button != SDL_BUTTON_LEFT)
      {
        return;
      }

      if (button->state == BUTTON_STATE_PRESSED)
      {
        SDL_FPoint point = { .x = event->button.x, .y = event->button.y };
        bool inside = SDL_PointInRectFloat(&point, &button->rect);

        if (inside && out_clicked)
        {
          *out_clicked = true;
        }

        button->state = inside ? BUTTON_STATE_HOVER : BUTTON_STATE_NEUTRAL;
      }
      break;
    }

    default:
      break;
  }
}

//------------------------------------------------------------------------------
bool button_render(SDL_Renderer *renderer, Button *button, TTF_Font *font)
{
  if (!renderer || !button || !font)
  {
    return SDL_SetError("Botao invalido para renderizacao.");
  }

  // Cores por estado (item 5/6 do escopo): azul neutro, azul claro hover,
  // azul escuro pressionado.
  switch (button->state)
  {
    case BUTTON_STATE_HOVER:
      SDL_SetRenderDrawColor(renderer, 100, 170, 255, 255);
      break;
    case BUTTON_STATE_PRESSED:
      SDL_SetRenderDrawColor(renderer, 10, 60, 140, 255);
      break;
    case BUTTON_STATE_NEUTRAL:
    default:
      SDL_SetRenderDrawColor(renderer, 40, 110, 220, 255);
      break;
  }

  SDL_RenderFillRect(renderer, &button->rect);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderRect(renderer, &button->rect);

  if (!button->text.texture &&
      !text_label_set(&button->text, renderer, font, button->label,
                      (SDL_Color){255, 255, 255, 255})) return false;
  return text_label_draw(renderer, &button->text,
                         button->rect.x + (button->rect.w - button->text.width) / 2,
                         button->rect.y + (button->rect.h - button->text.height) / 2);
}

void button_destroy(Button *button)
{
  if (button) text_label_destroy(&button->text);
}
