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

// Enhance Base64 features, and rewrite Base64 encode/decode implementation
// Copyright 2019 by Paul Nankervis <paulnank@hotmail.com>


#ifndef NPP_TOOLS_H
#define NPP_TOOLS_H

#define VERSION_VALUE "2.9\0"
#define VERSION_DIGITALVALUE 2, 9, 0, 0

#define IDD_ABOUTBOX 250

#ifndef IDC_STATIC 
#define IDC_STATIC -1
#endif



void convertToBase64FromAscii();
void convertToBase64FromAscii_pad();
void convertToBase64FromAscii_B64Format();
void convertToBase64FromAscii_byline();
void convertToAsciiFromBase64();
void convertToAsciiFromBase64_strict();
void convertToAsciiFromBase64_whitespaceReset();
void convertToQuotedPrintable();
void convertToAsciiFromQuotedPrintable();
void convertURLMinEncode();
void convertURLFullEncode();
void convertURLMinEncode_byline();
void convertURLFullEncode_byline();
void convertURLEncode(bool, bool);
void convertURLDecode();
void convertSamlDecode();
void convertURLDecode();
void about();

#endif //NPP_TOOLS_H
