// This file is part of Notepad++ plugin MIME Tools project
// Copyright (C)2023 Don HO <don.h@free.fr>

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


#pragma once

#define VERSION_VALUE "3.1\0"
#define VERSION_DIGITALVALUE 3, 1, 0, 0

#define IDD_ABOUTBOX 250

#ifndef IDC_STATIC 
#define IDC_STATIC -1
#endif

#include "url.h"

void convertToBase64FromAscii();
void convertToBase64FromAscii_pad();
void convertToBase64FromAscii_B64Format();
void convertToBase64FromAscii_pad_byline();
void convertToBase64FromAscii_byline();
void convertToAsciiFromBase64();
void convertToAsciiFromBase64_strict();
void convertToAsciiFromBase64_whitespaceReset();
void convertToQuotedPrintable();
void convertToAsciiFromQuotedPrintable();
void convertURLMinEncode();
void convertURLEncodeExtended();
void convertURLFullEncode();
void convertURLMinEncodeByLine();
void convertURLEncodeExtendedByLine();
void convertURLFullEncodeByLine();
void convertURLEncode(UrlEncodeMethod method, bool isByLine = false);
void convertURLDecode();
void convertSamlDecode();
void convertURLDecode();
void about();

