// This file is part of Notepad++ plugin MIME Tools project
// Copyright (C)2022 Don HO <don.h@free.fr>

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


#include "qp.h"
#include <string.h>

char * QuotedPrintable::encode(const char *str) 
{
	initVar();
	size_t len = strlen(str);
	
	_bufLen = len*2;
	_buffer = new char[_bufLen];
	
	for (size_t i = 0 ; i < len ; i++)
	{
		getQPChar(str[i]);
		putQPChar();
	}
	_buffer[_i] = '\0';

	return _buffer;
}

void QuotedPrintable::getQPChar(char c)
{
	bool crlf = false;
	if ((c != '=' && c > 32 && c < 127) || c == ' ' || c == '	' || (UCHAR)c == 0x0D)
	{
		_chars[0] = c;
		_nbChar = 1;
	}
	else if ((int)c == 0x0A)
	{
		_chars[0] = c;
		_nbChar = 1;
		crlf = true;
	}
	else
	{
		UCHAR uc = (UCHAR)c;
		_chars[0] = '=';
		_chars[1] = toChar(uc >> 4);
		_chars[2] = toChar(uc & 15);
		_chars[3] = '\0';
		_nbChar = 3;
	}
	
	if (crlf)
		_nbCharInLine = _nbChar;
	else
		_nbCharInLine += _nbChar;

	if (_nbCharInLine >= 76)
	{
		_buffer[_i++] = '=';
		_buffer[_i++] = 0x0D;
		_buffer[_i++] = 0x0A;
		_nbCharInLine = _nbChar;
	}

}
	
void QuotedPrintable::putQPChar() 
{
	// it happens rarely, but it happens
	if (_i >= _bufLen)
	{
		size_t oldLen = _bufLen;
		_bufLen *= 2;
		char *newBuf = new char[_bufLen];
		
		for (size_t i = 0 ; i < oldLen ; i++)
			newBuf[i] = _buffer[i];

		char *tmp = _buffer;
		_buffer = newBuf;
		delete [] tmp;
	}

	for (int i = 0 ; i < _nbChar ; i++)
		_buffer[_i++] = _chars[i];
}

char * QuotedPrintable::decode(const char *str)
{
	initVar();
	char line[QP_ENCODED_LINE_LEN_MAX + 2 + 1];
	char *p = (char *)str;
	size_t len = strlen(str);
	
	_bufLen = len + 1;
	_buffer = new char[_bufLen];
	
	while (*p)
	{
		if (readQPLine(&p, line) == -1)
			return NULL;

		if (!translate(line))
			return NULL;
	}
	_buffer[_i] = '\0';
	return _buffer;
}

int QuotedPrintable::readQPLine(char **pStr, char *lineBuf) 
{
	size_t len = strlen(*pStr);
	size_t i = 0;
	for (; i < len ; i++)
	{
		if (i >= (QP_ENCODED_LINE_LEN_MAX + 2 + 1)) return -1;

		char c = (*pStr)[i];
		if (c == 0x0D)
		{
			lineBuf[i] = c;
			i++;
			if ((i >= len) || (i >= (QP_ENCODED_LINE_LEN_MAX + 2 + 1))) return -1;
			if ((*pStr)[i] != (char)0x0A) return -1;
			lineBuf[i] = (*pStr)[i];
			i++;
			if (i >= (QP_ENCODED_LINE_LEN_MAX + 2 + 1)) return -1;
			lineBuf[i] = '\0';
			*pStr += i;

			// Make sure there's no soft line break.
			if (i >= 3 && lineBuf[i-3] == '=')
			{
				lineBuf[i-3] = '\0';
				return int(i - 3);
			}
			return int(i);
		}
		else if (c == 0x0A)
		{
			return -1;
		}
		else
			lineBuf[i] = c;
	}
	*pStr += i;
	lineBuf[i] = '\0';
	return int(i);
}

bool QuotedPrintable::translate(char *line2Trans) 
{
	size_t len = strlen(line2Trans);
	for (size_t i = 0 ; i < len ; i++)
	{
		if (line2Trans[i] == '=')
		{
			if (i == len || (i + 1) == len|| (i + 2) == len)
				return false;
			UCHAR restoredChar;
			//
			
			restoredChar = makeChar(line2Trans[i+1], line2Trans[i+2]);
			i += 2;

			if (!restoredChar)
				return false;
			_buffer[_i++] = restoredChar;
		}
		else
		{
			_buffer[_i++] = line2Trans[i];
		}
	}
	return true;
}
