#include                <ctype.h>
#include                <string.h>
#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

bool PatchRemoveID (dword *UncompressedFirmware, char *PatchID)
{
  dword                 i;
  word                  wPatchID;
  bool                  TempList = FALSE;

  if (!isValidRAMPtr(UncompressedFirmware) || !PatchID || !PatchID [0]) return FALSE;

  wPatchID = toupper (PatchID [0]) << 8;
  if (PatchID [1] != 0) wPatchID |= tolower(PatchID [1]);

  if (!PatchListSize)
  {
    PatchCleanList(UncompressedFirmware);
    TempList = TRUE;
  }

  for (i = 0; i < PatchListSize; i++)
    if (PatchList [i] == wPatchID)
    {
      if (i < (PatchListSize - 1)) memmove (&PatchList [i], &PatchList [i + 1], (PatchListSize - i - 1) * sizeof (*PatchList));
      PatchListSize--;

      if (TempList) PatchReinstallList(UncompressedFirmware);
      return TRUE;
    }

  if (TempList) PatchReinstallList(UncompressedFirmware);

  return FALSE;
}
