#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

void PatchReinstallList (dword *UncompressedFirmware)
{
  dword                 i, *p;

  p = (dword*) ((dword)UncompressedFirmware + PATCHLISTSTART);

  if (!isValidRAMPtr(p)) return;

  p = PatchFindNextListEntry (p);
  for (i = 0; i < PatchListSize; i++)
  {
    if (!p) break;
    *p = LUI_CMD | PatchList [i];
    p = PatchFindNextListEntry (p);
  }

  PatchListSize = 0;
}
