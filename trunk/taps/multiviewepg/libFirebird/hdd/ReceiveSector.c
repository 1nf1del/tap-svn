#include "FBLib_hdd.h"
#include "../libFireBird.h"

volatile word *HDD_pData        = (word*) 0xb0012d22;   // data register
volatile byte *HDD_pError       = (byte*) 0xb0012d27;   // feature/error register

int ReceiveSector (word *DataBuf)
{
  int                   CharCount;
  word                  d;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 4;

  //Copy the data into a buffer
  for (CharCount = 0; CharCount < 256; CharCount++)
  {
    busyWait ();
    if (((*HDD_pStatus & 0x08) == 0) || ((*HDD_pError & 0x04) != 0))
    {
#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("FireBirdLib: The read command failed or finished prematurely. Count=%d, ErrorReg=0x%x, StatusReg=0x%x\r\n", CharCount, *HDD_pError, *HDD_pStatus);
#endif
      return 4;
    }
    d = *HDD_pData;
    DataBuf [CharCount] = ((d & 0x00ff) << 8) | ((d & 0xff00) >> 8);
  }
  return 0;
}
