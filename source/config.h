#ifndef __PICKER_CONFIG__
#define __PICKER_CONFIG__
#include "structs.h"

const float config_ui_scale = 1.0f;

Runtime_Info runtime;
Window_Layout config_layout = {
	// Master window scale and dimensions
	.window	= (SDL_FRect){
		.x = 0.0f,
		.y = 0.0f,
		.w = config_ui_scale*512.0f,
		.h = config_ui_scale*512.0f
	},

	// satisfying rel.[xy]*2 == .[wh] centers axis in parent container
	.hsl_square.rel			=	(SDL_FRect){.x = 0.05, .y = 0.05, .w = 0.50, .h = 0.50},
	.hue_slider.rel			=	(SDL_FRect){.x = 0.70, .y = 0.05, .w = 0.08, .h = 0.50},
	.final_sample.rel		=	(SDL_FRect){.x = 0.05, .y = 0.65, .w = 0.20, .h = 0.20},
	.info_container.rel	=	(SDL_FRect){.x = 0.05, .y = 0.65, .w = 0.90, .h = 0.30},
	.info_boxes.rel			=	(SDL_FRect){.x = 0.25, .y = 0.00, .w = 0.75, .h = 1.00},
	.rgb_info.rel				=	(SDL_FRect){.x = 0.00, .y = 0.00, .w = 1.00, .h = 0.50},
	.red_component.body.rel						=	(SDL_FRect){.x = 0.00, .y = 0.00, .w = 0.30, .h = 1.00},
	.green_component.body.rel					=	(SDL_FRect){.x = 0.35, .y = 0.00, .w = 0.30, .h = 1.00},
	.blue_component.body.rel						=	(SDL_FRect){.x = 0.70, .y = 0.00, .w = 0.30, .h = 1.00},
	.hsl_info.rel				=	(SDL_FRect){.x = 0.00, .y = 0.50, .w = 1.00, .h = 0.50},
	.hue_component.body.rel						=	(SDL_FRect){.x = 0.00, .y = 0.00, .w = 0.30, .h = 1.00},
	.sat_component.body.rel			=	(SDL_FRect){.x = 0.35, .y = 0.00, .w = 0.30, .h = 1.00},
	.lum_component.body.rel			=	(SDL_FRect){.x = 0.70, .y = 0.00, .w = 0.30, .h = 1.00}
};

// For now we need a full path, maybe OpenFont can do automatic lookup
const char config_font_path[] = "/usr/share/fonts/TTF/iosevka-fixed-regular.ttf";
// This currently acts more like a static resolution scale rather than font size
const int config_font_size = 128;
const int config_framerate = 30;

#endif //__PICKER_CONFIG__

