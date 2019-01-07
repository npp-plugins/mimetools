//this file is part of MimeTools (plugin for Notepad++)
//Copyright (C)2019 Don HO <don.h@free.fr>
//
//
// Enhance Base64 features, and rewrite Base64 encode/decode implementation
// Copyright 2019 by Paul Nankervis <paulnank@hotmail.com>
//
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

#include <windows.h>

int base64Encode(char *resultString, const char *asciiString, size_t asciiStringLength, size_t wrapLength, bool padFlag, bool byLineFlag);
int base64Decode(char *resultString, const char *encodedString, size_t encodedStringLength, bool strictFlag, bool whitespaceReset);

#endif //NPP_B64_H