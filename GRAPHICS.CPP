/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Graphics Primitives for VGA mode 13h (only!)							 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

/*
Feature Set TODO:
	1) Implement viewport support someday
*/

/* Macros to quickly plot/get a point on any surface and stuff */
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_COLORS 256
#define VIDEO_INT 0x10
#define CLIP(_x_, _y_) ((_x_) < 0 || (_x_) >= SCREEN_WIDTH || (_y_) < 0 || (_y_) >= SCREEN_HEIGHT)
#define BUFFER_POINTER(_pbuf_, _px_, _py_) ((_pbuf_) + SCREEN_WIDTH * (word)(_py_) + (word)(_px_))
#define SET_PIXEL(_buf_, _x_, _y_, _c_) *BUFFER_POINTER(_buf_, _x_, _y_) = (_c_)
#define GET_PIXEL(_buf_, _x_, _y_) *BUFFER_POINTER(_buf_, _x_, _y_)
#define FONT_WIDTH 8
#define PRINT_BUFFERSIZE 1024
#define SPRITE_BITMAPS_MAX 64

// Some global constants
const dword Font::Signature = ID('F', 'O', 'N', 'T');

/***************************** Graphics code ******************************/

// Graphics ctor
Graphics::Graphics() {
	bIsVGA = false;
	cVideoBuffer = NULL;
	cFrameBuffer = NULL;
	cActiveBuffer = NULL;
	cFont = NULL;
	cFontHeight = 0;
	cFontDirection = FONT_HORIZONTAL;
	bFontOpaque = false;
	cFontBackground = BLACK;
	cFont8x8 = NULL;
	cFont8x16 = NULL;
}

// Graphics copy ctor
Graphics::Graphics(const Graphics &g) {
	bIsVGA = g.bIsVGA;
	cVideoBuffer = g.cVideoBuffer;
	cFrameBuffer = g.cFrameBuffer;
	cFont = g.cFont;
	cFontHeight = g.cFontHeight;
	cFontDirection = g.cFontDirection;
	bFontOpaque = g.bFontOpaque;
	cFontBackground = g.cFontBackground;
	cFont8x8 = g.cFont8x8;
	cFont8x16 = g.cFont8x16;

	if (bIsVGA && cFrameBuffer != NULL) {
		cFrameBuffer = new byte[SCREEN_WIDTH * SCREEN_HEIGHT];
		if (cFrameBuffer == NULL) {
			bIsVGA = false;
			return;
		}
		memcpy(cFrameBuffer, g.cFrameBuffer, SCREEN_WIDTH * SCREEN_HEIGHT);
	}

	cActiveBuffer = (g.cActiveBuffer == g.cVideoBuffer ? cVideoBuffer : cFrameBuffer);
}

// Graphics dtor
Graphics::~Graphics() {
	Terminate();
}

bool Graphics::IsAvailable() {
	return bIsVGA;
}

// Switch from text mode to graphics mode
bool Graphics::Initialize() {
	REGPACK regs;

	// Ok if we are done already
	if (bIsVGA) return true;

	// Check if we have a VGA (everybody should ;))
	_asm {
		mov ax, 0x1A00;
		int VIDEO_INT;
	}
	bIsVGA = bool(_AL == 0x1A && _BL > 6);
	if (!bIsVGA) return false;	// no VGA found!

	// Initialize the framebuffers
	cVideoBuffer = (byte *)MK_FP(0xA000, 0x0000);
	cFrameBuffer = new byte[SCREEN_WIDTH * SCREEN_HEIGHT];
	if (cFrameBuffer == NULL) {
		bIsVGA = false;
		return false;	// no memory!
	}
	cActiveBuffer = cFrameBuffer;

	// Finally setup some stuff and the screen mode
	_asm {
		// Install video BIOS printscreen handler
		mov bx, 0x0020;
		mov ax, 0x1200;
		int VIDEO_INT;
		// Set the screen mode
		mov ax, 0x0013;
		int VIDEO_INT;
	}

	// Get default font addresses (modifying bp using _asm is a real pain in the arse!)
	memset(&regs, 0, sizeof(regs));
	regs.r_bx = 0x0300;
	regs.r_ax = 0x1130;
	intr(VIDEO_INT, &regs);
	cFont8x8 = (byte *)MK_FP(regs.r_es, regs.r_bp);

	memset(&regs, 0, sizeof(regs));
	regs.r_bx = 0x0600;
	regs.r_ax = 0x1130;
	intr(VIDEO_INT, &regs);
	cFont8x16 = (byte *)MK_FP(regs.r_es, regs.r_bp);

	// Select the default font
	SetFont(FONT_8x16);
	SetFontDirection(FONT_HORIZONTAL);
	SetFontTransparency(true);
	SetFontBackgroundColor(BLACK);
	Clear();

	return true;
}

// Switch from graphics mode to text mode
void Graphics::Terminate() {
	if (!bIsVGA) return;

	// Sets text mode 80 x 25
	_asm {
		mov ax, 0x0003;
		int VIDEO_INT;
	}

	// Free the framebuffer memory if it was allocated
	if (cFrameBuffer != NULL) {
		delete[] cFrameBuffer;
		cFrameBuffer = NULL;
	}

	cActiveBuffer = NULL;
	cVideoBuffer = NULL;
	bIsVGA = false;		// graphics not available
}

word Graphics::GetWidth() {
	return SCREEN_WIDTH;
}

word Graphics::GetHeight() {
	return SCREEN_HEIGHT;
}

word Graphics::GetColors() {
	return SCREEN_COLORS;
}

/* Sets the screen border color */
void Graphics::SetBorderColor(byte c) {
	if (!bIsVGA) return;

	_asm {
		xor bl, bl;
		mov bh, c;
		mov ax, 0x1001;
		int VIDEO_INT;
	}
}

// Returns the current border color
byte Graphics::GetBorderColor() {
	if (!bIsVGA) return 0;

	_asm {
		mov ax, 0x1008;
		int VIDEO_INT;
		mov al, bh;
	}

	return _AL;
}

void Graphics::SetGrayScalePalette(byte s, byte n) {
	if (!bIsVGA) return;

	if (s > n) SWAP(byte, s, n);

	_CX = 1 + n - s;

	_asm {
		xor bh, bh;
		mov bl, s;
		mov ax, 0x101B;
		int VIDEO_INT;
	}
}

void Graphics::SetGrayScalePalette() {
	SetGrayScalePalette(0, UCHAR_MAX);
}

/* Waits for vertical retrace to start */
void Graphics::WaitForRetrace() {
	if (!bIsVGA) return;

	_asm mov dx, 0x03DA;
l1:
	_asm {
		in al, dx;
		test al, 0x08;
		jnz l1;
	}
l2:
	_asm {
		in al, dx;
		test al, 0x08;
		jz l2;
	}
}

// Sets a block of palette
void Graphics::SetPalette(Palette &p, byte s, byte n) {
	byte R, G, B;
	int i;

	if (!bIsVGA) return;

	if (s > n) SWAP(byte, s, n);

	// Select the starting color
	_asm {
		mov dx, 0x03C8; 	// load DX with 3C8 (write palette function)
		mov al, s;	    	// move color to AL
		out dx, al;     	// write DX to the VGA (tell VGA that we want to
							//   work with the color in AL
	}

	for (i = s; i <= n; i++) {
		// Shift out by 2 places to confirm to VGA's 0 - 63 range
		R = p.rgbPalette[i].r >> 2;
		G = p.rgbPalette[i].g >> 2;
		B = p.rgbPalette[i].b >> 2;

		// Set this entry (the vga increments it's counter)
		_asm {
			mov dx, 0x03C9;	// load DX with 3C9 (write RGB colors)
			mov al, R;      // move Red   to AL
			out dx, al;     // write DX to VGA
			mov al, G;      // move Green to AL
			out dx, al;     // write DX to VGA
			mov al, B;      // move Blue  to AL
			out dx, al;		// write DX to VGA
		}
	}
}

void Graphics::SetPalette(Palette &p) {
	SetPalette(p, 0, UCHAR_MAX);
}

// Gets a block of palette
void Graphics::GetPalette(Palette &p, byte s, byte n) {
	byte R, G, B;
	int i;

	if (!bIsVGA) return;

	if (s > n) SWAP(byte, s, n);

	// Select the starting color
	_asm {
		mov dx, 0x03C7; // load DX with 3C7 (read palette function)
		mov al, s;    	// move color to AL
		out dx, al;     // write DX to the VGA (tell VGA that we want to
						//   work with the color in AL
	}

	for (i = s; i <= n; i++) {
		// Get this entry (the vga increments it's counter)
		_asm {
			mov dx, 0x03C9;	// load DX with 3C9 (read RGB colors)
			in al, dx;      // read Red   to AL
			mov R, al;		// copy AL to R
			in al, dx;      // read Green to AL
			mov G, al;     	// copy AL to G
			in al, dx;      // read Blue  to AL
			mov B, al;		// copy AL to B
		}

		// Shift out by 2 places
		p.rgbPalette[i].r = R << 2;
		p.rgbPalette[i].g = G << 2;
		p.rgbPalette[i].b = B << 2;
	}
}

void Graphics::GetPalette(Palette &p) {
	GetPalette(p, 0, UCHAR_MAX);
}

// Set the system to draw directly to the video buffer
void Graphics::DirectVideo(bool bs) {
	cActiveBuffer = (bs ? cVideoBuffer : cFrameBuffer);
	directvideo = bs;
}

// Clears the screen with a specified color
void Graphics::Clear(byte c) {
	if (!bIsVGA) return;

	memset(cActiveBuffer, c, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void Graphics::Clear() {
	Clear(BLACK);
}

// Flips the vitual framebuffer to the real framebuffer
void Graphics::Flip() {
	if (!bIsVGA) return;

	memcpy(cVideoBuffer, cFrameBuffer, SCREEN_WIDTH * SCREEN_HEIGHT);
}

/* Plots a pixel (with clipping) */
void Graphics::SetPixel(int x, int y, byte c) {
	if (!bIsVGA || CLIP(x, y)) return;

	SET_PIXEL(cActiveBuffer, x, y, c);
}

/* Get a pixel from a buffer (clipped) */
byte Graphics::GetPixel(int x, int y) {
	if (!bIsVGA || CLIP(x, y)) return 0;

	return GET_PIXEL(cActiveBuffer, x, y);
}

/* Draws a horizontal line (clipped) */
void Graphics::DrawHorizontalLine(int lx, int ty, int rx, byte c) {
	int x;

	if (!bIsVGA) return;

	if (lx > rx) SWAP(int, lx, rx);

	// Smart clip
	if (lx < 0) lx = 0;
	if (rx >= SCREEN_WIDTH) rx = SCREEN_WIDTH - 1;

	for (x = lx; x <= rx; x++) {
		SET_PIXEL(cActiveBuffer, x, ty, c);
	}
}

/* Draws a vertical line (clipped) */
void Graphics::DrawVerticalLine(int lx, int ty, int by, byte c) {
	int y;

	if (!bIsVGA) return;

	if (ty > by) SWAP(int, ty, by);

	// Smart clip
	if (ty < 0) ty = 0;
	if (by >= SCREEN_HEIGHT) by = SCREEN_HEIGHT - 1;

	for (y = ty; y <= by; y++) {
		SET_PIXEL(cActiveBuffer, lx, y, c);
	}
}

/* Draws a filled box (clipped) */
void Graphics::DrawFilledBox(int lx, int ty, int rx, int by, byte c) {
	int y, x;

	if (!bIsVGA) return;

	if (lx > rx) SWAP(int, lx, rx);
	if (ty > by) SWAP(int, ty, by);

	// Smart clip
	if (lx < 0) lx = 0;
	if (rx >= SCREEN_WIDTH) rx = SCREEN_WIDTH - 1;
	if (ty < 0) ty = 0;
	if (by >= SCREEN_HEIGHT) by = SCREEN_HEIGHT - 1;

	for (y = ty; y <= by; y++) {
		for (x = lx; x <= rx; x++) {
			SET_PIXEL(cActiveBuffer, x, y, c);
		}
	}
}

/* Draws a box (clipped) */
void Graphics::DrawBox(int lx, int ty, int rx, int by, byte c) {
	int i;

	if (!bIsVGA) return;

	for (i = MIN(lx, rx); i <= MAX(lx, rx); i++) {
		if (!CLIP(i, ty)) SET_PIXEL(cActiveBuffer, i, ty, c);
		if (!CLIP(i, by)) SET_PIXEL(cActiveBuffer, i, by, c);
	}

	for (i = MIN(ty, by); i <= MAX(ty, by); i++) {
		if (!CLIP(lx, i)) SET_PIXEL(cActiveBuffer, lx, i, c);
		if (!CLIP(rx, i)) SET_PIXEL(cActiveBuffer, rx, i, c);
	}
}

/* Draws a line using an adapted Bresenham's algoritm (clipped) */
void Graphics::DrawLine(int x1, int y1, int x2, int y2, byte color) {
	int xdelta;							/* The change in x coordinates */
	int ydelta;  						/* The change in y coordinates */
	int xstep;   						/* The change to make in the x coordinate in each step */
	int ystep;   						/* The change to make in the y coordinate in each step */
	int change;  						/* The amount that the x or y coordinate has changed */

	if (!bIsVGA) return;

	xdelta = x2 - x1;					/* Calculate the change in x coordinates */
	ydelta = y2 - y1;           		/* Calculate the change in y coordinates */
	if (xdelta < 0) {	        		/* The line will be drawn from right to left */
		xdelta = -xdelta;
		xstep = -1;
	}
	else {                      		/* The line will be drawn from left to right */
		xstep = 1;
	}
	if (ydelta < 0) {           		/* The line will be drawn from bottom to top */
		ydelta = -ydelta;
		ystep = -1;
	}
	else {                      		/* The line will be drawn from top to bottom */
		ystep = 1;
	}

	if (xdelta > ydelta) {				/* x changes quicker than y */
		change = xdelta >> 1;  			/* change set to twice the value of xdelta */
		while (x1 != x2) {      		/* Draw until the terminating dot is reached */
			/* Draw a dot on the screen */
			if (!CLIP(x1, y1)) SET_PIXEL(cActiveBuffer, x1, y1, color);
			x1 += xstep;                /* Update x coordinate */
			change += ydelta;           /* Update change */
			if (change > xdelta) {
				y1 += ystep;       		/* Update the y coordinate */
				change -= xdelta;  		/* Reset change */
			}
		}
	}
	else {                             	/* y changes quicker than x */
		change = ydelta >> 1;			/* change set to twice the value of ydelta */
		while (y1 != y2) {          	/* Draw until the terminating dot is reached */
			/* Draw a dot on the screen */
			if (!CLIP(x1, y1)) SET_PIXEL(cActiveBuffer, x1, y1, color);
			y1 += ystep;                /* Update y coordinate */
			change += xdelta;           /* Update change */
			if (change > ydelta) {
				/* If change is large enough to update the x coordinate */
				x1 += xstep;            /* Update the x coordinate */
				change -= ydelta;       /* Reset change */
			}
		}
	}
}

/* Draws a circle using midpoint circle algorithm (clipped) */
void Graphics::DrawCircle(int x, int y, int radius, byte c) {
	int p = 1 - radius, cx = 0, cy = radius, px, py;

	if (!bIsVGA) return;

	do {
		px = x + cx;
		py = y + cy;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		px = x - cx;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		px = x + cx;
		py = y - cy;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		px = x - cx;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		px = x + cy;
		py = y + cx;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		py = y - cx;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		px = x - cy;
		py = y + cx;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);
		py = y - cx;
		if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, c);

		cx++;
		if (p < 0) {
			p += ((cx << 1) + 1);
		}
		else {
			cy--;
			p += (((cx - cy) << 1) + 1);
		}
	} while (cx <= cy);
}

// Draws a filled circle (clipped)
void Graphics::DrawFilledCircle(int x, int y, int radius, byte c) {
	int cx = 0;
	int cy = radius;
	int df = 1 - radius;
	int d_e = 3;
	int d_se = -2 * radius + 5;
	int hx;

	if (!bIsVGA) return;

	do {
		for (hx = (x - cy); hx <= (x + cy); hx++) {
			if (!CLIP(hx, y - cx)) SET_PIXEL(cActiveBuffer, hx, y - cx, c);
			if (!CLIP(hx, y + cx)) SET_PIXEL(cActiveBuffer, hx, y + cx, c);
		}

		if (df < 0)  {
			df += d_e;
			d_e += 2;
			d_se += 2;
		}
		else {
			for (hx = (x - cx); hx <= (x + cx); hx++) {
				if (!CLIP(hx, y - cy)) SET_PIXEL(cActiveBuffer, hx, y - cy, c);
				if (!CLIP(hx, y + cy)) SET_PIXEL(cActiveBuffer, hx, y + cy, c);
			}

			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}

		cx++;
	} while (cx <= cy);
}

/* Draw an ellipse into a buffer in color c */
void Graphics::DrawEllipse(int x, int y, int rx, int ry, byte c) {
	long ix, iy;
	long h, i, j, k;
	long oh, oi, oj, ok;

	if (!bIsVGA) return;

	if (rx < 1) rx = 1;
	if (ry < 1) ry = 1;

	h = i = j = k = 0xFFFF;

	if (rx > ry) {
		ix = 0;
		iy = rx << 6;

		do {
			oh = h;
			oi = i;
			oj = j;
			ok = k;

			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * ry) / rx;
			k = (i * ry) / rx;

			if (((h != oh) || (k != ok)) && (h < oi)) {
				if (!CLIP(x + h, y + k)) SET_PIXEL(cActiveBuffer, x + h, y + k, c);
				if (!CLIP(x - h, y + k)) SET_PIXEL(cActiveBuffer, x - h, y + k, c);
				if (!CLIP(x + h, y - k)) SET_PIXEL(cActiveBuffer, x + h, y - k, c);
				if (!CLIP(x - h, y - k)) SET_PIXEL(cActiveBuffer, x - h, y - k, c);
			}

			if (((i != oi) || (j != oj)) && (h < i)) {
				if (!CLIP(x + i, y + j)) SET_PIXEL(cActiveBuffer, x + i, y + j, c);
				if (!CLIP(x - i, y + j)) SET_PIXEL(cActiveBuffer, x - i, y + j, c);
				if (!CLIP(x + i, y - j)) SET_PIXEL(cActiveBuffer, x + i, y - j, c);
				if (!CLIP(x - i, y - j)) SET_PIXEL(cActiveBuffer, x - i, y - j, c);
			}

			ix = ix + iy / rx;
			iy = iy - ix / rx;
		} while (i > h);
	}
	else {
		ix = 0;
		iy = ry << 6;

		do {
			oh = h;
			oi = i;
			oj = j;
			ok = k;

			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * rx) / ry;
			k = (i * rx) / ry;

			if (((j != oj) || (i != oi)) && (h < i)) {
				if (!CLIP(x + j, y + i)) SET_PIXEL(cActiveBuffer, x + j, y + i, c);
				if (!CLIP(x - j, y + i)) SET_PIXEL(cActiveBuffer, x - j, y + i, c);
				if (!CLIP(x + j, y - i)) SET_PIXEL(cActiveBuffer, x + j, y - i, c);
				if (!CLIP(x - j, y - i)) SET_PIXEL(cActiveBuffer, x - j, y - i, c);
			}

			if (((k != ok) || (h != oh)) && (h < oi)) {
				if (!CLIP(x + k, y + h)) SET_PIXEL(cActiveBuffer, x + k, y + h, c);
				if (!CLIP(x - k, y + h)) SET_PIXEL(cActiveBuffer, x - k, y + h, c);
				if (!CLIP(x + k, y - h)) SET_PIXEL(cActiveBuffer, x + k, y - h, c);
				if (!CLIP(x - k, y - h)) SET_PIXEL(cActiveBuffer, x - k, y - h, c);
			}

			ix = ix + iy / ry;
			iy = iy - ix / ry;
		} while (i > h);
	}
}

/* Draw a filled ellipse into a buffer in color c */
void Graphics::DrawFilledEllipse(int x, int y, int rx, int ry, byte color) {
	long ix, iy;
	long a, b, c, d;
	long da, db, dc, dd;
	long na, nb, nc, nd;
	long hx;

	if (!bIsVGA) return;

	if (rx < 1) rx = 1;
	if (ry < 1) ry = 1;

	if (rx > ry) {
		dc = -1;
		dd = 0xFFFF;
		ix = 0;
		iy = rx << 6;
		na = 0;
		nb = (iy + 32) >> 6;
		nc = 0;
		nd = (nb * ry) / rx;

		do {
			a = na;
			b = nb;
			c = nc;
			d = nd;

			ix = ix + (iy / rx);
			iy = iy - (ix / rx);
			na = (ix + 32) >> 6;
			nb = (iy + 32) >> 6;
			nc = (na * ry) / rx;
			nd = (nb * ry) / rx;

			if ((c > dc) && (c < dd)) {
				for (hx = (x - b); hx <= (x + b); hx++) {
					if (!CLIP(hx, y - c)) SET_PIXEL(cActiveBuffer, hx, y - c, color);
					if (!CLIP(hx, y + c)) SET_PIXEL(cActiveBuffer, hx, y + c, color);
				}
				dc = c;
			}

			if ((d < dd) && (d > dc)) {
				for (hx = (x - a); hx <= (x + a); hx++) {
					if (!CLIP(hx, y - d)) SET_PIXEL(cActiveBuffer, hx, y - d, color);
					if (!CLIP(hx, y + d)) SET_PIXEL(cActiveBuffer, hx, y + d, color);
				}
				dd = d;
			}
		} while (b > a);
	}
	else {
		da = -1;
		db = 0xFFFF;
		ix = 0;
		iy = ry << 6;
		na = 0;
		nb = (iy + 32) >> 6;
		nc = 0;
		nd = (nb * rx) / ry;

		do {
			a = na;
			b = nb;
			c = nc;
			d = nd;

			ix = ix + (iy / ry);
			iy = iy - (ix / ry);
			na = (ix + 32) >> 6;
			nb = (iy + 32) >> 6;
			nc = (na * rx) / ry;
			nd = (nb * rx) / ry;

			if ((a > da) && (a < db)) {
				for (hx = (x - d); hx <= (x + d); hx++) {
					if (!CLIP(hx, y - a)) SET_PIXEL(cActiveBuffer, hx, y - a, color);
					if (!CLIP(hx, y + a)) SET_PIXEL(cActiveBuffer, hx, y + a, color);
				}
				da = a;
			}

			if ((b < db) && (b > da)) {
				for (hx = (x - c); hx <= (x + c); hx++) {
					if (!CLIP(hx, y - b)) SET_PIXEL(cActiveBuffer, hx, y - b, color);
					if (!CLIP(hx, y + b)) SET_PIXEL(cActiveBuffer, hx, y + b, color);
				}
				db = b;
			}
		} while (b > a);
	}
}

// Flood fills a specified area using a non-recursive stack based algorithm (clipped)
void Graphics::FloodFill(int x, int y, byte fc, byte bc) {
	byte c;
	// Simple singly link list based stack
	struct iStackNode {
		int x, y;			// node data
		iStackNode *p;		// previous node
	} *iStack = NULL;

	// Pushes a coordinate to the stack
	#define STACK_PUSH(_x_, _y_) {\
		iStackNode *tNode = (iStackNode *)farmalloc(sizeof(iStackNode));\
		if (tNode != NULL) {\
			tNode->x = (_x_);\
			tNode->y = (_y_);\
			tNode->p = iStack;\
			iStack = tNode;\
		}\
	}

	// Pops a coordinate off the stack
	#define STACK_POP(_x_, _y_) {\
		if (iStack != NULL) {\
			iStackNode *tNode = iStack;\
			_x_ = tNode->x;\
			_y_ = tNode->y;\
			iStack = tNode->p;\
			farfree(tNode);\
		}\
	}

	// Is stack empty?
	#define STACK_EMPTY() (iStack == NULL)

	if (!bIsVGA || CLIP(x, y)) return;

	// Push the initial point
	STACK_PUSH(x, y);

	while (!STACK_EMPTY()) {
		STACK_POP(x, y);

		// Continue popping off if point is clipped
		if CLIP(x, y) continue;

		c = GET_PIXEL(cActiveBuffer, x, y);

		if (c != bc && c != fc) {
			SET_PIXEL(cActiveBuffer, x, y, fc);
			STACK_PUSH(x + 1, y);
			STACK_PUSH(x, y + 1);
			STACK_PUSH(x, y - 1);
			STACK_PUSH(x - 1, y);
		}
	}
}

// Selects the text font (overloaded for custom fonts)
void Graphics::SetFont(byte f) {
	switch (f) {
		case FONT_8x8:
			cFont = cFont8x8;
			cFontHeight = 8;
			break;
		default:	// FONT_8x16
			cFont = cFont8x16;
			cFontHeight = 16;
	}
}

void Graphics::SetFont(const Font &f) {
	if (f.cFont == NULL) return;

	cFont = f.cFont;
	cFontHeight = f.cHeight;
}

// Sets the font direction
void Graphics::SetFontDirection(byte fd) {
	cFontDirection = fd;
}

// Set font transparency flag
void Graphics::SetFontTransparency(bool t) {
	bFontOpaque = bool(!t);
}

// Sets the background color for opaque fonts
void Graphics::SetFontBackgroundColor(byte c) {
	cFontBackground = c;
}

byte Graphics::GetFontWidth() {
	return FONT_WIDTH;
}

byte Graphics::GetFontHeight() {
	return cFontHeight;
}

// All new custom pixel perfect text renderer (yahoo!!!)
void Graphics::Print(int x, int y, byte tc, const char *sFormat, ...) {
	char sTemp[PRINT_BUFFERSIZE];
	byte *chPtr, cScanLine, *ch;
	int iCurLine, i, px, py;
	va_list vaPtr;

	if (!bIsVGA) return;

#ifdef PARANOID
	if (strlen(sFormat) >= PRINT_BUFFERSIZE) {
		Print(x, y, tc, "String too long!");
		return;
	}
#endif

	// Render the string
	va_start(vaPtr, sFormat);
	vsprintf(sTemp, sFormat, vaPtr);
	va_end(vaPtr);

	// Set to the first character to render
	ch = (byte *)sTemp;
	while (*ch != EOS) {
		// Select the start address for the character's scan lines
		chPtr = cFont + ((word)cFontHeight * (*ch++));

		// Select the direction to render
		switch (cFontDirection) {
			case FONT_DOWN:
				// Render the character till it's full height
				for (iCurLine = 0; iCurLine < (word)cFontHeight; iCurLine++) {
					// Select each scan line
					cScanLine = *chPtr++;
					// Break up the scan line and plot
					for (i = 0; i < FONT_WIDTH; i++) {
						px = x - iCurLine;
						py = y + i;

						if (cScanLine & 0x80) {
							if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, tc);
						}
						else if (bFontOpaque) {
							if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, cFontBackground);
						}

						cScanLine <<= 1;
					}
				}
				y += FONT_WIDTH;
				break;
			case FONT_UP:
				// Render the character till it's full height
				for (iCurLine = 0; iCurLine < (word)cFontHeight; iCurLine++) {
					// Select each scan line
					cScanLine = *chPtr++;
					// Break up the scan line and plot
					for (i = 0; i < FONT_WIDTH; i++) {
						px = x + iCurLine;
						py = y - i;

						if (cScanLine & 0x80) {
							if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, tc);
						}
						else if (bFontOpaque) {
							if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, cFontBackground);
						}

						cScanLine <<= 1;
					}
				}
				y -= FONT_WIDTH;
				break;
			default:	// FONT_HORIZONTAL
				// Render the character till it's full height
				for (iCurLine = 0; iCurLine < (word)cFontHeight; iCurLine++) {
					// Select each scan line
					cScanLine = *chPtr++;
					// Break up the scan line and plot
					for (i = 0; i < FONT_WIDTH; i++) {
						px = x + i;
						py = y + iCurLine;

						if (cScanLine & 0x80) {
							if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, tc);
						}
						else if (bFontOpaque) {
							if (!CLIP(px, py)) SET_PIXEL(cActiveBuffer, px, py, cFontBackground);
						}

						cScanLine <<= 1;
					}
				}
				x += FONT_WIDTH;
		}
	}
}

// Blits a bitmap to the framebuffer (with effects)
void Graphics::PutBitmap(const Bitmap &b, int x, int y, byte flags) {
	int fx, fy;			// framebuffer x and y
	int fmx, fmy;		// framebuffer MAX x & y
	int bx, by;			// bitmap x and y
	int bl, bt;			// bitmap left, top
	int bmx, bmy;		// bitmap MAX x & y
	int xc, yc;			// x and y +ve/-ve increments
	word bw;			// bitmap width
	byte *cB, c;		// bitmap pointers

	if (!bIsVGA || b.cBitmap == NULL) return;

	// Initialize bitmap pointers
	cB = b.cBitmap;

	// Calculate maximum xs and ys
	bw = b.wWidth;
	bmx = bw - 1;
	bmy = b.wHeight - 1;
	fmx = x + bmx;
	fmy = y + bmy;

	// Setup increments/decrements and region
	if (flags & BITMAP_HFLIP) {
		bl = bmx;
		xc = -1;
	}
	else {
		bl = 0;
		xc = 1;
	}

	if (flags & BITMAP_VFLIP) {
		bt = bmy;
		yc = -1;
	}
	else {
		bt = 0;
		yc = 1;
	}

	// Do opaque and transparent blits seperately
	if (flags & BITMAP_OPAQUE) {
		by = bt;
		for (fy = y; fy <= fmy; fy++) {
			bx = bl;
			for (fx = x; fx <= fmx; fx++) {
				if (!CLIP(fx, fy)) {
					SET_PIXEL(cActiveBuffer, fx, fy, *(cB + bw * by + bx));
				}
				bx += xc;
			}
			by += yc;
		}
	}
	else {
		by = bt;
		for (fy = y; fy <= fmy; fy++) {
			bx = bl;
			for (fx = x; fx <= fmx; fx++) {
				c = *(cB + bw * by + bx);
				if (!CLIP(fx, fy) && c != 0) {
					SET_PIXEL(cActiveBuffer, fx, fy, c);
				}
				bx += xc;
			}
			by += yc;
		}
	}
}

// Get a rectangular region from the framebuffer
void Graphics::GetBitmap(const Bitmap &b, int x, int y) {
	word px, py;
	int vx;
	byte *c;

	if (!bIsVGA || b.cBitmap == NULL) return;

	c = b.cBitmap;
	for (py = 0; py < b.wHeight; py++) {
		vx = x;
		for (px = 0; px < b.wWidth; px++) {
			*c = (CLIP(vx, y) ? 0 : GET_PIXEL(cActiveBuffer, vx, y));
			c++;
			vx++;
		}
		y++;
	}
}

void Graphics::PutSprite(const Sprite &s, byte flags) {
	if (s.cBitmaps < 1) return;

	PutBitmap(*s.arrBitmap[s.cCurBmp], s.x, s.y, flags);
}

/***************************** Palette code *******************************/

// Palette ctor
Palette::Palette() {
	Clear();
}

// Palette comparison operators
bool Palette::operator ==(const Palette &p) {
	return bool(memcmp(rgbPalette, p.rgbPalette, sizeof(rgbPalette)) == EQUAL);
}

bool Palette::operator !=(const Palette &p) {
	return bool(memcmp(rgbPalette, p.rgbPalette, sizeof(rgbPalette)) != EQUAL);
}

// Clears the palette to a specific color
void Palette::Clear(byte s, byte n, byte R, byte G, byte B) {
	if (s > n) SWAP(byte, s, n);

	for (int i = s; i <= n; i++) {
		rgbPalette[i].r = R;
		rgbPalette[i].g = G;
		rgbPalette[i].b = B;
	}
}

void Palette::Clear(byte s, byte n) {
	Clear(s, n, 0, 0, 0);
}

void Palette::Clear() {
	Clear(0, UCHAR_MAX);
}

// Sets a palette element
void Palette::Set(byte n, byte R, byte G, byte B) {
	rgbPalette[n].r = R;
	rgbPalette[n].g = G;
	rgbPalette[n].b = B;
}

// Gets a palette element
void Palette::Get(byte n, byte &R, byte &G, byte &B) {
	R = rgbPalette[n].r;
	G = rgbPalette[n].g;
	B = rgbPalette[n].b;
}

// Increments the palette
bool Palette::Increment(byte s, byte n, byte c) {
	bool bDone = true;

	if (s > n) SWAP(byte, s, n);

	for (int i = s; i <= n; i++) {
		if (rgbPalette[i].r < UCHAR_MAX) {
			bDone = false;
			rgbPalette[i].r = MIN(rgbPalette[i].r + c, UCHAR_MAX);
		}

		if (rgbPalette[i].g < UCHAR_MAX) {
			bDone = false;
			rgbPalette[i].g = MIN(rgbPalette[i].g + c, UCHAR_MAX);
		}

		if (rgbPalette[i].b < UCHAR_MAX) {
			bDone = false;
			rgbPalette[i].b = MIN(rgbPalette[i].b + c, UCHAR_MAX);
		}
	}

	return bDone;
}

bool Palette::Increment(byte c) {
	return Increment(0, UCHAR_MAX, c);
}

bool Palette::Increment() {
	return Increment(0, UCHAR_MAX, 1);
}

// Decrements the palette
bool Palette::Decrement(byte s, byte n, byte c) {
	bool bDone = true;

	if (s > n) SWAP(byte, s, n);

	for (int i = s; i <= n; i++) {
		if (rgbPalette[i].r > 0) {
			bDone = false;
			rgbPalette[i].r = MAX(rgbPalette[i].r - c, 0);
		}

		if (rgbPalette[i].g > 0) {
			bDone = false;
			rgbPalette[i].g = MAX(rgbPalette[i].g - c, 0);
		}

		if (rgbPalette[i].b > 0) {
			bDone = false;
			rgbPalette[i].b = MAX(rgbPalette[i].b - c, 0);
		}
	}

	return bDone;
}

bool Palette::Decrement(byte c) {
	return Decrement(0, UCHAR_MAX, c);
}

bool Palette::Decrement() {
	return Decrement(0, UCHAR_MAX, 1);
}

// Increments or decrements palette until this = p
bool Palette::Blend(const Palette &p, byte s, byte n) {
	bool bDone = true;

	if (s > n) SWAP(byte, s, n);

	for (int i = s; i <= n; i++) {
		if (rgbPalette[i].r < p.rgbPalette[i].r) {
			bDone = false;
			rgbPalette[i].r++;
		}
		else if (rgbPalette[i].r > p.rgbPalette[i].r) {
			bDone = false;
			rgbPalette[i].r--;
		}

		if (rgbPalette[i].g < p.rgbPalette[i].g) {
			bDone = false;
			rgbPalette[i].g++;
		}
		else if (rgbPalette[i].g > p.rgbPalette[i].g) {
			bDone = false;
			rgbPalette[i].g--;
		}

		if (rgbPalette[i].b < p.rgbPalette[i].b) {
			bDone = false;
			rgbPalette[i].b++;
		}
		else if (rgbPalette[i].b > p.rgbPalette[i].b) {
			bDone = false;
			rgbPalette[i].b--;
		}
	}

	return bDone;
}

bool Palette::Blend(const Palette &p) {
	return Blend(p, 0, UCHAR_MAX);
}

// Rotates the palette to the left i.e 0... <- ...255
void Palette::RotateLeft(byte s, byte n) {
	RGB tmp;

	if (s > n) SWAP(byte, s, n);

	// Save the first item in temp
	tmp = rgbPalette[s];

	// Move all to the left
	for (int i = s; i < n; i++) {
		rgbPalette[i] = rgbPalette[i + 1];
	}

	// Set last to first
	if (s < n) {
		rgbPalette[n] = tmp;
	}
}

void Palette::RotateLeft() {
	RotateLeft(0, UCHAR_MAX);
}

// Rotates the palette to the right i.e 0... -> ...255
void Palette::RotateRight(byte s, byte n) {
	RGB tmp;

	if (s > n) SWAP(byte, s, n);

	// Save the last item in temp
	tmp = rgbPalette[n];

	// Move all to the right
	for (int i = n; i > s; i--) {
		rgbPalette[i] = rgbPalette[i - 1];
	}

	// Set first to last
	if (s < n) {
		rgbPalette[s] = tmp;
	}
}

void Palette::RotateRight() {
	RotateRight(0, UCHAR_MAX);
}

// Inverts the palette (bitwise)... can be used for flashes
void Palette::Invert(byte s, byte n) {
	if (s > n) SWAP(byte, s, n);

	for (int i = s; i <= n; i++) {
		rgbPalette[i].r = ~rgbPalette[i].r;
		rgbPalette[i].g = ~rgbPalette[i].g;
		rgbPalette[i].b = ~rgbPalette[i].b;
	}
}

void Palette::Invert() {
	Invert(0, UCHAR_MAX);
}

// Generates a gradient
void Palette::GenerateGradient(byte s, byte n, byte rs, byte gs, byte bs, byte re, byte ge, byte be) {
	float rstep, gstep, bstep;
	float r, g, b;
	float h;

	if (s > n) SWAP(byte, s, n);

	// Calculate gradient height
	h = 1 + n - s;

	// Set initial rgb values
	r = rs;
	g = gs;
	b = bs;

	// Calculate RGB stepping
	rstep = ((float)re - (float)rs) / h;
	gstep = ((float)ge - (float)gs) / h;
	bstep = ((float)be - (float)bs) / h;

	// Generate palette
	for (int i = s; i <= n; i++) {
		rgbPalette[i].r = (byte)r;
		rgbPalette[i].g = (byte)g;
		rgbPalette[i].b = (byte)b;

		r += rstep;
		g += gstep;
		b += bstep;
	}
}

void Palette::GenerateGradient(byte rs, byte gs, byte bs, byte re, byte ge, byte be) {
	GenerateGradient(0, UCHAR_MAX, rs, gs, bs, re, ge, be);
}

// Copies the contents of palette p
void Palette::Copy(const Palette &p, byte s, byte n) {
	if (s > n) SWAP(byte, s, n);

	for (int i = s; i <= n; i++) {
		rgbPalette[i] = p.rgbPalette[i];
	}
}

void Palette::Copy(const Palette &p) {
	Copy(p, 0, UCHAR_MAX);
}

/******************************* Font code ********************************/

Font::Font() {
	cFont = NULL;
	cHeight = 0;
}

Font::Font(const Font &f) {
	Font();
	Copy(f);
}

Font::Font(Archive &f) {
	Font();
	Load(f);
}

Font::~Font() {
	if (cFont != NULL) {
		delete[] cFont;
		cFont = NULL;
	}
}

bool Font::IsAvailable() {
	return bool(cFont != NULL);
}

byte Font::GetHeight() {
	return cHeight;
}

byte Font::GetWidth() {
	return FONT_WIDTH;
}

void Font::Copy(const Font &f) {
	if (f.cFont == NULL) return;

	if (cFont != NULL) delete[] cFont;

	cHeight = f.cHeight;

	cFont = new byte[cHeight * 256u];
	if (cFont == NULL) {
		cHeight = 0;
		return;
	}

	memcpy(cFont, f.cFont, cHeight * 256u);
}

// Loads a font from a file
bool Font::Load(Archive &f) {
	word h;

	f.FileSeek(0);

	// Check file signature
	if (ID(f.FileGetByte(), f.FileGetByte(), f.FileGetByte(), f.FileGetByte()) != Signature) {
		return false;
	}

	// Get the height
	h = f.FileGetByte();
	if (h < 1 || h > 32) {
		return false;
	}

	// Free the previous font
	if (cFont != NULL) delete[] cFont;

	// Update height
	cHeight = (byte)h;

	// Allocate the required memory
	cFont = new byte[cHeight * 256u];
	if (cFont == NULL) {
		cHeight = 0;
		return false;
	}

	// Read in the font data
	for (h = 0; h < (cHeight * 256u); h++) {
		cFont[h] = f.FileGetByte();
	}

	return true;
}

/******************************* Bitmap code *******************************/

Bitmap::Bitmap() {
	cBitmap = NULL;
	wWidth = wHeight = 0;
}

Bitmap::Bitmap(word w, word h) {
	Bitmap();
	Create(w, h);
}

Bitmap::Bitmap(const Bitmap &b) {
	Bitmap();
	Copy(b);
}

Bitmap::Bitmap(Archive &f, Palette &pToMatch) {
	Bitmap();
	Load(f, pToMatch);
}

Bitmap::~Bitmap() {
	Destroy();
}

bool Bitmap::IsAvailable() {
	return bool(cBitmap != NULL);
}

word Bitmap::GetWidth() {
	return wWidth;
}

word Bitmap::GetHeight() {
	return wHeight;
}

void Bitmap::Destroy() {
	if (cBitmap != NULL) {
		delete[] cBitmap;
		cBitmap = NULL;
		wWidth = wHeight = 0;
	}
}

// Creates an empty bitmap
bool Bitmap::Create(word w, word h) {
	Destroy();

	wWidth = w;
	wHeight = h;

	cBitmap = new byte[wWidth * wHeight];
	if (cBitmap == NULL) {
		wWidth = wHeight = 0;
		return false;
	}

	return true;
}

void Bitmap::Copy(const Bitmap &b) {
	if (b.cBitmap == NULL) return;

	if (Create(b.wWidth, b.wHeight)) {
		memcpy(cBitmap, b.cBitmap, wWidth * wHeight);
	}
}

// Calculates the RGB distance in the RGB color cube
double Bitmap::RGBDistance(double r1, double g1, double b1, double r2, double g2, double b2) {
	return sqrt((((r2 - r1) * (r2 - r1)) + ((g2 - g1) * (g2 - g1)) + ((b2 - b1) * (b2 - b1))));
}

//	Loads a 256 color PCX file, matches the pallete and updates the bitmap
bool Bitmap::Load(Archive &f, Palette &pToMatch) {
	Palette pal;
	byte palMap[SCREEN_COLORS];
	word c;
	word width, height;
	word bytes_per_line;
	int x, y;
	char ch;
	byte *b;
	double newDist, oldDist;

	f.FileSeek(0);

	// Sanity check
	if (f.FileGetByte() != 0x0A) return false;
	if (f.FileGetByte() != 5) return false;
	if (f.FileGetByte() != 1) return false;
	if (f.FileGetByte() != 8) return false;	// has to be 8-bpp

	width = -(f.FileGetWord());        /* xmin */
	height = -(f.FileGetWord());       /* ymin */
	width += f.FileGetWord() + 1;      /* xmax */
	height += f.FileGetWord() + 1;     /* ymax */

	f.FileGetWord();             /* skip DPI values */
	f.FileGetWord();             /* skip DPI values */

	/* Read the 16 color pallete */
	for (c = 0; c < 16; c++) {
		pal.rgbPalette[c].r = f.FileGetByte();
		pal.rgbPalette[c].g = f.FileGetByte();
		pal.rgbPalette[c].b = f.FileGetByte();
	}

	f.FileGetByte();		// reserved

	/* How many color planes? */
	if (f.FileGetByte() != 1) return false;

	bytes_per_line = f.FileGetWord();

	/* Skip some more junk */
	for (c = 0; c < 30; c++) {
		f.FileGetWord();
	}

	// Create an empty bitmap
	if (!Create(width, height)) {
		return false;
	}

	b = cBitmap;
	for (y = 0; y < height; y++) {       /* read RLE encoded PCX data */
		x = 0;
		while (x < bytes_per_line) {
			ch = f.FileGetByte();
			if ((ch & 0xC0) == 0xC0) {
				c = (ch & 0x3F);
				ch = f.FileGetByte();
			}
			else
				c = 1;

			while (c--) {
				*b++ = ch;
				x++;
			}
		}
	}

	if (f.FileGetByte() == 12) {
		for (c = 0; c < SCREEN_COLORS; c++) {
			pal.rgbPalette[c].r = f.FileGetByte();
			pal.rgbPalette[c].g = f.FileGetByte();
			pal.rgbPalette[c].b = f.FileGetByte();
		}
	}

	// Leave if both palettes are the same
	if (pal == pToMatch) return true;

	// Match the palette
	for (x = 1; x < SCREEN_COLORS; x++) {
		oldDist = RGBDistance(0, 0, 0, 255, 255, 255);
		for (y = 1; y < SCREEN_COLORS; y++) {
			newDist = RGBDistance(pal.rgbPalette[x].r, pal.rgbPalette[x].g, pal.rgbPalette[x].b, pToMatch.rgbPalette[y].r, pToMatch.rgbPalette[y].g, pToMatch.rgbPalette[y].b);

			if (oldDist >= newDist) {
				oldDist = newDist;
				palMap[x] = y;
			}
		}
	}

	// Zero is normally transparent
	palMap[0] = 0;

	// Update the bitmap to use the matched palette
	for (c = 0; c < (wWidth * wHeight); c++) {
		cBitmap[c] = palMap[cBitmap[c]];
	}

	return true;
}

// Just goes through the image data and loads the palette
bool Bitmap::LoadPalette(Archive &f, Palette &p) {
	word c;
	word width, height;
	word bytes_per_line;
	int x, y;
	char ch;

	f.FileSeek(0);

	// Sanity check
	if (f.FileGetByte() != 0x0A) return false;
	if (f.FileGetByte() != 5) return false;
	if (f.FileGetByte() != 1) return false;
	if (f.FileGetByte() != 8) return false;	// has to be 8-bpp

	width = -(f.FileGetWord());        /* xmin */
	height = -(f.FileGetWord());       /* ymin */
	width += f.FileGetWord() + 1;      /* xmax */
	height += f.FileGetWord() + 1;     /* ymax */

	f.FileGetWord();             /* skip DPI values */
	f.FileGetWord();             /* skip DPI values */

	/* Read the 16 color pallete */
	for (c = 0; c < 16; c++) {
		p.rgbPalette[c].r = f.FileGetByte();
		p.rgbPalette[c].g = f.FileGetByte();
		p.rgbPalette[c].b = f.FileGetByte();
	}

	f.FileGetByte();		// reserved

	/* How many color planes? */
	if (f.FileGetByte() != 1) return false;

	bytes_per_line = f.FileGetWord();

	/* Skip some more junk */
	for (c = 0; c < 30; c++) {
		f.FileGetWord();
	}

	for (y = 0; y < height; y++) {       /* read RLE encoded PCX data */
		x = 0;
		while (x < bytes_per_line) {
			ch = f.FileGetByte();
			if ((ch & 0xC0) == 0xC0) {
				c = (ch & 0x3F);
				ch = f.FileGetByte();
			}
			else
				c = 1;

			while (c--) {
				x++;
			}
		}
	}

	if (f.FileGetByte() == 12) {
		for (c = 0; c < SCREEN_COLORS; c++) {
			p.rgbPalette[c].r = f.FileGetByte();
			p.rgbPalette[c].g = f.FileGetByte();
			p.rgbPalette[c].b = f.FileGetByte();
		}
	}

	return true;
}

/******************************* Sprite code *******************************/

// Initialize sprite
Sprite::Sprite() {
	Clear();
}

// Returns true if everything is ok (i.e. sprite is valid)
bool Sprite::IsAvailable() {
	return bool(cBitmaps > 0);
}

// Clears the entire sprite structure
void Sprite::Clear() {
	x = y = cBitmaps = cCurBmp = 0;
	memset(arrBitmap, NULL, sizeof(arrBitmap));
}

// Adds a bitmap to the sprite bitmap chain
bool Sprite::AddBitmap(Bitmap &b) {
	if (cBitmaps >= SPRITE_BITMAPS_MAX) return false;

	arrBitmap[cBitmaps] = &b;
	cBitmaps++;
	cCurBmp = 0;

	return true;
}

void Sprite::MoveAbsolute(int X, int Y) {
	x = X;
	y = Y;
}

void Sprite::MoveRelative(int X, int Y) {
	x += X;
	y += Y;
}

void Sprite::SetFirstFrame() {
	if (cBitmaps < 1) return;

	cCurBmp = 0;
}

void Sprite::SetNextFrame() {
	if (cBitmaps < 1) return;

	cCurBmp++;
	if (cCurBmp >= cBitmaps) {
		cCurBmp = 0;
	}
}

// Detects if sprite s collides with 'this'
bool Sprite::TestCollison(const Sprite &s) {
	Rect r1, r2, r;
	Bitmap *bmpCurr1, *bmpCurr2;
	byte *cB1, *cB2;
	int x1, y1, x2, y2, x1s, x2s;
	word w, h, i, j, bw1, bw2;

	bmpCurr1 = arrBitmap[cCurBmp];
	bmpCurr2 = s.arrBitmap[s.cCurBmp];
	cB1 = bmpCurr1->cBitmap;
	cB2 = bmpCurr2->cBitmap;

	// Leave if any sprite is invalid
	if (bmpCurr1 == NULL || bmpCurr2 == NULL || cB1 == NULL || cB2 == NULL) {
		return false;
	}

	bw1 = bmpCurr1->wWidth;
	bw2 = bmpCurr2->wWidth;

	// Create 'this' bounding rectangle
	r1.a.x = x;
	r1.a.y = y;
	r1.b.x = r1.a.x + bw1 - 1;
	r1.b.y = r1.a.y + bmpCurr1->wHeight - 1;

	// Create 's' bounding rectangle
	r2.a.x = s.x;
	r2.a.y = s.y;
	r2.b.x = r2.a.x + bw2 - 1;
	r2.b.y = r2.a.y + bmpCurr2->wHeight - 1;

	// Leave if the rectangle do not collide
	if (r1.a.x > r2.b.x || r2.a.x > r1.b.x || r1.a.y > r2.b.y || r2.a.y > r1.b.y) {
		return false;
	}

	// Rectangles collide; do a detailed check
	// Find the intersecting rectangle
	r = r1;
	r.Intersect(r2);

	// Find the width and height of the intersecting rectangle
	w = 1 + r.b.x - r.a.x;
	h = 1 + r.b.y - r.a.y;

	// Find the starting x & y positions of intersection in the bitmaps
	y1 = r.a.y - y;
	y2 = r.a.y - s.y;
	x1s = r.a.x - x;
	x2s = r.a.x - s.x;

	// Check both bitmap for collission
	for (j = 0; j < h; j++) {
		x1 = x1s;
		x2 = x2s;
		for (i = 0; i < w; i++) {
			if (*(cB1 + bw1 * y1 + x1) != 0 && *(cB2 + bw2 * y2 + x2) != 0) {
				return true;
			}
			x1++;
			x2++;
		}
		y1++;
		y2++;
	}

	return false;
}
