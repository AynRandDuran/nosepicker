#include <SDL2/SDL_ttf.h>
#include <sys/param.h>

#include "rendering.h"

#define unroll_sdl_color(color) color.r, color.g, color.b, color.a

SDL_Color red = {255, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color blue = {0, 0, 255, 255};
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {255, 255, 255, 255};
SDL_Color magenta = {255, 0, 255, 255};

sdl_group mgr;

int32_t init_renderer(runtime_info* runtime)
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "SDL initialization failed\n");
		exit(__LINE__);
	}

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

	// this would be really cool to turn into some stack-based type of thing
	// Also, if resizing is disabled, this can be moved to a static initialization section
	// orrrr, maybe we can finally do callbacks
	render_container(runtime, &runtime->layout.window, &runtime->layout.rgb_square, green);
	render_container(runtime, &runtime->layout.window, &runtime->layout.hue_slider, green);
	render_container(runtime, &runtime->layout.window, &runtime->layout.info_container, blue);
	render_container(runtime, &runtime->layout.window, &runtime->layout.final_sample, green);
	render_container(runtime, &runtime->layout.info_container.real, &runtime->layout.info_boxes, green);
	render_container(runtime, &runtime->layout.info_boxes.real, &runtime->layout.rgb_info, black);
	render_container(runtime, &runtime->layout.rgb_info.real, &runtime->layout.red, red);
	render_container(runtime, &runtime->layout.rgb_info.real, &runtime->layout.green, green);
	render_container(runtime, &runtime->layout.rgb_info.real, &runtime->layout.blue, blue);
	render_container(runtime, &runtime->layout.info_boxes.real, &runtime->layout.hsl_info, white);
	render_container(runtime, &runtime->layout.hsl_info.real, &runtime->layout.hue, green);
	render_container(runtime, &runtime->layout.hsl_info.real, &runtime->layout.saturation, blue);
	render_container(runtime, &runtime->layout.hsl_info.real, &runtime->layout.luminence, red);
}

int32_t shutdown_renderer()
{
	SDL_DestroyRenderer(mgr.rend);
	SDL_DestroyWindow(mgr.win);
	SDL_Quit();
}

int32_t delay(int32_t delay_time)
{
	SDL_Delay(delay_time);

	return 0;
}

// Rename this eventually
// It's actually HSL
int32_t render_rgb_square(runtime_info* runtime, SDL_FRect* container)
{
	HSL_Color active_hsl = runtime->active_hsl;
	SDL_Color hsl_pixel;
	SDL_Point draw_point;
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

int32_t render_color_preview(runtime_info* runtime, SDL_FRect* container)
{
	runtime->active_rgb = hsl_to_rgb(runtime->active_hsl);
	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(runtime->active_rgb));
	SDL_RenderFillRectF(mgr.rend, container);

	return 0;
}

// https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2
// This is horrible horrible horrible
int32_t render_info_boxes(runtime_info* runtime, SDL_FRect* container)
{
	char red_string[32];
	char blu_string[32];
	char grn_string[32];
	char hue_string[32];
	char sat_string[32];
	char lum_string[32];

	sprintf(red_string, "R:%d/%X", runtime->active_rgb.r, runtime->active_rgb.r);
	sprintf(grn_string, "G:%d/%X", runtime->active_rgb.g, runtime->active_rgb.g);
	sprintf(blu_string, "B:%d/%X", runtime->active_rgb.b, runtime->active_rgb.b);
	sprintf(hue_string, "H:%d/%X", runtime->active_hsl.h, runtime->active_hsl.h);
	sprintf(sat_string, "S:%d/%X", runtime->active_hsl.s, runtime->active_hsl.s);
	sprintf(lum_string, "L:%d/%X", runtime->active_hsl.l, runtime->active_hsl.l);

	runtime->layout.red_component_text_surface		= TTF_RenderText_Solid(runtime->font, red_string, black);
	runtime->layout.green_component_text_surface	= TTF_RenderText_Solid(runtime->font, grn_string, black);
	runtime->layout.blue_component_text_surface		= TTF_RenderText_Solid(runtime->font, blu_string, black);
	runtime->layout.hue_component_text_surface		= TTF_RenderText_Solid(runtime->font, hue_string, black);
	runtime->layout.sat_component_text_surface		= TTF_RenderText_Solid(runtime->font, sat_string, black);
	runtime->layout.lum_component_text_surface		= TTF_RenderText_Solid(runtime->font, lum_string, black);

	runtime->layout.red_component_text_tex				= SDL_CreateTextureFromSurface(mgr.rend, runtime->layout.red_component_text_surface);
	runtime->layout.green_component_text_tex			= SDL_CreateTextureFromSurface(mgr.rend, runtime->layout.green_component_text_surface);
	runtime->layout.blue_component_text_tex				= SDL_CreateTextureFromSurface(mgr.rend, runtime->layout.blue_component_text_surface);
	runtime->layout.hue_component_text_tex				= SDL_CreateTextureFromSurface(mgr.rend, runtime->layout.hue_component_text_surface);
	runtime->layout.sat_component_text_tex				= SDL_CreateTextureFromSurface(mgr.rend, runtime->layout.sat_component_text_surface);
	runtime->layout.lum_component_text_tex				= SDL_CreateTextureFromSurface(mgr.rend, runtime->layout.lum_component_text_surface);

	// Now we can render
	SDL_RenderCopyF(mgr.rend, runtime->layout.red_component_text_tex,		NULL, &runtime->layout.red.real);
	SDL_RenderCopyF(mgr.rend, runtime->layout.green_component_text_tex,	NULL, &runtime->layout.green.real);
	SDL_RenderCopyF(mgr.rend, runtime->layout.blue_component_text_tex,	NULL, &runtime->layout.blue.real);
	SDL_RenderCopyF(mgr.rend, runtime->layout.hue_component_text_tex,		NULL, &runtime->layout.hue.real);
	SDL_RenderCopyF(mgr.rend, runtime->layout.sat_component_text_tex,		NULL, &runtime->layout.saturation.real);
	SDL_RenderCopyF(mgr.rend, runtime->layout.lum_component_text_tex,		NULL, &runtime->layout.luminence.real);

	// Cleanup
	SDL_FreeSurface(runtime->layout.red_component_text_surface);
	SDL_FreeSurface(runtime->layout.green_component_text_surface);
	SDL_FreeSurface(runtime->layout.blue_component_text_surface);
	SDL_FreeSurface(runtime->layout.hue_component_text_surface);
	SDL_FreeSurface(runtime->layout.sat_component_text_surface);
	SDL_FreeSurface(runtime->layout.lum_component_text_surface);

	SDL_DestroyTexture(runtime->layout.red_component_text_tex);
	SDL_DestroyTexture(runtime->layout.green_component_text_tex);
	SDL_DestroyTexture(runtime->layout.blue_component_text_tex);
	SDL_DestroyTexture(runtime->layout.hue_component_text_tex);
	SDL_DestroyTexture(runtime->layout.sat_component_text_tex);
	SDL_DestroyTexture(runtime->layout.lum_component_text_tex);
}

int32_t render_vertical_hue_spectrum(runtime_info* runtime, SDL_FRect* container)
{

	int hue_slice_scale = container->h;
	float hue_slice_height = hue_slice_scale/360.0f;

	int bar_y = runtime->active_hsl.h/360.0f*container->h + container->y;
	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(black));
	SDL_RenderDrawLine(mgr.rend, container->x-16, bar_y, container->w+container->x+16, bar_y);

	for (int n = 0; n < (int)container->h; n++)
	{
		HSL_Color slice_hsl = {((float)n/(float)container->h) * 360, 100, 50};
		SDL_Color slice_color = hsl_to_rgb (slice_hsl);

		SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(slice_color));
		SDL_RenderDrawLine(mgr.rend, container->x, container->y + n, container->w+container->x, container->y+n);
	}

	return 0;
}

// REALLY this should be "generate layout", and not a true rendering step
int32_t render_container(runtime_info* runtime, SDL_FRect* parent, Layout_Rect* child, SDL_Color color)
{
	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(color));
	child->real = fr_margin_adjust(*parent, child->rel);

	return 0;
}

int32_t display(runtime_info* runtime)
{
	SDL_SetRenderDrawColor(mgr.rend, 0xCB, 0xCB, 0xCB, 0xCB);
	SDL_RenderClear(mgr.rend);

	runtime->active_rgb = hsl_to_rgb(runtime->active_hsl);

	render_color_preview(runtime, &runtime->layout.final_sample.real);
	render_vertical_hue_spectrum(runtime, &runtime->layout.hue_slider.real);
	render_rgb_square(runtime, &runtime->layout.rgb_square.real);
	render_info_boxes(runtime, &runtime->layout.info_boxes.real);

	SDL_RenderPresent(mgr.rend);
	return 0;
}

int32_t check_inputs(runtime_info* runtime)
{
	while(SDL_PollEvent(&(mgr.event)))
	{
		if (mgr.event.type == SDL_KEYDOWN)
		{
			switch(mgr.event.key.keysym.sym)
			{
				case SDLK_q:
					runtime->keep_alive = 0;
					break;
				case SDLK_b:
					runtime->active_hsl.h += 1;
					if(runtime->active_hsl.h > 360)
						runtime->active_hsl.h -= 360;
					break;
				case SDLK_n:
					runtime->active_hsl.h -= 1;
					if(runtime->active_hsl.h < 0)
						runtime->active_hsl.h += 360;
					break;
				case SDLK_k:
					if(runtime->active_hsl.l < 100)
						runtime->active_hsl.l += 1;
					break;
				case SDLK_j:
					if(runtime->active_hsl.l > 0)
						runtime->active_hsl.l -= 1;
					break;
				case SDLK_l:
					if(runtime->active_hsl.s < 100)
						runtime->active_hsl.s += 1;
					break;
				case SDLK_h:
					if(runtime->active_hsl.s > 0)
						runtime->active_hsl.s -= 1;
					break;
			}
		}
	}

	return 0;
}

SDL_FRect fr_subtract(const SDL_FRect left, const SDL_FRect right)
{
	return
	(SDL_FRect){
		left.x - right.x,
		left.y - right.y,
		left.w - right.w,
		left.h - right.h
	};
}

SDL_FRect fr_add(const SDL_FRect left, const SDL_FRect right)
{
	return
	(SDL_FRect){
		left.x + right.x,
		left.y + right.y,
		left.w + right.w,
		left.h + right.h
	};
}

SDL_FRect fr_mult(const SDL_FRect left, const SDL_FRect right)
{
	return
	(SDL_FRect){
		left.x * right.x,
		left.y * right.y,
		left.w * right.w,
		left.h * right.h
	};
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

