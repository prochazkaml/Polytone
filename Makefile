.DEFAULT_GOAL := build/polytone
.PHONY: clean

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

build/polytone.exe: CC = x86_64-w64-mingw32-gcc
build/polytone.exe: $(objectdests)
	$(CC) $(objectdests) -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -Wl,-subsystem,windows -o $@

build/polytonedbg.exe: CC = x86_64-w64-mingw32-gcc
build/polytonedbg.exe: $(objectdests)
	$(CC) $(objectdests) -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -o $@

clean:
	rm -f polytone *.exe *zip $(objectdests)

pkg_windows: build/polytone.exe
	zip build/polytone.zip -r build/polytone.exe SDL2.dll examples
