#include                <ctype.h>
#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

char *PatchGetInstalled (dword *UncompressedFirmware)
{
  static char           InstPatches [PATCHLISTSIZE << 1];
  dword                 i, *p;
  size_t                l = 0;

  p = (dword*) ((dword)UncompressedFirmware + PATCHLISTSTART);

  if (!isValidRAMPtr(p)) return NULL;

  memset (InstPatches, 0, sizeof(InstPatches));

  if (PatchListSize)
  {
    for (i = 0; i < PatchListSize; i++)
    {
      //Make sure that there is enough room for 2 characters and a terminator
      if (l > sizeof(InstPatches) - 4) break;

      InstPatches [l++] = toupper(PatchList[i] >> 8);
      if (PatchList[i] & 0xff) InstPatches [l++] = tolower(PatchList[i] & 0xff);
    }
  }
  else
  {
    do
    {
      p = PatchFindNextListEntry (p);
      if (!p) break;
      if (*p)
      {
        if ((*p >> 8) & 0xff)
        {
          if (l > sizeof(InstPatches) - 4) break;
          InstPatches [l++] = toupper((*p >> 8) & 0xff);
          if (*p & 0xff) InstPatches [l++] = tolower(*p & 0xff);
        }
      }
    } while (*p != NOP_CMD);
  }

  return InstPatches;
}
