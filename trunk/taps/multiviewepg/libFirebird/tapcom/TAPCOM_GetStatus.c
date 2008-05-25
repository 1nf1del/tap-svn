#include                "FBLib_tapcom.h"

TAPCOM_Status TAPCOM_GetStatus (TAPCOM_Channel Channel)
{
  TAPCOM_InternalMesBuf *buf = (TAPCOM_InternalMesBuf *) Channel;

  if (!isValidChannel (Channel)) return TAPCOM_Status_NO_CHANNEL;

  //Prüfe, ob der Server überhaupt noch aktiv ist.
  if (((buf->Status == TAPCOM_Status_OPEN) ||
       (buf->Status == TAPCOM_Status_ACKNOWLEDGED)) &&
       (!HDD_TAP_isRunning (buf->ToID))) buf->Status = TAPCOM_Status_SERVER_NOT_AVAILABLE;

  return buf->Status;
}
