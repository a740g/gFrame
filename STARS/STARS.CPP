// Stars
// mailto: blade_g@rediffmail.com

#include <gFrame.h>

#define MAX_STARS 256
#define DIVIDER 1024

Graphics gfx;
Keyboard kb;

// star class
class Star {
	float x;
	float y;
	word z;
	byte c;
	word a, b;

public:
	Star();

	void Go();
};

Star::Star() {
	x = y = z = c = 0;
	a = gfx.GetWidth() >> 1;
	b = gfx.GetHeight() >> 1;
}

void Star::Go() {
	gfx.SetPixel(x, y, 0);
	if ((x > gfx.GetWidth() - 2) || (x < 1) || (y > gfx.GetHeight() - 2) || (y < 1)) {
		x = random(gfx.GetWidth());
		y = random(gfx.GetHeight());
		z = DIVIDER;
		c = random(gfx.GetColors());
	}
	z = z + 1;
	x = ((x - a) * ((float)z / DIVIDER)) + a;
	y = ((y - b) * ((float)z / DIVIDER)) + b;
	gfx.SetPixel(x, y, c);
}

int main() {
	int i = 0;
	Star *star = NULL;

	randomize();

	star = new Star[MAX_STARS];

	gfx.Initialize();
	gfx.DirectVideo(true);

	while (!kb.IsWaiting()) {
		gfx.WaitForRetrace();
		for (i = 0; i < MAX_STARS; i++) {
			star[i].Go();
		}
		Processor::Yield();
	}

	gfx.Terminate();

	delete[] star;

	return EXIT_SUCCESS;
}
