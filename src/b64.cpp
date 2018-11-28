//this file is part of MimeTools (plugin for Notepad++)
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

#include "PluginInterface.h"
#include "mimeTools.h"
#include "b64.h"
#include "qp.h"
#include "url.h"
#include "saml.h"

char base64CharSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64CharMap[] = {  // base64 values or: -1 for illegal character, -2 to ignore character, and -3 for pad ('=')
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1, -2, -1, -1,  // <tab> <lf> & <cr> are ignored
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,  // <space> is ignored
	52, 53, 54, 55 ,56, 57, 58, 59, 60, 61, -1, -1, -1, -3, -1, -1,  // '=' is the pad character
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1 ,-1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

// base64Encode simply converts ascii to base64 with appropriate wrapping and padding. Encoding is done by loading
// three ascii characters at a time into a bitField, and then extracting them as four base64 values.
// returnString is assumed to be large enough to contain the result, and the function return is the length of the result

int base64Encode(char *resultString, const char *asciiString, size_t asciiStringLength, size_t wrapLength, bool padFlag)
{
	size_t index; // input string index
	size_t lineLength = 0; // Current line length
	int resultLength = 0, // result string length
		bitField, // assembled bit field (up to 3 ascii characters at a time)
		bitOffset = -1, // offset into bit field (8 bit input: 16, 8, 0 -> 6 bit output: 18, 12, 6, 0)
		endOffset; // end offset index value
		
	for (index = 0; index < asciiStringLength; )
	{
		bitField = 0;
		for (bitOffset = 16; bitOffset >= 0 && index < asciiStringLength; bitOffset -= 8)
		{
			bitField |= (UCHAR)asciiString[index++] << bitOffset;
		}
		endOffset = bitOffset + 3; // end indicator
		for (bitOffset = 18; bitOffset > endOffset; bitOffset -= 6)
		{
			if (wrapLength > 0 && lineLength++ >= wrapLength)
			{
				resultString[resultLength++] = '\n';
				lineLength = 1;
			}
			resultString[resultLength++] = base64CharSet[(bitField >> bitOffset) & 0x3f];
		}
	}
	if (padFlag)
	{
		for (; bitOffset >= 0; bitOffset -= 6)
		{
			if (wrapLength > 0 && lineLength++ >= wrapLength)
			{
				resultString[resultLength++] = '\n';
				lineLength = 1;
			}
			resultString[resultLength++] = '=';
		}
	}
	return resultLength;
}

// base64Decode converts base64 to ascii. But there are choices about what to do with illegal characters or
// malformed strings. In this version there is a strict flag to indicate that the input must be a single
// valid base64 string with no illegal characters, no extra padding, and no short segments. Otherwise
// there is best effort to decode around illegal characters which ARE preserved in the output.
// So  TWFyeQ==.aGFk.YQ.bGl0dGxl.bGFtYg==  decodes to  Mary.had.a.little.lamb  with five seperate
// base64 strings decoded, each separated by the illegal character dot. In strict mode the first dot
// would trigger a fatal error. Some other implementations choose to ignore illegal characters which
// of course has it's own issues.
// The four whitespace characters <CR> <LF> <TAB> and <SPACE> are always silently ignored. Decoding is
// done by loading four base64 values at a time into a bitField, and then extracting them as three ascii
// characters.
// returnString is assumed to be large enough to contain the result, and the function return is the length of
// the result, or a negative value in case of an error

int base64Decode(char *resultString, const char *encodedString, size_t encodedStringLength, bool strictFlag)
{
	size_t index; // input string index

	int resultLength = 0, // result string length
		bitField, // assembled bit field (up to 3 ascii characters at a time)
		bitOffset, // offset into bit field (6 bit intput: 18, 12, 6, 0 -> 8 bit output: 16, 8, 0)
		endOffset, // end offset index value
		charValue = 0, // character value
		charIndex = 0, // character index
		padLength = 0; // Pad characters seen

	for (index = 0; index < encodedStringLength; )
	{
		bitField = 0;
		for (bitOffset = 18; bitOffset >= 0 && index < encodedStringLength; )
		{
			charValue = (UCHAR)encodedString[index++];
			charIndex = base64CharMap[charValue & 0x7f];
			if (charIndex >= 0)
			{
				if (padLength > 0 && strictFlag)
				{
					return -1; // **ERROR** Data after pad character
				}
				bitField |= charIndex << bitOffset;
				bitOffset -= 6;
			}
			else
			{
				if (charIndex != -2) // -2 is Ignore character - eg <cr> <lf> <space> <tab>
				{
					if (charIndex == -3) // -3 is Pad character '='
					{
						padLength++;
						if (strictFlag && bitOffset > 6)
						{
							return -2; // **ERROR** Pad character in wrong place
						}
					}
					else
					{
						break; // -1 is Illegal character - deal with it later
					}
				}
			}
		}
		
		if (strictFlag && bitOffset == 12)
		{
			return -3; // **ERROR** Single symbol block not valid
		}
		endOffset = bitOffset + 3; // end indicator
		
		for (bitOffset = 16; bitOffset > endOffset; bitOffset -= 8)
		{
			resultString[resultLength++] = (bitField >> bitOffset) & 0xff;
		}

		if (charIndex == -1) // Was there an illegal character?
		{
			if (strictFlag)
			{
				return -4; // **ERROR** Bad character in input string
			}
			resultString[resultLength++] = (char)charValue;
		}
	}
	return resultLength;
}
