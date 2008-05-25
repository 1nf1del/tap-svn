#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

void PatchCleanList (dword *UncompressedFirmware)
{
  dword                 *p;
  bool                  DontExit;

  p = (dword*) ((dword)UncompressedFirmware + PATCHLISTSTART);

  if (PatchListSize || !isValidRAMPtr(p)) return;

  memset (PatchList, 0, sizeof(PatchList));
  do
  {
    DontExit = FALSE;
    p = PatchFindNextListEntry (p);
    if (!p) return;
    if ((*p & 0xffff0000) == LUI_CMD)
    {
      if (PatchListSize < PATCHLISTSIZE) PatchList [PatchListSize++] = *p & 0xffff;
      *p = NOP_CMD;
      DontExit = TRUE;
    }
  } while (*p != NOP_CMD || DontExit);
}
