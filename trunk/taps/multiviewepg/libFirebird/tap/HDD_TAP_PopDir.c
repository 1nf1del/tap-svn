#include "FBLib_tap.h"

bool HDD_TAP_PopDir (void)
{
  if (TAPDirStackDepth >= 0) return HDD_TAP_SetCurrentDirCluster(TAPDirStack[TAPDirStackDepth--]);
  else return FALSE;
}
