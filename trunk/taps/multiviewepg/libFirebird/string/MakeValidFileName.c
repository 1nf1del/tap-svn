#include "FBLib_string.h"

//-------------------------------- MakeValidFileName ---------------------------
//
void MakeValidFileName (char *strName, eRemoveChars ControlCharacters)
{
  unsigned char         *p, c;
  int                   i;
  int                   fileNameLength;

  p = strName;
  i = 0;
  fileNameLength = strlen(strName);
  while (i < fileNameLength)
  {
    c = strName[i];
    if (isLegalChar(c, ControlCharacters))
    {
      *p = c;
      p++;
    }
    i++;
  }
  *p = '\0';
}
