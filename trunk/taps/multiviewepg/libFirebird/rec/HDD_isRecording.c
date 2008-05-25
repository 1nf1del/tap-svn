#include "../libFireBird.h"

//---------------------------------------  HDD_isRecording --------------------------------
//
bool HDD_isRecording (byte RecSlot)
{
  TYPE_RecInfo          recInfo;

  InitTAPAPIFix();
  return (TAP_Hdd_GetRecInfo(RecSlot, &recInfo) && (recInfo.recType == RECTYPE_Normal || recInfo.recType == RECTYPE_Copy));
}
