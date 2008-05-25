#include                <ctype.h>
#include                <string.h>
#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

word                    PatchList [PATCHLISTSIZE];
dword                   PatchListSize = 0;

bool PatchInstallID (dword *UncompressedFirmware, char *PatchID)
{
  dword                 i;
  word                  wPatchID;
  bool                  TempList = FALSE;

  if (!isValidRAMPtr(UncompressedFirmware) || !PatchID || !PatchID [0]) return FALSE;

  wPatchID = toupper(PatchID [0]) << 8;
  if (PatchID [1] != 0) wPatchID |= tolower(PatchID [1]);

  if (!PatchListSize)
  {
    PatchCleanList(UncompressedFirmware);
    TempList = TRUE;
  }

  for (i = 0; i < PatchListSize; i++)
    if (PatchList [i] == wPatchID)
    {
      if (TempList) PatchReinstallList(UncompressedFirmware);
      return FALSE;
    }


  if (PatchListSize < PATCHLISTSIZE)
    for (i = 0; i < PatchListSize; i++)
      if (wPatchID < PatchList [i])
      {
        //Shift the list by one and insert the new PatchID
        memmove (&PatchList [i + 1], &PatchList [i], (PatchListSize - i) * sizeof (*PatchList));
        PatchList [i] = wPatchID;
        PatchListSize++;
        if (TempList) PatchReinstallList(UncompressedFirmware);
        return TRUE;
      }

  //No hit, PatchID is the last item of the list
  if (PatchListSize < PATCHLISTSIZE) PatchList [PatchListSize++] = wPatchID;

  if (TempList) PatchReinstallList(UncompressedFirmware);

  return TRUE;
}
