#include <SDL2/SDL2_gfxPrimitives.h>
#include <sys/param.h>

#include "SDL_Utils.h"
#include "../Math_Utils.h"

#define unroll_sdl_color(color) color.r, color.g, color.b, color.a
#define lineColorFPoints(r, a, b, c) lineColor(r, a.x - camera_offset.x, a.y - camera_offset.y, b.x - camera_offset.x, b.y - camera_offset.y, c)

sdl_group mgr;
const int keypress_delta = 4;

int32_t init_renderer(Game_Info* gi)
{
	mgr.pad = NULL;

	// default game states
	// start overriding these with configs

	//load_config(argv[1], gs);

	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER))
	{
		fprintf(stderr, "SDL initialization failed\n");
		exit(__LINE__);
	}

	mgr.win = SDL_CreateWindow("Color Picker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gi->window.w, gi->window.h, 0);
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

int32_t render_rgb_square(Game_Info* gi)
{
	SDL_SetRenderDrawColor(mgr.rend, 0x00, 0x00, 0x00, 0xFF);
	gi->rgb_square.real = fr_margin_adjust(gi->window, gi->rgb_square.rel);
	SDL_RenderFillRectF(mgr.rend, &gi->rgb_square.real);

	return 0;
}

// REALLY this should be "generate layout", and not a true rendering step
int32_t render_container(Game_Info* gi, SDL_FRect* parent, Layout_Rect* child, SDL_Color color)
{
	SDL_SetRenderDrawColor(mgr.rend, unroll_sdl_color(color));
	child->real = fr_margin_adjust(*parent, child->rel);

	// replace this with corresponding item's callback
	// hmm maybe that can be wedged into the layout_rect?
	// nahhhh
	SDL_RenderFillRectF(mgr.rend, &child->real);

	return 0;
}

int32_t display(Game_Info* gi)
{
	SDL_Color red = {255, 0, 0, 255};
	SDL_Color green = {0, 255, 0, 255};
	SDL_Color blue = {0, 0, 255, 255};
	SDL_Color black = {0, 0, 0, 255};
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color magenta = {255, 0, 255, 255};

	// This is kinda the important stuff
	SDL_RenderPresent(mgr.rend);
	SDL_SetRenderDrawColor(mgr.rend, 0xCB, 0xCB, 0xCB, 0xCB);
	SDL_RenderClear(mgr.rend);

	// this would be really cool to turn into some stack-based type of thing
	render_container(gi, &gi->window, &gi->rgb_square, green);
	render_container(gi, &gi->window, &gi->hue_slider, green);
	render_container(gi, &gi->window, &gi->info_container, blue);
	render_container(gi, &gi->window, &gi->final_sample, green);

	render_container(gi, &gi->info_container.real, &gi->info_boxes, green);
	render_container(gi, &gi->info_boxes.real, &gi->rgb_info, black);
	render_container(gi, &gi->info_boxes.real, &gi->hsl_info, white);

	render_container(gi, &gi->rgb_info.real, &gi->red, red);
	render_container(gi, &gi->rgb_info.real, &gi->green, green);
	render_container(gi, &gi->rgb_info.real, &gi->blue, blue);

	render_container(gi, &gi->hsl_info.real, &gi->hue, green);
	render_container(gi, &gi->hsl_info.real, &gi->saturation, blue);
	render_container(gi, &gi->hsl_info.real, &gi->luminence, red);
	return 0;
}

int32_t check_inputs(Game_Info* gi)
{
	while(SDL_PollEvent(&(mgr.event)))
	{
		// wasd movement and quitting
		{
			if (mgr.event.type == SDL_KEYDOWN && mgr.event.key.keysym.sym == SDLK_q)
			{
				gi->game_alive = 0;
			}
		}
	}

	// controller support later
	if(mgr.pad)
	{
		// update_for_controller(mgr, gs);
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

// https://www.rapidtables.com/convert/color/hsl-to-rgb.html
SDL_Color hsl_to_rgb (const HSL_Color hsl)
{
	float C, X, m;
	float rP, gP, bP;

	float h = fmod(hsl.h, 360.0);
	float s = hsl.s/100.0f;
	float l = hsl.l/100.0f;

	C = (1 - abs((2*(l)) - 1)) * s;
	X = C * (1 - abs(fmod(h/60, 2.0f) -1));
	m = l - (C/2.0);


	if (h >= 0 && h <= 60)
	{
		rP = C;
		gP = X;
		bP = 0;
	}
	else if (h >= 60 && h <= 120)
	{
		rP = X;
		gP = C;
		bP = 0;
	}
	else if (h >= 120 && h <= 180)
	{
		rP = 0;
		gP = C;
		bP = X;
	}
	else if (h >= 180 && h <= 240)
	{
		rP = 0;
		gP = X;
		bP = C;
	}
	else if (h >= 240 && h <= 300)
	{
		rP = X;
		gP = 0;
		bP = C;
	}
	else if (h >= 300 && h <= 360)
	{
		rP = C;
		gP = 0;
		bP = X;
	}

	return (SDL_Color){
		(rP + m) * 255,
		(gP + m) * 255,
		(bP + m) * 255};
}

