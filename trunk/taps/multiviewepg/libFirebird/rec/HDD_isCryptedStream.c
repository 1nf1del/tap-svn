#include "../libFireBird.h"

//-------------------------------- HDD_isCryptedStream ---------------------------
//
bool HDD_isCryptedStream (char *Buffer, dword BufferSize)
{
  dword                 i;

  for (i = 0; i < BufferSize; i += 188)
  {
    if ((Buffer [i] == 0x47) && ((Buffer [i + 3] & 0xC0) != 0)) return TRUE;
  }

  return FALSE;
}
