#include                "FBLib_tapcom.h"

TAPCOM_Channel TAPCOM_GetChannel (dword param1, dword *CallerID, dword *ServiceID, dword *ParamBlockVersion, void **ParamBlock)
{
  TAPCOM_InternalMesBuf *mesBuf = (TAPCOM_InternalMesBuf *) param1;

  if (!isValidChannel (mesBuf)) return NULL;

  if (mesBuf->tapcomVersion >= TAPCOM_REQVERSION)
  {
    if ((mesBuf->ToID == __tap_ud__) || (mesBuf->ToID == TAPCOM_App_BROADCAST))
    {
      // die Nachricht ist für mich oder Broadcast
      if (CallerID          != NULL) *CallerID           = mesBuf->FromID;
      if (ServiceID         != NULL) *ServiceID          = mesBuf->ServiceID;
      if (ParamBlockVersion != NULL) *ParamBlockVersion  = mesBuf->ParamBlockVersion;
      if (ParamBlock        != NULL) *ParamBlock         = mesBuf->ParamBlock;
      // Infos aus Message-Buffer holen

      //Falls es sich um keinen Broadcast handelt, den Block auf bestätigt setzen
      if (mesBuf->ToID != TAPCOM_App_BROADCAST) mesBuf->Status = TAPCOM_Status_ACKNOWLEDGED;
    }
    else return NULL;
  }
  else
  {
    mesBuf->Status = TAPCOM_Status_VERSIONMISMATCH;
    mesBuf->ReturnVal = TAPCOM_REQVERSION;
  }

  //Server-Timeout zurücksetzen
  mesBuf->ServerAlive = TAP_GetTick();

  return (TAPCOM_Channel) mesBuf;
}
