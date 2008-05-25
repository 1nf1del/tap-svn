#include "FBLib_av.h"
#include "../libFireBird.h"

//--------------------------------------- GetTuner --------------------------------
//
byte TunerGet (int MainSub)
{
  //Parameter sanity check
  if (MainSub != CHANNEL_Main && MainSub != CHANNEL_Sub) return -1;

  //Get the tuner number
  return *(byte *)(GetEEPROMAddress() + 0x16 + MainSub);
}
