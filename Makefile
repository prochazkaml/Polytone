.DEFAULT_GOAL := polytone
.PHONY: clean

objects = main.o sdl.o lz4.o menu.o input.o diskio.o tinyfiledialogs.o MTPlayer/mtplayer.o

main.o: sdl.h input.h diskio.h
sdl.o: sdl.h MTPlayer/mtplayer.h font.h intro.h menu.h lz4.h
lz4.o: lz4.h
menu.o: menu.h sdl.h
input.o: input.h sdl.h menu.h
diskio.o: diskio.h sdl.h MTPlayer/mtplayer.h
tinyfiledialogs.o: tinyfiledialogs.h
MTPlayer/mtplayer.o: MTPlayer/mtplayer.h

polytone: CC = gcc
polytone: $(objects)
	$(CC) $(objects) -lSDL2 -lm -o $@

polytone.exe: CC = x86_64-w64-mingw32-gcc
polytone.exe: $(objects)
	$(CC) $(objects) -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -Wl,-subsystem,windows -o $@

clean:
	rm -f polytone polytone.exe $(objects)
