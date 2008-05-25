#include <string.h>
#include "FBLib_ini.h"
#include "../libFireBird.h"

//---------------------------------------  INIFindStartEnd --------------------------------
//
void INIFindStartEnd (char *Key, char **Start, char **End, dword MaxEntrylen)
{
  char                  *CR, *LF;

  if(!Start || !End || !MaxEntrylen) return;

  if (!INIBuffer)
  {
    *Start = NULL;
    return;
  }

  *Start = stricstr (INIBuffer, Key);
  *End = NULL;

  if (*Start == NULL) return;

  CR = strchr (*Start, '\x0d');
  LF = strchr (*Start, '\x0a');

  if (CR)
  {
    if (LF)
    {
      if (CR + 1 == LF) *End = CR - 1;   // Windows
      else                               // junk
      {
        *End = NULL;
        return;
      }
    }
    else *End = CR - 1;                  // Mac
  }
  else if (LF) *End = LF - 1;            // Unix
  else                                   // junk
  {
    *End = NULL;
    return;
  }

  if (*End >= *Start + MaxEntrylen) *End = *Start + MaxEntrylen - 1;

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: INIFindStartEnd %x - %x\n", *Start, *End);
#endif
}
