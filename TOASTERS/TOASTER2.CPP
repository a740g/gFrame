////////////////////////////////////////////////////////////////////////
//
//	Flying Toasters: A simple animation demo using the gFrame Library
//
//	Copyright (c) Samuel Gomes (Blade), 1999-2003
//	mailto: blade_go@hotmail.com
//
////////////////////////////////////////////////////////////////////////

#include <gFrame.h>

#define NUMTOASTERS 3
#define NUMTOASTERFRAMES 3

#pragma warn -inl

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
		// Seed randomizer
		randomize();

		// Set default resource file
		ach.SetArchiveFile("toasters.pak");

		// Load background pic
		ach.FileOpen("graphics/moon.pcx");
		bmpBackground.LoadPalette(ach, pal);
		bmpBackground.Load(ach, pal);
		// Load toaster frames
		ach.FileOpen("graphics/toaster1.pcx");
		bmpToaster[0].Load(ach, pal);
		ach.FileOpen("graphics/toaster2.pcx");
		bmpToaster[1].Load(ach, pal);
		ach.FileOpen("graphics/toaster3.pcx");
		bmpToaster[2].Load(ach, pal);

		// Set graphics mode
		gfx.Initialize();
		gfx.SetFont(FONT_8x8);

		// Set the initial palette
		gfx.WaitForRetrace();
		gfx.SetPalette(pal);
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
			if (!bBlended) {
				gfx.SetPalette(pNew);
				bBlended = pNew.Blend(pal);
			}

			// Scroll a cute marquee
			gfx.Print(mx, gfx.GetHeight() - gfx.GetFontHeight(), 128, sMsg);
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
