/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Transparent Quake PAK file and disk file access interface.				 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

// Pak file ID
const dword Archive::Signature = ID('P', 'A', 'C', 'K');

void Archive::NormalizePath(char *sPath) {
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	size_t i;

	// Split the pathname; take only drv and dir
	_splitpath(sPath, drive, dir, NULL, NULL);

	// Store it back to sPath
	strcpy(sPath, drive);
	strcat(sPath, dir);

	// Add a backslash if needed
	i = strlen(sPath);

	if ((i <= 0) || (sPath[i - 1] == '/') || (sPath[i - 1] == '\\')) return;

	sPath[i++] = '/';
	sPath[i] = 0;
}

// Default ctor
Archive::Archive() {
	sPak[0] = EOS;
	strcpy(sParent, _argv[0]);
	NormalizePath(sParent);
	if (EMPTY_STRING(sParent)) {
		getcwd(sParent, _MAX_PATH);
		strcat(sParent, "/");
		NormalizePath(sParent);
	}
	memset(&pakHead, 0, sizeof(pakHead));
	memset(&pakDir, 0, sizeof(pakDir));
	fPak = NULL;
	fLocal = NULL;
}

Archive::Archive(const Archive &a) {
	Archive();
	strcpy(sPak, a.sPak);
	strcpy(sParent, a.sParent);
}

Archive::Archive(const char *sPakFile) {
	Archive();
	SetArchiveFile(sPakFile);
}

Archive::Archive(const char *sPakFile, const char *sParentDir) {
	Archive();
	SetArchiveFile(sPakFile);
	SetParentDirectory(sParentDir);
}

Archive::~Archive() {
	if (fLocal != NULL) {
		fclose(fLocal);
		fLocal = NULL;
	}

	if (fPak != NULL) {
		fclose(fPak);
		fPak = NULL;
	}
}

void Archive::SetParentDirectory(const char *sParentDir) {
	if (EMPTY_STRING(sParentDir)) return;

	strcpy(sParent, sParentDir);
	NormalizePath(sParent);
}

bool Archive::SetArchiveFile(const char *sPakFile) {
	if (fPak != NULL) {
		fclose(fPak);
		fPak = NULL;
		memset(&pakDir, 0, sizeof(pakDir));
	}

	// Open the pak file
	fPak = fopen(sPakFile, "rb");
	if (fPak == NULL) {
		return false;
	}

	// Read the header
	if (fread(&pakHead, sizeof(byte), sizeof(pakHead), fPak) != sizeof(pakHead)) {
		fclose(fPak);
		fPak = NULL;
		return false;
	}

	if (pakHead.Signature != Signature || pakHead.DirectorySize == 0) {
		fclose(fPak);
		fPak = NULL;
		return false;
	}

	return true;
}

bool Archive::FileExists(const char *sFile) {
	char filename[_MAX_PATH];
	PAKDir pkd;
	long pos, i;

	strcpy(filename, sParent);
	strcpy(filename, sFile);

	if (access(filename, 0) == 0) {
		return true;
	}

	if (access(sFile, 0) == 0) {
		return true;
	}

	if (fPak == NULL) return false;

	// Save the original position
	pos = ftell(fPak);

	// Seek to the pak directory
	if (fseek(fPak, pakHead.DirectoryOffset, SEEK_SET) != 0) {
		fseek(fPak, pos, SEEK_SET);
		return false;
	}

	// Start reading and searching the directory
	for (i = 0; i < (pakHead.DirectorySize / sizeof(pakDir)); i++) {
		if (fread(&pkd, sizeof(byte), sizeof(pkd), fPak) != sizeof(pkd)) {
			fseek(fPak, pos, SEEK_SET);
			return false;
		}

		if (stricmp(sFile, pkd.Name) == EQUAL) {
			fseek(fPak, pos, SEEK_SET);
			return true;
		}
	}

	fseek(fPak, pos, SEEK_SET);
	return false;
}

bool Archive::FileOpen(const char *sFile) {
	char filename[_MAX_PATH];
	long i;

	if (fLocal != NULL) {
		fclose(fLocal);
		fLocal = NULL;
	}

	strcpy(filename, sParent);
	strcpy(filename, sFile);

	fLocal = fopen(filename, "rb");
	if (fLocal != NULL) {
		return true;
	}

	fLocal = fopen(sFile, "rb");
	if (fLocal != NULL) {
		return true;
	}

	if (fPak == NULL) return false;

	// Seek to the pak directory
	if (fseek(fPak, pakHead.DirectoryOffset, SEEK_SET) != 0) {
		return false;
	}

	// Start reading and searching the directory
	for (i = 0; i < (pakHead.DirectorySize / sizeof(pakDir)); i++) {
		if (fread(&pakDir, sizeof(byte), sizeof(pakDir), fPak) != sizeof(pakDir)) {
			return false;
		}

		if (stricmp(sFile, pakDir.Name) == EQUAL) {
			if (fseek(fPak, pakDir.Offset, SEEK_SET) != 0) {
				return false;
			}
			else {
				return true;
			}
		}
	}

	return false;
}

long Archive::FileSize() {
	if (fLocal != NULL) {
		return filelength(fileno(fLocal));
	}

	if (fPak == NULL) return EOF;

	return pakDir.Size;
}

bool Archive::FileSeek(long lPos) {
	if (fLocal != NULL) {
		return bool(fseek(fLocal, lPos, SEEK_SET) == 0);
	}

	if (fPak == NULL || lPos >= pakDir.Size) return false;

	return bool(fseek(fPak, pakDir.Offset + lPos, SEEK_SET) == 0);
}

long Archive::FilePosition() {
	if (fLocal != NULL) {
		return ftell(fLocal);
	}

	if (fPak == NULL) return EOF;

	return ftell(fPak) - pakDir.Offset;
}

bool Archive::FileEnd() {
	if (fLocal != NULL) {
		return bool(feof(fLocal));
	}

	return bool(FilePosition() >= pakDir.Size);
}

byte Archive::FileGetByte() {
	if (fLocal != NULL) {
		return fgetc(fLocal);
	}

	if (fPak == NULL) return EOF;

	return fgetc(fPak);
}

word Archive::FileGetWord() {
	if (fLocal != NULL) {
		return getw(fLocal);
	}

	if (fPak == NULL) return EOF;

	return getw(fPak);
}

dword Archive::FileGetDWord() {
	int b1, b2, b3, b4;

	if (fLocal != NULL)
		if ((b1 = fgetc(fLocal)) != EOF)
			if ((b2 = fgetc(fLocal)) != EOF)
				if ((b3 = fgetc(fLocal)) != EOF)
					if ((b4 = fgetc(fLocal)) != EOF)
						return (((dword)b4 << 24) | ((dword)b3 << 16) | ((dword)b2 << 8) | (dword)b1);

	if (fPak == NULL) return EOF;

	if ((b1 = fgetc(fPak)) != EOF)
		if ((b2 = fgetc(fPak)) != EOF)
			if ((b3 = fgetc(fPak)) != EOF)
				if ((b4 = fgetc(fPak)) != EOF)
					return (((dword)b4 << 24) | ((dword)b3 << 16) | ((dword)b2 << 8) | (dword)b1);

	return EOF;
}
