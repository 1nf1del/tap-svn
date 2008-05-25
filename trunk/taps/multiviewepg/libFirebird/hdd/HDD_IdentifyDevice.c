#include "FBLib_hdd.h"
#include "../libFireBird.h"

bool HDD_IdentifyDevice (char *IdentifyDeviceBuffer)
{
  int                   CharCount;
  dword                 RS;
  word                  Data;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  RS = intLock ();
  if (SendHDDCommand (0, 0, 0, 0, 0, 0xA0, 0xEC) != 0) return FALSE;

  //Copy the data into a buffer
  for (CharCount = 0; CharCount < 256; CharCount++)
  {
    Data = *HDD_pData;
    IdentifyDeviceBuffer [2 * CharCount]  = (Data >> 8);
    IdentifyDeviceBuffer [2 * CharCount + 1] = (Data & 0xFF);
  }

  //Reset the PIC2 interrupt 8 and 9 flags
  *EMMA_PIO1_Status = *EMMA_PIO1_Status & 0xffefffff;
  *EMMA_PIO2_Status = 0xf7ff;
  intUnlock (RS);

  return TRUE;
}
