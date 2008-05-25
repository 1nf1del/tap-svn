#include                "FBLib_dialog.h"

void DialogMsgBoxShowOK (void)
{
  if (!FBDialogMsgBox) return;

  strcpy (FBDialogMsgBox->ButtonText [0], "OK");
  FBDialogMsgBox->ButtonText [1][0] = '\0';
  DialogMsgBoxShow ();
}
