#include "FBLib_main.h"

//--------------------------------------- isMasterpiece --------------------------------
//
bool isMasterpiece (void)
{
  if (!LibInitialized) InitTAPex();
  if (!LibInitialized) return FALSE;

  switch (SysID)
  {
    case  1416:
    case  1426:
    case  1456:
    case  1466:
    case  1486:
    case  2416:
    case  2446:
    case  2456:
    case  3416:
    case 21031:
    case 30002:
    case 40052:
      return TRUE;
      break;

    default:
      return FALSE;
      break;
  }
}
