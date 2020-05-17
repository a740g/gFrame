/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Processor fiddling interface.               							 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

Processor::Processor() {
	ResetFPU();
}

Processor::~Processor() {
	ResetFPU();
}

/*
   Determine the processor type. This is adapted from code in the
   Pentium<tm> Family User's Manual, Volume 3:  Architecture and
   Programming Manual, Intel Corp., 1994, Chapter 5. That code contains
   the following comment:

   This program has been developed by Intel Corporation.
   Software developers have Intel's permission to incorporate
   this source code into your software royalty free.
*/
word Processor::GetCPUID() {
	word cpuId = 0x0100;	/* 80186 by default; dunno 8086 (ideas???) */

	/*
	Intel 8086 CPU check.
	Bits 12-15 of the FLAGS register are always set on the 8086 processor.
	Probably the 80186 as well.
	*/
	_asm {
		pushf;					/* get original flags into AX */
		pop	ax;
		mov	cx,	ax;				/* save them */
		and	ax,	0xFFF;			/* clear bits 12-15 */
		push ax;				/* save new flags value on stack */
		popf;					/* replace current flags value */
		pushf;					/* get new flags */
		pop	ax;					/* store new flags in AX */
		and	ax,	0xF000;			/* check to see whether bits 12-15 are set */
		cmp	ax,	0xF000;
		je cpuOK;				/* if 8086 or 80186 (can't tell them apart) */
	}

	/*
	Intel 286 CPU check.
	Bits 12-15 of the flags register are always clear on the
	Intel 286 processor in real-address mode.
	*/
	cpuId =	0x0200;
	_asm {
		or cx, 0xF000;			/* try to set bits 12-15 */
		push cx; 				/* save new flags value on stack */
		popf;					/* replace current flags value */
		pushf;					/* get new flags */
		pop	ax;					/* store new flags in AX */
		test ax, 0xF000;		/* if bits 12-15 clear, CPU = 80286 */
		jz cpuOK;				/* if 80286 */
	}

	/*
	Intel 386 CPU check.
	The AC bit, bit #18, is a new bit introduced in the EFLAGS
	register on the Intel486 DX cpu to generate alignment faults.
	This bit cannot be set on the Intel386 CPU.
	Ok, now we can execute 32-bit instructions.
	*/
	cpuId =	0x0300;
	_asm {
		mov	bx,	sp;				/* save current stack pointer to align */
		and	sp,	not	3;			/* align stack to avoid AC fault */
		db 0x66;				/* 32-bit data prefix */
		pushf;					/* push original EFLAGS */
		db 0x66;
		pop ax;					/* get original EFLAGS */
		db 0x66;
		mov cx, ax;				/* save original EFLAGS in CX */
		db 0x66;
		db 0x35;
		db 0x0;
		db 0x0;
		db 0x4;
		db 0x0;				/* flip (XOR) AC bit in EFLAGS */
		db 0x66;
		push ax;				/* put new EFLAGS value on stack */
		db 0x66;
		popf;					/* replace EFLAGS value */
		db 0x66;
		pushf;					/* get new EFLAGS */
		db 0x66;
		pop ax;					/* store new EFLAGS value in EAX */
		db 0x66;
		cmp	ax,	cx;
		jz fixRegs;				/* if 80386 CPU */
	}

	/*
	Intel486 DX CPU, Intel487 SX NDP, and Intel486 SX CPU check.
	Checking for ability to set/clear ID flag (bit 21) in EFLAGS
	which indicates the presence of a processor with the ability
	to use the CPUID instruction.
	*/
	cpuId =	0x0400;
	_asm {
		db 0x66;
		mov	ax,	cx;				/* get original EFLAGS */
		db 0x66;
		db 0x35;
		db 0x0;
		db 0x0;
		db 0x20;
		db 0x0;				/* flip (XOR) ID bit in EFLAGS */
		db 0x66;
		push ax;				/* save new EFLAGS value on stack */
		db 0x66;
		popf;					/* replace current EFLAGS value */
		db 0x66;
		pushf;					/* get new EFLAGS */
		db 0x66;
		pop	ax;					/* store new EFLAGS in EAX */
		db 0x66;
		cmp	ax,	cx;				/* check if it's changed */
		je fixRegs;				/* if it's a 486 (can't toggle ID bit) */
		db 0x66;
		push cx;
		db 0x66;
		popf;					/* restore AC bit in EFLAGS first */
		mov	sp,	bx;				/* restore original stack pointer */
		/* Execute CPUID instruction. */
		db 0x66;
		xor	ax,	ax;				/* set up input for CPUID instruction */
		db 0xF;
		db 0xA2;				/* CPUID instruction */
		db 0x66;
		db 0x83;
		db 0xF8;
		db 0x1;				/* cmp eax,1 */
		jl cpuOK;				/* if 1 is not a valid input value for CPUID */
		db 0x66;
		xor	ax,	ax;				/* otherwise, run CPUID with ax = 1 */
		db 0x66;
		inc	ax;
		db 0xF;
		db 0xA2;				/* CPUID instruction */
		mov cpuId, ax;
		jmp cpuOK				/* don't restore SP */
	}
fixRegs:
	_asm {
		db 0x66;
		push cx;
		db 0x66;
		popf;					/* restore AC bit in EFLAGS first */
		mov	sp,	bx;				/* restore original stack pointer */
   }
cpuOK:
	return cpuId;
}

/* Gets the CPU name */
const char *Processor::GetCPUName() {
	static char sCPU[_MAX_PATH];
	const char *sIntel = "Intel";
	const char *sAMD = "AMD";
	const char *sCyrix = "Cyrix";
	const char *sCentaurIDT = "Centaur/IDT";
	word idCPU = GetCPUID();
	bool isFPU = IsFPUAvailable();

	switch (idCPU) {
		case 0x0:
			if (isFPU) {
				sprintf(sCPU, "%s i%s + i%s", sIntel, "86", "87");
			}
			else {
				sprintf(sCPU, "%s i%s", sIntel, "86");
			}
			break;
		case 0x100:
			if (isFPU) {
				sprintf(sCPU, "%s i%s + i%s", sIntel, "186", "187");
			}
			else {
				sprintf(sCPU, "%s i%s", sIntel, "186");
			}
			break;
		case 0x200:
			if (isFPU) {
				sprintf(sCPU, "%s i%s + i%s", sIntel, "286", "287");
			}
			else {
				sprintf(sCPU, "%s i%s", sIntel, "286");
			}
			break;
		case 0x300:
			if (isFPU) {
				sprintf(sCPU, "%s i%s + i%s", sIntel, "386", "387");
			}
			else {
				sprintf(sCPU, "%s i%s", sIntel, "386");
			}
			break;
		case 0x400:
			if (isFPU) {
				sprintf(sCPU, "%s i%s + i%s", sIntel, "486DX", "487");
			}
			else {
				sprintf(sCPU, "%s i%s", sIntel, "486DX");
			}
			break;
		case 0x435:
			sprintf(sCPU, "%s i%s", sIntel, "486DX2");
			break;
		case 0x480:
			sprintf(sCPU, "%s i%s", sIntel, "486DX4");
			break;
		case 0x517:
		case 0x525:
		case 0x526:
		case 0x52B:
		case 0x52C:
			sprintf(sCPU, "%s %s", sIntel, "Pentium");
			break;
		case 0x1532:
			sprintf(sCPU, "%s %s", sIntel, "Pentium OverDrive");
			break;
		case 0x543:
		case 0x544:
		case 0x581:
			sprintf(sCPU, "%s %s", sIntel, "Pentium MMX");
			break;
		case 0x617:
		case 0x619:
			sprintf(sCPU, "%s %s", sIntel, "Pentium Pro");
			break;
		case 0x633:
		case 0x634:
		case 0x651:
		case 0x652:
			sprintf(sCPU, "%s %s", sIntel, "Pentium II");
			break;
		case 0x650:
		case 0x6B4:
		case 0xF13:
			sprintf(sCPU, "%s %s", sIntel, "Celeron");
			break;
		case 0x660:
		case 0x665:
			sprintf(sCPU, "%s %s", sIntel, "Celeron A");
			break;
		case 0x66A:
			sprintf(sCPU, "%s %s", sIntel, "Pentium II PE");
			break;
		case 0x672:
		case 0x673:
			sprintf(sCPU, "%s %s", sIntel, "Pentium III");
			break;
		case 0x681:
		case 0x683:
			sprintf(sCPU, "%s %s", sIntel, "Pentium III E");
			break;
		case 0x494:
			sprintf(sCPU, "%s %s", sAMD, "Am486DX4");
			break;
		case 0x4F4:
			sprintf(sCPU, "%s %s", sAMD, "Am5x86");
			break;
		case 0x500:
		case 0x501:
		case 0x514:
			sprintf(sCPU, "%s %s", sAMD, "K5");
			break;
		case 0x562:
		case 0x570:
			sprintf(sCPU, "%s %s", sAMD, "K6");
			break;
		case 0x580:
		case 0x584:
		case 0x58C:
			sprintf(sCPU, "%s %s", sAMD, "K6-2");
			break;
		case 0x591:
            sprintf(sCPU, "%s %s", sAMD, "K6-3");
			break;
		case 0x5D4:
			sprintf(sCPU, "%s %s", sAMD, "K6-2+");
			break;
		case 0x611:
		case 0x612:
		case 0x621:
		case 0x622:
			sprintf(sCPU, "%s %s", sAMD, "Athlon");
			break;
		case 0x630:
			sprintf(sCPU, "%s %s", sAMD, "Duron");
			break;
		case 0x520:
			sprintf(sCPU, "%s %s", sCyrix, "6x86L");
			break;
		case 0x600:
			sprintf(sCPU, "%s %s", sCyrix, "6x86MX");
			break;
		case 0x601:
			sprintf(sCPU, "%s %s", sCyrix, "M-II");
			break;
		case 0x541:
		case 0x542:
			sprintf(sCPU, "%s %s", sCentaurIDT, "WinChip");
			break;
		case 0x585:
			sprintf(sCPU, "%s %s", sCentaurIDT, "WinChip2");
			break;
		default:
			sprintf(sCPU, "x86%c (0x%X)", (isFPU ? 'f' : 'i'), idCPU);
	}

	return sCPU;
}

/* Gives up the current time slice under multitasking environments */
void Processor::Yield() {
	_asm {
		mov ax, 0x1680;
		int 0x2F;			/* DPMI give up rest of time slice */
	}
}

/* Returns true if a FPU is installed */
bool Processor::IsFPUAvailable() {
	return bool(_8087);
}

void Processor::ResetFPU() {
	_fpreset();
}

word Processor::ClearFPUStatus() {
	return _clear87();
}

word Processor::GetFPUStatus() {
	return _status87();
}

word Processor::SetFPUControlWord(word newcw, word mask) {
	return _control87(newcw, mask);
}
