#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_Callback --------------------------------
//
dword HDD_TAP_Callback (dword TAPID, void *ProcedureAddress, dword param1, dword param2, dword param3, dword param4)
{
  dword                 Ret = 0;

  if (!LibInitialized) InitTAPex();
  if (!LibInitialized) return 0;

  if (TAPID)
  {
    int Index = HDD_TAP_GetIndexByID (TAPID);
    if (Index >= 0 && Index < TAP_MAX) Ret = Callback (Index, ProcedureAddress, param1, param2, param3, param4);
  }

  return Ret;
}

