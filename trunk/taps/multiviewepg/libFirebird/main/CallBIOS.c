#include "FBLib_main.h"

//--------------------------------------- CallBIOS --------------------------------
//
void CallBIOS (dword Function, dword SubFunction, dword P1, dword P2, dword P3)
{
  tBIOSType             BIOSStruct;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return;

  BIOSStruct.SubCommand = SubFunction;
  BIOSStruct.P1   = P1;
  BIOSStruct.P2   = P2;
  BIOSStruct.P3   = P3;
  CallFirmware ((dword) BIOSAddress, Function, (dword) &BIOSStruct, 0, 0);
}
