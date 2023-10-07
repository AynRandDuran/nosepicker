#ifndef STRUCTS__
#define STRUCTS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define Relative_Rect SDL_FRect
#include <stdint.h>

// A placeable rect, with relative components to its parent,
// and its finalized real placement rectangle
typedef struct
{
	Relative_Rect rel;
	SDL_FRect real;

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
	SDL_FPoint position;
	uint32_t radius;

} Circle;

typedef struct
{
	Layout_Rect body;
	SDL_Surface* surface;
	SDL_Texture* texture;
	size_t text_len;
	char* text;

} Text_Container;

typedef struct
{
	// Tabbing these to visually indicate layout
	SDL_FRect window;
		Layout_Rect hsl_square; // big clicky draggy square
		Layout_Rect hue_slider; // HSV slider bar
		Layout_Rect info_container;
			Layout_Rect final_sample; // small square showing full selected color
			Layout_Rect info_boxes;
				Layout_Rect rgb_info;
					Text_Container red_component;
					Text_Container green_component;
					Text_Container blue_component;
				Layout_Rect hsl_info;
					Text_Container hue_component;
					Text_Container sat_component;
					Text_Container lum_component;

} Window_Layout;

typedef struct
{
	SDL_Color active_rgb;
	HSL_Color active_hsl;
	Window_Layout layout;

	int keep_alive;
	const int framerate;
	const int font_size;
	const char font_path[128];
	TTF_Font* font;

	SDL_Point mouse_click_pos;
	int mouse_click;

} Runtime_Info;

#endif // STRUCTS__

