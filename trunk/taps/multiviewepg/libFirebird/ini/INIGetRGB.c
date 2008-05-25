#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "FBLib_ini.h"
#include "../libFireBird.h"

//Expects 5 bit RGB values

bool INIGetRGB (char *Key, byte *Red, byte *Green, byte *Blue, dword DefaultValue)
{
  char                  *i = NULL, *j = NULL, *k;
  char                  TempKey [80];
  dword                 l, x;
  size_t                plen;

  if (!Key || !Red || !Green || !Blue) return FALSE;

  strncpy (TempKey, Key, sizeof(TempKey) - 2);
  TempKey[sizeof(TempKey) - 2] = '\0';
  strcat (TempKey, "=");
  l = strlen(TempKey);

  INIFindStartEnd (TempKey, &i, &j, l + 14);

  if (!i || !j || (j < i + l)) x = DefaultValue;
  else
  {
    k = i + l;
    x = 0;

    // red
    if ((k = ParseLine(k, &plen, ',')) && (plen > 0 && k + plen < j))
    {
      x |= (strtoul(k, NULL, (plen > 2 && tolower(*(k + 1)) == 'x' ? 16 : 10)) & 0x1f) << 10;
      k += plen;

      // green
      if ((k = ParseLine(k, &plen, ',')) && (plen > 0 && k + plen < j))
      {
        x |= (strtoul(k, NULL, (plen > 2 && tolower(*(k + 1)) == 'x' ? 16 : 10)) & 0x1f) << 5;
        k += plen;

        // blue
        if (*k == ',') k++;
        x |= strtoul(k, NULL, (j - k > 2 && tolower(*(k + 1)) == 'x' ? 16 : 10)) & 0x1f;
      }
      else x = DefaultValue;
    }
    else x = DefaultValue;
  }

  *Red   = (x >> 10) & 0x1f;
  *Green = (x >> 5) & 0x1f;
  *Blue  = x & 0x1f;

  return TRUE;
}
