#include                "FBLib_main.h"
#include                "../libFireBird.h"

bool IdentifyFirmware (dword *UncompressedFirmware, char *Model, dword *FWDate, word *SysID, word *App, dword *FWgp)
{
  dword                 gp;
  tFWInfo              *FWInfo;
  tFWDATHeader         *FWDatHeader;
  dword                 i;

  if (!isValidRAMPtr(UncompressedFirmware)) return FALSE;

  gp = FIS_GetGP ((dword*)0x80000000);

  if (Model)  TAP_SPrint (Model, "unknown $gp=0x%8.8x", gp);
  if (SysID)  *SysID  = 0;
  if (FWDate) *FWDate = 0;
  if (App)    *App    = 0;
  if (FWgp)   *FWgp   = gp;

  if (!LoadFirmwareDat(&FWDatHeader, NULL, &FWInfo)) return FALSE;

  for (i = 0; i < FWDatHeader->NrOfFWInfoEntries; i++)
  {
    if (FWInfo->FWgp == gp)
    {
      if (FWgp)  *FWgp = gp;
      if (SysID) *SysID = FWInfo->SysID;

      //Toppy style MJD
      if (FWDate) *FWDate = TAP_MakeMjd (FWInfo->FWDate >> 16, (FWInfo->FWDate >> 8) & 0xff, FWInfo->FWDate & 0xff);
      if (App) *App = FWInfo->AppVersion;
      if (Model) strcpy (Model, GetToppyString (FWInfo->SysID));
      return TRUE;
    }
    else FWInfo++;
  }

  return FALSE;
}
