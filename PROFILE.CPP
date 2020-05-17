/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Initialization file interface.											 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

//----------------------------------------------------------------------------
//	titlePos: get a section title position & length in a string.
//	Something like this:
//		"  [section] "
//			^p	  ^q
//----------------------------------------------------------------------------
char *Profile::titlePos(char *buf, size_t *len) {
	char *p = buf, *q;

	while (*p && isspace(*p)) p++;
	if (*p != '[') return NULL;

	q = ++p;
	while (*q && *q != ']') q++;
	if (*q != ']') return NULL;
	if (len != NULL) *len = (size_t)(q - p);
	return p;
}

//----------------------------------------------------------------------------
//	isTitleLine: check if a string is a section title line
//----------------------------------------------------------------------------
bool Profile::isTitleLine(char *bufPtr) {
	return bool(titlePos(bufPtr, NULL) != NULL);
}

//----------------------------------------------------------------------------
//	containTitle: check if a string contain a section a title
//----------------------------------------------------------------------------
bool Profile::containTitle(char *buf, const char *section) {
	char *p;
	size_t len;

	p = titlePos(buf, &len);
	if (p != NULL) {
		if (strlen(section) == len && strnicmp(section, p, len) == EQUAL)
		   return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//	removeDelimiters: removes Cr, Lf or CrLf pair from a string
//----------------------------------------------------------------------------
void Profile::removeDelimiters(char *bufPtr) {
	char *pos;

	pos = strchr(bufPtr, '\n');
	if (pos != NULL) *pos = EOS;

	pos = strchr(bufPtr, '\r');
	if (pos != NULL) *pos = EOS;
}

//----------------------------------------------------------------------------
//	gotoSection: move file position to start line of a section
//----------------------------------------------------------------------------
bool Profile::gotoSection(FILE *is, const char *section) {
	char line[_MAX_PATH];

	while (fgets(line, sizeof(line) - 1, is) != NULL) {
		removeDelimiters(line);
		if (containTitle(line, section)) return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//	textPos: get content's position of a entry
//----------------------------------------------------------------------------
char *Profile::textPos(char *buf, const char *entry) {
	char *p;
	size_t len;
	size_t i = 0;

	/* See if line is a comment */
	while (isspace(buf[i])) i++;
	if (buf[i] == ';') return NULL;

	p = strchr(buf, '=');
	if (p == NULL) return NULL;

	len = (size_t)(p - buf);
	if (strlen(entry) == len && strnicmp(buf, entry, len) == EQUAL)
		return p + 1;

	return NULL;
}

//----------------------------------------------------------------------------
//	stripQuotationChar: strip a pair of quotation chars in a string
//----------------------------------------------------------------------------
void Profile::stripQuotationChar(char *buf) {
	char *p;
	char *q;
	size_t len;

	p = buf;
	while (*p && isspace(*p)) p++;

	if (!(*p == '\"' || *p == '\'')) return;

	q = p + strlen(p);
	while (*q != *p && q > p) q--;
	if (q == p) return;
	len = (size_t)(q - p - 1);
	memmove(buf, p + 1, len);
	buf[len] = EOS;
}

//----------------------------------------------------------------------------
//	readEntry: read content of entry
//----------------------------------------------------------------------------
int Profile::readEntry(FILE *is, const char *entry, char *buf, int bufSize, bool strip) {
	char lineBuf[_MAX_PATH];
	char *p, *cur;
	int len;

	cur  = buf;
	*cur = EOS;
	len  = -1;
	while (fgets(lineBuf, sizeof(lineBuf) - 1, is) != NULL) {
		removeDelimiters(lineBuf);

		if (isTitleLine(lineBuf))		/* section is ended */
			break;

		p = textPos(lineBuf, entry);	/* not equal this entry */
		if (p == NULL)
			continue;

		if (strip)
			stripQuotationChar(p);

		len = strlen(p);
		if (bufSize - 1 < len)
			len = bufSize - 1;

		strncpy(cur, p, len);
		cur[len] = EOS;
		break;
	}

	return len;
}

//----------------------------------------------------------------------------
//	ProfileReadString:
//----------------------------------------------------------------------------
int Profile::Read(const char *section, const char *entry, const char *defaultString, char *buffer, int bufLen) {
	FILE *is;
	int len = -1;

	is = fopen(sIniFile, "rt");

	if (is != NULL && gotoSection(is, section))
		len = readEntry(is, entry, buffer, bufLen, true);

	if (len < 0) {		/* can not read entry, use default string */
		strncpy(buffer, defaultString, bufLen - 1);
		buffer[bufLen - 1] = EOS;
		len = strlen(buffer);
	}

	fclose(is);

	return len;
}

//----------------------------------------------------------------------------
//	ProfileReadWord:
//----------------------------------------------------------------------------
word Profile::Read(const char *section, const char *entry, word defaultWord) {
	char buf[_MAX_PATH];
	char iBuf[CHAR_MAX];

	itoa(defaultWord, iBuf, 10);
	Read(section, entry, iBuf, buf, sizeof(buf));
	return atoi(buf);
}

//----------------------------------------------------------------------------
//	ProfileReadDWord:
//----------------------------------------------------------------------------
dword Profile::Read(const char *section, const char *entry, dword defaultDWord) {
	char buf[_MAX_PATH];
	char iBuf[CHAR_MAX];

	ltoa(defaultDWord, iBuf, 10);
	Read(section, entry, iBuf, buf, sizeof(buf));
	return atol(buf);
}

//----------------------------------------------------------------------------
//	ProfileWriteString:
//----------------------------------------------------------------------------
bool Profile::Write(const char *section, const char *entry, const char *string) {
	FILE *is, *os;
	char temp_file[_MAX_PATH];
	char drive[MAXDRIVE];
	char dir[MAXDIR];
	char buf[_MAX_PATH];
	bool titleFound, writeSection;

	/* Check if entry is valid */
	if (EMPTY_STRING(entry)) return false;

	/* Work better on network! */
	_fullpath(buf, sIniFile, _MAX_PATH - 1);
	fnsplit(buf, drive, dir, NULL, NULL);
	fnsplit(tmpnam(NULL), NULL, NULL, buf, NULL);	/* screwed up C lib */
	fnmerge(temp_file, drive, dir, buf, ".TMP");

	if ((is = fopen(sIniFile, "rt")) == NULL) {
		/* Create file */
		if ((is = fopen(sIniFile, "wt")) == NULL) return false;
		fclose(is);
		/* Reopen it */
		if ((is = fopen(sIniFile, "rt")) == NULL) return false;
	}
	if ((os = fopen(temp_file, "wt")) == NULL) {
		fclose(is);
		return false;
	}

	titleFound = writeSection = false;
	while (fgets(buf, sizeof(buf) - 1, is) != NULL) {
		if (fputs(buf, os) == EOF) {
			fclose(is);
			fclose(os);
			unlink(temp_file);
			return false;
		}

		if (containTitle(buf, section)) {
			titleFound = true;
			break;
		}
	}

	if (!titleFound) {		/* add section */
		if (fputc('[', os) == EOF ||
			fputs(section, os) == EOF ||
			fputc(']', os) == EOF ||
			fputc('\n', os) == EOF ||
			fputs(entry, os) == EOF ||
			fputc('=', os) == EOF ||
			fputs(string, os) == EOF ||
			fputc('\n', os) == EOF) {
			fclose(is);
			fclose(os);
			unlink(temp_file);
			return false;
		}
	}
	else {
		while (fgets(buf, sizeof(buf) - 1, is) != NULL) {
			if (isTitleLine(buf)) {	/* section ended, but still not found the entry */
				writeSection = true;
				break;
			}

			if (textPos(buf, entry)) {	/* entry found, so rewrite it */
				break;
			}

			if (fputs(buf, os) == EOF) {
				fclose(is);
				fclose(os);
				unlink(temp_file);
				return false;
			}
		}

		if (fputs(entry, os) == EOF ||
			fputc('=', os) == EOF ||
			fputs(string, os) == EOF ||
			fputc('\n', os) == EOF) {
			fclose(is);
			fclose(os);
			unlink(temp_file);
			return false;
		}

		if (writeSection) {		/* write the next section title */
			if (fputs(buf, os) == EOF) {
				fclose(is);
				fclose(os);
				unlink(temp_file);
				return false;
			}
		}

		while (fgets(buf, sizeof(buf) - 1, is) != NULL) {	/* copy left lines */
			if (fputs(buf, os) == EOF) {
				fclose(is);
				fclose(os);
				unlink(temp_file);
				return false;
			}
		}
	}

	/* Cleanup */
	fclose(is);
	fclose(os);
	unlink(sIniFile);
	rename(temp_file, sIniFile);

	return true;
}

//----------------------------------------------------------------------------
//	ProfileWriteWord:
//----------------------------------------------------------------------------
bool Profile::Write(const char *section, const char *entry, word wordVal) {
	char iBuf[CHAR_MAX];

	itoa(wordVal, iBuf, 10);
	return Write(section, entry, iBuf);
}

//----------------------------------------------------------------------------
//	ProfileWriteDWord:
//----------------------------------------------------------------------------
bool Profile::Write(const char *section, const char *entry, dword dWordVal) {
	char iBuf[CHAR_MAX];

	ltoa(dWordVal, iBuf, 10);
	return Write(section, entry, iBuf);
}

void Profile::SetFile(const char *sProfFile) {
	if (!EMPTY_STRING(sProfFile)) {
		strcpy(sIniFile, sProfFile);
	}
}

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
Profile::Profile() {
	strcpy(sIniFile, "config.ini");
}

Profile::Profile(const char *sProfFile) {
	Profile();
	SetFile(sProfFile);
}
