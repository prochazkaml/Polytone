.DEFAULT_GOAL := linux
.PHONY: clean check_sdl_mingw_path

objects = main.o sdl.o menu.o input.o diskio.o tracker.o \
	menu/file.o menu/player.o menu/edit.o menu/track.o menu/help.o \
	libs/lz4.o libs/tinyfiledialogs.o libs/PTPlayer/ptplayer.o

objectdests = $(patsubst %.o,build/%.o,$(objects))

headers = $(sort $(shell find src -name '*.h'))

force_rebuild_files = \
	Makefile \
	$(headers)

build/%.o: src/%.c $(force_rebuild_files)
	@mkdir -p $(@D)
	$(CC) -c -O3 -flto -o $@ $<

build/polytone: CC = gcc
build/polytone: $(objectdests)
	$(CC) $(objectdests) -lSDL2 -lm -o $@

check_sdl_mingw_path:
ifndef SDL_MINGW_PATH
	$(error SDL_MINGW_PATH is undefined)
endif

build/polytone.exe: CC = x86_64-w64-mingw32-gcc -I$(SDL_MINGW_PATH)/include
build/polytone.exe: check_sdl_mingw_path $(objectdests)
	$(CC) $(objectdests) -L$(SDL_MINGW_PATH)/lib -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -Wl,-subsystem,windows -o $@

build/polytonedbg.exe: CC = x86_64-w64-mingw32-gcc
build/polytonedbg.exe: $(objectdests)
	$(CC) $(objectdests) -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -o $@

linux: build/polytone

windows: build/polytone.exe

clean:
	rm -rf build

pkg_windows: build/polytone.exe
	zip build/polytone.zip -r build/polytone.exe SDL2.dll examples
