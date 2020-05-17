/****************************************************************************\
*																			 *
*	Copyright (c) Samuel Gomes (Blade), 1998-2003.							 *
*	All rights reserved.													 *
*																			 *
*	mailto: blade_go@hotmail.com || blade_g@rediffmail.com					 *
*																			 *
*	Password/Serial number authenticator.									 *
*																			 *
\****************************************************************************/

#include <gFrame.h>

// Serial number generator function (32-bit_xor_invert_hex algorithm)
void Authenticator::GenerateSerial() {
	dword val = 0, val2;
	char *p = (char *)&val2;
	int i;
	char buf[_MAX_PATH];

	memset(buf, 0, _MAX_PATH);
	strcpy(buf, sUserName);

	// Handle cases where name is < 4 characters
	while (strlen(buf) < sizeof(dword)) {
		buf[strlen(buf)] = 127;
	}

	for (i = 0; i < ((strlen(buf) / sizeof(dword)) * sizeof(dword)); i += sizeof(dword)) {
		*p = buf[i];
		*(p + 1) = buf[i + 1];
		*(p + 2) = buf[i + 2];
		*(p + 3) = buf[i + 3];
		val ^= val2;			// xor
	}

	val = ~val;					// invert

	ultoa(val, sSerial, 16);	// hex
}

const char *Authenticator::GetPassword() {
	return sSerial;
}

const char *Authenticator::GetUser() {
	return sUserName;
}

void Authenticator::SetUser(const char *sUser) {
	if (!EMPTY_STRING(sUser)) {
		strcpy(sUserName, sUser);
	}
	else {
		strcpy(sUserName, "User");
	}
	GenerateSerial();
}

bool Authenticator::IsPasswordValid(const char *sPassword) {
	return bool(strcmpi(sPassword, sSerial) == EQUAL);
}

// Constructors
Authenticator::Authenticator() {
	SetUser(NULL);
}

Authenticator::Authenticator(const char *sUser) {
	SetUser(sUser);
}
