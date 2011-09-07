//this file is part of Notepad++
//Copyright (C)2007 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef NPP_B64_H
#define NPP_B64_H

#define ASCII_BITLEN 8
#define B64_BITLEN 6
#include <windows.h>

size_t getBase64LenFromAsciiLen(size_t asciiStrLen);
size_t getAsciiLenFromBase64Len(size_t base64StrLen);

size_t asciiToBase64(char *dest, const UCHAR *asciiStr, size_t asciiStrLen);
int base64ToAscii(char *dest, const char *base64Str);

#endif //NPP_B64_H
