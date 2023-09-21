LIBS := -lX11 -lm
SOURCES := $(wildcard source/*.c)
SDL_SOURCES := $(wildcard source/sdl/*.c)
IMGUI_SOURCES = imgui/imgui.cpp imgui/imgui_impl_sdl.cpp imgui/imgui_impl_sdlrenderer.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui_draw.cpp

sdl: source/*.c source/*.h source/sdl/*.c source/sdl/*.h
	gcc -DBUILD_SDL=1 $(SDL_SOURCES) $(SOURCES) $(LIBS) -lSDL2 -lSDL2_gfx -lSDL2_ttf -o bin/$@

d_sdl: source/*.c source/*.h source/sdl/*.c source/sdl/*.h
	gcc -g -DBUILD_SDL=1 $(SDL_SOURCES) $(SOURCES) $(LIBS) -lSDL2 -lSDL2_gfx -o bin/$@
