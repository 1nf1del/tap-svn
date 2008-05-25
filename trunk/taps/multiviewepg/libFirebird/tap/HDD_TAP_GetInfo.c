#include "FBLib_tap.h"

//--------------------------------------- GetTAPInfo --------------------------------
//
void HDD_TAP_GetInfo (int TAPIndex, tTAPTableInfo *pTAPInfo)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return;

  memcpy (pTAPInfo, (void *) (TAP_TableAddress + 32 * TAPIndex), 32);
  if (pTAPInfo->LoadAddress == 0)
  {
    pTAPInfo->TAPName        [0] = 0;
    pTAPInfo->TAPAuthor      [0] = 0;
    pTAPInfo->TAPDescription [0] = 0;
    pTAPInfo->TAPID              = 0;
    memset (&pTAPInfo->AddressTableAddress, 0, 0x0400);
  }
  else
  {
    strcpy (pTAPInfo->TAPName          , (char *) (pTAPInfo->LoadAddress + 0x0040));
    strcpy (pTAPInfo->TAPAuthor        , (char *) (pTAPInfo->LoadAddress + 0x0080));
    strcpy (pTAPInfo->TAPDescription   , (char *) (pTAPInfo->LoadAddress + 0x00C0));
    memcpy (&pTAPInfo->TAPID           , (void *) (pTAPInfo->LoadAddress + 0x0020), 4);
    memcpy (&pTAPInfo->AddressTable [0], (void *) pTAPInfo->AddressTableAddress, 0x0400);
  }
}
