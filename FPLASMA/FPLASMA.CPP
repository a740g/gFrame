// Simple Plasma demo using the gFrame Class Library
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
	//pal.GenerateGradient(0, 85, 127, 127, 127, 255, 255, 255);
	//pal.GenerateGradient(85, 170, 255, 255, 255, 0, 0, 255);
	//pal.GenerateGradient(170, 255, 0, 0, 255, 127, 127, 127);
	gfx.SetPalette(pal);
}

// Plots the correct plasma pixels with the correct color
void setColor(int xa, int ya, int x, int y, int xb, int yb) {
	int color;

	if (gfx.GetPixel(x, y) != 0) return;

	color = abs(xa - xb) + abs(ya - yb);
	color = random(color * 2) - color;
	color += ((gfx.GetPixel(xa, ya) + gfx.GetPixel(xb ,yb)) / 2);
	if (color < 1) color = 1;
	if (color >= gfx.GetColors()) color = gfx.GetColors() - 1;
	gfx.SetPixel(x, y, color);
}

// Recursive fractal plasma algorithm
void plasmaSubDivide(int x1, int y1, int x2, int y2) {
	int x, y;
	int color;

	if ((x2 - x1) < 2 && (y2 - y1) < 2) return;

	x = (x1 + x2) / 2;
	y = (y1 + y2) / 2;

	setColor(x1, y1, x, y1, x2, y1);
	setColor(x2, y1, x2, y, x2, y2);
	setColor(x1, y2, x, y2, x2, y2);
	setColor(x1, y1, x1, y, x1, y2);

	if (gfx.GetPixel(x, y) == 0) {
		color = (gfx.GetPixel(x1, y1) + gfx.GetPixel(x2, y1) + gfx.GetPixel(x2, y2) + gfx.GetPixel(x1, y2)) / 4;
		if (color < 1) color = 1;
		if (color >= gfx.GetColors()) color = gfx.GetColors() - 1;
		gfx.SetPixel(x, y, color);
	}

	plasmaSubDivide(x1, y1, x, y);
	plasmaSubDivide(x, y1, x2, y);
	plasmaSubDivide(x, y, x2, y2);
	plasmaSubDivide(x1, y, x, y2);
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

	plasmaSubDivide(0, 0, gfx.GetWidth() - 1, gfx.GetHeight() - 1);

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
