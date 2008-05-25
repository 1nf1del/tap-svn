#include                "FBLib_dialog.h"

void DialogProfileChange (tDialogProfile *DialogProfile)
{
  FBDialogProfile = DialogProfile;
  ProfileDirty();
}
