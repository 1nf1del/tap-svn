#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_GetStartParameter --------------------------------
//
void *HDD_TAP_GetStartParameter (void)
{
  tTAPTableInfo         TAPInfo;
  dword                 i, LoadAddress, ParameterBlockPointer;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return NULL;

  //Get my own TAPID
  HDD_TAP_GetInfo (TAP_TableIndex, &TAPInfo);

  //Locate a launcher TAP
  for (i = 0; i < TAP_MAX; i++)
  {
    LoadAddress = *(dword *) (TAP_TableAddress + (i << 5) + 12);
    if (LoadAddress)
    {
      //Has the parameter block pointer being initialized
      ParameterBlockPointer = *(dword *) (LoadAddress + 0x28);
      if ((ParameterBlockPointer > 0x80000000) && (ParameterBlockPointer < 0x84000000) && (*(dword *) ParameterBlockPointer == MAGIC))
      {
        //Has this TAP being started with parameter?
        if (*(dword *) (ParameterBlockPointer + sizeof(dword)) == TAPInfo.TAPID)
        {
          return (void *) (*(dword *) (ParameterBlockPointer + sizeof(dword) * 2));
        }
      }
    }
  }

  //No launcher found
  return NULL;
}
