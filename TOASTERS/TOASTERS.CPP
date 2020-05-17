////////////////////////////////////////////////////////////////////////
//
//	Flying Toasters: A simple animation demo using the gFrame Library
//
//	Copyright (c) Samuel Gomes (Blade), 1999-2003
//	mailto: blade_go@hotmail.com
//
////////////////////////////////////////////////////////////////////////

#include <gFrame.h>

#define NUMCIRCLES 200
#define NUMTOASTERS 3
#define NUMTOASTERFRAMES 3

#pragma warn -inl

// Circle pixelmap
const byte cCircleMap[5][5] = {
	{ 0, 10, 10, 10,  0},
	{10, 13, 12, 11, 10},
	{10, 12, 12, 11, 10},
	{10, 11, 11, 11, 10},
	{ 0, 10, 10, 10,  0}
};

// Toaster class (sans bread ;)
class Toaster {
	friend Demo;

	int x;
	int y;
	word speed;
	word frame;

public:
	Toaster() {
		// Non realistic values
		x = -319;
		y = 398;
		speed = frame = 0;
	}

	void Go() {
		// Move the toaster
		x -= speed;
		y += speed;

		// Increment the frame
		frame++;
		if (frame > 4) frame = 1;

		// When toaster reaches the edge of the screen, render it inactive
		// and bring a new one into existance.
		if ((x < -48) || (y > 199)) {
			x = random(272) + 48;
			y = -30;
			frame = 1;
			speed = random(3) + 1;
		}
	}
};

class Demo {
	Graphics gfx;
	Keyboard kb;
	Archive ach;
	Palette pal;
	Toaster toaster[NUMTOASTERS];
	Bitmap bmpToaster[NUMTOASTERFRAMES];
	Bitmap bmpBackground;

public:
	Demo() {
		int x, y, loop1, loop2, loop3;

		// Seed randomizer
		randomize();

		// Set default resource file
		ach.SetArchiveFile("toasters.pak");

		// Load toaster frames
		ach.FileOpen("graphics/toaster1.pcx");
		bmpToaster[0].LoadPalette(ach, pal);
		bmpToaster[0].Load(ach, pal);
		ach.FileOpen("graphics/toaster2.pcx");
		bmpToaster[1].Load(ach, pal);
		ach.FileOpen("graphics/toaster3.pcx");
		bmpToaster[2].Load(ach, pal);

		// Set graphics mode
		gfx.Initialize();

		// Set pallette colors 50-100 to blue values between 13 and 63
		for (loop1 = 50; loop1 < 101; loop1++) {
			pal.Set(loop1, 0, 0, (loop1 - 37) << 2);
		}

		// draw funny lines
		for (loop1 = 0; loop1 < gfx.GetWidth(); loop1++) {
			DrawFunnyLine(0, 199, loop1, 0);
		}
		for (loop1 = 0; loop1 < gfx.GetHeight(); loop1++) {
			DrawFunnyLine(0, 199, 319, loop1);
		}

		// Draw circles
		for (loop1 = 0; loop1 < NUMCIRCLES; loop1++) {
			x = random(315);
			y = random(195);
			for (loop2 = 0; loop2 < 5; loop2++) {
				for (loop3 = 0; loop3 < 5; loop3++) {
					// don't plot a pixel if its color is 0
					if (cCircleMap[loop2][loop3] != 0) {
						gfx.SetPixel(x + loop2, y + loop3, cCircleMap[loop2][loop3]);
					}
				}
			}
		}

		// Make a copy of the background
		bmpBackground.Create(gfx.GetWidth(), gfx.GetHeight());
		gfx.GetBitmap(bmpBackground, 0, 0);

		// Set the initial palette
		gfx.WaitForRetrace();
		gfx.SetPalette(pal);
	}

	// Draws "funny lines"
	void DrawFunnyLine (int a, int b, int c, int d) {
		int i, s, d1x, d1y, d2x, d2y, u, v, m, n, count;

		count = 50;
		u = c - a;       // u = x2 - x1  (change in x)
		v = d - b;       // v = y2 - y1  (change in y)

		d1x = SGN(u);    // d1x = sign of the change in x
		d1y = SGN(v);    // d1y = sign of the change in y

		d2x = SGN(u);    // d2x = sign of the change in x
		d2y = 0;         // d2y = 0
		m = abs(u);      // m = positive change in x
		n = abs(v);      // n = positive change in y

		if (m <= n) {
			d2x = 0;       // d2x = 0
			d2y = SGN(v);  // d2y = sign of the change in y
			m = abs(v);    // m = positive change in y
			n = abs(u);    // n = positive change in x
		}
		s = m / 2;       // s = half of the change in x/y

		for (i=0;i<m+1;i++) {
			gfx.SetPixel(a, b, count);   // plot the original x1 and y1
			count++;                     // increment the color
			if (count > 100) count = 50; // if color is out of bounds, reset
			s += n;
			if (s >= m) {
				s -= m;
				a += d1x;
				b += d1y;
			}
			else {
				a += d2x;
				b += d2y;
			}
		}
	}

	void Run() {
		int i, mx, frame;
		Palette pNew;
		bool bBlended = false;
		char sMsg[] = "This program demonstrates multi-object non-destructive animation using the gFrame Class Library. This simple example shows some of the power of the Graphics::PutBitmap() method. This example also shows the usage of the Palette class. Check out the source code for more details! If you have any question/suggestions/etc., mail me at 'blade_go@hotmail.com'. Watching toasters fly is NOT a healthy activity. ;)";

		mx = gfx.GetWidth();

		while (!kb.IsWaiting()) {
			// Blit the background
			gfx.PutBitmap(bmpBackground, 0, 0, BITMAP_OPAQUE);

			// Blit and make all the toasters go
			for (i = 0; i < NUMTOASTERS; i++) {
				toaster[i].Go();

				// Select the actual bitmap frame
				switch (toaster[i].frame) {
					case 1:
						frame = 0;
						break;
					case 2:
						frame = 1;
						break;
					case 3:
						frame = 2;
						break;
					case 4:
						frame = 1;
						break;
					default:
						frame = 0;
				}

				gfx.PutBitmap(bmpToaster[frame], toaster[i].x, toaster[i].y);
			}

			// Blend in the palette
			if (bBlended) {
				pNew.RotateRight(50, 100);
				gfx.SetPalette(pNew, 50, 100);
			}
			else {
				gfx.SetPalette(pNew);
				bBlended = pNew.Blend(pal);
			}

			// Scroll a cute marquee
			gfx.Print(mx, gfx.GetHeight() - gfx.GetFontHeight(), 4, sMsg);
			mx--;
			if (abs(mx) > (strlen(sMsg) * gfx.GetFontWidth())) {
				mx = gfx.GetWidth();
			}

			// Wait for the vertical retrace then flip
			gfx.WaitForRetrace();
			gfx.Flip();
			Processor::Yield();
		}
		kb.WaitPress();

		// Fade out
		do {
			gfx.WaitForRetrace();
			gfx.SetPalette(pNew);
		} while (!pNew.Decrement());
	}
};

// Entry point
void main() {
	Demo *demo;

	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}

	demo = new Demo();
	demo->Run();
	delete demo;
}
