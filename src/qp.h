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


#ifndef NPP_QP_H
#define NPP_QP_H

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

constexpr auto QP_ENCODED_LINE_LEN_MAX = 76;

class QuotedPrintable {

public:	
	QuotedPrintable() : _buffer(NULL) {};
	~QuotedPrintable() {
		if (_buffer)
			delete [] _buffer; 
	};
	char * encode(const char *str);
	char * decode(const char *str);

private:
	char *_buffer = nullptr;
	size_t _bufLen = 0;
	size_t _i = 0;
	int _nbCharInLine = 0;
	
	int _nbChar = 0;
	char _chars[4] = {};

	int readQPLine(char **pStr, char *lineBuf);
	bool translate(char *line2Trans);

	void putQPChar();
	void getQPChar(char c);
	
	int32_t charToDigit(char c) const {
		if (c >= '0' && c <= '9')
			return (c - '0');
		if (c >= 'A' && c <= 'F')
			return (10 + c - 'A');
		return -1;
	};

	unsigned char makeChar(char hiChar, char loChar) const {
		auto hi = charToDigit(hiChar);
		if (hi == -1)
			return 0;
		auto lo = charToDigit(loChar);
		if (lo == -1)
			return 0;
		return static_cast<unsigned char>(hi << 4 | lo);
	};


	void initVar() {
		if (_buffer)
		{
			delete [] _buffer;
			_buffer = NULL;
		}
		_bufLen = 0; 
		_i = 0;
		_nbChar = 0;
		_nbCharInLine = 0;
	};

	char toChar(int i) {
		if (i < 10)
			return (char)((int)'0'+ i);
		else
			return (char)((int)'A'+ i-10);
	};
	
};

#endif //NPP_QP_H
