SRC = src/*.c
OUT = tanks
CC  = gcc
CFLAGS = -w -lm -lSDL2 -lSDL2_image


build: $(SRC)
	mkdir -p build
	gcc $(CFLAGS) -o build/$(OUT) $(SRC)
	cp res/* build


clean: 
	rm -rf build