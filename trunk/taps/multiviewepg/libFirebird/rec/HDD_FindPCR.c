#include "../libFireBird.h"

//-------------------------------- HDD_FindPCR ---------------------------
//
dword HDD_FindPCR (byte *pBuffer, dword BufferSize)
{
  dword		        i = 0;
  dword		        PCR;

  do
  {
    if ((pBuffer [i + 0] == 0x47) && ((pBuffer [i + 3] & 0x20) != 0) && (pBuffer [i + 4] > 0) && ((pBuffer [i + 5] & 0x10) != 0))
    {
      //Extract the time out of the PCR bit pattern
      //The PCR is clocked by a 90kHz generator. To convert to minutes
      //the 33 bit number can be shifted right 6 times and divided by 84375
      PCR = (pBuffer [i + 6] << 19) | (pBuffer [i + 7] << 11) | (pBuffer [i + 8] << 3) | (pBuffer [i + 9] >> 5);
      PCR /= 84375;
#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("PCR @  0x%8.8x = %d\n", i, PCR);
#endif
      return PCR;
    }
    i += 188;
  } while (i < (BufferSize - 188));

  return 0;
}
