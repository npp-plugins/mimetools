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

#include "b64.h"
#include <string.h>


char base64Array[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64IndexArray[123] =\
{\
-1, -1, -1, -1, -1, -1, -1, -1,\
-1, -1, -1, -1, -1, -1, -1, -1,\
-1, -1, -1, -1, -1, -1, -1, -1,\
-1, -1, -1, -1, -1, -1, -1, -1,\
-1, -1, -1, -1, -1, -1, -1, -1,\
-1, -1, -1, 62, -1, -1, -1, 63,\
52, 53, 54, 55 ,56, 57, 58, 59,\
60, 61, -1, -1, -1, -1, -1, -1,\
-1,  0,  1,  2,  3,  4,  5,  6,\
 7,  8,  9, 10, 11, 12, 13, 14,\
15, 16, 17, 18, 19, 20, 21, 22,\
23, 24, 25, -1, -1, -1, -1 ,-1,\
-1, 26, 27, 28, 29, 30, 31, 32,\
33, 34, 35, 36, 37, 38, 39, 40,\
41, 42, 43, 44, 45, 46, 47, 48,\
49, 50, 51\
};

size_t getBase64LenFromAsciiLen(size_t asciiStrLen)
{
	size_t nbBase64Byte = asciiStrLen * ASCII_BITLEN / B64_BITLEN;
	return nbBase64Byte + (((asciiStrLen * ASCII_BITLEN) % B64_BITLEN)?3:0);
}

size_t getAsciiLenFromBase64Len(size_t base64StrLen)
{
	if (base64StrLen % 4) return 0;
	return  base64StrLen - base64StrLen / 4;
}

size_t asciiToBase64(char *dest, const UCHAR *asciiStr, size_t asciiStrLen)
{
	//size_t asciiStrLen = strlen(asciiStr);
	size_t nbLoop = asciiStrLen / 3;
	size_t nbByteRemain = asciiStrLen % 3;

	size_t i = 0, k = 0;
	for ( ; i < nbLoop ; i++)
	{
		size_t j = i * 3;
		UCHAR p1, p2;

		UCHAR a = asciiStr[j] >> 2;
		dest[k++] = base64Array[a];
		p1 = asciiStr[j] << 6;
		p2 = asciiStr[j+1] >> 2;
		UCHAR b = p1 | p2;
		b >>= 2;
		dest[k++] = base64Array[b];

		p1 = asciiStr[j+1] << 4;
		p2 = asciiStr[j+2] >> 4;
		UCHAR c = p1 | p2;
		c >>= 2;
		dest[k++] = base64Array[c];

		p2 = asciiStr[j+2] << 2;
		UCHAR d = p2 >> 2;
		dest[k++] = base64Array[d];
	}

	if (nbByteRemain == 1)
	{
		size_t j = i * 3;
		UCHAR p;
		dest[k++] = base64Array[asciiStr[j] >> 2];

		p = asciiStr[j] << 6;
		dest[k++] = base64Array[p >> 2];

		dest[k++] = '=';
		dest[k++] = '=';
	}
	else if (nbByteRemain == 2)
	{
		size_t j = i * 3;
		UCHAR p1, p2;

		dest[k++] = base64Array[asciiStr[j] >> 2];

		p1 = asciiStr[j] << 6;
		p2 = asciiStr[j+1] >> 2;
		UCHAR b = p1 | p2;
		dest[k++] = base64Array[b >> 2];

		p1 = asciiStr[j+1] << 4;
		dest[k++] = base64Array[p1 >> 2];

		dest[k++] = '=';
	}

	dest[k] = '\0';
	return k;//
}

int base64ToAscii(char *dest, const char *base64Str)
{
	size_t b64StrLen = strlen(base64Str);	
	size_t nbLoop = b64StrLen / 4;

	size_t i = 0;
	int k = 0;

	enum {b64_just, b64_1padded, b64_2padded} padd = b64_just;
	for ( ; i < nbLoop ; i++)
	{
		size_t j = i * 4;
		UCHAR uc0, uc1, uc2, uc3, p0, p1;

		uc0 = base64IndexArray[base64Str[j]];
		uc1 = base64IndexArray[base64Str[j+1]];
		uc2 = base64IndexArray[base64Str[j+2]];
		uc3 = base64IndexArray[base64Str[j+3]];

		if ((uc0 == -1) || (uc1 == -1) || (uc2 == -1) || (uc3 == -1))
			return -1;

		if (base64Str[j+2] == '=') // && (uc3 == '=')
		{
			uc2 = uc3 = 0;
			padd = b64_2padded;
		}
		else if (base64Str[j+3] == '=')
		{
			uc3 = 0;
			padd = b64_1padded;
		}
		p0 = uc0 << 2;
		p1 = uc1 << 2;
		p1 >>= 6;
		dest[k++] = p0 | p1;

		p0 = uc1 << 4;
		p1 = uc2 << 2;
		p1 >>= 4;
		dest[k++] = p0 | p1;

		p0 = uc2 << 6;
		p1 = uc3;
		dest[k++] = p0 | p1;
	}

	//dest[k] = '\0';
	if (padd == b64_1padded)
	//	dest[k-1] = '\0';
		return k-1;
	else if (padd == b64_2padded)
	//	dest[k-2] = '\0';
		return k-2;

	return k;
}
