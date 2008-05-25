#include                "FBLib_tapcom.h"

int TAPCOM_GetReturnValue (TAPCOM_Channel Channel)
{
  TAPCOM_InternalMesBuf *buf = (TAPCOM_InternalMesBuf *) Channel;

  if (!isValidChannel(Channel) || (buf->Status != TAPCOM_Status_FINISHED)) return TAPCOM_NO_RETURN_VALUE;
  else return buf->ReturnVal;
}
