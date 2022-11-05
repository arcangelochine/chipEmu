.PHONY: build

CC=gcc

build:
	mkdir -p bin bin/roms
	rm -f bin/chipEmu
	$(CC) src/*.c -o ./bin/chipEmu -O2 -I include -L lib -l SDL2
