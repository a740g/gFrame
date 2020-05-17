// Fire simulator using the gFrame Class Library
// The algorithm is orginal and simple
// It stresses one important property of fire: randomness
//
// Copyright (c) Samuel Gomes (Blade), 2003
// mailto: blade_go@hotmail.com

#include <gFrame.h>

#define START_COLOR 128

Graphics gfx;
Palette pal;
Keyboard kb;

void setPalette(int type) {
	if (type == 1) {
		// black -> red -> yellow -> white
		pal.GenerateGradient(1, 85, 0, 0, 0, 255, 0, 0);
		pal.GenerateGradient(85, 170, 255, 0, 0, 255, 255, 0);
		pal.GenerateGradient(170, 255, 255, 255, 0, 255, 255, 255);
	}
	else if (type == 2) {
		// black -> green -> cyan -> white
		pal.GenerateGradient(1, 85, 0, 0, 0, 0, 255, 0);
		pal.GenerateGradient(85, 170, 0, 255, 0, 0, 255, 255);
		pal.GenerateGradient(170, 255, 0, 255, 255, 255, 255, 255);
	}
	else {
		// black -> blue -> magenta -> white
		pal.GenerateGradient(1, 85, 0, 0, 0, 0, 0, 255);
		pal.GenerateGradient(85, 170, 0, 0, 255, 255, 0, 255);
		pal.GenerateGradient(170, 255, 255, 0, 255, 255, 255, 255);
	}

	gfx.SetPalette(pal);
}

int main() {
	int p, q;
	word mx, my, clrs;
	word key;

	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}

	randomize();

	cout << endl << "This simple fire effect was written entirely in C++" << endl
		<< "using the gFrame Class Library with no assembly language." << endl
		<< "Try pressing 'r','g' or 'b' for different colored flames." << endl
		<< endl << "Press any key to continue...";
	getch();

	if (!gfx.Initialize()) {
		cout << "Failed to initialize Graphics subsystem!" << endl;
		return EXIT_FAILURE;
	}

	gfx.DirectVideo(true);

	/* start with the real red */
	setPalette(1);

	mx = gfx.GetWidth() - 1;
	my = gfx.GetHeight() - 1;
	clrs = gfx.GetColors() - START_COLOR;

	for (;;) {
		/* retrace sync */
		//gfx.WaitForRetrace();

		/* fill the bottom line with random values */
		for (p = 0; p <= mx; p++) {
			gfx.SetPixel(p, my, START_COLOR + random(clrs));
		}

		/* calculate pixel values */
		for (q = my - 1; q >= (my >> 1); q--) {
			for (p = 0; p <= mx; p++) {
				// Crazy six point anti-aliasing
				//gfx.SetPixel(p + random(3) - 1, q + random(3) - 1, (gfx.GetPixel(p, q) + gfx.GetPixel(p, q + random(3) - 1) + gfx.GetPixel(p - random(2), q) + gfx.GetPixel(p + random(2), q) + gfx.GetPixel(p - random(2), q + random(2)) + gfx.GetPixel(p + random(2), q + random(2))) / 6);
				// Crazy four point anti-aliasing
				//gfx.SetPixel(p + random(3) - 1, q + random(3) - 1, (gfx.GetPixel(p, q) + gfx.GetPixel(p, q + random(3) - 1) + gfx.GetPixel(p - random(2), q) + gfx.GetPixel(p + random(2), q)) >> 2);
				// Crazy three point anti-aliasing
				//gfx.SetPixel(p + random(3) - 1, q + random(3) - 1, (gfx.GetPixel(p, q) + gfx.GetPixel(p, q + random(3) - 1) + gfx.GetPixel(p + random(3) - 1, q)) / 3);
				// Crazy two point anti-aliasing (probably the best)
				gfx.SetPixel(p + random(3) - 1, q + random(3) - 1, (gfx.GetPixel(p, q) + gfx.GetPixel(p + random(3) - 1, q + random(3) - 1)) >> 1);
				//gfx.SetPixel(p, q + random(3) - 1, (gfx.GetPixel(p, q) + gfx.GetPixel(p + random(3) - 1, q + random(3) - 1)) >> 1);
				//gfx.SetPixel(p, q, (gfx.GetPixel(p, q) + gfx.GetPixel(p - random(2), q + random(2)) + gfx.GetPixel(p + random(2), q + random(2))) / 3);
			}
		}

		if (random(32) == 16) {
			gfx.Print(random(mx - 120) , 150 + random(my - 150), START_COLOR + random(clrs), "Burn baby burn!");
		}

		key = kb.GetInput();

		if (key == KB_ESC)
			break;
		else if (key == 'r')
			setPalette(1);
		else if (key == 'g')
			setPalette(2);
		else if (key == 'b')
			setPalette(3);

		Processor::Yield();
	}

	gfx.Terminate();

	return EXIT_SUCCESS;
}
