#include <string.h>
#include "../libFireBird.h"

//--------------------------------------- ExtractLine --------------------------------
//
void ExtractLine (char *Text, char *Line)
{
  char                  *CRLFPos, *LFPos;

  if (!*Text)
  {
    *Line = '\0';
    return;
  }

  CRLFPos = strstr (Text, "\r\n");
  LFPos = strchr (Text, '\n');

  if (CRLFPos == NULL && LFPos == NULL)
  {
    strcpy(Line, Text);
    *Text = '\0';
  }
  else if ((CRLFPos == NULL) || (LFPos < CRLFPos))
  {
    strncpy (Line, Text, LFPos - Text);
    strcpy (Text, LFPos + 1);
    Line [LFPos - Text] = '\0';
  }
  else
  {
    strncpy (Line, Text, CRLFPos - Text);
    strcpy (Text, CRLFPos + 2);
    Line [CRLFPos - Text] = '\0';
  }
}
