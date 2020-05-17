// Snowflakes simulation using the gFrame Class Library
//
// Copyright (c) Samuel Gomes (Blade), 2003
// mailto: blade_go@hotmail.com

#include <gFrame.h>

#define NUMFLAKE 1024

struct SnowFlake_t {
	int x;
	int y;
	byte c;
};

Graphics gfx;
Palette pal;
Keyboard kb;

int main() {
	int i;
	int oy, ox;
	int ty, tx;
	bool bHasMoved;
	SnowFlake_t *flake = NULL;

	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}

	flake = new SnowFlake_t[NUMFLAKE];

	/* change video mode */
	if (!gfx.Initialize()) {
		cout << endl << "Failed to initalize graphics subsystem!" << endl;
		return EXIT_FAILURE;
	}

	gfx.DirectVideo(true);

	randomize();

	// Setup the palette
	pal.GenerateGradient(2, 255, 64, 64, 64, 255, 255, 255);
	pal.Set(1, 63, 127, 255);
	gfx.SetPalette(pal);

	gfx.Print(26, 90, 1, "Merry Christmas & Happy New Year!");

	// Init the flakes
	for (i = 0; i < NUMFLAKE; i++) {
		flake[i].x = random(gfx.GetWidth());
		flake[i].y = -random(gfx.GetHeight());
		flake[i].c = 2 + random(gfx.GetColors() - 2);
	}

	// Do the snow
	while (!kb.IsWaiting()) {
		/* retrace sync */
		gfx.WaitForRetrace();

		for (i = 0; i < NUMFLAKE; i++) {
			bHasMoved = false;
			oy = flake[i].y;
			ox = flake[i].x;
			ty = oy;
			tx = ox;

			// Find an 'empty' space to move the flake and move it
			if (gfx.GetPixel(tx, ty + 1) == 0) {
				ty += random(2);
				bHasMoved = true;
			}
			else if (gfx.GetPixel(tx - 1, ty + 1) == 0) {
				ty++;
				tx--;
				bHasMoved = true;
			}
			else if (gfx.GetPixel(tx + 1, ty + 1) == 0) {
				ty++;
				tx++;
				bHasMoved = true;
			}
			/*
			else if (gfx.GetPixel(tx - 1, ty) == 0) {
				tx--;
				bHasMoved = true;
			}
			else if (gfx.GetPixel(tx + 1, ty) == 0) {
				tx++;
				bHasMoved = true;
			}
			*/

			if (!bHasMoved || ty >= (gfx.GetHeight() - 1)) {
				flake[i].x = random(gfx.GetWidth());
				flake[i].y = -random(gfx.GetHeight());
				flake[i].c = 2 + random(gfx.GetColors() - 2);
				continue;
			}

			if (oy != ty) {
				gfx.SetPixel(ox, oy, 0);
				gfx.SetPixel(tx, ty, flake[i].c);
			}

			flake[i].x = tx;
			flake[i].y = ty;
		}
	}

	/* Text mode */
	gfx.Terminate();

	delete[] flake;

	return EXIT_SUCCESS;
}
