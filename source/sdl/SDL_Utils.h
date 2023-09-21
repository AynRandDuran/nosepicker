#ifndef SDL_Utils__
#define SDL_Utils__
#include "../structs.h"

typedef struct sdl_group
{
  SDL_Window* win;
  SDL_Renderer* rend;
  SDL_Texture* text;
  SDL_Event event;

  SDL_GameController* pad;
}
sdl_group;

int32_t init_renderer(Game_Info* gi);
int32_t shutdown_renderer();
int32_t delay(int32_t delay_time);
int32_t display(Game_Info* gi);
int32_t check_inputs(Game_Info* gi);

SDL_FRect fr_add(const SDL_FRect left, const SDL_FRect right);
SDL_FRect fr_subtract(const SDL_FRect left, const SDL_FRect right);
SDL_FRect fr_mult(const SDL_FRect left, const SDL_FRect right);
SDL_FRect fr_margin_adjust(const SDL_FRect parent, const Relative_Rect child);
SDL_Color hsl_to_rgb (const HSL_Color hsl);
#endif // SDL_Utils__

