#include "FBLib_main.h"

bool SendToFP (byte *FPData)
{
  dword                 SendFrontPanelString;

  if (!LibInitialized) InitTAPex();
  if (!LibInitialized) return FALSE;

  if (!(SendFrontPanelString = FIS_fwSendToFP())) return FALSE;

  //Call the routine, which expects a pointer to the data array.
  //SendFrontPanelString is taking care about the STX/ETX characters
  CallFirmware (SendFrontPanelString, (dword) &FPData [0], 0, 0, 0);

  return TRUE;
}
