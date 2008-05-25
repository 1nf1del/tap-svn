#include                "FBLib_tapcom.h"

void TAPCOM_Finish (TAPCOM_Channel Channel, int val)
{
  TAPCOM_InternalMesBuf *buf = (TAPCOM_InternalMesBuf *) Channel;

  if (isValidChannel (Channel))
  {
    buf->Status    = TAPCOM_Status_FINISHED;
    buf->ReturnVal = val;

    //Server-Timeout zurücksetzen
    buf->ServerAlive = TAP_GetTick();
  }
}
