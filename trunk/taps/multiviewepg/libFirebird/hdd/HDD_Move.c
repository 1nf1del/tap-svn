#include "FBLib_hdd.h"
#include "../libFireBird.h"

tTAP_Hdd_Move TAP_Hdd_Move = NULL;

bool HDD_Move (char *FileName, char *FromDir, char *ToDir)
{
  dword                 TAP_Hdd_MoveOld;
  char                  FromFile [FBLIB_DIR_SIZE], TempFromDir [FBLIB_DIR_SIZE], TempToDir [FBLIB_DIR_SIZE];

  if (strlen(FromDir) >= sizeof(TempFromDir) || strlen(ToDir) >= sizeof(TempToDir)) return FALSE;

  //The move function doesn't recognize a / as root. Use /DataFiles/.. instead.
  strcpy (TempFromDir, FromDir);
  if (!strcmp (TempFromDir, "/")) strcpy (TempFromDir, "/DataFiles/..");

  strcpy (TempToDir, ToDir);
  if (!strcmp (TempToDir, "/")) strcpy (TempToDir, "/DataFiles/..");

  if (TAP_Hdd_Move == NULL) TAP_Hdd_Move = (tTAP_Hdd_Move) TAP_GetSystemProc(oTAP_Hdd_Move);
  if (TAP_Hdd_Move == NULL)
  {
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: HDD_Move: _appl_version = %4.4x\n", _appl_version);
#endif
    if (!(TAP_Hdd_MoveOld = FIS_fwMoveOld())) return FALSE;

    if (strlen(TempFromDir) + 1 + strlen(FileName) >= sizeof(FromFile)) return FALSE;
    TAP_SPrint (FromFile, "%s/%s", TempFromDir, FileName);

#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: MoveOld: %s => %s\n", FromFile, TempToDir);
#endif
    CallFirmware (TAP_Hdd_MoveOld, (dword) FromFile, (dword) TempToDir, 0, 0);
    return TRUE;
  }
  else return TAP_Hdd_Move(TempFromDir, TempToDir, FileName);
}
