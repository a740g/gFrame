// Antialiased Circles Plasma demo using the gFrame Class Library
// Copyright (c) Samuel Gomes (Blade), 2003

#include <gFrame.h>

Graphics gfx;
Keyboard kb;
Palette pal;

// Setup a gradient palette (b->r->g->b)
void setupPalette() {
	pal.GenerateGradient(0, 85, 0, 0, 255, 255, 0, 0);
	pal.GenerateGradient(85, 170, 255, 0, 0, 0, 255, 0);
	pal.GenerateGradient(170, 255, 0, 255, 0, 0, 0, 255);
	gfx.SetPalette(pal);
}

// Initialize the plasma
void startPlasma() {
	int x, y;

	// Draw random pixels
	for (y = 0; y < gfx.GetHeight(); y++) {
		for (x = 0; x < gfx.GetWidth(); x++) {
			gfx.SetPixel(x, y, random(gfx.GetColors()));
		}
	}

	// Draw a few concentric circles
	for (x = gfx.GetColors(); x > 0; x--) {
		gfx.DrawCircle(159, 99, x, x - 1);
	}
}

// Simply anti-alias the framebuffer
void doPlasma() {
	int x, y;
	byte l, t, r, b, c;

	for (y = 0; y < gfx.GetHeight(); y++) {
		for (x = 0; x < gfx.GetWidth(); x++) {
			c = gfx.GetPixel(x, y);
			l = gfx.GetPixel(x - 1, y);
			t = gfx.GetPixel(x, y - 1);
			r = gfx.GetPixel(x + 1, y);
			b = gfx.GetPixel(x, y + 1);
			gfx.SetPixel(x, y, ((c + l + t + r + b) / 5));
		}
	}
}

void main() {
	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}

	randomize();

	// Switch to graphics mode
	gfx.Initialize();

	// Draw directly to video buffer
	gfx.DirectVideo(true);

	setupPalette();

	startPlasma();
	for (int i = 0; i < 5; i++) {
		doPlasma();
	}

	kb.Clear();
	do {
		pal.RotateLeft();
		gfx.WaitForRetrace();
		gfx.SetPalette(pal);
		Processor::Yield();
	} while (!kb.IsWaiting());

	kb.Clear();
	do {
		pal.RotateRight();
		gfx.WaitForRetrace();
		gfx.SetPalette(pal);
		Processor::Yield();
	} while (!kb.IsWaiting());
}
