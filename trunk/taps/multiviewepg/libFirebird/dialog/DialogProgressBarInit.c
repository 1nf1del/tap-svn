#include                "FBLib_dialog.h"

tDialogProgressBar      *FBDialogProgressBar  = NULL;

void DialogProgressBarInit (tDialogProgressBar *DialogProgressBar, tDialogProfile *DialogProfile, dword X, dword Y, char *Title, int Minimum, int Maximum)
{
  if (FBDialogProgressBar || !(FBDialogProgressBar = DialogProgressBar)) return;

  memset(FBDialogProgressBar, 0, sizeof(tDialogProgressBar));

  FBDialogProgressBar->OSDX   = X;
  FBDialogProgressBar->OSDY   = Y;
  FBDialogProgressBar->Min    = Minimum;
  FBDialogProgressBar->Max    = Maximum;
  FBDialogProgressBar->Value  = Minimum;

  DialogProgressBarTitleSet(Title);

  if (!FBDialogProfile)
  {
    FBDialogProfile = DialogProfile;
    ProfileInit();
  }
}
