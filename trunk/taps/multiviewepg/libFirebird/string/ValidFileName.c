#include "FBLib_string.h"

char *ValidFileName (char *strName, eRemoveChars ControlCharacters)
{
  char                  *s, *p, *l;
  static char           validName [TF_FILE_NAME_SIZE];

  s = strName;
  p = validName;
  l = p + TF_FILE_NAME_SIZE - 1;

  while (*s)
  {
    if (isLegalChar(*s, ControlCharacters)) *p++ = *s;

    if (p == l) break;

    s++;
  }

  *p = '\0';

  return validName;
}
