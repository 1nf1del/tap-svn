#include                "FBLib_tapcom.h"

void TAPCOM_CloseChannel (TAPCOM_Channel Channel)
{
  TAPCOM_InternalMesBuf *buf = (TAPCOM_InternalMesBuf *) Channel;

  if (isValidChannel (Channel) && (buf->FromID == __tap_ud__))
  {
    buf->tapcomSignature = 0;
    TAP_MemFree(Channel);
  }
}
