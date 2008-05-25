#include "../libFireBird.h"

word GetOSDRegionHeight (word Region)
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

  return OSDMapInfo[Region].Height;
}
