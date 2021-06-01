SRC = src/*.c
OUT = tanks
CC  = gcc
CFLAGS = `sdl2-config --libs --cflags` -lSDL2 -lSDL2_image -lm -Wall


build: $(SRC)
	mkdir -p build
	$(CC) $(SRC) -o build/$(OUT) $(CFLAGS)
	cp res/* build


clean: 
	rm -rf build