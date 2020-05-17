// Antialiased Circles demo using the gFrame Class Library
// Copyright (c) Samuel Gomes (Blade), 2004

#include <gFrame.h>

Graphics gfx;
Keyboard kb;
Palette pal;

// Setup a gradient palette (b->r->g->b) !0
void setupPalette() {
	pal.GenerateGradient(1, 85, 0, 0, 255, 255, 0, 0);
	pal.GenerateGradient(85, 170, 255, 0, 0, 0, 255, 0);
	pal.GenerateGradient(170, 255, 0, 255, 0, 0, 0, 255);
	gfx.SetPalette(pal);
}

// Simply anti-alias the framebuffer
void antiAlias() {
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

void drawCEllipse(int x, int y, int rx, int ry) {
	int c = gfx.GetColors();

	// Draw a few concentric circles
	for (int m = MAX(rx, ry); m > 0; m--) {
		gfx.DrawFilledEllipse(x, y, rx--, ry--, --c);
	}
}

void drawCBox(int l, int t, int r, int b) {
	int c = gfx.GetColors();
	int w = abs(1 + r - l) / 2;
	int h = abs(1 + b - t) / 2;

	for (int m = MAX(w, h); m > 0; m--) {
		gfx.DrawFilledBox(l++, t++, r--, b--, --c);
	}
}

// Initialize the plasma
void drawSmiley() {
	drawCEllipse(159, 99, 90, 80);
	drawCEllipse(121, 79, 18, 15);
	drawCEllipse(197, 79, 18, 15);
	drawCBox(157, 101, 161, 121);
	drawCEllipse(159, 141, 39, 9);
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

	drawSmiley();
	antiAlias();

	kb.Clear();
	do {
		pal.RotateLeft(1, 254);
		gfx.WaitForRetrace();
		gfx.SetPalette(pal);
		Processor::Yield();
	} while (!kb.IsWaiting());

	kb.Clear();
	do {
		pal.RotateRight(1, 254);
		gfx.WaitForRetrace();
		gfx.SetPalette(pal);
		Processor::Yield();
	} while (!kb.IsWaiting());
}
