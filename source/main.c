#include <sys/param.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>

#include "rendering.h"
#include "structs.h"

runtime_info runtime;
void killterm_handler(int signum);

int init()
{
	signal(SIGINT, killterm_handler);
	signal(SIGKILL, killterm_handler);
	signal(SIGTERM, killterm_handler);

	float scale = 1.0f;
	runtime.layout.window = (Rect){.x = 0.0f, .y = 0.0f, .w = scale*512.0f, .h = scale*512.0f};

	// satisfying rel.[xy]*2 == .[wh] centers axis in parent container
	runtime.layout.rgb_square.rel = (Rect){.x = 0.05, .y = 0.05, .w = 0.5, .h = 0.5};
	runtime.layout.hue_slider.rel = (Rect){.x = 0.70, .y = 0.05, .w = .08, .h = 0.5};
	runtime.layout.final_sample.rel = (Rect){.x = 0.05, .y = .65, .w = 0.20, .h = 0.20};
	runtime.layout.info_container.rel = (Rect){.x = 0.05, .y = .65, .w = .9, .h = .30};
	runtime.layout.info_boxes.rel = (Rect){.x = .25, .y = 0.00, .w = 0.75, .h = 1.00};
	runtime.layout.rgb_info.rel = (Rect){.x = 0.00, .y = 0.00, .w = 1.00, .h = 0.50};
	runtime.layout.red.rel = (Rect){.x = 0.00, .y = 0.00, .w = 0.30, .h = 1.00};
	runtime.layout.green.rel = (Rect){.x = 0.35, .y = 0.00, .w = 0.30, .h = 1.00};
	runtime.layout.blue.rel = (Rect){.x = 0.70, .y = 0.00, .w = 0.30, .h = 1.00};
	runtime.layout.hsl_info.rel = (Rect){.x = 0.00, .y = 0.50, .w = 1.00, .h = 0.50};
	runtime.layout.hue.rel = (Rect){.x = 0.00, .y = 0.00, .w = 0.30, .h = 1.00};
	runtime.layout.saturation.rel = (Rect){.x = 0.35, .y = 0.00, .w = 0.30, .h = 1.00};
	runtime.layout.luminence.rel = (Rect){.x = 0.70, .y = 0.00, .w = 0.30, .h = 1.00};

	runtime.active_hsl = (HSL_Color){.h = 0, .s = 100, .l = 50};
	runtime.active_rgb = hsl_to_rgb(runtime.active_hsl);

	TTF_Init();
	// Probably figure out how to get reliable fonts
	runtime.font = TTF_OpenFont("/usr/share/fonts/TTF/iosevka-fixed-regular.ttf", 64);
	assert(runtime.font != NULL);

	init_renderer(&runtime);

	runtime.keep_alive = 1;

	return 0;
};

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

	while(runtime.keep_alive)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts_start);
		check_inputs(&runtime);
		display(&runtime);
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

