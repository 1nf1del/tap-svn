#include                "FBLib_dialog.h"

tDialogMsgBox           *FBDialogMsgBox  = NULL;

void DialogMsgBoxInit (tDialogMsgBox *DialogMsgBox, tDialogProfile *DialogProfile, char *Title, char *SubTitle)
{
  if (FBDialogMsgBox || !(FBDialogMsgBox = DialogMsgBox)) return;

  memset(FBDialogMsgBox, 0, sizeof(tDialogMsgBox));

  DialogMsgBoxTitleSet(Title, SubTitle);

  if (!FBDialogProfile)
  {
    FBDialogProfile = DialogProfile;
    ProfileInit();
  }
}
