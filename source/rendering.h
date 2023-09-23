#ifndef SDL_Utils__
#define SDL_Utils__
#include "structs.h"

typedef struct sdl_group
{
  SDL_Window* win;
  SDL_Renderer* rend;
  SDL_Event event;
}
sdl_group;

int32_t init_renderer(runtime_info* runtime);
int32_t shutdown_renderer();
int32_t delay(int32_t delay_time);
int32_t display(runtime_info* runtime);
int32_t check_inputs(runtime_info* runtime);

int32_t render_container(runtime_info* runtime, SDL_FRect* parent, Layout_Rect* child, SDL_Color color);

SDL_FRect fr_add(const SDL_FRect left, const SDL_FRect right);
SDL_FRect fr_subtract(const SDL_FRect left, const SDL_FRect right);
SDL_FRect fr_mult(const SDL_FRect left, const SDL_FRect right);
SDL_FRect fr_margin_adjust(const SDL_FRect parent, const Relative_Rect child);
SDL_Color hsl_to_rgb (const HSL_Color hsl);
SDL_Color wikipedia_hsl_to_rgb (const HSL_Color hsl);
#endif // SDL_Utils__

