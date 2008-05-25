#include                <ctype.h>
#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

bool PatchIsInstalled (dword *UncompressedFirmware, char *PatchID)
{
  dword                 i, *p;
  word                  wPatchID;

  p = (dword*) ((dword)UncompressedFirmware + PATCHLISTSTART);

  if (!isValidRAMPtr(p) || !PatchID || !PatchID [0]) return FALSE;

  wPatchID = toupper(PatchID [0]) << 8;
  if (PatchID [1] != 0) wPatchID |= tolower(PatchID [1]);

  if (PatchListSize)
  {
    for (i = 0; i < PatchListSize; i++)
      if (PatchList[i] == wPatchID) return TRUE;
  }
  else
  {
    do
    {
      p = PatchFindNextListEntry (p);
      if (!p) break;
      if ((*p & 0xffff) == wPatchID) return TRUE;
    } while (*p != NOP_CMD);
  }

  return FALSE;
}
