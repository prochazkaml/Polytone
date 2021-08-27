#include <stdio.h>
#include <stdint.h>
#include "sdl.h"
#include "input.h"
#include "diskio.h"

int main(int argc, char *argv[]) {
	Init();

	if(argc == 2) {
		LoadMON(argv[1]);
	}

	while(1) {
		ParseMainInput();

		RenderFrame();
	}
}
