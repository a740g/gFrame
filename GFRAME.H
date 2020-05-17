/****************************************************************************\
*																			 *
*	 Copyright (c) Samuel Gomes	(Blade), 1998-2004.							 *
*	 All rights	reserved.													 *
*																			 *
*	 mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	 Common	header file	(contains all declerations)							 *
*																			 *
\****************************************************************************/

#ifndef	__GFRAME_H_
#define	__GFRAME_H_

/* Check for compiler platform compliance */
#ifndef	__MSDOS__
#error This	library	was	designed only for the MS-DOS platform!
#endif

#if	!defined(__LARGE__)	&& !defined(__HUGE__)
#error Only	the	large and huge memory models are supported!
#endif

#if	!defined(__TCPLUSPLUS__) ||	!defined(__cplusplus)
#error Only	the	Borland	Turbo C++ compiler is supported!
#endif

// Some	include	files...
#include <alloc.h>
#include <assert.h>
#include <bcd.h>
#include <bios.h>
#include <complex.h>
#include <conio.h>
#include <constrea.h>
#include <ctype.h>
#include <dir.h>
#include <direct.h>
#include <dirent.h>
#include <dos.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <fstream.h>
#include <generic.h>
#include <graphics.h>
#include <io.h>
#include <iomanip.h>
#include <iostream.h>
#include <limits.h>
#include <locale.h>
#include <malloc.h>
#include <math.h>
#include <mem.h>
#include <memory.h>
#include <new.h>
#include <process.h>
#include <search.h>
#include <setjmp.h>
#include <share.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdiostr.h>
#include <stdlib.h>
#include <string.h>
#include <strstrea.h>
#include <time.h>
#include <utime.h>
#include <values.h>
#include <sys/locking.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>

// Byte packing to save memory
#pragma option -a-

// Some	macro definitions
#define	LESSER (-1)
#define	EQUAL 0
#define	GREATER	1
#define EOS '\0'
#define	LF '\n'
#define	CR '\r'
#define	CRLF "\r\n"
#define	ISEVEN(_x_)	(((_x_)	>> 1) +	((_x_) >> 1) ==	(_x_))
#define	ROUND(_a_) ((((_a_)	- (long)(_a_)) < 0.5) ?	floor(_a_) : ceil(_a_))
#define	SGN(_x_) (((_x_) ==	0) ? EQUAL : (((_x_) > 0) ?	GREATER	: LESSER))
#define	DIM(_x_) (sizeof(_x_) /	sizeof(_x_[0]))
#define	ID(_a_,	_b_, _c_, _d_) ((dword)(_a_) | ((dword)(_b_) <<	8) | ((dword)(_c_) << 16) |	((dword)(_d_) << 24))
#define	EMPTY_STRING(_s_) ((_s_) ==	NULL ||	(_s_)[0] ==	EOS)
#define	CLEAR_VAR(__v) memset(&(__v), EOS, sizeof(__v))
#define	SWAP(__DATA_TYPE__,	_a_, _b_) {__DATA_TYPE__ _z_ = _a_;	_a_	= _b_; _b_ = _z_;}
#define	MAKEBYTE(_x_, _y_) ((byte)((byte)(_y_) | ((byte)(_x_) << 4)))
#define	MAKEWORD(_x_, _y_) ((word)(((byte)(_y_)) | (((word)((byte)(_x_))) << 8)))
#define	MAKEDWORD(_x_, _y_)	((dword)((_y_) | ((dword)(_x_) << 16)))
#define	HINIBBLE(_x_) ((byte)((byte)(_x_) >> 4))
#define	LONIBBLE(_x_) ((byte)((byte)(_x_) &	0xFu))
#define	HIBYTE(_x_)	((byte)(((word)(_x_) >>	8) & 0xFF))
#define	LOBYTE(_x_)	((byte)(_x_))
#define	HIWORD(_x_)	((word)(((dword)(_x_) >> 16) & 0xFFFF))
#define	LOWORD(_x_)	((word)(_x_))
#define	NIBBLESWAP(_x_)	MAKEBYTE(LONIBBLE(_x_),	HINIBBLE(_x_))
#define	BYTESWAP(_x_) MAKEWORD(LOBYTE(_x_),	HIBYTE(_x_))
#define	WORDSWAP(_x_) MAKEDWORD(LOWORD(_x_), HIWORD(_x_))
#define MAX(_a_, _b_) (((_a_) > (_b_)) ? (_a_) : (_b_))
#define MIN(_a_, _b_) (((_a_) < (_b_)) ? (_a_) : (_b_))

/* Keyboard	key	codes */
#define	KB_NOKEY 0
#define	KB_ESC 27
#define	KB_BACKSPACE 8
#define	KB_ENTER 13
#define	KB_TAB 9
#define	KB_F1 315
#define	KB_F2 316
#define	KB_F3 317
#define	KB_F4 318
#define	KB_F5 319
#define	KB_F6 320
#define	KB_F7 321
#define	KB_F8 322
#define	KB_F9 323
#define	KB_F10 324
#define	KB_F11 389
#define	KB_F12 390
#define	KB_UP 328
#define	KB_DOWN	336
#define	KB_LEFT	331
#define	KB_RIGHT 333
#define	KB_INSERT 338
#define	KB_DELETE 339
#define	KB_HOME	327
#define	KB_END 335
#define	KB_PAGEUP 329
#define	KB_PAGEDOWN	337
#define	KB_SHIFTTAB	271
#define	KB_CTRLA 1
#define	KB_CTRLB 2
#define	KB_CTRLC 3
#define	KB_CTRLD 4
#define	KB_CTRLE 5
#define	KB_CTRLF 6
#define	KB_CTRLG 7
#define	KB_CTRLH 8
#define	KB_CTRLI 9
#define	KB_CTRLJ 10
#define	KB_CTRLK 11
#define	KB_CTRLL 12
#define	KB_CTRLM 13
#define	KB_CTRLN 14
#define	KB_CTRLO 15
#define	KB_CTRLP 16
#define	KB_CTRLQ 17
#define	KB_CTRLR 18
#define	KB_CTRLS 19
#define	KB_CTRLT 20
#define	KB_CTRLU 21
#define	KB_CTRLV 22
#define	KB_CTRLW 23
#define	KB_CTRLX 24
#define	KB_CTRLY 25
#define	KB_CTRLZ 26
#define	KB_CTRLENTER 10
#define	KB_SPACEBAR	32
#define	KB_CTRLBACKSPACE 127
#define	KB_CTRLF1 350
#define	KB_CTRLF2 351
#define	KB_CTRLF3 352
#define	KB_CTRLF4 353
#define	KB_CTRLF5 354
#define	KB_CTRLF6 355
#define	KB_CTRLF7 356
#define	KB_CTRLF8 357
#define	KB_CTRLF9 358
#define	KB_CTRLF10 359
#define	KB_CTRLF11 393
#define	KB_CTRLF12 394
#define	KB_SHIFTF1 340
#define	KB_SHIFTF2 341
#define	KB_SHIFTF3 342
#define	KB_SHIFTF4 343
#define	KB_SHIFTF5 344
#define	KB_SHIFTF6 345
#define	KB_SHIFTF7 346
#define	KB_SHIFTF8 347
#define	KB_SHIFTF9 348
#define	KB_SHIFTF10	349
#define	KB_SHIFTF11	391
#define	KB_SHIFTF12	392
#define	KB_ALTF1 360
#define	KB_ALTF2 361
#define	KB_ALTF3 362
#define	KB_ALTF4 363
#define	KB_ALTF5 364
#define	KB_ALTF6 365
#define	KB_ALTF7 366
#define	KB_ALTF8 367
#define	KB_ALTF9 368
#define	KB_ALTF10 369
#define	KB_ALTF11 395
#define	KB_ALTF12 396
#define	KB_ALTMINUS	386
#define	KB_ALTPLUS 387
#define	KB_ALT1	376
#define	KB_ALT2	377
#define	KB_ALT3	378
#define	KB_ALT4	379
#define	KB_ALT5	380
#define	KB_ALT6	381
#define	KB_ALT7	382
#define	KB_ALT8	383
#define	KB_ALT9	384
#define	KB_ALT0	385
#define	KB_ALTA	286
#define	KB_ALTB	304
#define	KB_ALTC	302
#define	KB_ALTD	288
#define	KB_ALTE	274
#define	KB_ALTF	289
#define	KB_ALTG	290
#define	KB_ALTH	291
#define	KB_ALTI	279
#define	KB_ALTJ	292
#define	KB_ALTK	293
#define	KB_ALTL	294
#define	KB_ALTM	306
#define	KB_ALTN	305
#define	KB_ALTO	280
#define	KB_ALTP	281
#define	KB_ALTQ	272
#define	KB_ALTR	275
#define	KB_ALTS	287
#define	KB_ALTT	276
#define	KB_ALTU	278
#define	KB_ALTV	303
#define	KB_ALTW	273
#define	KB_ALTX	301
#define	KB_ALTY	277
#define	KB_ALTZ	300
// Keyboard	shift flags
#define	KB_FLAGSHIFTRIGHT 0x1
#define	KB_FLAGSHIFTLEFT 0x2
#define	KB_FLAGSHIFT (0x1 |	0x2)
#define	KB_FLAGCTRL	0x4
#define	KB_FLAGALT 0x8
#define	KB_FLAGSCROLLLOCK (0x10	| 0x1000)
#define	KB_FLAGNUMLOCK (0x20 | 0x2000)
#define	KB_FLAGCAPSLOCK	(0x40 |	0x4000)
#define	KB_FLAGINSERT 0x80
#define	KB_FLAGCTRLLEFT	0x100
#define	KB_FLAGALTLEFT 0x200
#define	KB_FLAGCTRLRIGHT 0x400
#define	KB_FLAGALTRIGHT	0x800
#define	KB_FLAGSYSREQ 0x8000
// Keyboard	raw	scancodes
#define	KEY_A (0x1E)
#define	KEY_B (0x30)
#define	KEY_C (0x2E)
#define	KEY_D (0x20)
#define	KEY_E (0x12)
#define	KEY_F (0x21)
#define	KEY_G (0x22)
#define	KEY_H (0x23)
#define	KEY_I (0x17)
#define	KEY_J (0x24)
#define	KEY_K (0x25)
#define	KEY_L (0x26)
#define	KEY_M (0x32)
#define	KEY_N (0x31)
#define	KEY_O (0x18)
#define	KEY_P (0x19)
#define	KEY_Q (0x10)
#define	KEY_R (0x13)
#define	KEY_S (0x1F)
#define	KEY_T (0x14)
#define	KEY_U (0x16)
#define	KEY_V (0x2F)
#define	KEY_W (0x11)
#define	KEY_X (0x2D)
#define	KEY_Y (0x15)
#define	KEY_Z (0x2C)
#define	KEY_1 (0x02)
#define	KEY_2 (0x03)
#define	KEY_3 (0x04)
#define	KEY_4 (0x05)
#define	KEY_5 (0x06)
#define	KEY_6 (0x07)
#define	KEY_7 (0x08)
#define	KEY_8 (0x09)
#define	KEY_9 (0x0A)
#define	KEY_0 (0x0B)
#define	KEY_DASH (0x0C)
#define	KEY_EQUAL (0x0D)
#define	KEY_LBRACKET (0x1A)
#define	KEY_RBRACKET (0x1B)
#define	KEY_SEMICOLON (0x27)
#define	KEY_RQUOTE (0x28)
#define	KEY_LQUOTE (0x29)
#define	KEY_PERIOD (0x33)
#define	KEY_COMMA (0x34)
#define	KEY_SLASH (0x35)
#define	KEY_BACKSLASH (0x2B)
#define	KEY_F1 (0x3B)
#define	KEY_F2 (0x3C)
#define	KEY_F3 (0x3D)
#define	KEY_F4 (0x3E)
#define	KEY_F5 (0x3F)
#define	KEY_F6 (0x40)
#define	KEY_F7 (0x41)
#define	KEY_F8 (0x42)
#define	KEY_F9 (0x43)
#define	KEY_F10	(0x44)
#define	KEY_ESC	(0x01)
#define	KEY_BACKSPACE (0x0E)
#define	KEY_TAB	(0x0F)
#define	KEY_ENTER (0x1C)
#define	KEY_CONTROL	(0x1D)
#define	KEY_LSHIFT (0x2A)
#define	KEY_RSHIFT (0x36)
#define	KEY_PRTSC (0x37)
#define	KEY_ALT	(0x38)
#define	KEY_SPACE (0x39)
#define	KEY_CAPSLOCK (0x3A)
#define	KEY_NUMLOCK	(0x45)
#define	KEY_SCROLLLOCK (0x46)
#define	KEY_HOME (0x47)
#define	KEY_UP (0x48)
#define	KEY_PGUP (0x49)
#define	KEY_MINUS (0x4A)
#define	KEY_LEFT (0x4B)
#define	KEY_CENTER (0x4C)
#define	KEY_RIGHT (0x4D)
#define	KEY_PLUS (0x4E)
#define	KEY_END	(0x4F)
#define	KEY_DOWN (0x50)
#define	KEY_PGDOWN (0x51)
#define	KEY_INS	(0x52)
#define	KEY_DEL	(0x53)

// Default fonts
#define	FONT_8x8 8
#define	FONT_8x16 16
// Text	orientation
#define	FONT_HORIZONTAL	0
#define	FONT_UP	1
#define	FONT_DOWN 2

// Blitting	options
// transparent blits (default)
#define	BITMAP_TRANSPARENT 0x0
// opaque blits
#define	BITMAP_OPAQUE 0x1
// flip	horizontal
#define	BITMAP_HFLIP 0x2
// flip	vertical
#define	BITMAP_VFLIP 0x4

// OS constants
#define	OS_DOS 0
#define	OS_WINDOWS3X 1
#define	OS_WINDOWS9X 2
#define	OS_WINDOWSNT 3
#define	OS_OS2 4
#define	OS_LINUX 5

// Some	type definitions to	make life easier
// TC 3.x has no bool so we	simulate bool, true and false
#if	(__TCPLUSPLUS__	< 0x0400)
enum bool {false, true};
#endif
typedef	unsigned char byte;
typedef	signed char	sbyte;
typedef	unsigned short word;
typedef	signed short sword;
typedef	unsigned long dword;
typedef	signed long	sdword;

class Point {
public:
	int x, y;

	Point(int X, int Y);
	Point();

	Point& operator +=(const Point& adder);
	Point& operator -=(const Point& subber);
	Point operator -(const Point& p);
	Point operator +(const Point& p);
	bool operator ==(const Point& p) const;
	bool operator !=(const Point& p) const;
};

class Rect {
public:
	Point a, b;

	Rect(int ax, int ay, int bx, int by);
	Rect(Point p1, Point p2);
	Rect();

	void Move(int aDX, int aDY);
	void Grow(int aDX, int aDY);
	void Intersect(const Rect& r);
	void Union(const Rect& r);
	bool Contains(const Point& p) const;
	bool operator ==(const Rect& r) const;
	bool operator !=(const Rect& r) const;
	bool IsEmpty();
};

// System diagnostic class.. the .NET idea :)
class Diagnostic {
	static void	GetWindowsVersion(byte &cVerMajor, byte	&cVerMinor);
	static void	GetWinNTVersion(byte &cVerMajor, byte &cVerMinor);
	static void	GetOS2Version(byte &cVerMajor, byte	&cVerMinor);
	static void	GetLinuxVersion(byte &cVerMajor, byte &cVerMinor);

public:
	static word	GetTotalBaseMemory();
	static dword GetTotalExtendedMemory();
	static dword GetTotalMemory();
	static const char *GetOSName();
	static byte	GetOSVersion(byte &cVerMajor, byte &cVerMinor);
	static void	DebugPrint(const char *sFormat,	...);
};

// Processor class... very informative...
class Processor	{
public:
	Processor();
	~Processor();

	static word	GetCPUID();
	static const char *GetCPUName();
	static void	Yield();
	static bool	IsFPUAvailable();
	static void	ResetFPU();
	static word	ClearFPUStatus();
	static word	GetFPUStatus();
	static word	SetFPUControlWord(word newcw, word mask);
};

// Keyboard	class... now more complicated than ever	;)
class Keyboard {
	static byte	cIsKeyPressed[128];		   // current key state
	static byte	cWasKeyPressed[128];	// flags set if	key	hit
	static bool	bHandlerInstalled;		  // is	our	isr	installed?
	static void	interrupt (*OldInt9Handler)(...);	 //	old	keybd int handler
	static void	interrupt InterruptHandler(...);	// our int 9 ISR...	yea!
	static int ControlBreakHandler();	 //	empty ctrl+break handler
	void SetButtonMode();
	void SetNormalMode();

public:
	Keyboard();
	~Keyboard();

	bool IsAvailable();
	bool WasKeyPressed(word	k);
	bool IsKeyPressed(word k);
	void SimulateKeyPress(word k);
	void Clear();
	bool IsWaiting();
	word GetInput();
	void WaitPress();
	void WaitPressClear();
	bool GetKeyboardFlag(word wFlagId);
};

// Archive class; supports transparent Quake PAK and local files
class Archive {
	// PAK file	ID
	static const dword Signature;

	// PAK file	name
	char sPak[_MAX_PATH];

	// Parent directory	name
	char sParent[_MAX_PATH];

	// PAK file	header
	struct PAKHead {
		dword Signature;		/* signature of	the	data file ("PACK") */
		dword DirectoryOffset;	  /* offset	of the directory table */
		dword DirectorySize;	/* no. of data chunk entries in	the	directory *	sizeof(datDir_t) */
	} pakHead;

	// PAK file	directory entry
	struct PAKDir {
		char Name[0x38];		/* name	of the data	chunk (including path) */
		dword Offset;			 /*	offset of the data chunk in	the	data file */
		dword Size;	   /* size of the data chunk */
	} pakDir;

	// PAK file	or local file handle
	FILE *fPak;
	FILE *fLocal;

	void NormalizePath(char	*sPath);

public:
	Archive();
	Archive(const char *sPakFile);
	Archive(const char *sPakFile, const	char *sParentDir);
	Archive(const Archive &a);
	~Archive();

	bool SetArchiveFile(const char *sPakFile);
	void SetParentDirectory(const char *sParentDir);
	bool FileExists(const char *sFile);
	bool FileOpen(const	char *sFile);
	long FileSize();
	long FilePosition();
	bool FileSeek(long lPos);
	bool FileEnd();
	byte FileGetByte();
	word FileGetWord();
	dword FileGetDWord();
};

// Palette class
class Palette {
	friend Graphics;
	friend Bitmap;

	struct RGB {
		byte r;	  // 0-255
		byte g;	  // 0-255
		byte b;	  // 0-255
	} rgbPalette[256];

public:
	Palette();

	bool operator ==(const Palette &p);
	bool operator !=(const Palette &p);
	void Clear(byte	s, byte	n, byte	R, byte	G, byte	B);
	void Clear(byte	s, byte	n);
	void Clear();
	void Set(byte n, byte R, byte G, byte B);
	void Get(byte n, byte &R, byte &G, byte	&B);
	bool Increment(byte	s, byte	n, byte	c);
	bool Increment(byte	c);
	bool Increment();
	bool Decrement(byte	s, byte	n, byte	c);
	bool Decrement(byte	c);
	bool Decrement();
	bool Blend(const Palette &p, byte s, byte n);
	bool Blend(const Palette &p);
	void RotateLeft(byte s,	byte n);
	void RotateLeft();
	void RotateRight(byte s, byte n);
	void RotateRight();
	void Invert(byte s,	byte n);
	void Invert();
	void GenerateGradient(byte s, byte n, byte rs, byte	gs,	byte bs, byte re, byte ge, byte	be);
	void GenerateGradient(byte rs, byte	gs,	byte bs, byte re, byte ge, byte	be);
	void Copy(const	Palette	&p,	byte s,	byte n);
	void Copy(const	Palette	&p);
};

// VGA font	class... cool fonts	with little	memory :)
class Font {
	friend Graphics;

	// Font	file ID
	static const dword Signature;

	byte *cFont;
	// Note: The width is always 8 and font	table size = Height	* 256
	byte cHeight;

public:
	Font();
	Font(const Font	&f);
	Font(Archive &f);
	~Font();

	bool IsAvailable();
	byte GetHeight();
	byte GetWidth();
	void Copy(const	Font &f);
	bool Load(Archive &f);
};

// Bitmap class	(supports PCX loading and palette matching)
class Bitmap {
	friend Graphics;
	friend Sprite;
	friend Mouse;

	byte *cBitmap;
	word wWidth;
	word wHeight;

	static double RGBDistance(double r1, double	g1,	double b1, double r2, double g2, double	b2);

public:
	Bitmap();
	Bitmap(word	w, word	h);
	Bitmap(const Bitmap	&b);
	Bitmap(Archive &f, Palette &pToMatch);
	~Bitmap();

	bool IsAvailable();
	bool Create(word w,	word h);
	void Destroy();
	word GetWidth();
	word GetHeight();
	void Copy(const	Bitmap &b);
	bool Load(Archive &f, Palette &pToMatch);
	bool LoadPalette(Archive &f, Palette &p);
};

// The rodent class...
class Mouse	{
	bool bMousePresent;
	word wCursor[32];		 //	cursor bitmask

public:
	Mouse();
	~Mouse();

	bool Initialize();
	void Terminate();
	bool IsAvailable();
	void ShowCursor();
	void HideCursor();
	void Poll(int &mouseX, int &mouseY,	bool &lButton, bool	&rButton);
	void SetPosition(int mouseX, int mouseY);
	void SetCursorBounds(int mLeft,	int	mTop, int mRight, int mBottom);
	bool SetCursorBitmap(const Bitmap &b, int hsx, int hsy);
	byte GetVersion(byte &cVerMajor, byte &cVerMinor);
};

// Sprite class
class Sprite {
	friend Graphics;

	int	x, y;
	Bitmap *arrBitmap[64];
	byte cCurBmp;
	byte cBitmaps;

public:
	Sprite();

	bool IsAvailable();
	bool AddBitmap(Bitmap &b);
	void Clear();
	bool TestCollison(const	Sprite &s);
	void MoveAbsolute(int X, int Y);
	void MoveRelative(int X, int Y);
	void SetFirstFrame();
	void SetNextFrame();
};

// VGA mode	13h	graphics class... spiffy!
class Graphics {
	// Is VGA available?
	bool bIsVGA;
	// Mode	13h	linear framebuffer
	byte *cVideoBuffer;
	// Virtual linear framebuffer
	byte *cFrameBuffer;
	// Active framebuffer
	byte *cActiveBuffer;
	// Default Font
	byte *cFont;
	// Font	height
	byte cFontHeight;
	// Text	rendering direction
	byte cFontDirection;
	// Font	transparency
	bool bFontOpaque;
	// Font	background color
	byte cFontBackground;
	// Default font	pointers
	byte *cFont8x8;		   // 0x03 - 0x04
	byte *cFont8x16;	// 0x06

public:
	Graphics();
	Graphics(const Graphics	&g);
	~Graphics();

	bool Initialize();
	void Terminate();
	bool IsAvailable();
	word GetWidth();
	word GetHeight();
	word GetColors();
	void Clear(byte	c);
	void Clear();
	void Flip();
	void SetBorderColor(byte c);
	byte GetBorderColor();
	void SetGrayScalePalette(byte s, byte n);
	void SetGrayScalePalette();
	void WaitForRetrace();
	void DirectVideo(bool bs);
	void SetPalette(Palette	&p,	byte s,	byte n);
	void SetPalette(Palette	&p);
	void GetPalette(Palette	&p,	byte s,	byte n);
	void GetPalette(Palette	&p);
	void SetPixel(int x, int y,	byte c);
	byte GetPixel(int x, int y);
	void DrawHorizontalLine(int	lx,	int	ty,	int	rx,	byte c);
	void DrawVerticalLine(int lx, int ty, int by, byte c);
	void DrawLine(int x1, int y1, int x2, int y2, byte color);
	void DrawBox(int lx, int ty, int rx, int by, byte c);
	void DrawFilledBox(int lx, int ty, int rx, int by, byte	c);
	void DrawCircle(int	x, int y, int radius, byte c);
	void DrawFilledCircle(int x, int y,	int	radius,	byte c);
	void DrawEllipse(int x,	int	y, int rx, int ry, byte	c);
	void DrawFilledEllipse(int x, int y, int rx, int ry, byte color);
	void FloodFill(int x, int y, byte fc, byte bc);
	void SetFont(byte f);
	void SetFont(const Font	&f);
	void SetFontDirection(byte fd);
	void SetFontTransparency(bool t);
	void SetFontBackgroundColor(byte c);
	byte GetFontWidth();
	byte GetFontHeight();
	void Print(int x, int y, byte tc, const	char *sFormat, ...);
	void PutBitmap(const Bitmap	&b,	int	x, int y, byte flags = BITMAP_TRANSPARENT);
	void GetBitmap(const Bitmap	&b,	int	x, int y);
	void PutSprite(const Sprite	&s,	byte flags = BITMAP_TRANSPARENT);
};

// Profile class decleration...	reminds	me of Windows 3.1
class Profile {
	char sIniFile[_MAX_PATH];

	char *titlePos(char	*buf, size_t *len);
	bool isTitleLine(char *bufPtr);
	bool containTitle(char *buf, const char	*section);
	void removeDelimiters(char *bufPtr);
	bool gotoSection(FILE *is, const char *section);
	char *textPos(char *buf, const char	*entry);
	void stripQuotationChar(char *buf);
	int	readEntry(FILE *is,	const char *entry, char	*buf, int bufSize, bool	strip);

public:
	Profile();
	Profile(const char *sProfFile);

	void SetFile(const char	*sProfFile);
	int	Read(const char	*section, const	char *entry, const char	*defaultString,	char *buffer, int bufLen);
	word Read(const	char *section, const char *entry, word defaultWord);
	dword Read(const char *section,	const char *entry, dword defaultDWord);
	bool Write(const char *section,	const char *entry, const char *string);
	bool Write(const char *section,	const char *entry, word	wordVal);
	bool Write(const char *section,	const char *entry, dword dWordVal);
};

// Serial number authenticator class...
class Authenticator	{
	char sUserName[_MAX_PATH];
	char sSerial[1 + sizeof(dword) * 2];

	void GenerateSerial();

public:
	Authenticator();
	Authenticator(const	char *sUser);

	bool IsPasswordValid(const char	*sPassword);
	const char *GetPassword();
	const char *GetUser();
	void SetUser(const char	*sUser);
};

#endif
