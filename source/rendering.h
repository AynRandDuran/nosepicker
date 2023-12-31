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

int32_t init_renderer(Runtime_Info* runtime);
int32_t shutdown_renderer(Runtime_Info* runtime);
int32_t delay(int32_t delay_time);
int32_t display(Runtime_Info* runtime);
int32_t check_inputs(Runtime_Info* runtime);
int32_t render_container(SDL_FRect* parent, Layout_Rect* child);
int32_t init_text_container(Text_Container* tc, size_t text_size);
int32_t free_text_container(Text_Container* tc);

SDL_FRect fr_margin_adjust(const SDL_FRect parent, const Relative_Rect child);
SDL_Color hsl_to_rgb (const HSL_Color hsl);
SDL_Color wikipedia_hsl_to_rgb (const HSL_Color hsl);
#endif // SDL_Utils__

