/* Ferda Prantl - character coding conversion engine  */

#include "StdAfx.h"
#include "cs2cs.h"
#include <cstring>
#include <malloc.h>

// Escaped character constants in range 0x80-0xFF are interpreted in current codepage
// Using C locale gets us direct mapping to Unicode codepoints
#pragma setlocale("C")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FD_ENCODING_LBRACKET _T ("<!--")
#define FD_ENCODING_MARK     _T ("MYCHARSET")
#define FD_ENCODING_RBRACKET _T ("-->")

#define codes_count 13
#define chars_all_count 66
#define chars_alphabet_count 44

type_codes source_codes[] =
{
  {_T ("ASCII"), _T ("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x26\x5c\x22\x3c\x3e\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")},
  {_T ("CP1250"), _T ("\xc1\xc8\xcf\xc9\xcc\xcd\xd2\xd3\xd8\x8a\x8d\xda\xd9\xdd\x8e\xbc\xc0\xc5\xd4\xd6\xdc\xc4\xe1\xe8\xef\xe9\xec\xed\xf2\xf3\xf8\x9a\x9d\xfa\xf9\xfd\x9e\xbe\xe0\xe5\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\x74\xa0\x7c\x63\x52\xb0\x2b\xb6\x7\0\0\0\xd7\0\xf7\0")},
  {_T ("CP1252"), _T ("\xc1\0\0\xc9\0\xcd\0\xd3\0\x8a\0\xda\0\xdd\0\0\0\0\xd4\xd6\xdc\xc4\xe1\0\0\xe9\0\xed\0\xf3\0\x9a\0\xfa\0\xfd\0\0\0\0\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\x74\xa0\x7c\x63\x52\xb0\x2b\xb6\x7\xbc\xbd\xbe\xd7\xd8\xf7\xf8")},
  {_T ("CP850"), _T ("\xb5\0\0\x90\0\xd6\0\xe0\0\0\0\xe9\0\xed\0\0\0\0\xe2\x99\x9a\x8e\xa0\0\0\x82\0\xa1\0\xa2\0\0\0\xa3\0\xec\0\0\0\0\x93\x94\x81\x84\xe1\0\x26\x5c\x22\x3c\x3e\0\0\0\0\0\xf8\xf1\xf5\xfa\xac\xab\0\x9e\0\xf6\xed")},
  {_T ("CP852"), _T ("\xb5\xac\xd2\x90\xb7\xd6\xd5\xe0\xfc\xe6\x9b\xe9\xde\xed\xa6\x95\xe8\x91\xe2\x99\x9a\x8e\xa0\x9f\xd4\x82\xd8\xa1\xe5\xa2\xfd\xe7\x9c\xa3\x85\xec\xa7\x96\xea\x92\x93\x94\x81\x84\xe1\xf5\x26\x5c\x22\x3c\x3e\0\0\0\0\0\xf8\0\xf5\0\0\0\0\x9e\0\xf6\0")},
  {_T ("IBM852"), _T ("\xb5\xab\xd2\x90\xb7\xd6\xd5\xe0\xfd\xe6\x9b\xe9\xde\xed\xa5\x95\xe8\x91\xe2\x99\x9a\x8e\xa0\x9f\xd4\x82\xd8\xa1\xe5\xa2\xfe\xe7\x9c\xa3\x85\xec\xa6\x96\xea\x92\x93\x94\x81\x84\xe1\xf5\x26\x5c\x22\x3c\x3e\0\0\0\0\0\xf8\0\xf5\0\0\0\0\x9e\0\xf6\0")},
  {_T ("ISO-8859-1"), _T ("\xc1\0\0\xc9\0\xcd\0\xd3\0\0\0\xda\0\xdd\0\0\0\0\xd4\xd6\xdc\xc4\xe1\0\0\xe9\0\xed\0\xf3\0\0\0\xfa\0\xfd\0\0\0\0\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\0\xa0\x7c\x63\x52\xb0\x2b\xb6\x7\xbc\xbd\xbe\xd7\xd8\xf7\xf8")},
  {_T ("ISO-8859-2"), _T ("\xc1\xc8\xcf\xc9\xcc\xcd\xd2\xd3\xd8\xa9\xab\xda\xd9\xdd\xae\xa5\xc0\xc5\xd4\xd6\xdc\xc4\xe1\xe8\xef\xe9\xec\xed\xf2\xf3\xf8\xb9\xbb\xfa\xf9\xfd\xbe\xb5\xe0\xe5\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\0\xa0\x7c\x63\x52\xb0\x2b\xb6\x7\xbc\xbd\xbe\xd7\xd8\xf7\xf8")},
  {_T ("KEYBCS2"), _T ("\x8f\x80\x85\x90\x89\x8b\xa5\x95\x9e\x9b\x86\x97\xa6\x9d\x92\x9c\xab\x8a\xa7\x99\x9a\x8e\xa0\x87\x83\x82\x88\xa1\xa4\xa2\xa9\xa8\x9f\xa3\x96\x98\x91\x8c\xaa\x8d\x93\x94\x81\x84\xe1\xad\x26\x5c\x22\x3c\x3e\0\0\0\0\0\xf8\xf1\xf5\xfa\xac\0\0\x9e\0\xf6\0")},
  {_T ("KOI8-CS"), _T ("\xe1\xe3\xe4\xf7\xe5\xe9\xee\xef\xf2\xf3\xf4\xf5\xea\xf9\xfa\xec\xe6\xeb\xf0\xed\xe8\xf1\xc1\xc3\xc4\xd7\xc5\xc9\xce\xcf\xd2\xd3\xd4\xd5\xca\xd9\xda\xcc\xc6\xcb\xd0\xcd\xc8\xd1\0\0\x26\x5c\x22\x3c\x3e\0\0\0\0\0\xfe\0\0\0\0\0\0\0\0\0\0")},
  {_T ("MAC"), _T ("\xe7\0\0\x83\0\xea\0\xee\0\0\0\xf2\0\0\0\0\0\0\xef\x85\x86\x80\x87\0\0\x8e\0\x92\0\x97\0\0\0\x9c\0\0\0\0\0\0\x99\x9a\x9f\x8a\xa7\xa4\x26\x5c\x22\x3c\x3e\xaa\xca\0\x63\xa8\xa1\x2b\x7c\0\0\0\0\0\xaf\xd6\xbf")},
  {_T ("MACCE"), _T ("\xe7\x89\x91\x83\x9d\xea\xc5\xee\xdb\xe1\xe8\xf2\xf1\xf8\xeb\xbb\xd9\xbd\xef\x85\x86\x80\x87\x8b\x93\x8e\x9e\x92\xcb\x97\xde\xe4\xe9\x9c\xf3\xf9\xec\xbc\xda\xbe\x99\x9a\x9f\x8a\xa7\xa4\x26\x5c\x22\x3c\x3e\xaa\xca\0\x63\xa8\xa1\x2b\x7c\0\0\0\0\0\xaf\xd6\xbf")},
  {_T ("CORK"), _T ("\xc1\x83\x84\xc9\x85\xcd\x8c\xd3\x90\x92\x94\xda\x97\xdd\x9a\x89\x8f\x88\xd4\xd6\xdc\xc4\xe1\xa3\xa4\xe9\xa5\xed\xac\xf3\xb0\xb2\xb4\xfa\xb7\xfd\xba\xa9\xaf\xa8\xf4\xf6\xfc\xe4\0\0\x26\x5c\x22\x3c\x3e\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\0\xf8")}
}, destination_codes[] =
{
  {_T ("ASCII"), _T ("\x41\x43\x44\x45\x45\x49\x4e\x4f\x52\x53\x54\x55\x55\x59\x5a\x4c\x52\x4c\x4f\x4f\x55\x41\x61\x63\x64\x65\x65\x69\x6e\x6f\x72\x73\x74\x75\x75\x79\x7a\x6c\x72\x6c\x6f\x6f\x75\x61\x73\x53\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20")},
  {_T ("CP1250"), _T ("\xc1\xc8\xcf\xc9\xcc\xcd\xd2\xd3\xd8\x8a\x8d\xda\xd9\xdd\x8e\xbc\xc0\xc5\xd4\xd6\xdc\xc4\xe1\xe8\xef\xe9\xec\xed\xf2\xf3\xf8\x9a\x9d\xfa\xf9\xfd\x9e\xbe\xe0\xe5\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\x99\xa0\xa6\xa9\xae\xb0\xb1\xb6\xb7\x20\x20\x20\xd7\x20\xf7\x20")},
  {_T ("CP1252"), _T ("\xc1\x43\x44\xc9\x45\xcd\x4e\xd3\x52\x8a\x54\xda\x55\xdd\x5a\x4c\x52\x4c\xd4\xd6\xdc\xc4\xe1\x63\x64\xe9\x65\xed\x6e\xf3\x72\x9a\x74\xfa\x75\xfd\x7a\x6c\x72\x6c\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\x99\xa0\xa6\xa9\xae\xb0\xb1\xb6\xb7\xbc\xbd\xbe\xd7\xd8\xf7\xf8")},
  {_T ("CP850"), _T ("\xb5\x43\x44\x90\x45\xd6\x4e\xe0\x52\x53\x54\xe9\x55\xed\x5a\x4c\x52\x4c\xe2\x99\x9a\x8e\xa0\x63\x64\x82\x65\xa1\x6e\xa2\x72\x73\x74\xa3\x75\xec\x7a\x6c\x72\x6c\x93\x94\x81\x84\xe1\xf5\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\xf8\xf1\xf5\xfa\xac\xab\x20\x9e\x20\xf6\xed")},
  {_T ("CP852"), _T ("\xb5\xac\xd2\x90\xb7\xd6\xd5\xe0\xfc\xe6\x9b\xe9\xde\xed\xa6\x95\xe8\x91\xe2\x99\x9a\x8e\xa0\x9f\xd4\x82\xd8\xa1\xe5\xa2\xfd\xe7\x9c\xa3\x85\xec\xa7\x96\xea\x92\x93\x94\x81\x84\xe1\xf5\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\xf8\x20\xf5\x20\x20\x20\x20\x9e\x20\xf6\x20")},
  {_T ("IBM852"), _T ("\xb5\xab\xd2\x90\xb7\xd6\xd5\xe0\xfd\xe6\x9b\xe9\xde\xed\xa5\x95\xe8\x91\xe2\x99\x9a\x8e\xa0\x9f\xd4\x82\xd8\xa1\xe5\xa2\xfe\xe7\x9c\xa3\x85\xec\xa6\x96\xea\x92\x93\x94\x81\x84\xe1\xf5\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\xf8\x20\xf5\x20\x20\x20\x20\x9e\x20\xf6\x20")},
  {_T ("ISO-8859-1"), _T ("\xc1\x43\x44\xc9\x45\xcd\x4e\xd3\x52\x53\x54\xda\x55\xdd\x5a\x4c\x52\x4c\xd4\xd6\xdc\xc4\xe1\x63\x64\xe9\x65\xed\x6e\xf3\x72\x73\x74\xfa\x75\xfd\x7a\x6c\x72\x6c\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\x20\xa0\xa6\xa9\xae\xb0\xb1\xb6\xb7\xbc\xbd\xbe\xd7\xd8\xf7\xf8")},
  {_T ("ISO-8859-2"), _T ("\xc1\xc8\xcf\xc9\xcc\xcd\xd2\xd3\xd8\xa9\xab\xda\xd9\xdd\xae\xa5\xc0\xc5\xd4\xd6\xdc\xc4\xe1\xe8\xef\xe9\xec\xed\xf2\xf3\xf8\xb9\xbb\xfa\xf9\xfd\xbe\xb5\xe0\xe5\xf4\xf6\xfc\xe4\xdf\xa7\x26\x5c\x22\x3c\x3e\x20\xa0\xa6\xa9\xae\xb0\xb1\xb6\xb7\xbc\xbd\xbe\xd7\xd8\xf7\xf8")},
  {_T ("KEYBCS2"), _T ("\x8f\x80\x85\x90\x89\x8b\xa5\x95\x9e\x9b\x86\x97\xa6\x9d\x92\x9c\xab\x8a\xa7\x99\x9a\x8e\xa0\x87\x83\x82\x88\xa1\xa4\xa2\xa9\xa8\x9f\xa3\x96\x98\x91\x8c\xaa\x8d\x93\x94\x81\x84\xe1\xad\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\xf8\xf1\xf5\xfa\xac\x20\x20\x9e\x20\xf6\x20")},
  {_T ("KOI8-CS"), _T ("\xe1\xe3\xe4\xf7\xe5\xe9\xee\xef\xf2\xf3\xf4\xf5\xea\xf9\xfa\xec\xe6\xeb\xf0\xed\xe8\xf1\xc1\xc3\xc4\xd7\xc5\xc9\xce\xcf\xd2\xd3\xd4\xd5\xca\xd9\xda\xcc\xc6\xcb\xd0\xcd\xc8\xd1\x73\x53\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\xfe\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20")},
  {_T ("MAC"), _T ("\xe7\x43\x44\x83\x45\xea\x4e\xee\x52\x53\x54\xf2\x55\x59\x5a\x4c\x52\x4c\xef\x85\x86\x80\x87\x63\x64\x8e\x65\x92\x6e\x97\x72\x73\x74\x9c\x75\x79\x7a\x6c\x72\x6c\x99\x9a\x9f\x8a\xa7\xa4\x26\x5c\x22\x3c\x3e\xaa\xca\x20\xa9\xa8\xa1\xb1\xa6\x20\x20\x20\x20\x20\xaf\xd6\xbf")},
  {_T ("MACCE"), _T ("\xe7\x89\x91\x83\x9d\xea\xc5\xee\xdb\xe1\xe8\xf2\xf1\xf8\xeb\xbb\xd9\xbd\xef\x85\x86\x80\x87\x8b\x93\x8e\x9e\x92\xcb\x97\xde\xe4\xe9\x9c\xf3\xf9\xec\xbc\xda\xbe\x99\x9a\x9f\x8a\xa7\xa4\x26\x5c\x22\x3c\x3e\xaa\xca\x20\xa9\xa8\xa1\xb1\xa6\x20\x20\x20\x20\x20\xaf\xd6\xbf")},
  {_T ("CORK"), _T ("\xc1\x83\x84\xc9\x85\xcd\x8c\xd3\x90\x92\x94\xda\x97\xdd\x9a\x89\x8f\x88\xd4\xd6\xdc\xc4\xe1\xa3\xa4\xe9\xa5\xed\xac\xf3\xb0\xb2\xb4\xfa\xb7\xfd\xba\xa9\xaf\xa8\xf4\xf6\xfc\xe4\x73\x53\x26\x5c\x22\x3c\x3e\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xd8\x20\xf8")}
};

void
str_fill (tchar_t* s, tchar_t ch, long count)
{
  while (count--)
    *s++ = ch;
  *s = _T ('\0');
}

ptrdiff_t
str_pos (const tchar_t* whole, const tchar_t* piece)
{
  const tchar_t* s = whole;
  size_t l = tc::tcslen (piece);

  while (*s)
    if (!tc::tcsnicmp (s++, piece, l))
      return (s - whole - 1);
  return -2;
}

bool
str_same (const tchar_t* str1, const tchar_t* str2, long count)
{
  if (!count)
    return false;
  while (count--)
    if (*str1++ != *str2++)
      return false;
  return true;
}

const tchar_t*
skip_spaces (const tchar_t* s)
{
  while (*s)
    if (*s == _T (' ') || *s == _T ('\t') || *s == _T ('\r') || *s == _T ('\n'))
      s++;
    else
      break;
  return s;
}

const tchar_t*
skip_word (const tchar_t* s)
{
  s = skip_spaces (s);
  while (*s)
    if (*s != _T (' ') && *s != _T ('\t') && *s != _T ('\r') && *s != _T ('\n') && *s != _T ('='))
      s++;
    else
      break;
  return skip_spaces (s);
}

ptrdiff_t
get_coding (const tchar_t* name, type_codes *codes, int *coding)
{
  for (int i = 0; i < codes_count; i++)
    {
      ptrdiff_t pos = str_pos (name, codes[i].name);
      if (pos >= 0)
        {
          *coding = i;
          return pos;
        }
    }
  *coding = -2;
  return -2;
}


ptrdiff_t
fget_coding (const tchar_t* text, int *coding)
{
  ptrdiff_t posit = 0;
  ptrdiff_t i = 0;
  const tchar_t* s, *s1;

  while ((i = str_pos (text, FD_ENCODING_LBRACKET)) >= 0)
    {
      s = text + i;

      if ((i = str_pos (s, FD_ENCODING_LBRACKET FD_ENCODING_MARK)) >= 0)
        posit += (s += tc::tcslen (FD_ENCODING_LBRACKET)) - text;
      else if (*(s = skip_word (s1 = s)) != _T ('\0'))
        posit += s - text;
      if ((i = str_pos (s, FD_ENCODING_MARK)) >= 0)
        {
          if (*(s = skip_word ((s1 = s) + i)) != _T ('\0'))
            posit += s - s1;
          if (*s == _T ('='))
            {
              if (*(s = skip_spaces ((s1 = s) + 1)) != _T ('\0'))
                posit += s - s1;
              i = get_coding (s, source_codes, coding);
              if (i >= 0)
                return posit + i;
            }
        }
    }
  *coding = -2;
  return -2;
}

tchar_t iconvert_char (tchar_t ch, int source_coding, int destination_coding, bool alphabet_only)
  {
    long i;
    const tchar_t* source_chars, *destination_chars;
  
    if (source_coding < 0)
      return ch;
    if (destination_coding < 0)
      return ch;
  
    int chars_count = alphabet_only ? chars_alphabet_count : chars_all_count;
    source_chars = source_codes[source_coding].codes;
    destination_chars = destination_codes[destination_coding].codes;
    i = chars_count;
    if ((unsigned) ch > 127)
      for (i = 0; i < chars_count; i++)
        if (ch == source_chars[i])
          break;
    return i < chars_count ? destination_chars[i] : ch;
  }

int
iconvert (tchar_t* string, int source_coding, int destination_coding, bool alphabet_only)
  {
    const tchar_t* source_chars, *destination_chars, *cod_pos = nullptr;
    tchar_t ch;
    tchar_t* s = string;
  
    if (string == nullptr)
      return -1;
    if (source_coding < 0)
      {
        ptrdiff_t posit = fget_coding (string, &source_coding);
        if (posit != 0)
          cod_pos = string + posit;
      }
    if (source_coding < 0)
      return -1;
    if (destination_coding < 0)
      return -2;
  
    int chars_count = alphabet_only ? chars_alphabet_count : chars_all_count;
    source_chars = source_codes[source_coding].codes;
    destination_chars = destination_codes[destination_coding].codes;
    for (;;)
      if (cod_pos == s)
        {
          size_t i, j;
          i = tc::tcslen (source_codes[source_coding].name);
          j = tc::tcslen (destination_codes[destination_coding].name);
          if (i != j)
            memmove (s + j, s + i, tc::tcslen (s + i) + 1);
          memcpy (s, destination_codes[destination_coding].name, j);
          s += j;
        }
      else
        {
          ch = *s;
          if (!ch)
            break;
          int i = chars_count;
          if ((unsigned) ch > 127)
            for (i = 0; i < chars_count; i++)
              if (ch == source_chars[i])
                break;
          if (i < chars_count)
            {
              ch = destination_chars[i];
              *s = ch;
            }
          s++;
        }
      return 0;
  }

int
iconvert_new (const tchar_t* source, tchar_t* *destination, int source_coding, int destination_coding, bool alphabet_only)
  {
    const size_t destSiz = tc::tcslen(source) + 1 + 10;
    tchar_t* dest = static_cast<tchar_t*> (malloc (sizeof(tchar_t) * destSiz)); /* reserved for MYCHARSET= replacement */
    int result = -3;
    if (dest)
      {
        tc::tcslcpy (dest, destSiz, source);
        result = iconvert (dest, source_coding, destination_coding, alphabet_only);
        if (!result)
          {
            *destination = dest;
            return 0;
          }
        free (dest);
      }
    return result;
  }
