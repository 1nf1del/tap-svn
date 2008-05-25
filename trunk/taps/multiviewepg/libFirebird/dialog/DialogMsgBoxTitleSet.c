#include                <string.h>
#include                "FBLib_dialog.h"

void DialogMsgBoxTitleSet (char *Title, char *SubTitle)
{
  if (!FBDialogMsgBox) return;

  if (Title)
  {
    strncpy (FBDialogMsgBox->DialogTitle, Title, sizeof(FBDialogMsgBox->DialogTitle) - 1);
    FBDialogMsgBox->DialogTitle[sizeof(FBDialogMsgBox->DialogTitle) - 1] = '\0';
  }

  if (SubTitle)
  {
    strncpy (FBDialogMsgBox->DialogSubTitle, SubTitle, sizeof(FBDialogMsgBox->DialogSubTitle) - 1);
    FBDialogMsgBox->DialogSubTitle[sizeof(FBDialogMsgBox->DialogSubTitle) - 1] = '\0';
  }

  if (FBDialogMsgBox->isVisible) DrawMsgBoxTitle();
}
