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


#include <string.h>
#include <string>
#include <ctype.h>

#include "url.h"


// Unsafe:
// 
// Characters can be unsafe for a number of reasons.The space
// character is unsafe because significant spaces may disappearand
// insignificant spaces may be introduced when URLs are transcribed or
// typeset or subjected to the treatment of word - processing programs.
// The characters "<" and ">" are unsafe because they are used as the
// delimiters around URLs in free text; the quote mark(""") is used to
//     delimit URLs in some systems.The character "#" is unsafe and should
//     always be encoded because it is used in World Wide Web and in other
//     systems to delimit a URL from a fragment / anchor identifier that might
//     follow it.The character "%" is unsafe because it is used for
//     encodings of other characters.Other characters are unsafe because
//     gatewaysand other transport agents are known to sometimes modify
//     such characters.These characters are "{", "}", "|", "\", " ^ ", "~",
//     "[", "]", and "`".
// 
//     All unsafe characters must always be encoded within a URL.For
//     example, the character "#" must be encoded within URLs even in
//     systems that do not normally deal with fragment or anchor
//     identifiers, so that if the URL is copied into another system that
//     does use them, it will not be necessary to change the URL encoding.
//
// Reserved:
//
// Many URL schemes reserve certain characters for a special meaning :
// their appearance in the scheme - specific part of the URL has a
// designated semantics.If the character corresponding to an octet is
// reserved in a scheme, the octet must be encoded.The characters ";",
// "/", "?", ":", "@", "=" and "&" are the characters which may be
// reserved for special meaning within a scheme.No other characters may
// be reserved within a scheme.
//
// Usually a URL has the same interpretation when an octet is
// represented by a characterand when it encoded.However, this is not
// true for reserved characters : encoding a character reserved for a
// particular scheme may change the semantics of a URL.
// 
// Thus, only alphanumerics, the special characters "$-_.+!*'(),", and
// reserved characters used for their reserved purposes may be used
// unencoded within a URL.
// 
// On the other hand, characters that are not required to be encoded
// (including alphanumerics) may be encoded within the scheme - specific
// part of a URL, as long as they are not being used for a reserved
// purpose.
//
// https://datatracker.ietf.org/doc/html/rfc1738

// These "unsafe" characters must be encoded in a URL, as per RFC1738
static const char gReservedAscii[] = "<>\"#%{}|\\^~[]`;/?:@=& ";

// In order to follow the "standard" implementations,
// the following characters which are allowed to not be encoded (according RFC 1738)
// can be included into "must be encoded" characters.
static const char gExtendedChar[] = "!*'()+$,";

static const char gHexChar[] = "0123456789ABCDEF";

int AsciiToUrl(char* dest, const char* src, int destSize, UrlEncodeMethod method, bool isByLine)
{
  int i;
  memset (dest, 0, destSize);
  std::string reservedAscii = gReservedAscii;
  
  if (method == UrlEncodeMethod::extended)
      reservedAscii += gExtendedChar;

  for (i = 0; (i < (destSize - 2)) && *src; ++i, ++src)
  {
    if ((isByLine ? (strchr("\r\n", *src) != nullptr ? false : true) : true) &&       // if "by line" is demanded and current char is EOL, the false is returned to stop remain tests and EOL is not treated. Otherwise (true) we keep testing... 
        (method == UrlEncodeMethod::full ||                                           // if encode method is full, true is return to stop remain tests, then we convert the char whatever it is. Otherwise (true) we keep testing...
            ((strchr(reservedAscii.c_str(), *src) != nullptr) || !isprint(*src))))    // Here we convert only reserved characters or non-printable characters.
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