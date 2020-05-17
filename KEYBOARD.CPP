/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Keyboard handling interface.  											 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

/* constants */
#define MAX_SCANCODES 128
#define KEYBOARD_INPUT 0x60
#define KEYBOARD_XT_CLEAR 0x61
/* You got this! Right? */
#define KEYBOARD_DATA_READY_INT 0x9

// Globals
byte Keyboard::cIsKeyPressed[MAX_SCANCODES];			// current key state
byte Keyboard::cWasKeyPressed[MAX_SCANCODES];			// flags set if key hit
bool Keyboard::bHandlerInstalled = false;				// is our isr installed?
void interrupt (*Keyboard::OldInt9Handler)(...) = NULL;	// old keybd int handler

// Continue even if user presses ctrl+break
int Keyboard::ControlBreakHandler() {
	return true;
}

/*
Replacement for the BIOS Int 9 handler.  Detects when a
key is pressed and released.  Updates KeyStateArray to
reflect the state of each key.  Sets KeyStateArray to 1 if key
is currently being held down, 0 if released.  When a key is
released, WasPressedArray is set to 1.  This allows a program to
detect that a key was pressed and then released between
checks of the array.  WasPressedArray is cleared by the program
reading the state of a key, not by this routine.  KeyStateArray
and WasPressedArray are indexed by the keyboard scan code returned
from the keyboard controller.
*/
void interrupt Keyboard::InterruptHandler(...) {
	int ScanCode;
	int Temp;

	/* read scan code */
	ScanCode = inportb(KEYBOARD_INPUT);

	/* clear keyboard controller on XT machines */
	Temp = inportb(KEYBOARD_XT_CLEAR);
	outportb(KEYBOARD_XT_CLEAR, Temp | 0x80);
	outportb(KEYBOARD_XT_CLEAR, Temp & 0x7F);

	if (ScanCode & 0x80) {
		/* key up */
		ScanCode &= 0x7F;
		cIsKeyPressed[ScanCode] = false;
	}
	else {
		/* key down */
		cIsKeyPressed[ScanCode] =
		cWasKeyPressed[ScanCode] = true;
	}

	outportb(0x20, 0x20);
}

// Changes the keyboard operation to button mode
void Keyboard::SetButtonMode() {
	// Return if the ISR is already installed
	if (bHandlerInstalled) return;

	// Save the original ISR
	OldInt9Handler = _dos_getvect(KEYBOARD_DATA_READY_INT);
	// Install our ISR
	_dos_setvect(KEYBOARD_DATA_READY_INT, InterruptHandler);
	// Set global flag
	bHandlerInstalled = true;
}

/* Changes the keyboard operation to normal mode */
void Keyboard::SetNormalMode() {
	// Return if already in normal mode
	if (!bHandlerInstalled) return;

	// Restore the original ISR
	_dos_setvect(KEYBOARD_DATA_READY_INT, OldInt9Handler);
	// Clear global flag
	bHandlerInstalled = false;
}

// Keyboard ctor
Keyboard::Keyboard() {
	// Must do this since all privates are statics
	// If we don't we will leak the memory
	SetNormalMode();

	// Install the Ctrl+Break handler
	ctrlbrk(ControlBreakHandler);
	// Turn off MS-DOS ctrl break checking
	setcbrk(false);
	// Ignore Ctrl+C interrupt signal
	signal(SIGINT, (void (*)(int))SIG_IGN);

	Clear();
}

// Keyboard dtor
Keyboard::~Keyboard() {
	SetNormalMode();
	Clear();
}

// Returns true if we can override the keyboard interrupt
bool Keyboard::IsAvailable() {
	// Switch to button mode
	SetButtonMode();

	return bHandlerInstalled;
}

/* Return true if the key was held down since this function was called */
bool Keyboard::WasKeyPressed(word k) {
	bool result;

	// Switch to button mode
	SetButtonMode();

	if (!bHandlerInstalled || k >= MAX_SCANCODES) return false;

	result = (bool)(cIsKeyPressed[k] | cWasKeyPressed[k]);
	cWasKeyPressed[k] = false;
	return result;
}

/* Return true if the key is currently being help down */
bool Keyboard::IsKeyPressed(word k) {
	// Switch to button mode
	SetButtonMode();

	if (!bHandlerInstalled || k >= MAX_SCANCODES) return false;

	return bool(cIsKeyPressed[k]);
}

/* Simulates a keypress */
void Keyboard::SimulateKeyPress(word k) {
	// Works in both modes
	if (k < MAX_SCANCODES) {
		cIsKeyPressed[k] = cWasKeyPressed[k] = true;
	}

	// Set normal mode
	SetNormalMode();

	_asm {
		mov cx, k;
		mov ax, 0x0500;
		int 0x16;
	}
}

/* Empties the keyboard buffer */
void Keyboard::Clear() {
	// Works in both modes
	for (int i = 0; i < MAX_SCANCODES; i++) {
		cIsKeyPressed[i] = cWasKeyPressed[i] = false;
	}

	while (GetInput() != KB_NOKEY) {
		Processor::Yield();
	}
}

/* Returns true if a key is waiting in the keyboard buffer */
bool Keyboard::IsWaiting() {
	// Switch to normal mode
	SetNormalMode();

	if (bHandlerInstalled) return false;

	return bool(_bios_keybrd(_NKEYBRD_READY));
}

/* Retuns the code of a normal ascii or extended key press else 0 */
word Keyboard::GetInput() {
	word c = KB_NOKEY;
	byte cAsc, cScan;

	// Switch to normal mode
	SetNormalMode();

	if (bHandlerInstalled) return KB_NOKEY;

	if (_bios_keybrd(_NKEYBRD_READY)) {
		c = _bios_keybrd(_NKEYBRD_READ);
		cAsc = LOBYTE(c);
		cScan = HIBYTE(c);
		c = (cAsc == 0 ? 256u + cScan : cAsc);
	}

	return c;
}

/* Waits for a keypress to occur */
void Keyboard::WaitPress() {
	// Switch to normal mode
	SetNormalMode();

	if (bHandlerInstalled) return;

	while (!_bios_keybrd(_NKEYBRD_READY)) {
		Processor::Yield();
	}
}

/* Waits for a keypress to occur; then consumes it */
void Keyboard::WaitPressClear() {
	Clear();
	WaitPress();
	Clear();
}

/* Return true if the given keyboard flag is set */
bool Keyboard::GetKeyboardFlag(word wFlagId) {
	// Set to normal mode
	SetNormalMode();

	return bool(_bios_keybrd(_NKEYBRD_SHIFTSTATUS) & wFlagId);
}
