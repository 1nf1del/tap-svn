#include                "FBLib_dialog.h"

bool ProfileMayReload (void)
{
  if (!FBDialogProfile || (FBDialogProfile->Magic != DIALOGPROFILE_MAGIC)) return TRUE;
  else return (FBDialogProfile->HDDAccess == 1 || (FBDialogProfile->HDDAccess == 2 && TAP_GetSystemVar(SYSVAR_Timeshift) == 1));
}
