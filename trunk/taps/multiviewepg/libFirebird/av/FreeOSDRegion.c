#include "../libFireBird.h"

void FreeOSDRegion (word Region)
{
  tOSDMapInfo           *OSDMapInfo;

  OSDMapInfo = (tOSDMapInfo*) GetOSDMapAddress();

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: %d/%d %dx%d @%8.8x %2.2x %2.2x %2.2x %2.2x\n",
                                           OSDMapInfo [Region].x, OSDMapInfo [Region].y,
                                           OSDMapInfo [Region].Width, OSDMapInfo [Region].Height,
                                           OSDMapInfo [Region].pMemoryOSD,
                                           OSDMapInfo [Region].Flags [0], OSDMapInfo [Region].Flags [1],
                                           OSDMapInfo [Region].Flags [2], OSDMapInfo [Region].Flags [3]);
#endif

  if (OSDMapInfo[Region].pMemoryOSD) TAP_MemFree((void *) OSDMapInfo[Region].pMemoryOSD);

  memset(&OSDMapInfo[Region], 0, sizeof(tOSDMapInfo));
}
