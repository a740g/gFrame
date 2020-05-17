// A simple plasma generator using the gFrame Class Library
// Copyright (c) Samuel Gomes, 2003
// mailto: blade_go@hotmail.com

#include <gFrame.h>

Graphics gfx;
Palette pal;
Keyboard kb;

/* Set RGB values for a color */
void setRGB(byte col, byte R, byte G, byte B) {
	pal.Set(col, R << 2, G << 2, B << 2);
}

int main() {
	int costbl[256];
	int t, y, x;
	byte a1, a2, a3, a4;
	byte b1 = 0, b2 = 0, b3 = 0, b4 = 0;

	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}

	randomize();

	if (!gfx.Initialize()) {
		cout << endl << "Failed to initialize graphics subsystem!" << endl;
		return EXIT_FAILURE;
	}

	/* Initialize cos table */
	for (t = 0; t < 256; t++)
		costbl[t] = (int)(27 * cos(t * (M_PI / 128)));

	/* Set up a gradient like palette */
	for (x = 1; x <= 32; x++) {
		setRGB(x, 0, 0, x*2-1);       /* Blue part */
		setRGB(x+32, x*2-1, 0, 63);
		setRGB(x+64, 63, x*2-1, 63);
		setRGB(x+96, 63, 63, 63);

		setRGB(x+128, 63, 63, 63);
		setRGB(x+160, 63, 63, 63-(x*2-1));
		setRGB(x+192, 63, 63-(x*2-1), 0);
		setRGB(x+224, 63-(x*2-1), 0, 0); /* Red part */
	}

	gfx.SetPalette(pal);

	/* Loop until key is pressed */

	while (!kb.IsWaiting()) {
		a1 = b1;
		a2 = b2;

		for (y = 0; y < 200; y++) {
			a3 = b3;
			a4 = b4;

			for (x = 0; x < 320; x++) {
				gfx.SetPixel(x, y, costbl[a1] + costbl[a2] + costbl[a3] + costbl[a4]);

				/* Higher values result in many slower plasmas */
				a3 += 1;
				a4 += 2;
			}

			/* Same as the previous comment*/
			a1 += 1;
			a2 += 2;
		}

		/* The higher these vars are incremented, the faster is
		** the plasma. Need'nt be similar. */
		b1 += 3;
		b2 -= 5;
		b3 -= 3;
		b4 += 2;

		/* retrace sync */
		gfx.WaitForRetrace();

		/* Blit plasmas unto screen */
		gfx.Flip();

		// Give up rest of time-slice
		Processor::Yield();
	}

	gfx.Terminate();

	return EXIT_SUCCESS;
}
