#include "FBLib_main.h"

//--------------------------------------- GetFWInfo --------------------------------
//
void GetFWInfo (dword *pSysID, word *pApplID, dword *pBIOSAddress, dword *pEnqueueEventAddress, dword *pTAPgp, dword *pFWgp, dword *pTAP_TableAddress, dword *pTAP_Ident)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return;

  *pSysID                   = SysID;
  *pApplID                  = ApplID;
  *pBIOSAddress             = BIOSAddress;
  *pEnqueueEventAddress     = (dword)EnqueueEventAddress;
  *pTAPgp                   = TAPgp;
  *pFWgp                    = FWgp;
  *pTAP_TableAddress        = TAP_TableAddress;
  *pTAP_Ident               = TAP_IndexAddress;
}
