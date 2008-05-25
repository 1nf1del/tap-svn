#include                "FBLib_tapcom.h"

void TAPCOM_StillAlive (TAPCOM_Channel Channel)
{
  if (isValidChannel (Channel))
  {
    //Reset the server timeout
    ((TAPCOM_InternalMesBuf *) Channel)->ServerAlive = TAP_GetTick();
  }
}
