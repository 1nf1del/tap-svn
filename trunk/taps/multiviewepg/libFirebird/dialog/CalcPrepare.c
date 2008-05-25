#include                "FBLib_dialog.h"

int PrevTopItem;
int PrevSelItem;

void CalcPrepare (void)
{
  if (FBDialogWindow)
  {
    PrevTopItem = FBDialogWindow->TopItem;
    PrevSelItem = FBDialogWindow->SelectedItem;
  }
}
