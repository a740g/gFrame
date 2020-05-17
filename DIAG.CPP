/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	System Diagnostic class													 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

/* Upper memory block size in KB */
#define UMB_TOTAL 384
#define MULTIPLEX_INT 0x2F

// Finds Windows (3.x & 9x) version numbers
void Diagnostic::GetWindowsVersion(byte &cVerMajor, byte &cVerMinor) {
	byte vmax1, vmin1;
	byte vmax2, vmin2;
	byte vmax3, vmin3;

	// 2.x check
	_asm {
		mov ax, 0x1600;
		int MULTIPLEX_INT;
		mov vmax1, al;
		mov vmin1, ah;
	}

	switch (vmax1) {
		case 0x01:
		case 0xFF:
			vmax1 = 2;
			break;
		default:
			vmax1 = vmin1 = 0;
	}

	// 3.x check
	_asm {
		mov ax, 0x4680;
		int MULTIPLEX_INT;
	}

	if (_AX == 0x0000) {
		vmax2 = 3;
		vmin2 = 0;
	}
	else {
		vmax2 = vmin2 = 0;
	}

	// standard check
	_asm {
		mov ax, 0x160A;
		int MULTIPLEX_INT;
		mov vmax3, bh;
		mov vmin3, bl;
	}

	cVerMajor = MAX(MAX(vmax1, vmax2), vmax3);
	cVerMinor = MAX(MAX(vmin1, vmin2), vmin3);
}

// Finds Windows NT (NT 4.x, 2000, XP, Server etc.) version numbers
void Diagnostic::GetWinNTVersion(byte &cVerMajor, byte &cVerMinor) {
	// Get true DOS version
	_asm {
		mov ax, 0x3306;
		int 0x21;
	}

	if (_BX == 0x3205) {
		cVerMajor = _osmajor;
		cVerMinor = _osminor;
	}
	else {
		cVerMajor = cVerMinor = 0;
	}
}

// Finds IBM's OS/2 version numbers
void Diagnostic::GetOS2Version(byte &cVerMajor, byte &cVerMinor) {
	word axReg, bxReg;

	// Get OS/2 version
	_asm {
		mov ax, 0x4010;
		int MULTIPLEX_INT;
		mov axReg, ax;
		mov bxReg, bx;
	}

	if (axReg != 0x4010) {
		if (axReg == 0x0000) {
			cVerMajor = 3;
			cVerMinor = 0;
		}
		else {
			cVerMajor = HIBYTE(bxReg);
			cVerMinor = LOBYTE(bxReg);
		}
	}
	else {
		cVerMajor = cVerMinor = 0;
	}
}

// Finds Linux's DOSEMU's version numbers
void Diagnostic::GetLinuxVersion(byte &cVerMajor, byte &cVerMinor) {
	word axReg, bxReg;
	void *DOSEmuPtr = MK_FP(0xF000, 0xFFF5);

	cVerMajor = cVerMinor = 0;

	if (memcmp(DOSEmuPtr, "02/25/93", 8) == EQUAL) {
		_asm {
			mov ax, 0x0000;
			int 0xE6;
			mov axReg, ax;
			mov bxReg, bx;
		}

		if (axReg == 0xAA55) {
			cVerMajor = HIBYTE(bxReg);
			cVerMinor = LOBYTE(bxReg);
		}
	}
}

// Get's the total amount of base memory :)
word Diagnostic::GetTotalBaseMemory() {
	_asm {
		int 0x12;
		add ax, UMB_TOTAL;
	}

	return _AX;
}

// Get the total amount of extended memory (may be faked by many OSes)
dword Diagnostic::GetTotalExtendedMemory() {
	word lsb1, msb1, lsb2, msb2;
	dword xm1, xm2, xm3, xm4;

	// Try asking the BIOS
	_asm {
		mov ax, 0x8800;
		int 0x15;
	}
	xm4 = _AX;

	/* Read the first Xtended memory information from CMOS */
	outportb(0x70, 0x17);
	lsb1 = (word)inportb(0x71);
	outportb(0x70, 0x18);
	msb1 = (word)inportb(0x71);

	/* Read the second Xtended memory information from CMOS */
	outportb(0x70, 0x30);
	lsb2 = (word)inportb(0x71);
	outportb(0x70, 0x31);
	msb2 = (word)inportb(0x71);

	/* Calculate individual Xtended memory sizes */
	xm1 = MAKEWORD(msb1, lsb1);
	xm2 = MAKEWORD(msb2, lsb2);
	xm3 = (xm1 == xm2 ? 0 : xm1 + xm2);

	/* Return the maximum of it all */
	return MAX(MAX(MAX(xm1, xm2), xm3), xm4);
}

// Get the total amount of installed memory (may be faked by NT, Linux ...)
dword Diagnostic::GetTotalMemory() {
	return GetTotalExtendedMemory() + GetTotalBaseMemory();
}

// Returns the OS name along with it's version
const char *Diagnostic::GetOSName() {
	static char sOS[_MAX_PATH];
	char const *s_name;
	byte c_type, c_major, c_minor;

	// Get OS type and version
	c_type = GetOSVersion(c_major, c_minor);

	switch (c_type) {
		case OS_LINUX:
			s_name = "Linux";
			break;
		case OS_OS2:
			s_name = "OS/2";
			break;
		case OS_WINDOWSNT:
			s_name = "Windows NT";
			break;
		case OS_WINDOWS9X:
			s_name = "Windows 9x";
			break;
		case OS_WINDOWS3X:
			s_name = "Windows";
			break;
		default:	//OS_DOS:
			s_name = "DOS";
	}

	sprintf(sOS, "%s %i.%i", s_name, (int)c_major, (int)c_minor);

	return sOS;
}

// Returns the OS version numbers and type
byte Diagnostic::GetOSVersion(byte &cVerMajor, byte &cVerMinor) {
	byte cOS;

	// Try linux first
	cOS = OS_LINUX;
	GetLinuxVersion(cVerMajor, cVerMinor);

	// Try OS/2
	if (cVerMajor == 0) {
		cOS = OS_OS2;
		GetOS2Version(cVerMajor, cVerMinor);
	}

	// Try Windows NT
	if (cVerMajor == 0) {
		cOS = OS_WINDOWSNT;
		GetWinNTVersion(cVerMajor, cVerMinor);
	}

	// Try Windows 9x/3.x
	if (cVerMajor == 0) {
		cOS = OS_WINDOWS9X;
		GetWindowsVersion(cVerMajor, cVerMinor);
		// Adjust for 3.x
		if (cVerMajor < 4) cOS = OS_WINDOWS3X;
	}

	// Finally try DOS
	if (cVerMajor == 0) {
		cOS = OS_DOS;
		cVerMajor = _osmajor;
		cVerMinor = _osminor;
	}

	return cOS;
}

void Diagnostic::DebugPrint(const char *sFormat, ...) {
	char sTemp[1024];
	FILE *f;
	va_list vaPtr;

#ifdef PARANOID
	if (strlen(sFormat) >= 1024) {
		DebugPrint("String too long!");
		return;
	}
#endif

	// Render the string
	va_start(vaPtr, sFormat);
	vsprintf(sTemp, sFormat, vaPtr);
	va_end(vaPtr);

	f = fopen("debug.log", "a");

	if (f == NULL) return;

	fputs(sTemp, f);
	fputc('\n', f);

	fclose(f);
}
