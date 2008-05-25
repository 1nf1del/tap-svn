#include                "FBLib_dialog.h"

void DialogProfileCheck (tDialogProfile *DialogProfile, char *AppName)
{
  if (FBDialogProfile || !DialogProfile) return;

  FBDialogProfile = DialogProfile;
  ProfileInit();
  FBDialogProfile->Magic = 0;
  DialogProfileLoadMy(AppName);
}
