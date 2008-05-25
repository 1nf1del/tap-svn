#include "FBLib_av.h"
#include "../libFireBird.h"

//It is not possible to set the tuner for the PIP
bool TunerSet (byte Tuner)
{
  int                   tvRadio, chNum;

  //Parameter sanity check
  if (Tuner > 1) return FALSE;

  //Set the tuner number for the channel
  *(byte *)(GetEEPROMAddress() + 0x17) = Tuner;
  TAP_Channel_GetCurrent (&tvRadio, &chNum);
  TAP_Channel_Start (CHANNEL_Main, tvRadio, chNum);

  return TRUE;
}
