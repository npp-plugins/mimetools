//this file is part of MimeTools (plugin for Notepad++)
//Copyright (C)2019 Don HO <don.h@free.fr>
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

#include <string.h>
#include <ctype.h>

#include "url.h"

// These characters must be encoded in a URL, as per RFC1738
static const char gReservedAscii[] = "<>\"#%{}|\\^~[]`;/?:@=& ";  
static const char gHexChar[] = "0123456789ABCDEF";

int AsciiToUrl (char* dest, const char* src, int destSize, bool encodeAll)
{
  int i;

  memset (dest, 0, destSize);

  for (i = 0; (i < (destSize - 2)) && *src; ++i, ++src)
  {
    // Encode source if it is a reserved or non-printable character.
    //
    if (encodeAll || (strchr (gReservedAscii, *src) != 0) || !isprint(*src))
    {
      *dest++ = '%';
      *dest++ = gHexChar [((*src >> 4) & 0x0f)];
      *dest++ = gHexChar [(*src & 0x0f)];
      i += 2;
    }
    else  // don't encode character
    {
      *dest++ = *src;
    }
  }

  return i;  // return characters stored to destination
}


int UrlToAscii (char* dest, const char* src, int destSize)
{
  char	val;
  int i, j;

  memset (dest, 0, destSize);

  for (i = 0; (i < destSize) && *src; ++i)
  {
    if (*src == '%')
    {
      ++src;

      // Found an encoded triplet.
      // The next two characters must be hex.
      //
      if (isxdigit(src[0]) && isxdigit(src[1]))
      {
        for (val = 0, j = 0; j < 2; ++j, ++src)
        {
          val <<= 4;

          if (isdigit(*src))
          {
            val += *src - '0';
          }
          else if (isupper(*src))
          {
            val += *src - 'A' + 10;
          }
          else
          {
            val += *src - 'a' + 10;
          }
        }

        *dest++ = val;
      }
      else  // invalid encoding
      {
        return -1;
      }
    }
    else  // non-encoded character, so just copy it
    {
      *dest++ = *src;
      ++src;
    }
  }

  return i;
}