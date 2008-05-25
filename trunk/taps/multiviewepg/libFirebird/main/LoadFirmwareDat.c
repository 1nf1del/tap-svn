#include                "FBLib_main.h"

bool LoadFirmwareDat (tFWDATHeader **FWDatHeader, tToppyInfo **ToppyInfo, tFWInfo **FWInfo)
{
  static byte           *FWDATBin = NULL;
  TYPE_File             *fp;
  dword                 blk;
  tFWDATHeader          *FWDAT;

  if (!FWDATBin)
  {
    if (!HDD_TAP_PushDir()) return FALSE;

    if (INILocateFile (FIRMWAREDAT, NULL) == INILOCATION_NotFound)
    {
      (void) HDD_TAP_PopDir();
      return FALSE;
    }

    if (!(fp = TAP_Hdd_Fopen (FIRMWAREDAT)))
    {
      (void) HDD_TAP_PopDir();
      return FALSE;
    }

    if (!(FWDATBin = TAP_MemAlloc (TAP_Hdd_Flen (fp))))
    {
      TAP_Hdd_Fclose (fp);
      (void) HDD_TAP_PopDir();
      return FALSE;
    }

    blk = TAP_Hdd_Fread (FWDATBin, TAP_Hdd_Flen(fp), 1, fp);
    TAP_Hdd_Fclose(fp);
  }
  else blk = 1;   // already loaded

  FWDAT = (tFWDATHeader *) FWDATBin;

  //Check Magic and other things
  if (blk != 1 ||
      FWDAT->Magic != 0x12345678 ||
      FWDAT->ToppyInfoLayoutVersion != 0 ||
      FWDAT->FWInfoLayoutVersion != 0)
  {
    TAP_MemFree (FWDATBin);
    FWDATBin = NULL;
    (void) HDD_TAP_PopDir();
    return FALSE;
  }

  if (FWDatHeader) *FWDatHeader = FWDAT;
  if (ToppyInfo)   *ToppyInfo   = (tToppyInfo *) (FWDAT + 1);
  if (FWInfo)      *FWInfo      = (tFWInfo *) ((tToppyInfo *) (FWDAT + 1) + FWDAT->NrOfToppyInfoEntries);

  (void) HDD_TAP_PopDir();

  return TRUE;
}
