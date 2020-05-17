/**********************************************************************\

	FISHTANK.CPP
	This program demonstrates multi-object non-destructive
	animation using the gFrame Class Library.

	Copyright (c) Samuel Gomes (Blade), 1999-2003
	mailto: blade_go@hotmail.com

\**********************************************************************/

#include <gFrame.h>

#define NFISH 11
#define NFISHTYPE 6

#pragma warn -inl

// Fish details in the fish bitmap
static const struct {
	int x;	// left
	int y;	// bottom!!!
	int w;	// width
	int h;	// height
} FishBound[NFISHTYPE] = {
	{  0, 199, 56, 25},
	{ 64, 199, 54, 38},
	{128, 199, 68, 26},
	{200, 199, 56, 30},
	{  0, 150, 62, 22},
	{ 80, 150, 68, 36}
};

/*
*	There are 11 fish total, and 6 different kinds of fish.  These
*   arrays keep track of what kind of fish each fish is, and how each
*   fish moves:
*
*   tfish  -- which fish bitmap applies to this fish?
*   x      -- starting x coordinate
*   y      -- starting y coordinate
*
*   xmin   -- how far left (off screen) the fish can go
*   xmax   -- how far right (off screen) the fish can go
*   xinc   -- how fast the fish goes left and right
*   dir    -- starting direction for each fish
*
*   ymin   -- how far up this fish can go
*   ymax   -- how far down this fish can go
*   yinc   -- how fast the fish moves up or down
*   yturn  -- how long fish can go in the vertical direction
*               before stopping or turning around
*   ycount -- counter to compare to yturn
*/
static struct {
	int tfish;
	int x;
	int y;
	int xmin;
	int xmax;
	int xinc;
	int dir;
	int ymin;
	int ymax;
	int yturn;
	int ycount;
	int yinc;
} FishCharacteristic[NFISH] = {
	{1, -100,  40, -300,  600,  2, 0,  40,  80, 50, 0, 0},
	{1, -150,  60, -300,  601,  2, 0,  60, 100, 30, 0, 0},
	{2, -450, 150, -800, 1100,  8, 0, 120, 170, 10, 0, 0},
	{3, -140,  80, -200, 1000,  5, 0,  70, 110, 30, 0, 0},
	{3, -200,  70, -200, 1001,  5, 0,  60, 100, 20, 0, 0},
	{0,  520, 190, -200,  750, -3, 1, 160, 199, 10, 0, 0},
	{0,  620, 180, -300,  800, -3, 1, 160, 199, 10, 0, 0},
	{5, -800, 100, -900, 1200,  7, 0,  80, 120, 10, 0, 0},
	{4,  800,  30, -900, 1400, -8, 1,  30,  70, 30, 0, 0},
	{2,  800, 130, -900, 1201, -9, 1, 110, 150, 20, 0, 0},
	{3, -300,  92, -400,  900,  6, 0,  72, 122, 10, 0, 0}
};

// Fish class - controls fish movement and behaviour
class Fish {
	friend Fishtank;

	int tfish;
	int x;
	int y;
	int xmin;
	int xmax;
	int xinc;
	int dir;
	int ymin;
	int ymax;
	int yturn;
	int ycount;
	int yinc;

public:
	// Initialize fish
	Fish() {
		tfish = x = y = xmin = xmax = xinc = dir =
		ymin = ymax = yturn = ycount = yinc = 0;
	}

	// Set fish characteristics
	void Set(int i) {
		tfish = FishCharacteristic[i].tfish;
		x = FishCharacteristic[i].x;
		y = FishCharacteristic[i].y;
		xmin = FishCharacteristic[i].xmin;
		xmax = FishCharacteristic[i].xmax;
		xinc = FishCharacteristic[i].xinc;
		dir = FishCharacteristic[i].dir;
		ymin = FishCharacteristic[i].ymin;
		ymax = FishCharacteristic[i].ymax;
		yturn = FishCharacteristic[i].yturn;
		ycount = FishCharacteristic[i].ycount;
		yinc = FishCharacteristic[i].yinc;
	}

	// Fish movement and behaviour
	void Go() {
		ycount++;
		if (ycount > yturn) {
			ycount = 0;
			yinc = random(3) - 1;
		}
		y += yinc;
		y = MIN(ymax, MAX(y, ymin));

		x += (xinc >> 1);
		if (x <= xmin || x >= xmax) {
			xinc = -xinc;
			dir = 1 - dir;
		}
	}
};

// Fishtank class - controls the full simulation
class Fishtank {
	Graphics gfx;
	Keyboard kb;
	Archive ach;
	Palette pal;
	Bitmap bmpCoral;
	Bitmap bmpFish[NFISHTYPE];
	Fish fish[NFISH];

public:
	// Initalize fishtank
	Fishtank() {
		Bitmap bmpTmp;
		int i;

		// Seed randomizer
		randomize();

		// Set default resource file
		ach.SetArchiveFile("fishtank.pak");

		// Load background pic
		ach.FileOpen("graphics/coral.pcx");
		bmpCoral.LoadPalette(ach, pal);
		bmpCoral.Load(ach, pal);

		// Set graphics mode
		gfx.Initialize();
		gfx.SetFont(FONT_8x8);

		// Load fish bitmap
		ach.FileOpen("graphics/fish.pcx");
		bmpTmp.Load(ach, pal);

		// Blit the fish bitmap to the framebuffer
		gfx.PutBitmap(bmpTmp, 0, 0, BITMAP_OPAQUE);

		// Get individual fish bitmaps
		for (i = 0; i < NFISHTYPE; i++) {
			bmpFish[i].Create(FishBound[i].w, FishBound[i].h);
			gfx.GetBitmap(bmpFish[i], FishBound[i].x, 1 + FishBound[i].y - FishBound[i].h);
		}

		// Initialize individual fish characteristics
		for (i = 0; i < NFISH; i++) {
			fish[i].Set(i);
		}
	}

	// Make the whole fishtank go
	void Go() {
		int i, mx;
		Palette pNew;
		bool bBlended = false;
		char sMsg[] = "This program demonstrates multi-object non-destructive animation using the gFrame Class Library. This simple example also shows some of the power of the Graphics::PutBitmap() method. Check out the source code for more details! If you have any question/suggestions/etc., mail me at 'blade_go@hotmail.com'. Watching fishes swim is a healthy activity. ;)";

		mx = gfx.GetWidth();

		while (!kb.IsWaiting()) {
			// Blit the coral background
			gfx.PutBitmap(bmpCoral, 0, 0, BITMAP_OPAQUE);

			// Blit and make all the fish go
			for (i = 0; i < NFISH; i++) {
				gfx.PutBitmap(bmpFish[fish[i].tfish], fish[i].x, 1 + fish[i].y - FishBound[fish[i].tfish].h, (fish[i].dir ? 0 : BITMAP_HFLIP));
				fish[i].Go();
			}

			// Wait for the vertical retrace then flip
			gfx.WaitForRetrace();

			// Blend in the palette
			if (!bBlended) {
				gfx.SetPalette(pNew);
				bBlended = pNew.Blend(pal);
			}

			// Scroll a cute marquee
			gfx.Print(mx, gfx.GetHeight() - gfx.GetFontHeight(), 1, sMsg);
			mx--;
			if (abs(mx) > (strlen(sMsg) * gfx.GetFontWidth())) {
				mx = gfx.GetWidth();
			}

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
	Fishtank *fishtank;

	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}

	fishtank = new Fishtank();
	fishtank->Go();
	delete fishtank;
}
