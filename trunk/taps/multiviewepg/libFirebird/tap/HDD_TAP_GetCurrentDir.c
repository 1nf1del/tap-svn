#include "../libFireBird.h"

//Return codes:     0: OK
//                 -1: not enough memory (from HDD_TAP_GetCurrentDirCluster)
//                 -2: buffer overrun in subdir search (from HDD_TAP_GetCurrentDirCluster)
//                 -3: Lib init failed (from HDD_TranslateDirCluster via HDD_TAP_GetCurrentDirCluster)

int HDD_TAP_GetCurrentDir (char *CurrentDir)
{
  return HDD_TranslateDirCluster ((dword)HDD_TAP_GetCurrentDirCluster(), CurrentDir);
}
