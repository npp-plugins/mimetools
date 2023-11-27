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


#ifndef NPP_URL_H
#define NPP_URL_H

int AsciiToUrl (char* dest, const char* src, int destSize, bool encodeAll, bool perLine);
int UrlToAscii (char* dest, const char* src, int destSize);

#endif //NPP_URL_H
