#if BUILD_SDL
#include "sdl/SDL_Utils.h"
#endif // BUILD_SDL

#include "structs.h"
#include "Math_Utils.h"
#include <sys/param.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>

Game_Info gi;
void killterm_handler(int signum);

int init()
{
	signal(SIGINT, killterm_handler);
	signal(SIGKILL, killterm_handler);
	signal(SIGTERM, killterm_handler);

	gi.window = (Rect){.x = 0.0f, .y = 0.0f, .w = 512.0f, .h = 512.0f};
	// satisfying rel.[xy]*2 == .[wh] centers axis in parent container
	gi.rgb_square.rel = (Rect){.x = 0.05, .y = 0.05, .w = 0.5, .h = 0.5};
	gi.hue_slider.rel = (Rect){.x = 0.65, .y = 0.05, .w = .08, .h = 0.5};
	gi.final_sample.rel = (Rect){.x = 0.05, .y = .65, .w = 0.20, .h = 0.20};

	gi.info_container.rel = (Rect){.x = 0.05, .y = .65, .w = .9, .h = .30};
	gi.info_boxes.rel = (Rect){.x = .25, .y = 0.00, .w = 0.75, .h = 1.00};

	gi.rgb_info.rel = (Rect){.x = 0.00, .y = 0.00, .w = 1.00, .h = 0.50};
	gi.red.rel = (Rect){.x = 0.00, .y = 0.00, .w = 0.30, .h = 1.00};
	gi.green.rel = (Rect){.x = 0.35, .y = 0.00, .w = 0.30, .h = 1.00};
	gi.blue.rel = (Rect){.x = 0.70, .y = 0.00, .w = 0.30, .h = 1.00};

	gi.hsl_info.rel = (Rect){.x = 0.00, .y = 0.50, .w = 1.00, .h = 0.50};
	gi.hue.rel = (Rect){.x = 0.00, .y = 0.00, .w = 0.30, .h = 1.00};
	gi.saturation.rel = (Rect){.x = 0.35, .y = 0.00, .w = 0.30, .h = 1.00};
	gi.luminence.rel = (Rect){.x = 0.70, .y = 0.00, .w = 0.30, .h = 1.00};

	// lime (120 100 50) - > (0 255 0)
	gi.active_hsv = (HSL_Color){.h = 120, .s = 100, .l = 50};

	// silver (0 0 75) - > (0 255 0)
	gi.active_hsv = (HSL_Color){.h = 0, .s = 0, .l = 75};

	Color rgb = hsl_to_rgb(gi.active_hsv);
	printf("%d %d %d\n", rgb.r, rgb.g, rgb.b);

	init_renderer(&gi);

	gi.game_alive = 1;

	return 0;
};

int handle_collisions(Game_Info* gi)
{
}

int game_loop(Game_Info* gi, float time_step)
{

	return 0;
}

int main(void)
{
	int quit = 0;
	struct timespec ts_start;
	struct timespec ts_end;
	float time_step = 1000.0f/30;

	if(init() != 0)
	{
		exit(__LINE__);
	}

	while(gi.game_alive)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts_start);

		check_inputs(&gi);
		game_loop(&gi, time_step);
		display(&gi);

		clock_gettime(CLOCK_MONOTONIC_RAW, &ts_end);
		uint64_t frameproc_ms = (ts_end.tv_nsec - ts_start.tv_nsec) / 1000000;
		frameproc_ms = MIN(time_step, frameproc_ms);
		delay(time_step - frameproc_ms);
	}

	killterm_handler(15);

	return 0;
}

void killterm_handler(int signum)
{
	printf("handling sig %d, bye bye\n", signum);
	shutdown_renderer();

	exit(0);
}

