#include                "FBLib_dialog.h"

void DialogWindowChange (tDialogWindow *DialogWindow, bool Redraw)
{
  FBDialogWindow = DialogWindow;

  if (Redraw) WindowDirty();
}
