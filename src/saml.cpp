//this file is part of MimeTools (plugin for Notepad++)
//Copyright (C)2013 Robert Meakins <rmeakins@users.sf.net>
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

#include "saml.h"
#include "b64.h"
#include "url.h"
#include "tinf.h"


int samlDecode(char *dest, const char *encodedSamlStr, int bufLength)
{
  char *pUrlDecodedText = new char[bufLength];
	
  memset(dest, 0, SAML_MESSAGE_MAX_SIZE);
  memset(pUrlDecodedText, 0, bufLength);


  // URL Decode
  size_t urlDecodedLen = UrlToAscii(pUrlDecodedText, encodedSamlStr, bufLength);

  if (urlDecodedLen < 0)
  {
    delete [] pUrlDecodedText;
	return SAML_DECODE_ERROR_URLDECODE;
  }


  // Base64 Decode
  char *pBase64Text = new char[urlDecodedLen];
  memset(pBase64Text, 0, urlDecodedLen);
  size_t m = 0, n = 0;
  for ( ; m < urlDecodedLen ; )
  {
    if (pUrlDecodedText[m] == 0x0A || pUrlDecodedText[m] == 0x0D)
      m++;
    else
      pBase64Text[n++] = pUrlDecodedText[m++];
  }
  delete [] pUrlDecodedText;
  
  size_t asciiLen = getAsciiLenFromBase64Len(n);
  if (!asciiLen)
  {
    return SAML_DECODE_ERROR_BASE64DECODE;
  }

  char *base64DecodedText = new char[asciiLen];

  size_t base64DecodedLen = base64ToAscii(base64DecodedText, pBase64Text);
  delete [] pBase64Text;

  // A SAML message should be longer than 10 chars
  if (base64DecodedLen < 10)
  {
	delete [] base64DecodedText;
	return SAML_DECODE_ERROR_BASE64DECODE;
  }

  // If the first 5 chars are "<?xml" or "<saml", no need to inflate
  if (   (base64DecodedText[0] == '<')
	  && (base64DecodedText[3] == 'm')
	  && (base64DecodedText[4] == 'l'))
  {
	memcpy(dest, base64DecodedText, base64DecodedLen);
	delete [] base64DecodedText;
    return int(base64DecodedLen);
  }
  

  // Inflate the Base64 decoded text
  char *inflatedText = new char[SAML_MESSAGE_MAX_SIZE];
  unsigned int inflatedTextLen = 0;
  
  tinf_init();
  int inflateReturnCode = tinf_uncompress(inflatedText, &inflatedTextLen, base64DecodedText);
  delete [] base64DecodedText;

  if (inflateReturnCode != TINF_OK) {
	delete [] inflatedText;
	return SAML_DECODE_ERROR_INFLATE;
  }
  
  memcpy(dest, inflatedText, inflatedTextLen);
  delete [] inflatedText;
  
  // If the first 5 chars are not "<?xml" or "<saml", there's a problem
  if (!( (dest[0] == '<')
	  && (dest[3] == 'm')
	  && (dest[4] == 'l')))
  {
	  return SAML_DECODE_ERROR_INFLATE;
  }
  return int(inflatedTextLen);
  
}
