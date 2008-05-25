#include                "FBLib_tapcom.h"

bool isValidChannel (TAPCOM_Channel Channel)
{
  //Zeigt der Pointer ausserhalb des zulässigen Speicherbereichs?
  if (((dword) Channel < 0x80000000) || ((dword) Channel > 0x84000000 - sizeof(TAPCOM_InternalMesBuf))) return FALSE;

  //Gültige Signatur?
  return (((TAPCOM_InternalMesBuf *) Channel)->tapcomSignature == TAPCOM_SIGNATURE);
}
