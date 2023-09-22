LIBS := -lm
SOURCES := $(wildcard source/*.c)

picker: source/*.c source/*.h
	gcc $(SOURCES) $(LIBS) -lSDL2 -lSDL2_ttf -o bin/$@

picker_debug: source/*.c source/*.h
	gcc -g $(SOURCES) $(LIBS) -lSDL2 -lSDL2_ttf -o bin/$@

install:
	make picker
	cp bin/picker /usr/local/bin/picker
