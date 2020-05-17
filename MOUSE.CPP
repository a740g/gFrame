/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Mouse handling interface.     											 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

// Mouse int
#define MOUSE_INT 0x33

/* Mouse initialization routine */
Mouse::Mouse() {
	bMousePresent = false;
	Initialize();
}

/* Shuts down the mouse */
Mouse::~Mouse() {
	Terminate();
}

bool Mouse::Initialize() {
	byte *mouseVect;

	if (bMousePresent) return true;

	mouseVect = (byte *)_dos_getvect(MOUSE_INT);
	if (mouseVect == NULL || *mouseVect == 0xCF || *mouseVect == 0x40) {
		bMousePresent = false;
		return false;
	}

	_asm {
		xor ax, ax;
		int MOUSE_INT;
	}

	bMousePresent = bool(_AX);

	return bMousePresent;
}

void Mouse::Terminate() {
	HideCursor();
	bMousePresent = false;
}

bool Mouse::IsAvailable() {
	return bMousePresent;
}

/* Shows the mouse pointer */
void Mouse::ShowCursor() {
	if (!bMousePresent) return;

	_asm {
		mov ax, 0x0001;
		int MOUSE_INT;
	}
}

/* Hides mouse pointer */
void Mouse::HideCursor() {
	if (!bMousePresent) return;

	_asm {
		mov ax, 0x0002;
		int MOUSE_INT;
	}
}

/* Returns mouse status */
void Mouse::Poll(int &mouseX, int &mouseY, bool &lButton, bool &rButton) {
	if (!bMousePresent) return;

	_asm {
		mov ax, 0x0003;
		int MOUSE_INT;
	}

	lButton = bool(_BX & 3);
	rButton = lButton & 2;
	lButton = lButton & 1;
	mouseX = _CX >> 1;
	mouseY = _DX;
}

/* Sets the mouse pointer position */
void Mouse::SetPosition(int mouseX, int mouseY) {
	if (!bMousePresent) return;

	_CX = mouseX << 1;
	_DX = mouseY;
	_asm {
		mov ax, 0x0004;
		int MOUSE_INT;
	}
}

/* Sets max and min bounds on mouse movement both vertically, and horizontally */
void Mouse::SetCursorBounds(int mLeft, int mTop, int mRight, int mBottom) {
	if (!bMousePresent) return;

	/* Limit X-axis */
	_CX = mLeft << 1;
	_DX = mRight << 1;
	_asm {
		mov ax, 0x0007;
		int MOUSE_INT;
	}

	/* Limit Y-axis */
	_CX = mTop;
	_DX = mBottom;
	_asm {
		mov ax, 0x0008;
		int MOUSE_INT;
	}
}

/* Sets the cursor bitmap */
bool Mouse::SetCursorBitmap(const Bitmap &b, int hsx, int hsy) {
	word x, y;

	if (!bMousePresent || b.cBitmap == NULL || b.wWidth != 16 || b.wHeight != 16) return false;

	// Prepare the cursor bitmask (AND)
	for (y = 0; y < 16; y++) {
		wCursor[y] = 0x0000;
		for (x = 0; x < 16; x++) {
			wCursor[y] = wCursor[y] | ((word)((byte)*(b.cBitmap + b.wWidth * y + x) ? 0 : 1) << (15 - x));
		}
	}

	// Prepare the cursor bitmask (XOR)
	for (y = 0; y < 16; y++) {
		wCursor[y + 16] = 0x0000;
		for (x = 0; x < 16; x++) {
			wCursor[y + 16] = wCursor[y + 16] | ((word)((byte)*(b.cBitmap + b.wWidth * y + x) ? 1 : 0) << (15 - x));
		}
	}

	_ES = FP_SEG(wCursor);
	_DX = FP_OFF(wCursor);

	_asm {
		mov cx, hsy;
		mov bx, hsx;
		mov ax, 0x9;
		int MOUSE_INT;
	}

	return true;
}

byte Mouse::GetVersion(byte &cVerMajor, byte &cVerMinor) {
	if (!bMousePresent) return 0;

	_asm {
		mov ax, 0x0024;
		int MOUSE_INT;
	}

	cVerMajor = _BH;
	cVerMinor = _BL;
	return _CH;
}
