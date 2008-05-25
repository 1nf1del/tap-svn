#include "FBLib_tap.h"

dword                   TAPDirStack[TAPDIR_MAX_STACK];
int                     TAPDirStackDepth = -1;

bool HDD_TAP_PushDir (void)
{
  if (++TAPDirStackDepth < TAPDIR_MAX_STACK)
  {
    if ((int)(TAPDirStack[TAPDirStackDepth] = (dword)HDD_TAP_GetCurrentDirCluster()) >= 0) return TRUE;
  }
  TAPDirStackDepth--;
  return FALSE;
}

