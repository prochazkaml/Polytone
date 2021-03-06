.DEFAULT_GOAL := polytone
.PHONY: clean

objects = main.o sdl.o menu.o input.o diskio.o tracker.o \
	menu/file.o menu/player.o menu/edit.o menu/track.o menu/help.o \
	libs/lz4.o libs/tinyfiledialogs.o PTPlayer/ptplayer.o

main.o: sdl.h input.h diskio.h
sdl.o: sdl.h menu.h diskio.h tracker.h PTPlayer/ptplayer.h assets/font.h assets/intro.h assets/icon.h libs/lz4.h
menu.o: menu.h sdl.h input.h
input.o: input.h sdl.h menu.h tracker.h diskio.h
diskio.o: diskio.h sdl.h tracker.h libs/tinyfiledialogs.h PTPlayer/ptplayer.h
tracker.o: tracker.h sdl.h PTPlayer/ptplayer.h diskio.h
menu/file.o: menu/file.h menu/player.h menu.h diskio.h tracker.h libs/tinyfiledialogs.h
menu/player.o: menu/player.h menu.h diskio.h tracker.h sdl.h PTPlayer/ptplayer.h
menu/edit.o: menu/edit.h menu/player.h menu.h diskio.h tracker.h PTPlayer/ptplayer.h
menu/track.o: menu/track.h menu.h tracker.h diskio.h PTPlayer/ptplayer.h menu/player.h
menu/help.o: menu/help.h menu.h
libs/tinyfiledialogs.o: libs/tinyfiledialogs.h
libs/lz4.o: libs/lz4.h
PTPlayer/ptplayer.o: PTPlayer/ptplayer.h

polytone: CC = gcc
polytone: $(objects)
	$(CC) $(objects) -lSDL2 -lm -o $@

polytone.exe: CC = x86_64-w64-mingw32-gcc
polytone.exe: $(objects)
	$(CC) $(objects) -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -Wl,-subsystem,windows -o $@

polytonedbg.exe: CC = x86_64-w64-mingw32-gcc
polytonedbg.exe: $(objects)
	$(CC) $(objects) -lmingw32 -lSDL2 -lSDL2main -lm -lcomdlg32 -lole32 -o $@

clean:
	rm -f polytone *.exe *zip $(objects)

pkg_windows: polytone.exe
	zip polytone.zip -r polytone.exe SDL2.dll examples
