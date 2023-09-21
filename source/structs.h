#ifndef STRUCTS__
#define STRUCTS__

#if BUILD_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define Point SDL_Point
#define FPoint SDL_FPoint
#define Rect SDL_FRect
#define Relative_Rect SDL_FRect
#define Color SDL_Color
#endif // BUILD_SDL

#include <stdint.h>

// A placeable rect, with relative components to its parent,
// and its finalized real placement rectangle
typedef struct
{
	Relative_Rect rel;
	Rect real;

	// bool
	// center in parent axis (even spacing on both sides?)
	// unused so far
	int x_center;
	int y_center;
	int x_offset;
	int y_offset;
} Layout_Rect;

typedef struct
{
	// Internet says hue likes to go 0-360
	// Saturation and luminence are 0-100
	int16_t h;
	int8_t s;
	int8_t l;
	uint8_t a;

} HSL_Color;

typedef struct
{
	FPoint position;
	uint32_t radius;

} Circle;

typedef struct
{
	// Tabbing these to visually indicate layout
	Rect window;
		Layout_Rect rgb_square; // big clicky draggy square
		Layout_Rect hue_slider; // HSV slider bar
		Layout_Rect info_container;
			Layout_Rect final_sample; // small square showing full selected color
			Layout_Rect info_boxes;
				Layout_Rect rgb_info;
					Layout_Rect red;
					SDL_Surface* red_component_text_surface;
					SDL_Texture* red_component_text_tex;
					Layout_Rect green;
					SDL_Surface* green_component_text_surface;
					SDL_Texture* green_component_text_tex;
					Layout_Rect blue;
					SDL_Surface* blue_component_text_surface;
					SDL_Texture* blue_component_text_tex;
				Layout_Rect hsl_info;
					Layout_Rect hue;
					SDL_Surface* hue_component_text_surface;
					SDL_Texture* hue_component_text_tex;
					Layout_Rect saturation;
					SDL_Surface* sat_component_text_surface;
					SDL_Texture* sat_component_text_tex;
					Layout_Rect luminence;
					SDL_Surface* lum_component_text_surface;
					SDL_Texture* lum_component_text_tex;

	Color active_rgb;
	HSL_Color active_hsl;

	int game_alive;

	TTF_Font* font;

} Game_Info;

#endif // STRUCTS__

