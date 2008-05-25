#include "../libFireBird.h"

dword FIS_fwSetPlaybackSpeed (void)
{
  static dword          fwSetPlaybackSpeed = 0;
  dword                *p = (dword*)FIS_fwSetPlaybackMode();

  if (!p) return 0;

  if (!fwSetPlaybackSpeed)
  {
    //Locate the first JAL in fwSetPlaybackMode
    while ((*p & CMD_MASK) != JAL_CMD)
    {
      //Skip the first "addiu $sp"
      p++;
      if ((dword)p > 0x82000000 || (*p & CMD_MASK) == ADD_SP_CMD) return 0;
    }
    fwSetPlaybackSpeed = ABS_ADDR (*p);
  }

  return fwSetPlaybackSpeed;
}
