// This file is part of Notepad++ plugin MIME Tools project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Enhance Base64 features, and rewrite Base64 encode/decode implementation
// Copyright 2019 by Paul Nankervis <paulnank@hotmail.com>

#include "PluginInterface.h"
#include "mimeTools.h"
#include "b64.h"
#include "qp.h"
#include "url.h"
#include "saml.h"

// Base64 encoding decoding - where 8 bit ascii is re-represented using just 64 ascii characters (plus optional padding '=').
//
// This code includes options to encode to base64 in multiple ways. For example the text lines:-
//
//	If you can keep your head when all about you
//	Are losing theirs and blaming it on you;
//
// Using "Encode with Unix EOL" would produce a single base64 string with line breaks after each 64 characters:-
//
//	SWYgeW91IGNhbiBrZWVwIHlvdXIgaGVhZCB3aGVuIGFsbCBhYm91dCB5b3UNCkFy
//	ZSBsb3NpbmcgdGhlaXJzIGFuZCBibGFtaW5nIGl0IG9uIHlvdTs=
//
// That would be decoded using a single base64 decode which ignored whitespace characters (the line breaks).
//
// Alternatively the same lines could be encoded using a "by line" option to encode each line of input as
// its own separate base64 string:-
//
//	SWYgeW91IGNhbiBrZWVwIHlvdXIgaGVhZCB3aGVuIGFsbCBhYm91dCB5b3U
//	QXJlIGxvc2luZyB0aGVpcnMgYW5kIGJsYW1pbmcgaXQgb24geW91Ow
//
// Each of these output lines could be decoded separately, or multiple lines decoded using "reset on whitespace"
// to cause base64 decoding to restart on each line


char base64CharSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64CharMap[] = {	 // base64 values or: -1 for illegal character, -2 to ignore character, and -3 for pad ('=')
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1, -2, -1, -1,	 // <tab> <lf> & <cr> are ignored
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,	 // <space> is ignored
	52, 53, 54, 55 ,56, 57, 58, 59, 60, 61, -1, -1, -1, -3, -1, -1,	 // '=' is the pad character
	-1,	 0,	 1,	 2,	 3,	 4,	 5,	 6,	 7,	 8,	 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1 ,-1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

// base64Encode simply converts ascii to base64 with appropriate wrapping and padding. Encoding is done by loading
// three ascii characters at a time into a bitField, and then extracting them as four base64 values.
// returnString is assumed to be large enough to contain the result (which is typically 4 / 3 the input size
// plus line breaks), and the function return is the length of the result
// wrapLength sets the length at which to wrap the encoded test at (not valid with byLineFlag)
// padFlag controls whether the one or two '=' pad characters are included at the end of encoding
// byLineFlag causes each input line to be encoded as a separate base64 string

int base64Encode(char *resultString, const char *asciiString, size_t asciiStringLength, size_t wrapLength, bool padFlag, bool byLineFlag)
{
	size_t index; // input string index
	size_t lineLength = 0; // current line length
	int resultLength = 0, // result string length
		bitField, // assembled bit field (up to 3 ascii characters at a time)
		bitOffset = -1, // offset into bit field (8 bit input: 16, 8, 0 -> 6 bit output: 18, 12, 6, 0)
		endOffset, // end offset index value
		charValue; // character value

	for (index = 0; index < asciiStringLength; )
	{
		bitField = 0;
		for (bitOffset = 16; bitOffset >= 0 && index < asciiStringLength; bitOffset -= 8)
		{
			charValue = (UCHAR)asciiString[index];
			if (byLineFlag && (charValue == '\n' || charValue == '\r'))
			{
				break;
			}
			index++;
			bitField |= charValue << bitOffset;
		}
		endOffset = bitOffset + 3; // end indicator
		for (bitOffset = 18; bitOffset > endOffset; bitOffset -= 6)
		{
			if (wrapLength > 0 && lineLength++ >= wrapLength && !byLineFlag)
			{
				resultString[resultLength++] = '\n';
				lineLength = 1;
			}
			resultString[resultLength++] = base64CharSet[(bitField >> bitOffset) & 0x3f];
		}
		if (byLineFlag)
		{
			while (index < asciiStringLength && (asciiString[index] == '\n' || asciiString[index] == '\r'))
			{
				resultString[resultLength++] = asciiString[index++];
			}
		}
	}
	if (padFlag && !byLineFlag)
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
// So  "TWFyeQ==.aGFk.YQ.bGl0dGxl.bGFtYg=="	 decodes to	 "Mary.had.a.little.lamb"  with five seperate
// base64 strings decoded, each separated by the illegal character dot. In strict mode the first dot
// would trigger a fatal error. Some other implementations choose to ignore illegal characters which
// of course has it's own issues.
// The four whitespace characters <CR> <LF> <TAB> and <SPACE> are silently ignored unless noWhitespaceFlag
// is set. In this case whitespace is treated similar to illegal characters and base64 decoding operates
// around the white space. So "TWFyeQ== aGFk YQ bGl0dGxl bGFtYg==" would decode as "Mary had a little lamb".
// Decoding is done by loading four base64 characters at a time into a bitField, and then extracting them as
// three ascii characters.
// returnString is assumed to be large enough to contain the result (which could be the same size as the input),
// and the function return is the length of the result, or a negative value in case of an error

int base64Decode(char *resultString, const char *encodedString, size_t encodedStringLength, bool strictFlag, bool whitespaceReset)
{
	size_t index; // input string index

	int resultLength = 0, // result string length
		bitField, // assembled bit field (up to 3 ascii characters at a time)
		bitOffset, // offset into bit field (6 bit intput: 18, 12, 6, 0 -> 8 bit output: 16, 8, 0)
		endOffset, // end offset index value
		charValue = 0, // character value
		charIndex = 0, // character index
		padLength = 0; // pad characters seen

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
				if (charIndex == -3) // -3 is Pad character '='
				{
					padLength++;
					if (strictFlag && bitOffset > 6)
					{
						return -2; // **ERROR** Pad character in wrong place
					}
				}
				else // either -1 for illegal character or -2 for whitespace (ignored)
				{
					if (charIndex == -1 || whitespaceReset)
					{
						charIndex = -1; // Remember it as an illegal character for copy below
						break;	// exit loop to deal with illegal character
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