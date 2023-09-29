LIBS := -lm -lSDL2 -lSDL2_ttf
SOURCES := $(wildcard source/*.c)

picker: source/*.c source/*.h
	gcc -Wall $(SOURCES) $(LIBS) -o bin/$@

picker_debug: source/*.c source/*.h
	gcc -Wall -g $(SOURCES) $(LIBS) -o bin/$@

install:
	make picker
	cp bin/picker /usr/local/bin/picker
