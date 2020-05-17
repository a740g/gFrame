// Test stub

#include <gFrame.h>

void main() {
	Graphics g;
	Keyboard kb;
	Mouse ms;
	Palette pOld, pNew1, pNew2;
	Font fnt;
	Archive f;
	Bitmap bmpCursor;
	int mx = 0, my = 0, mxOld = 0, myOld = 0;
	byte clr = 1;
	bool lb, rb;

	// Initialize overlay manager
	if (_OvrInitEms(0, 0, 0) == 0) {
		cout << "Using EMS for overlay swapping." << endl;
	}
	if (_OvrInitExt(0, 0) == 0) {
		cout << "Using XMS for overlay swapping." << endl;
	}
	kb.WaitPressClear();

	randomize();

	// Switch to graphics mode
	g.Initialize();

	// Draw directly to video buffer
	g.DirectVideo(true);

	// Set the border color
	g.SetBorderColor(14);

	g.Print(0, 0, 11, "Press any key to start the DOS gFrame");
	g.Print(0, 16, 11, "Class Library demo...");
	g.Print(0, 32, 13, "CPU = %s", Processor::GetCPUName());
	g.Print(0, 48, 13, "OS = %s", Diagnostic::GetOSName());
	g.Print(0, 64, 13, "Memory = %ld KB", Diagnostic::GetTotalMemory());
	kb.WaitPressClear();

	g.DrawEllipse(159, 99, 80, 70, 12);
	g.FloodFill(159, 99, 10, 12);
	f.FileOpen("dig8x16.fnt");
	fnt.Load(f);
	g.SetFont(fnt);
	g.SetFontDirection(FONT_UP);
	g.Print(10, 142, 12, "Hello World!");
	g.SetFontDirection(FONT_DOWN);
	g.Print(310, 55, 12, "Hello World!");
	g.SetFontDirection(FONT_HORIZONTAL);
	g.Print(113, 90, 1, "Hello World!");
	kb.WaitPressClear();
	g.SetFontTransparency(false);
	g.SetFontBackgroundColor(BLUE);

	while (!kb.IsWaiting()) {
		g.Clear(random(256));
	}
	g.Print(0, 0, 13, "Screen clear demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.SetPixel(random(320), random(200), random(256));
	}
	g.Print(0, 0, 13, "Pixel drawing demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawHorizontalLine(0, random(200), 319, random(256));
	}
	g.Print(0, 0, 13, "Horizontal line demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawVerticalLine(random(320), 0, 199, random(256));
	}
	g.Print(0, 0, 13, "Vertical line demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawFilledBox(random(320), random(200), random(320), random(200), random(256));
	}
	g.Print(0, 0, 13, "Filled box demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawBox(random(320), random(200), random(320), random(200), random(256));
	}
	g.Print(0, 0, 13, "Box demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawLine(random(320), random(200), random(320), random(200), random(256));
	}
	g.Print(0, 0, 13, "Line demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawCircle(random(320), random(200), random(64), random(256));
	}
	g.Print(0, 0, 13, "Circle demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawFilledCircle(random(320), random(200), random(64), random(256));
	}
	g.Print(0, 0, 13, "Filled circle demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawEllipse(random(320), random(200), random(64), random(64), random(256));
	}
	g.Print(0, 0, 13, "Ellipse demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawFilledEllipse(random(320), random(200), random(64), random(64), random(256));
	}
	g.Print(0, 0, 13, "Filled ellipse demo.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.DrawCircle(159, 99, random(320), random(256));
	}
	g.Print(0, 0, 13, "Concentric circles demo.");
	kb.WaitPressClear();

	g.SetFontTransparency(true);
	while (!kb.IsWaiting()) {
		g.SetFontDirection(random(4));
		g.Print(random(320), random(200), random(256), "Hello World!");
	}
	g.SetFontDirection(FONT_HORIZONTAL);
	g.SetFontTransparency(false);
	g.Print(0, 0, 13, "Font demo with trasparent background.");
	kb.WaitPressClear();

	while (!kb.IsWaiting()) {
		g.SetFontDirection(random(4));
		g.SetFontBackgroundColor(random(256));
		g.Print(random(320), random(200), random(256), "Hello World!");
	}
	g.SetFontDirection(FONT_HORIZONTAL);
	g.SetFontBackgroundColor(BLUE);
	g.Print(0, 0, 13, "Font demo with opaque background.");
	kb.WaitPressClear();

	// Get the original palette
	g.GetPalette(pOld);
	g.GetPalette(pNew1);
	// Fade to black
	do {
		g.WaitForRetrace();
		g.SetPalette(pNew1);
	} while (!pNew1.Decrement());
	// Set up a gradient like palette
	pNew2.GenerateGradient(0, 85, 0, 0, 255, 255, 0, 0);
	pNew2.GenerateGradient(85, 170, 255, 0, 0, 0, 255, 0);
	pNew2.GenerateGradient(170, 255, 0, 255, 0, 0, 0, 255);
	// Draw a few concentric circles
	for (int i = g.GetColors(); i > 0; i--) {
		g.DrawFilledCircle(159, 99, i, i - 1);
	}
	// Fade in to the new palette
	do {
		g.WaitForRetrace();
		g.SetPalette(pNew1);
	} while (!pNew1.Blend(pNew2));
	// Rotate the palette to the right
	while (!kb.IsWaiting()) {
		pNew1.RotateRight();
		g.WaitForRetrace();
		g.SetPalette(pNew1);
		Processor::Yield();
	}
	kb.Clear();
	// Rotate the palette to the left
	while (!kb.IsWaiting()) {
		pNew1.RotateLeft();
		g.WaitForRetrace();
		g.SetPalette(pNew1);
		Processor::Yield();
	}
	// Fadeout to all white
	do {
		g.WaitForRetrace();
		g.SetPalette(pNew1);
	} while (!pNew1.Increment());
	kb.WaitPress();
	g.SetPalette(pOld);

	g.Clear();
	g.Print(0, 0, 11, "Keyboard demo (1234567890, ESC to end):");
	for (;;) {
		if (kb.WasKeyPressed(KEY_ESC)) break;

		if (kb.WasKeyPressed(KEY_0)) {
			g.Print(0, 16, 10, "0");
		}
		else {
			g.Print(0, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_1)) {
			g.Print(8, 16, 10, "1");
		}
		else {
			g.Print(8, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_2)) {
			g.Print(16, 16, 10, "2");
		}
		else {
			g.Print(16, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_3)) {
			g.Print(24, 16, 10, "3");
		}
		else {
			g.Print(24, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_4)) {
			g.Print(32, 16, 10, "4");
		}
		else {
			g.Print(32, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_5)) {
			g.Print(40, 16, 10, "5");
		}
		else {
			g.Print(40, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_6)) {
			g.Print(48, 16, 10, "6");
		}
		else {
			g.Print(48, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_7)) {
			g.Print(56, 16, 10, "7");
		}
		else {
			g.Print(56, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_8)) {
			g.Print(64, 16, 10, "8");
		}
		else {
			g.Print(64, 16, 10, " ");
		}

		if (kb.WasKeyPressed(KEY_9)) {
			g.Print(72, 16, 10, "9");
		}
		else {
			g.Print(72, 16, 10, " ");
		}
	}
	kb.Clear();

	f.FileOpen("pointer.pcx");
	bmpCursor.Load(f, pOld);

	g.Clear();
	g.PutBitmap(bmpCursor, 151, 91);
	g.Print(0, 0, 11, "Mouse demo:");
	g.SetFontDirection(FONT_HORIZONTAL);
	g.SetFontTransparency(false);
	ms.SetCursorBitmap(bmpCursor, 8, 7);
	ms.ShowCursor();
	while (!kb.IsWaiting()) {
		ms.Poll(mx, my, lb, rb);
		g.Print(0, 16, 12, "X = %3d Y = %3d LB = %d RB = %d PC = %3d", mx, my, lb, rb, (int)clr);
		if (lb)	{
			ms.HideCursor();
			g.DrawLine(mxOld, myOld, mx, my, clr);
			ms.ShowCursor();
		}
		if (rb) {
			clr++;
		}
		if (lb && rb) {
			ms.HideCursor();
			g.Clear();
			ms.ShowCursor();
		}
		mxOld = mx;
		myOld = my;
	}
	ms.HideCursor();
	kb.Clear();

	char sMsg[] = "This is the end of the gFrame Class Library demo. Hope you enjoyed it. Go through the source code to find out more about the library. Currently I am working on the Win32/DirectX version of the gFrame Library. If you have any question/suggestions/etc., mail me at 'blade_go@hotmail.com'. Now press any key to exit this demo... :)";
	g.Clear();
	g.SetFontBackgroundColor(BLACK);
	mx = 320;
	while (!kb.IsWaiting()) {
		Processor::Yield();
		g.WaitForRetrace();
		g.Print(mx, 92, 15, sMsg);
		mx--;
		if (abs(mx) > (strlen(sMsg) * 8)) {
			mx = 320;
		}
	}
}
