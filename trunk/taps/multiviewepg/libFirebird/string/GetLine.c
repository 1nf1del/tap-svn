#include <string.h>
#include "../libFireBird.h"

//--------------------------------------- GetLine --------------------------------
//
size_t GetLine (char *data, bool strip)
{
  char                  *CRLFPos, *LFPos;

  if (!*data) return 0;

  CRLFPos = strstr (data, "\r\n");
  LFPos = strchr (data, '\n');

  if (CRLFPos == NULL && LFPos == NULL) return strlen(data);
  else if (CRLFPos == NULL || LFPos < CRLFPos)
  {
    if (strip) *LFPos = '\0';
    return LFPos - data + 1;
  }
  else
  {
    if (strip) *CRLFPos = '\0';
    return CRLFPos - data + 2;
  }
}
