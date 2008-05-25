#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_SendEvent --------------------------------
//
dword HDD_TAP_SendEvent (dword TAPID, bool AllowParamInterception, word event, dword param1, dword param2)
{
  dword                 Ret, i;

  if (!LibInitialized) InitTAPex();
  if (!LibInitialized) return 0;

  if (TAPID)
  {
    int Index = HDD_TAP_GetIndexByID (TAPID);
    if (Index < 0 || Index >= TAP_MAX) return 0;
    Ret = SendEvent (Index, event, param1, param2);
    if ((Ret == 0) && AllowParamInterception) return 0;
  }
  else
  {
    for (i = 0 ; i < TAP_MAX; ++i)
    {
      //if (i != TAP_TableIndex)  // don't call ourselves
      {
        Ret = SendEvent (i, event, param1, param2);

        // Zero return value should mean don't pass the value on to other TAPs
        // In this case we don't even call the remaining TAPs
        if ((Ret == 0) && AllowParamInterception) return 0;
      }
    }
  }

  return param1;
}
