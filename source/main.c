#include <sys/param.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>

#include "rendering.h"
#include "structs.h"
#include "config.h"

void killterm_handler(int signum);

int init()
{
	signal(SIGINT, killterm_handler);
	signal(SIGKILL, killterm_handler);
	signal(SIGTERM, killterm_handler);

	runtime.layout = config_layout;
	runtime.active_hsl = (HSL_Color){.h = 0, .s = 100, .l = 50};
	runtime.active_rgb = hsl_to_rgb(runtime.active_hsl);

	init_renderer(&runtime);
	runtime.keep_alive = 1;

	return 0;
};

int main(void)
{
	struct timespec ts_start;
	struct timespec ts_end;
	float time_step = 1000.0f/runtime.framerate;

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

	shutdown_renderer(&runtime);
	return 0;
}

void killterm_handler(int signum)
{
	shutdown_renderer(&runtime);
	exit(0);
}

