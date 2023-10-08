#include <SDL2/SDL_ttf.h>
#include <sys/param.h>
#include <assert.h>

#include "rendering.h"

#define unroll_sdl_color(color) color.r, color.g, color.b, color.a
#define NULL_CHECK(ptr) \
	if (!ptr)	{ 					\
		printf("Nullptr check fail [%s %s:%d]\n", #ptr, __FILE__, __LINE__); \
		return 1;}					\

SDL_Color red = {255, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color blue = {0, 0, 255, 255};
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {255, 255, 255, 255};
HSL_Color hsl_white = {0, 0, 100};
SDL_Color magenta = {255, 0, 255, 255};

sdl_group mgr;

int32_t refresh_layout(Runtime_Info* runtime)
{
	NULL_CHECK(runtime);

	int win_w, win_h;
	// SDL_GetWindowSize(mgr.win, &win_w, &win_h);
	SDL_GetRendererOutputSize(mgr.rend, &win_w, &win_h);
	runtime->layout.window.w = win_w;
	runtime->layout.window.h = win_h;

	// this would be really cool to turn into some stack-based type of thing
	render_container(&runtime->layout.window, &runtime->layout.hsl_square);
	render_container(&runtime->layout.window, &runtime->layout.hue_slider);
	render_container(&runtime->layout.window, &runtime->layout.info_container);
	render_container(&runtime->layout.window, &runtime->layout.final_sample);
	render_container(&runtime->layout.info_container.real, &runtime->layout.info_boxes);
	render_container(&runtime->layout.info_container.real, &runtime->layout.palette);
	render_container(&runtime->layout.info_boxes.real, &runtime->layout.rgb_info);
	render_container(&runtime->layout.rgb_info.real, &runtime->layout.red_component.body);
	render_container(&runtime->layout.rgb_info.real, &runtime->layout.green_component.body);
	render_container(&runtime->layout.rgb_info.real, &runtime->layout.blue_component.body);
	render_container(&runtime->layout.info_boxes.real, &runtime->layout.hsl_info);
	render_container(&runtime->layout.hsl_info.real, &runtime->layout.hue_component.body);
	render_container(&runtime->layout.hsl_info.real, &runtime->layout.sat_component.body);
	render_container(&runtime->layout.hsl_info.real, &runtime->layout.lum_component.body);

	return 0;
}

int32_t init_renderer(Runtime_Info* runtime)
{
	NULL_CHECK(runtime);
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "SDL initialization failed\n");
		exit(__LINE__);
	}

	// SDL_WINDOW_RESIZABLE forces terminal swallow and tiling doesn't like it
	// Figure something out later
	mgr.win = SDL_CreateWindow("Color Picker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, runtime->layout.window.w, runtime->layout.window.h, 0);
	if (mgr.win == NULL)
	{
		fprintf(stderr, "SDL initialization failed\n");
		exit(__LINE__);
	}

	mgr.rend = SDL_CreateRenderer(mgr.win, -1, SDL_RENDERER_ACCELERATED);
	if (mgr.rend == NULL)
	{
		fprintf(stderr, "SDL initialization failed\n");
		exit(__LINE__);
	}

	SDL_SetRenderDrawBlendMode(mgr.rend, SDL_BLENDMODE_BLEND);

	TTF_Init();
	runtime->font = TTF_OpenFont(runtime->font_path, runtime->font_size);
	assert(runtime->font != NULL);

	init_text_container(&runtime->layout.red_component, 64);
	init_text_container(&runtime->layout.green_component, 64);
	init_text_container(&runtime->layout.blue_component, 64);
	init_text_container(&runtime->layout.hue_component, 64);
	init_text_container(&runtime->layout.sat_component, 64);
	init_text_container(&runtime->layout.lum_component, 64);

	for (int p = 0; p < PALETTE_SIZE; p++)
	{
		runtime->layout.palette_color[p] = hsl_white;
	}

	refresh_layout(runtime);

	return 0;
}

int32_t shutdown_renderer(Runtime_Info* runtime)
{
	NULL_CHECK(runtime);
	free_text_container(&runtime->layout.red_component);
	free_text_container(&runtime->layout.green_component);
	free_text_container(&runtime->layout.blue_component);
	free_text_container(&runtime->layout.hue_component);
	free_text_container(&runtime->layout.sat_component);
	free_text_container(&runtime->layout.lum_component);

	TTF_CloseFont(runtime->font);
	TTF_Quit();
	SDL_DestroyRenderer(mgr.rend);
	SDL_DestroyWindow(mgr.win);
	SDL_Quit();

	return 0;
}

int32_t delay(int32_t delay_time)
{
	SDL_Delay(delay_time);

	return 0;
}

void grab_mouse_state(Runtime_Info* runtime)
{
	Uint32 buttons;
	SDL_PumpEvents();  // make sure we have the latest mouse state.
	buttons = SDL_GetMouseState(&runtime->mouse_click_pos.x, &runtime->mouse_click_pos.y);

	if ((buttons & SDL_BUTTON_LMASK) != 0) {
		runtime->mouse_click[M_LEFT] = 1;
	}
	if ((buttons & SDL_BUTTON_RMASK) != 0) {
		runtime->mouse_click[M_RIGHT] = 1;
	}
}

void clear_mouse_state(Runtime_Info* runtime)
{
	runtime->mouse_click[M_LEFT] = 0;
	runtime->mouse_click[M_RIGHT] = 0;
}

int32_t is_mouse_down(Runtime_Info* runtime, mouse_button mb)
{
	grab_mouse_state(runtime);
	int32_t down = runtime->mouse_click[mb];
	clear_mouse_state(runtime);
	return down;
}

int32_t mouse_click_in_container(Runtime_Info* runtime, SDL_FRect* container, mouse_button mb)
{
	if (is_mouse_down(runtime, mb) &&
			runtime->mouse_click_pos.x >= container->x &&
			runtime->mouse_click_pos.x <= container->x + container->w &&
			runtime->mouse_click_pos.y >= container->y &&
			runtime->mouse_click_pos.y <= container->y + container->h)
		return 1;
	return 0;
}

int32_t render_palette(Runtime_Info* runtime, SDL_FRect* container)
{
	for (int p = 0; p < PALETTE_SIZE; p++)
	{
		SDL_Color item_color = hsl_to_rgb(runtime->layout.palette_color[p]);
		SDL_FRect item = {
			.x = container->x + (p * ((float)container->w / (float)PALETTE_SIZE)), .y = container->y,
			.w = ((float)container->w / PALETTE_SIZE) * 0.90, .h = container->h};

		SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(item_color));
		if (p == runtime->active_palette)
		{
			SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(item_color));
			item.h *= 1.2;
		}
		SDL_RenderFillRectF(mgr.rend, &item);

		if (mouse_click_in_container(runtime, &item, M_LEFT))
		{
			runtime->active_palette = p;
		}
		if (mouse_click_in_container(runtime, &item, M_RIGHT))
		{
			runtime->layout.palette_color[p] = runtime->active_hsl;
		}
	}

	return 0;
}

int32_t render_hsl_square(Runtime_Info* runtime, SDL_FRect* container)
{
	NULL_CHECK(runtime);
	NULL_CHECK(container);

	HSL_Color active_hsl = runtime->active_hsl;
	SDL_Color hsl_pixel;
	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(runtime->active_rgb));
	SDL_RenderFillRectF(mgr.rend, container);

	// I guess not much other way than good ol' n^2
	// hmmm we'll fix this later
	active_hsl.s = 0;
	active_hsl.l = 0;
	for (int r = 0; r < container->w; r++)
	{
		active_hsl.s = ((float)r/(float)container->w) * 100;
		for (int c = 0; c < container->h; c++)
		{
			active_hsl.l = ((float)1.0-c/(float)container->h) * 100;

			hsl_pixel = hsl_to_rgb(active_hsl);
			SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(hsl_pixel));
			SDL_RenderDrawPoint(mgr.rend, r+container->x, c+container->y);
		}
	}

	if (mouse_click_in_container(runtime, container, M_LEFT))
	{
		runtime->active_hsl.s = 100.0f * (runtime->mouse_click_pos.x - container->x) / container->w;
		runtime->active_hsl.l = 100.0f * (1.0f - ((float)(runtime->mouse_click_pos.y - container->y) / container->h));
	}
	if (mouse_click_in_container(runtime, container, M_RIGHT))
	{
		runtime->active_hsl.s = 100.0f * (runtime->mouse_click_pos.x - container->x) / container->w;
		runtime->active_hsl.l = 100.0f * (1.0f - ((float)(runtime->mouse_click_pos.y - container->y) / container->h));
		runtime->layout.palette_color[runtime->active_palette] = runtime->active_hsl;
	}

	float s_norm = (float)runtime->active_hsl.s/100.0f;
	float l_norm = (float)runtime->active_hsl.l/100.0f;
	SDL_Rect cursor = {
		.x = ((s_norm) * container->w) + container->x,
		.y = ((1.0f - l_norm) * container->h) + container->y,
		.w = 10,
		.h = 10
	};

	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(black));
	SDL_RenderDrawLine(mgr.rend, container->x-16, cursor.y, container->x+container->w+16, cursor.y);
	SDL_RenderDrawLine(mgr.rend, cursor.x, container->y-16, cursor.x, container->y+container->h+16);

	return 0;
}

int32_t render_color_preview(Runtime_Info* runtime, SDL_FRect* container)
{
	NULL_CHECK(runtime);
	NULL_CHECK(container);

	runtime->active_rgb = hsl_to_rgb(runtime->active_hsl);
	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(runtime->active_rgb));
	SDL_RenderFillRectF(mgr.rend, container);

	return 0;
}

// I'd love if this pre-alloc'd surface and texture, but it looks like
// SDL_TTF just doesn't work that way
int32_t init_text_container(Text_Container* tc, size_t text_size)
{
	NULL_CHECK(tc);

	tc->text = (char*)malloc(sizeof(char) * text_size);
	tc->text_len = text_size;

	NULL_CHECK(tc->text);
	return 0;
}

// Inter-frame SDL cleanup until I figure out a good solution that doesn't
// involve creating a whole new surface and texture every single frame
int32_t release_text_container(Text_Container* tc)
{
	NULL_CHECK(tc);

	SDL_FreeSurface(tc->surface);
	SDL_DestroyTexture(tc->texture);

	tc->surface = NULL;
	tc->texture = NULL;

	return 0;
}

// Clean up the whole thing, when we're done with it for good
int32_t free_text_container(Text_Container* tc)
{
	NULL_CHECK(tc);
	NULL_CHECK(tc->text);

	release_text_container(tc);
	free(tc->text);

	return 0;
}

int32_t render_text_container(Runtime_Info* runtime, Text_Container* tc)
{
	NULL_CHECK(runtime);
	NULL_CHECK(tc);

	// basics
	tc->surface = TTF_RenderText_Solid(runtime->font, tc->text, black);
	tc->texture = SDL_CreateTextureFromSurface(mgr.rend, tc->surface);
	NULL_CHECK(tc->surface);
	NULL_CHECK(tc->texture);

	// Adjust text rendering so it doesn't fill the whole element
	int width, height;
	SDL_QueryTexture(tc->texture, NULL, NULL, &width, &height);
	tc->body.real.w = (float)width;
	tc->body.real.h = (float)height;

	// slap it in there
	SDL_RenderCopyF(mgr.rend, tc->texture, NULL, &tc->body.real);

	// we'll keep this implicit to rendering for now to keep
	// the bigger rendering picture a bit cleaner
	release_text_container(&runtime->layout.red_component);
	return 0;
}

// https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2
int32_t render_info_boxes(Runtime_Info* runtime, SDL_FRect* container)
{
	NULL_CHECK(runtime);
	NULL_CHECK(container);

	snprintf(runtime->layout.red_component.text,	runtime->layout.red_component.text_len, "R:%03d/x%02X", runtime->active_rgb.r, runtime->active_rgb.r);
	snprintf(runtime->layout.green_component.text,runtime->layout.green_component.text_len, "G:%03d/x%02X", runtime->active_rgb.g, runtime->active_rgb.g);
	snprintf(runtime->layout.blue_component.text,	runtime->layout.blue_component.text_len, "B:%03d/x%02X", runtime->active_rgb.b, runtime->active_rgb.b);
	snprintf(runtime->layout.hue_component.text,	runtime->layout.hue_component.text_len, "H:%03d/x%02X", runtime->active_hsl.h, runtime->active_hsl.h);
	snprintf(runtime->layout.sat_component.text,	runtime->layout.sat_component.text_len, "S:%03d/x%02X", runtime->active_hsl.s, runtime->active_hsl.s);
	snprintf(runtime->layout.lum_component.text,	runtime->layout.lum_component.text_len, "L:%03d/x%02X", runtime->active_hsl.l, runtime->active_hsl.l);

	render_text_container(runtime, &runtime->layout.red_component);
	render_text_container(runtime, &runtime->layout.green_component);
	render_text_container(runtime, &runtime->layout.blue_component);
	render_text_container(runtime, &runtime->layout.hue_component);
	render_text_container(runtime, &runtime->layout.sat_component);
	render_text_container(runtime, &runtime->layout.lum_component);
	return 0;
}

int32_t render_vertical_hue_spectrum(Runtime_Info* runtime, SDL_FRect* container)
{
	NULL_CHECK(runtime);
	NULL_CHECK(container);

	if (mouse_click_in_container(runtime, container, M_LEFT))
	{
		runtime->active_hsl.h = ((runtime->mouse_click_pos.y - container->y) / container->h) * 360;
	}
	if (mouse_click_in_container(runtime, container, M_RIGHT))
	{
		runtime->active_hsl.h = ((runtime->mouse_click_pos.y - container->y) / container->h) * 360;
		runtime->layout.palette_color[runtime->active_palette] = runtime->active_hsl;
	}

	int bar_y = runtime->active_hsl.h/360.0f*container->h + container->y;
	int spectrum_shifter = 0;

#if 0
	int spectrum_shifter = runtime->active_hsl.h + 180;
	int bar_y = container->h/2 + container->y;
#endif

	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(black));
	SDL_RenderDrawLine(mgr.rend, container->x-16, bar_y, container->w+container->x+16, bar_y);

	for (int n = 0; n < (int)container->h; n++)
	{
		HSL_Color slice_hsl = {spectrum_shifter + (((float)n/(float)container->h) * 360), 100, 50};
		SDL_Color slice_color = hsl_to_rgb (slice_hsl);

		SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(slice_color));
		SDL_RenderDrawLine(mgr.rend, container->x, container->y + n, container->w+container->x, container->y+n);
	}
	return 0;
}

int32_t render_container(SDL_FRect* parent, Layout_Rect* child)
{
	NULL_CHECK(parent);
	NULL_CHECK(child);

	child->real = fr_margin_adjust(*parent, child->rel);
	return 0;
}

int32_t render_layout(Runtime_Info* runtime, Window_Layout* layout)
{
	NULL_CHECK(runtime);
	NULL_CHECK(layout);

	runtime->active_rgb = hsl_to_rgb(runtime->active_hsl);
	render_color_preview(runtime, &layout->final_sample.real);
	render_vertical_hue_spectrum(runtime, &layout->hue_slider.real);
	render_hsl_square(runtime, &layout->hsl_square.real);
	render_info_boxes(runtime, &layout->info_boxes.real);
	render_palette(runtime, &layout->palette.real);

	return 0;
}

int32_t display(Runtime_Info* runtime)
{
	NULL_CHECK(runtime);

	SDL_SetRenderDrawColor(mgr.rend, 0xCB, 0xCB, 0xCB, 0xCB);
	SDL_RenderClear(mgr.rend);

	render_layout(runtime, &runtime->layout);

	SDL_RenderPresent(mgr.rend);

	return 0;
}

int move_speed = 1;
int32_t check_inputs(Runtime_Info* runtime)
{
	NULL_CHECK(runtime);

	while(SDL_PollEvent(&(mgr.event)))
	{
		if (mgr.event.type == SDL_KEYDOWN)
		{
			switch(mgr.event.key.keysym.sym)
			{
				case SDLK_b:
					runtime->active_hsl.h += move_speed * 1;
					if(runtime->active_hsl.h > 360)
						runtime->active_hsl.h -= 360;
					break;
				case SDLK_n:
					runtime->active_hsl.h -= move_speed * 1;
					if(runtime->active_hsl.h < 0)
						runtime->active_hsl.h += 360;
					break;
				case SDLK_k:
					runtime->active_hsl.l += move_speed * 1;
					if(runtime->active_hsl.l > 100)
						runtime->active_hsl.l = 100;
					break;
				case SDLK_j:
					runtime->active_hsl.l -= move_speed * 1;
					if(runtime->active_hsl.l < 0)
						runtime->active_hsl.l = 0;
					break;
				case SDLK_l:
					runtime->active_hsl.s += move_speed * 1;
					if(runtime->active_hsl.s > 100)
						runtime->active_hsl.s = 100;
					break;
				case SDLK_h:
					runtime->active_hsl.s -= move_speed * 1;
					if(runtime->active_hsl.s < 0)
						runtime->active_hsl.s = 0;
					break;

				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					move_speed = 4;
					break;
			}
		}
		if (mgr.event.type == SDL_KEYUP)
		{
			int p = runtime->active_palette;
			switch(mgr.event.key.keysym.sym)
			{
				case SDLK_q:
					runtime->keep_alive = 0;
					break;
				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					move_speed = 1;
					break;

				case SDLK_0:
				case SDLK_1:
				case SDLK_2:
				case SDLK_3:
				case SDLK_4:
				case SDLK_5:
				case SDLK_6:
				case SDLK_7:
				case SDLK_8:
				case SDLK_9:
				{
					int new_palette = mgr.event.key.keysym.sym - 48 - 1;
					if (new_palette < 0)
						new_palette = PALETTE_SIZE-1;

					if (new_palette <= PALETTE_SIZE)
					{
						runtime->active_palette = new_palette;
						runtime->active_hsl = runtime->layout.palette_color[new_palette];
						runtime->active_rgb = hsl_to_rgb(runtime->active_hsl);
					}
				} break;

				case SDLK_RETURN:
				case SDLK_KP_ENTER:
					runtime->layout.palette_color[p] = runtime->active_hsl;
					break;
			}

		}

	}

	return 0;
}

// Math out placement for a relative rect onto a concrete parent
SDL_FRect fr_margin_adjust(const SDL_FRect parent, const Relative_Rect child)
{
	return
	(SDL_FRect){
		.x = parent.x + ( parent.w * (child.x ) ),
		.y = parent.y + ( parent.h * (child.y ) ),
		.w = parent.w * child.w,
		.h = parent.h * child.h,
	};
}

float hsl_to_rgb_alt_internal(const HSL_Color hsl, int n)
{
	float H = (float)hsl.h;
	float L = (float)hsl.l/100.0f;
	float S = (float)hsl.s/100.0f;
	float a = S * MIN(L, 1-L);
	float k = fmod((n + (H/30.0)), 12.0);

	return L - a * MAX(-1, MIN(k-3, MIN(9-k, 1)));
}

// https://www.wikiwand.com/en/HSL_and_HSV#To_RGB
SDL_Color hsl_to_rgb (const HSL_Color hsl)
{
	return (SDL_Color){
		.r = (uint8_t)(hsl_to_rgb_alt_internal(hsl, 0) * 255),
		.g = (uint8_t)(hsl_to_rgb_alt_internal(hsl, 8) * 255),
		.b = (uint8_t)(hsl_to_rgb_alt_internal(hsl, 4) * 255),
		.a = 0xFF}; // Opaque
}

